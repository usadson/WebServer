/**
 * Copyright (C) 2020 Tristan. All Rights Reserved.
 * This file is licensed under the BSD 2-Clause license.
 * See the COPYING file for licensing information.
 */

#include "client.hpp"

#include <string>

#include <cstring>
#include <unistd.h>

#include "base/logger.hpp"
#include "http/server.hpp"

namespace HTTP {

Client::Client(Server *server, int sock) noexcept :
	connection(std::make_unique<Connection>(sock, server->config().useTransportSecurity)),
	server(server), thread(&Client::Entrypoint, this) {
}

void
Client::Clean() noexcept {
	connection = nullptr;

	server->SignalClientDeath(thread);
}

void
Client::Entrypoint() {
	static const std::string &str =
		"HTTP/1.1 200 OK\r\nContent-Length: 6\r\n\r\nHello!";
	if (!connection->WriteString(str))
		Logger::Warning("Client::Entrypoint", "Failed to send response!");

	Clean();
}

void
Client::ReadMethod() noexcept {
	std::vector<char> buffer;
	buffer.reserve(4);

	do {
		char dest;
	} while (1);
}

} // namespace HTTP
