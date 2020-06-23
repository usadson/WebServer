/**
 * Copyright (C) 2020 Tristan. All Rights Reserved.
 * This file is licensed under the BSD 2-Clause license.
 * See the COPYING file for licensing information.
 */

#include "connection/connection.hpp"

#include <unistd.h>

Connection::~Connection() noexcept {
	close(internalSocket);
	internalSocket = -1;
}

bool
Connection::Setup(const HTTP::Configuration &configuration) noexcept {
	if (useTransportSecurity) {
		// TODO Use TLS wrapper
		return false;
	}

	// No setup is needed for non-secure connections
	return true;
}

// TODO Make sure that using char isn't causing problems when it isn't 8-bits.
bool
Connection::ReadChar(char *buf) noexcept {
	if (buf == nullptr) {
		return false;
	}

	if (useTransportSecurity) {
		// TODO Use TLS wrapper
		return false;
	}

	int status = read(internalSocket, buf, 1);
	if (status == -1)
		return false;

	return true;
}

bool
Connection::WriteString(const std::string &str, bool includeNullCharacter) noexcept {
	// TODO put in while loop because write may not write all characters

	int status = write(internalSocket, str.c_str(),
					   str.length() + (includeNullCharacter ? 1 : 0));

	if (status == -1)
		return false;

	return true;
}
