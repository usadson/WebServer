/**
 * Copyright (C) 2020 Tristan. All Rights Reserved.
 * This file is licensed under the BSD 2-Clause license.
 * See the COPYING file for licensing information.
 */

#include "connection/connection.hpp"

#define TLS_LIBRARY_OPENSSL

#include <sstream>

#include <cerrno>
#include <ctime>
#include <unistd.h>

#if defined(__FreeBSD__)
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/uio.h>
#elif defined(__linux__)
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/ioctl.h>
#include <sys/sendfile.h>
#include <sys/socket.h>
#else
#include <array>
#endif

#if defined(TLS_LIBRARY_OPENSSL)
#include <array>
#include <openssl/err.h>
#include <openssl/ssl.h>
#else
#error Unsupported TLS Library
#endif

#include "base/logger.hpp"
#include "http/configuration.hpp"

#ifdef TLS_LIBRARY_OPENSSL
static const char *
GetSSLErrorString(int error) {
	switch (error) {
		case SSL_ERROR_NONE:
			return "SSL_ERROR_NONE";
		case SSL_ERROR_ZERO_RETURN:
			return "SSL_ERROR_ZERO_RETURN";
		case SSL_ERROR_WANT_READ:
			return "SSL_ERROR_WANT_READ or WRITE";
		case SSL_ERROR_WANT_CONNECT:
			return "SSL_ERROR_WANT_CONNECT or ACCEPT";
		case SSL_ERROR_WANT_X509_LOOKUP:
			return "SSL_ERROR_WANT_X509_LOOKUP";
		case SSL_ERROR_SYSCALL:
			return "SSL_ERROR_SYSCALL";
		case SSL_ERROR_SSL:
			return "SSL_ERROR_SSL";
		default:
			return "no string for this OpenSSL error code";
	}
}
#endif

Connection::~Connection() noexcept {
	if (hasWriteFailed) {
		// TODO Make sure if the socket can be viewed as void.
		// ... <<< NOTE no close() call
		return;
	}

	if (useTransportSecurity && securityContext != nullptr) {
#if defined(TLS_LIBRARY_OPENSSL)
		auto ssl = reinterpret_cast<SSL *>(securityContext);
		SSL_shutdown(ssl);
		SSL_free(ssl);
#endif
	}

	// This makes sure all data has been transferred before closing the
	// connection. Clever solutions like poll(2), read(?, ' ', 1), etc. didn't
	// work. Maybe non-blocking sockets solve it[?]
	struct timespec sleepTime { 0, 100000 };
	while (true) {
		int value;
		if (ioctl(internalSocket, TIOCOUTQ, &value) == -1 || value == 0) {
			break;
		}
		nanosleep(&sleepTime, nullptr);
	}

	/* ignore-return-value */ shutdown(internalSocket, SHUT_RDWR);
	/* ignore-return-value */ close(internalSocket);
}

bool
Connection::Setup(const HTTP::Configuration &configuration) noexcept {
	int i = 1;
	if (setsockopt(internalSocket, IPPROTO_TCP, TCP_NODELAY, static_cast<void *>(&i), sizeof(i)) == -1) {
		return false;
	}

	if (useTransportSecurity) {
#if defined(TLS_LIBRARY_OPENSSL)
		auto ctx = SSL_new(reinterpret_cast<SSL_CTX *>(configuration.tlsConfiguration.context));
		securityContext = ctx;
		if (securityContext == nullptr) {
			ERR_print_errors_fp(stderr);
			std::stringstream error;
			error << "SSL_new failed. TLS context is " << configuration.tlsConfiguration.context;
			Logger::Error(__PRETTY_FUNCTION__, error.str());
			return false;
		}

		if (SSL_set_fd(ctx, internalSocket) == 0) {
			ERR_print_errors_fp(stderr);
			Logger::Error("Connection::Setup", "Failed to set socket (FD)");
			return false;
		}

		auto status = SSL_do_handshake(ctx);
		if (status != 1) {
			ERR_print_errors_fp(stderr);
			Logger::Error("Connection::Setup", "Failed to perform TLS handshake.");
			std::stringstream error;
			error << "This is what OpenSSL has to say about it: \"";
			error << GetSSLErrorString(SSL_get_error(ctx, status)) << '"';
			Logger::Error("Connection::Setup", error.str());
			error = std::stringstream();
			error << "This is what error has to say about it: \"";
			error << std::strerror(errno) << '"';
			Logger::Error("Connection::Setup", error.str());
			return false;
		}

		return true;
#endif
	}

	// No setup is needed for non-secure connections
	return true;
}

