/**
 * Copyright (C) 2020 Tristan. All Rights Reserved.
 * This file is licensed under the BSD 2-Clause license.
 * See the COPYING file for licensing information.
 */

#include "client.hpp"

#include <algorithm>
#include <array>
#include <iterator>
#include <sstream>
#include <string>
#include <vector>

#include <cstdio>

#include "base/logger.hpp"
#include "base/strings.hpp"
#include "http/configuration.hpp"
#include "http/server.hpp"
#include "http/utils.hpp"
#include "io/file.hpp"

[[nodiscard]] inline bool
StringStartsWith(const std::string string, const std::string prefix) {
#ifdef __cpp_lib_starts_ends_with
	return str.starts_with(str);
#else
	return std::mismatch(std::begin(prefix), std::end(prefix), std::begin(string), std::end(string)).first == std::end(prefix);
#endif
}

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
	std::array<char, 8> buffer{};

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
	if (!connection->Setup(server->config())) {
		Logger::Error(__FUNCTION__, "Failed to setup connection!");
		Clean();
		return;
	}

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

	auto mediaType = server->config().mediaTypeFinder.DetectMediaType(file);

	if (!SendMetadata(Strings::Response::OK, file->Size(), mediaType)) {
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
	static const std::string indexPathTarget("/index.html");

	switch (error) {
		case ClientError::FILE_NOT_FOUND:
			if (StringStartsWith(indexPathTarget, currentRequest.path)) {
				return ServeDefaultPage();
			}
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
	const std::string &body = Strings::NotFoundPage;

	if (!SendMetadata(Strings::Response::NotFound, body.length(), MediaTypes::HTML)) {
		return false;
	}

	return connection->WriteString(body);
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

bool
Client::SendMetadata(const std::string &response, std::size_t contentLength, const MediaType &mediaType) noexcept {
	std::stringstream metadata;
	metadata << response;
	metadata << "\r\nContent-Length: " << contentLength;
	metadata << "\r\nServer: " << server->config().serverProductName;
	metadata << "\r\nContent-Type: " << mediaType.completeType;

	if (mediaType.includeCharset)
		metadata << ";charset=utf-8\r\n\r\n";
	else
		metadata << "\r\n\r\n";

	return connection->WriteString(metadata.str());
}

bool
Client::ServeDefaultPage() noexcept {
	return SendMetadata(Strings::Response::OK, Strings::DefaultWebPage.length(), MediaTypes::HTML) && connection->WriteString(Strings::DefaultWebPage);
}

} // namespace HTTP
