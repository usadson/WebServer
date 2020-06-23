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
#include "http/utils.hpp"

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

ClientError
Client::ConsumeMethod() noexcept {
	std::vector<char> buffer;

	// Reserve 4 octets because GET & POST fit in 4 octets, so no reallocation
	// is needed.
	buffer.reserve(4);

	while (true) {
		char character = 0;

		if (!connection->ReadChar(&character)) {
			return ClientError::FAILED_READ_METHOD;
		}

		if (character == ':') {
			return ClientError::NO_ERROR;
		}

		// Character validation
		if (!HTTP::Utils::IsTokenCharacter(character)) {
			return ClientError::INCORRECT_METHOD;
		}

		buffer.push_back(character);
	}

	this->currentRequest.method = std::string(std::begin(buffer), std::end(buffer));
}

} // namespace HTTP
