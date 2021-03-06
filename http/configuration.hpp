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
#include "security/tls_configuration.hpp"

namespace HTTP {

struct Configuration {

	inline Configuration(const MediaTypeFinder &mediaTypeFinder, const Security::Policies &policies,
						 const Security::TLSConfiguration &tlsConfiguration)
		: mediaTypeFinder(mediaTypeFinder), securityPolicies(policies),
		  tlsConfiguration(tlsConfiguration) {
	}

	// The hostname (domain name) of the server.
	// If unset, will try to get it from the environment.
	// If not in environment, try to get it from the POSIX gethostname(2) API.
	std::string hostname;

	// The header value of "Strict-Transport-Security".
	// Empty means omit header.
	// Spec: RFC 6797
	std::string hsts{ "max-age=31536000; includeSubDomains; preload" };

	// The amount of clients awaiting in the accept() queue
	std::size_t listenerBacklog { 100 };

	const MediaTypeFinder &mediaTypeFinder;

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

	// The configuration for the TLS implementation.
	const Security::TLSConfiguration &tlsConfiguration;

	// Use when on port 80. Redirect all OK requests to HTTPS.
	bool upgradeToHTTPS{ false };

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
