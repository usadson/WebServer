#pragma once

/**
 * Copyright (C) 2020 Tristan. All Rights Reserved.
 * This file is licensed under the BSD 2-Clause license.
 * See the COPYING file for licensing information.
 */

#include <iosfwd>

#include <string>

// ForwardDecl from http/configuration.hpp
namespace HTTP {
	struct Configuration;
} // namespace HTTP

class Connection {
public:
	inline Connection(int socket, bool useTransportSecurity) noexcept
		: internalSocket(socket), useTransportSecurity(useTransportSecurity) {
	}

	~Connection() noexcept;

	// Will read a single character from the source, either directly from the
	// socket or from the TLS wrapper.
	//
	// This function, being part of the network stack, can fail. There can be
	// many reasons for failure, but in general it means that the connection
	// has either been closed, or a catastrophic failure has occurred.
	//
	// The [bool] return value specifies whether or not the read was succesful.
	// The [char *] is an output field for the character.
	//
	// NOTE: If the [bool] return value is false, the value of [char *] is
	// undefined.
	// NOTE: If [char *] is nullptr, false is always returned.
	[[nodiscard]] bool
	ReadChar(char *) noexcept;

	[[nodiscard]] bool
	SendFile(int fd, std::size_t count) noexcept;

	[[nodiscard]] bool
	Setup(const HTTP::Configuration &) noexcept;

	[[nodiscard]] bool
	WriteString(const std::string &, bool includeNullCharacter = false) noexcept;

private:
	void *internalData;
	int internalSocket;
	bool useTransportSecurity;
};
