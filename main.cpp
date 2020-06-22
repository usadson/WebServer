/**
 * Copyright (C) 2020 Tristan. All Rights Reserved.
 * This file is licensed under the BSD 2-Clause license.
 * See the COPYING file for licensing information.
 */

#include <iostream>

#include <cstdlib>

#include "http/configuration.hpp"
#include "http/server.hpp"

int
main() {
	HTTP::Configuration config;
	// Change config before passing it to the HTTP::Server

	HTTP::Server server(config);
	server.Start();

	std::string test;
	std::cin >> test;

	server.SignalShutdown();
	server.Join();

	return EXIT_SUCCESS;
}
