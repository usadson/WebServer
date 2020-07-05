#pragma once

/**
 * Copyright (C) 2020 Tristan. All Rights Reserved.
 * This file is licensed under the BSD 2-Clause license.
 * See the COPYING file for licensing information.
 */

#include <exception> // IWYU pragma: keep
#include <iosfwd>
#include <string>

#include <cstdint>

#include "base/media_type.hpp"
#include "security/policies.hpp"

namespace HTTP {

struct Configuration {

	inline Configuration(const Security::Policies &policies)
		: securityPolicies(policies) {
	}

	// The amount of clients awaiting in the accept() queue
	std::size_t listenerBacklog { 100 };

	MediaTypeFinder mediaTypeFinder;

	// The amount of time should pass between poll() calls to the main server
	// socket.
	//
	// Time is in milliseconds.
	//
	// This is not really a performance issue, more a preference issue.
	int pollAcceptTimeout { 1000 };

	// The TCP port that should be used with the server.
	//
	// Some standard ports are:
	// 80    This port is the default port for non-secure (HTTP) connections.
	// 443   This port is the default port for secure (HTTPS) connections.
	// 8080  This port is regularly used on (local) test servers.
	uint16_t port { 8080 };

	std::string rootDirectory;

	const Security::Policies &securityPolicies;

	// The 'Server' header field value as defined per RFC 7231 § 7.4.2
	std::string serverProductName { "Wizard" };

	// Whether or a security layer should be used.
	// The security layer is TLS.
	bool useTransportSecurity { false };
};

class ConfigurationException : public std::exception {
public:
	explicit inline ConfigurationException(const char *desc) : description(desc) {
	}

	[[nodiscard]] inline const char *
	what() const noexcept override {
		return description;
	}

private:
	const char *description;
};

} // namespace HTTP
