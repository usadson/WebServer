/**
 * Copyright (C) 2020 Tristan. All Rights Reserved.
 * This file is licensed under the BSD 2-Clause license.
 * See the COPYING file for licensing information.
 */

#include <iostream>
#include <string>

#include <cstdlib>

#include "base/logger.hpp"
#include "cgi/manager.hpp"
#include "http/configuration.hpp"
#include "http/server.hpp"
#include "security/policies.hpp"

int
main() {
	CGI::Manager manager{};
	Security::Policies securityPolicies{};

	HTTP::Configuration config(securityPolicies);
	config.rootDirectory = "/var/www/html";

	HTTP::Server server(config, manager);
	server.Start();

	Logger::Log("Main", "Server Started");

	std::string test;
	std::cin >> test;

	Logger::Log("Main", "Stopping...");

	server.SignalShutdown();
	server.Join();

	Logger::Log("Main", "Stopped!");
	return EXIT_SUCCESS;
}
