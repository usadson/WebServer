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

#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/ioctl.h>

#if defined(__FreeBSD__)
#include <sys/types.h>
#include <sys/uio.h>
#elif defined(__linux__)
#include <sys/sendfile.h>
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
#include "connection/security_internals.hpp"
#include "http/configuration.hpp"
#include "posix/fcntl.hpp"
#include "posix/unistd.hpp"

Connection::~Connection() noexcept {
	if (hasWriteFailed) {
		// TODO Make sure if the socket can be viewed as void.
		// ... <<< NOTE no close() call
		return;
	}

	if (useTransportSecurity && securityContext != nullptr) {
		ConnectionSecureInternals::Destruct(this);
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
		return ConnectionSecureInternals::Setup(this, configuration);
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
		return ConnectionSecureInternals::ReadChar(this, buf);
	}

	return psx::read(internalSocket, buf, 1) != -1;
}

bool
Connection::SendFile(int fd, std::size_t count) noexcept {
	if (useTransportSecurity) {
		// TODO Use ktls for in-kernel TLS (= support for sendfile)
		return ConnectionSecureInternals::SendFile(this, fd, count);
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
		ssize_t result = psx::read(fd, buffer.data(), 4096);
		if (result == -1) {
			return false;
		}
		count -= result;
		do {
			ssize_t writeResult = psx::write(internalSocket, buffer.data(), result);
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
Connection::WriteBaseString(const base::String &str) noexcept {
	std::size_t off = 0;
	std::size_t len = str.length();

	while (len != 0) {
		ssize_t status;
		if (useTransportSecurity) {
			status = ConnectionSecureInternals::Write(this, str.data() + off, len);
		} else {
			status = psx::write(internalSocket, str.data() + off, len);
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



void
Connection::CheckLocalHostv4() noexcept {
	struct sockaddr_in address;
	socklen_t len = sizeof(address);

	if (getpeername(internalSocket, reinterpret_cast<struct sockaddr *>(&address), &len) == 0) {
		isLocalhost = address.sin_addr.s_addr == 0x00000000 ||
					  address.sin_addr.s_addr == 0x7f000001;
	}
}

void
Connection::CheckLocalHostv6() noexcept {
	struct sockaddr_in6 address;
	socklen_t len = sizeof(address);

	if (getpeername(internalSocket, reinterpret_cast<struct sockaddr *>(&address), &len) == 0) {
		[&address, this]() mutable {
			isLocalhost = false;
			for (std::size_t i = 0; i < 15; i++) {
				if (address.sin6_addr.s6_addr[i] != 0x00)
					return;
			}
			if (address.sin6_addr.s6_addr[15] != 0x01)
				return;
			isLocalhost = true;
		}();

		if (!isLocalhost) {
			[&address, this]() mutable {
				isLocalhost = false;
				for (std::size_t i = 0; i < 10; i++) {
					if (address.sin6_addr.s6_addr[i] != 0x00)
						return;
				}
				if (address.sin6_addr.s6_addr[10] != 0xff ||
					address.sin6_addr.s6_addr[11] != 0xff ||
					address.sin6_addr.s6_addr[12] != 0x7f ||
					address.sin6_addr.s6_addr[13] != 0x00 ||
					address.sin6_addr.s6_addr[14] != 0x00 ||
					address.sin6_addr.s6_addr[15] != 0x01) {
					return;
				}
				isLocalhost = true;
			}();
		}
	}
}