// TODO Make sure that using char isn't causing problems when it isn't 8-bits.
bool
Connection::ReadChar(char *buf) const noexcept {
	if (buf == nullptr) {
		return false;
	}

	if (useTransportSecurity) {
#if defined(TLS_LIBRARY_OPENSSL)
		auto status = SSL_read(reinterpret_cast<SSL *>(securityContext), buf, 1);
		std::stringstream error;
		error << "SSL_read failed. securityContext is " << securityContext;
		Logger::Error(__FUNCTION__, error.str());
		ERR_print_errors_fp(stderr);
		return status == 1;
#else
		return false;
#endif
	}

	return read(internalSocket, buf, 1) != -1;
}

bool
Connection::SendFile(int fd, std::size_t count) noexcept {
	if (useTransportSecurity) {
		// TODO Use ktls for in-kernel TLS (= support for sendfile)
#if defined(TLS_LIBRARY_OPENSSL)
		std::array<char, 4096> buffer {};
		do {
			ssize_t result = read(fd, buffer.data(), 4096);
			if (result == -1) {
				return false;
			}
			count -= result;
			do {
				ssize_t writeResult = SSL_write(reinterpret_cast<SSL *>(securityContext), buffer.data(), result);
				if (writeResult == -1) {
					hasWriteFailed = true;
					return false;
				}
				result -= writeResult;
			} while (result != 0);
		} while (count != 0);
#else
		hasWriteFailed = true;
		return false;
#endif
	}

#if defined(__FreeBSD__)
	// Sendfile syscall
	// (int/fd)  src
	// (int/fd)  dest
	// (off_t)   offset in file
	// (size_t)  amount of bytes of file (0 = to EOF)
	// (*)       unused
	// (off_t *) amount of bytes sent (useful for non-blocking)
	// (int)     flags
	// TODO
	return sendfile(fd, internalSocket, 0, 0, nullptr, nullptr, 0) == 0;
#elif defined(__linux__)
	// Sendfile syscall
	// (int/fd) dest
	// (int/fd) src
	// (*)      unused
	// (size_t) count of bytes to rw

	while (count != 0) {
		ssize_t status = sendfile64(internalSocket, fd, nullptr, count);
		if (status == -1) {
			std::stringstream errorInfo;
			errorInfo << "[Linux] Error occurred: " << status << " errno is: " << errno;
			Logger::Error("Connection::SendFile", errorInfo.str());
			hasWriteFailed = true;
			return false;
		}
		count -= status;
	}

	return true;
#else
	std::array<char, 4096> buffer {};
	do {
		ssize_t result = read(fd, buffer.data(), 4096);
		if (result == -1) {
			return false;
		}
		count -= result;
		do {
			ssize_t writeResult = write(internalSocket, buffer.data(), result);
			if (writeResult == -1) {
				hasWriteFailed = true;
				return false;
			}
			result -= writeResult;
		} while (result != 0);
	} while (count != 0);
#endif
}

bool
Connection::WriteString(const std::string &str, bool includeNullCharacter) noexcept {
	std::size_t off = 0;
	std::size_t len = str.length() + (includeNullCharacter ? 1 : 0);

	while (len != 0) {
		ssize_t status;
		if (useTransportSecurity) {
#if defined(TLS_LIBRARY_OPENSSL)
			status = SSL_write(reinterpret_cast<SSL *>(securityContext), str.c_str() + off, len);
#endif
		} else {
			status = write(internalSocket, str.c_str() + off, len);
		}

		if (status == -1) {
			hasWriteFailed = true;
			return false;
		}

		len -= status;
		off += status;
	}

	return true;
}

bool
Connection::WriteStringView(const std::string_view &str, bool includeNullCharacter) noexcept {
	std::size_t off = 0;
	std::size_t len = str.length() + (includeNullCharacter ? 1 : 0);

	while (len != 0) {
		ssize_t status;
		if (useTransportSecurity) {
#if defined(TLS_LIBRARY_OPENSSL)
			status = SSL_write(reinterpret_cast<SSL *>(securityContext), str.data() + off, len);
#endif
		} else {
			status = write(internalSocket, str.data() + off, len);
		}

		if (status == -1) {
			hasWriteFailed = true;
			return false;
		}

		len -= status;
		off += status;
	}

	return true;
}
