/**
 * Copyright (C) 2020 Tristan. All Rights Reserved.
 * This file is licensed under the BSD 2-Clause license.
 * See the COPYING file for licensing information.
 */

#include "client.hpp"

#include <array>
#include <iterator>
#include <sstream>
#include <string>
#include <vector>

#include <cstdio>

#include "base/logger.hpp"
#include "http/configuration.hpp"
#include "http/server.hpp"
#include "http/utils.hpp"
#include "io/file.hpp"

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
		if (!Utils::IsTokenCharacter(character)) {
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
		if (!Utils::IsPathCharacter(character)) {
			return ClientError::INCORRECT_PATH;
		}

		buffer.push_back(character);
	}
}

ClientError
Client::ConsumeVersion() noexcept {
	std::array<char, 8> buffer;

	std::array<char, 7> expectedChars = {
		'H', 'T', 'T', 'P', '/', '1', '.'
	};

	for (std::size_t i = 0; i < 8; i++) {
		if (!connection->ReadChar(&buffer[i])) {
			return ClientError::FAILED_READ_VERSION;
		}

		if (i == 7 ? !Utils::IsNumericCharacter(buffer[i]) : (buffer[i] != expectedChars[i])) {
			return ClientError::INCORRECT_VERSION;
		}
	}

	// Not storing the version atm.

	return ClientError::NO_ERROR;
}

void
Client::Entrypoint() {
	bool previousRequestSuccess;

	do {
		previousRequestSuccess = RunMessageExchange();
	} while (previousRequestSuccess && persistentConnection);

	Clean();
}

ClientError
Client::HandleRequest() noexcept {
	auto file = server->fileResolver.Resolve(currentRequest);

	if (!file) {
		return ClientError::FILE_NOT_FOUND;
	}

	std::stringstream response;
	response << "HTTP/1.1 200 OK\r\nContent-Length: ";
	response << file->Size();
	response << "\r\n\r\n";

	if (!connection->WriteString(response.str())) {
		return ClientError::FAILED_WRITE_RESPONSE_METADATA;
	}

	if (!connection->SendFile(file->Handle(), file->Size())) {
		perror("HandleRequest");
		return ClientError::FAILED_WRITE_RESPONSE_BODY;
	}

	return ClientError::NO_ERROR;
}

bool
Client::RecoverError(ClientError error) noexcept {
	switch (error) {
		case ClientError::FILE_NOT_FOUND:
			return RecoverErrorFileNotFound();
		default:
			break;
	}

	std::stringstream test;
	test << "Error Occurred: " << error << '\n';
	Logger::Info("HTTPClient::RecoverError", test.str());
	return false;
}

bool
Client::RecoverErrorFileNotFound() noexcept {
	const std::string body = "<!doctype html>"
							 "<html>"
							 "<head><title>File Not Found</title></head>"
							 "<body><h1>File Not Found</h1></body>"
							 "</html>";

	std::stringstream metadata;
	metadata << "HTTP/1.1 404 Not Found\r\n"
				"Content-Length: " << body.length() << "\r\n"
				"Content-Type: text/html;charset=utf-8\r\n"
				"\r\n";

	if (!connection->WriteString(metadata.str())) {
		return false;
	}

	return connection->WriteString(body.c_str());
}

void
Client::ResetExchangeState() noexcept {
	this->currentRequest = Request();
}

bool
Client::RunMessageExchange() noexcept {
	auto error = ConsumeMethod();
	if (error != ClientError::NO_ERROR) {
		return RecoverError(error);
	}

	error = ConsumePath();
	if (error != ClientError::NO_ERROR) {
		return RecoverError(error);
	}

	error = ConsumeVersion();
	if (error != ClientError::NO_ERROR) {
		return RecoverError(error);
	}

	error = HandleRequest();
	if (error != ClientError::NO_ERROR) {
		return RecoverError(error);
	}

	return true;
}

} // namespace HTTP
