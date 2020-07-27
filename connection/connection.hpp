#pragma once

/**
 * Copyright (C) 2020 Tristan. All Rights Reserved.
 * This file is licensed under the BSD 2-Clause license.
 * See the COPYING file for licensing information.
 */

#include <iosfwd>

#include <string>

#include "base/string.hpp"

// ForwardDecl from http/configuration.hpp
namespace HTTP {
struct Configuration;
} // namespace HTTP

class Connection {
public:
#ifndef CONNECTION_MEMORY_VARIANT
	inline Connection(int socket, bool useTransportSecurity, void *userData=nullptr) noexcept
		: userData(userData), internalSocket(socket), useTransportSecurity(useTransportSecurity) {
	}
#else
	inline explicit Connection(void *userData=nullptr) noexcept
		: userData(userData) {
	}
#endif /* CONNECTION_MEMORY_VARIANT */

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
	ReadChar(char *) const noexcept;

	[[nodiscard]] bool
	SendFile(int fd, std::size_t count) noexcept;

	// Will setup the connection. If the configuration specifies the use of TLS,
	// it will setup this as well.
	//
	// Won't/can't modify the [configuration] parameter.
	//
	// Returns success status
	[[nodiscard]] bool
	Setup(const HTTP::Configuration &) noexcept;

	// Writes the contents of the string. Doens't include the null termination
	// character.
	//
	// Won't/can't modify the [string] parameter.
	//
	// Returns success status
	[[nodiscard]] bool
	WriteString(const std::string &) noexcept;

	// Writes the contents of the string. Doens't include the null termination
	// character.
	//
	// Won't/can't modify the [string] parameter.
	//
	// Returns success status
	[[nodiscard]] bool
	WriteBaseString(const base::String &) noexcept;

	// Is used by memory_connection.hpp but isn't used in the normal
	// implementation.
	void *userData;

	// Will be modified by Setup and its descendants if TLS is enabled.
	void *securityContext{ nullptr };

#ifndef CONNECTION_MEMORY_VARIANT
#ifndef CONNECTION_ALLOW_EXTENDED_VISIBILITY
private:
#endif
	bool hasWriteFailed{ false };
	const int internalSocket;
	const bool useTransportSecurity;
#endif /* CONNECTION_MEMORY_VARIANT */
};
