/**
 * Copyright (C) 2020 Tristan. All Rights Reserved.
 * This file is licensed under the BSD 2-Clause license.
 * See the COPYING file for licensing information.
 */

#include <cstdlib>

#include "http/server.hpp"

int
main() {
	HTTP::Configuration config;
	config.port = 8080;
	config.useTransportSecurity = false;

	HTTP::Server server(config);
	server.Start();

	server.Join();

	return EXIT_SUCCESS;
}
