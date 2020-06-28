/**
 * Copyright (C) 2020 Tristan. All Rights Reserved.
 * This file is licensed under the BSD 2-Clause license.
 * See the COPYING file for licensing information.
 */

#include "connection/connection.hpp"

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

#include "base/logger.hpp"

Connection::~Connection() noexcept {
	// This makes sure all data has been transferred before closing the
	// connection. Clever solutions like poll(2), read(?, ' ', 1), etc. didn't
	// work. Maybe non-blocking sockets solve it[?]
	struct timespec sleepTime { 0, 100000 };
	while (true) {
		int value;
		if (ioctl(internalSocket, TIOCOUTQ, &value) == -1 || value == 0)
			break;
		nanosleep(&sleepTime, nullptr);
	}

	/* ignore-return-value */ shutdown(internalSocket, SHUT_RDWR);
	/* ignore-return-value */ close(internalSocket);
	internalSocket = -1;
}

bool
Connection::Setup(const HTTP::Configuration & /* configuration */) noexcept {
	int i = 1;
	if (setsockopt(internalSocket, IPPROTO_TCP, TCP_NODELAY, static_cast<void *>(&i), sizeof(i)) == -1) {
		return false;
	}

	if (useTransportSecurity) {
		// TODO Use TLS wrapper
		return false;
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
		// TODO Use TLS wrapper
		return false;
	}

	return read(internalSocket, buf, 1) != -1;
}

bool
Connection::SendFile(int fd, std::size_t count) const noexcept {
	if (useTransportSecurity) {
		// TODO Use TLS wrapper
		return false;
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
			return false;
		}
		count -= status;
	}

	return true;
#else
	std::array<char, 4096> buffer {};
	do {
		ssize_t result = read(fd, buffer.data(), 4096);
		if (result == -1)
			return false;
		count -= result;
		do {
			ssize_t writeResult = write(internalSocket, buffer.data(), result);
			if (writeResult == -1)
				return false;
			result -= writeResult;
		} while (result != 0);
	} while (count != 0);
#endif
}

bool
Connection::WriteString(const std::string &str, bool includeNullCharacter) const noexcept {
	std::size_t off = 0;
	std::size_t len = str.length() + (includeNullCharacter ? 1 : 0);
	while (len != 0) {
		int status = write(internalSocket, str.c_str() + off, len);

		if (status == -1)
			return false;
		len -= status;
		off += status;
	}
	return true;
}
