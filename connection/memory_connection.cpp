/**
 * Copyright (C) 2020 Tristan. All Rights Reserved.
 * This file is licensed under the BSD 2-Clause license.
 * See the COPYING file for licensing information.
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
	return true;
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
Connection::WriteString(const std::string &str) noexcept {
	auto *internalData = reinterpret_cast<MemoryUserData *>(userData);

	std::copy(std::cbegin(str), std::cend(str), std::back_inserter(internalData->output));

	return true;
}

bool
Connection::WriteStringView(const std::string_view &str) noexcept {
	auto *internalData = reinterpret_cast<MemoryUserData *>(userData);

	std::copy(std::cbegin(str), std::cend(str), std::back_inserter(internalData->output));

	return true;
}
