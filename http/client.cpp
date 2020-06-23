/**
 * Copyright (C) 2020 Tristan. All Rights Reserved.
 * This file is licensed under the BSD 2-Clause license.
 * See the COPYING file for licensing information.
 */

#include "client.hpp"

#include <sstream>
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

		if (character == ' ') {
			this->currentRequest.method = std::string(std::begin(buffer), std::end(buffer));
			return ClientError::NO_ERROR;
		}

		// Character validation
		if (!HTTP::Utils::IsTokenCharacter(character)) {
			std::cout << "Invalid Character: " << character << '\n';
			return ClientError::INCORRECT_METHOD;
		}

		buffer.push_back(character);
	}
}

ClientError
Client::ConsumePath() noexcept {
	std::vector<char> buffer;

	while (true) {
		char character = 0;

		if (!connection->ReadChar(&character)) {
			return ClientError::FAILED_READ_PATH;
		}

		if (character == ' ') {
			this->currentRequest.path = std::string(std::begin(buffer), std::end(buffer));
			return ClientError::NO_ERROR;
		}

		// Character validation
		if (!HTTP::Utils::IsPathCharacter(character)) {
			std::cout << "Invalid Character: " << character << '\n';
			return ClientError::INCORRECT_PATH;
		}

		buffer.push_back(character);
	}
}

void
Client::Entrypoint() {
	if (ConsumeMethod() != ClientError::NO_ERROR) {
		Logger::Warning("Client::Entrypoint", "Failed to read method!");
		return;
	}

	if (ConsumePath() != ClientError::NO_ERROR) {
		Logger::Warning("Client::Entrypoint", "Failed to read path!");
		return;
	}

	const std::string prefix = "This was your method: \"";
	const std::string infix = "\"\nThis was your path: \"";
	const std::string suffix = "\"";
	std::stringstream response;
	response << "HTTP/1.1 200 OK\r\nContent-Length: ";
	response << prefix.length() + this->currentRequest.method.length() + infix.length() + this->currentRequest.path.length() + suffix.length();
	response << "\r\n\r\n";
	response << prefix;
	response << this->currentRequest.method;
	response << infix;
	response << this->currentRequest.path;
	response << suffix;

	if (!connection->WriteString(response.str())) {
		Logger::Warning("Client::Entrypoint", "Failed to send response!");
	}

	Clean();
}

} // namespace HTTP
