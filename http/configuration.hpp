#pragma once

/**
 * Copyright (C) 2020 Tristan. All Rights Reserved.
 * This file is licensed under the BSD 2-Clause license.
 * See the COPYING file for licensing information.
 */

#include <iosfwd>

#include <cstdint>

#include <exception>

namespace HTTP {

class ConfigurationException : public std::exception {
public:
	inline ConfigurationException(const char *desc) : description(desc) {
	}

	[[nodiscard]] inline const char *
	what() const noexcept override {
		return description;
	}
private:
	const char *description;
};

struct Configuration {

	// The amount of clients awaiting in the accept() queue
	std::size_t listenerBacklog;

	// The amount of time should pass between poll() calls to the main server
	// socket.
	//
	// Time is in milliseconds.
	//
	// This is not really a performance issue, more a preference issue.
	int pollAcceptTimeout{ 1000 };

	// The TCP port that should be used with the server.
	//
	// Some standard ports are:
	// 80    This port is the default port for non-secure (HTTP) connections.
	// 443   This port is the default port for secure (HTTPS) connections.
	// 8080  This port is regularly used on (local) test servers.
	uint16_t port;

	// Whether or a security layer should be used.
	// The security layer is TLS.
	bool useTransportSecurity;

};

} // namespace HTTP
