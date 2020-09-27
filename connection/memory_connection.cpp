/**
 * Copyright (C) 2020 Tristan. All Rights Reserved.
 * This file is licensed under the BSD 2-Clause license.
 * See the COPYING file for licensing information.
 */

/**
 * A "memory" connection is the implementation of the Connection class as
 * defined by 'connection.hpp'. Instead of using POSIX sockets like the normal
 * implementation, this file uses vectors, hence the "memory" name.
 *
 * This is useful for tests, since they can simply use this implementation BLOB.
 * Otherwise they should create a temporary server which is a big hassle.
 *
 * The data of this class is defined in memory_userdata.hpp, upcasted to void *
 * and stored in "Connection::userData".
 */

#define CONNECTION_MEMORY_VARIANT

#include "connection/connection.hpp"

#include <vector>

#include <unistd.h>

#include "connection/memory_userdata.hpp"

Connection::~Connection() noexcept {
}

bool
Connection::Setup(const HTTP::Configuration & /* configuration */) noexcept {
	isLocalhost = true;
	return true;
}

void
Connection::CheckLocalHostv4() noexcept {
}

void
Connection::CheckLocalHostv6() noexcept {
}

bool
Connection::ReadChar(char *buf) const noexcept {
	auto *internalData = reinterpret_cast<MemoryUserData *>(userData);

	if (internalData->input.empty()) {
		return false;
	}

	*buf = internalData->input.back();
	internalData->input.pop_back();
	return true;
}

bool
Connection::SendFile(int fd, std::size_t count) noexcept {
	auto *internalData = reinterpret_cast<MemoryUserData *>(userData);

	if (!internalData->writeSendFile) {
		return true;
	}

	auto offset = internalData->output.size();
	internalData->output.reserve(offset + count);

	do {
		ssize_t result = read(fd, internalData->output.data() + offset, count);

		if (result == -1) {
			return false;
		}

		offset += result;
		count -= result;
	} while (count != 0);

	return true;
}

bool
Connection::WriteBaseString(const base::String &str) noexcept {
	auto *internalData = reinterpret_cast<MemoryUserData *>(userData);

	std::copy(std::cbegin(str), std::cend(str), std::back_inserter(internalData->output));

	return true;
}
