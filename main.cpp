/**
 * Copyright (C) 2020 Tristan. All Rights Reserved.
 * This file is licensed under the BSD 2-Clause license.
 * See the COPYING file for licensing information.
 */

#include <iostream>
#include <string>

#include <cstdlib>

#include "base/logger.hpp"
#include "base/media_type.hpp"
#include "cgi/manager.hpp"
#include "http/configuration.hpp"
#include "http/server.hpp"
#include "security/policies.hpp"
#include "security/tls_configuration.hpp"

bool
LoadTLSConfiguration(Security::TLSConfiguration &config);

int
main() {
	CGI::Manager manager{};
	MediaTypeFinder mediaTypeFinder{};
	Security::Policies securityPolicies{};
	Security::TLSConfiguration tlsConfiguration{};

	if (!LoadTLSConfiguration(tlsConfiguration)) {
		Logger::Error("Main", "Failed to load TLS configuration");
		return EXIT_FAILURE;
	}

	HTTP::Configuration httpConfig1(mediaTypeFinder, securityPolicies, tlsConfiguration);
	HTTP::Configuration httpConfig2(mediaTypeFinder, securityPolicies, tlsConfiguration);
	httpConfig1.rootDirectory = "/var/www/html";
	httpConfig1.port = 8080;
	httpConfig2.rootDirectory = "/var/www";
	httpConfig2.port = 8081;
	httpConfig2.useTransportSecurity = true;

	HTTP::Server httpServer1(httpConfig1, manager);
	HTTP::Server httpServer2(httpConfig2, manager);
	httpServer1.Start();
	httpServer2.Start();

	Logger::Log("Main", "Server Started");

	std::string test;
	std::cin >> test;

	Logger::Log("Main", "Stopping...");

	httpServer1.SignalShutdown();
	httpServer2.SignalShutdown();
	httpServer1.Join();
	httpServer2.Join();

	Logger::Log("Main", "Stopped!");

	fclose(stdin);
	fclose(stdout);
	fclose(stderr);

	return EXIT_SUCCESS;
}

bool
LoadTLSConfiguration(Security::TLSConfiguration &config) {
	if (const char *certificateFile = std::getenv("WS_TLS_CERT")) {
		if (const char *privateKeyFile = std::getenv("WS_TLS_PRIVATE_KEY")) {
			config.CertificateFile = certificateFile;
			config.PrivateKeyFile = privateKeyFile;
			return true;
		} else {
			Logger::Error("TLS Configuration", "WS_TLS_PRIVATE_KEY not found in environment");
			return false;
		}
	} else {
		Logger::Error("TLS Configuration", "WS_TLS_CERT not found in environment");
		return false;
	}

	return config.CreateContext() && config.context != nullptr;
}
