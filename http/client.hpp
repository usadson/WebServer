#pragma once

/**
 * Copyright (C) 2020 Tristan. All Rights Reserved.
 * This file is licensed under the BSD 2-Clause license.
 * See the COPYING file for licensing information.
 */

#include <memory>
#include <thread>
#include <vector>

#include "connection/connection.hpp"
#include "http/client_error.hpp"
#include "http/configuration.hpp"
#include "http/request.hpp"

namespace HTTP {

	// Forward-decl from server.hpp
	class Server;

} // namespace HTTP

namespace HTTP {

class Client {
public:
	Client(Server *server, int socket) noexcept;

private:
	std::unique_ptr<Connection> connection;
	Request currentRequest;

	// This is turned on after checking the headers.
	// NOTE that this isn't a configuration option, but a state.
	bool persistentConnection{ false };

	Server *server;

	void
	Clean() noexcept;

	[[nodiscard]] ClientError
	ConsumeCRLF() noexcept;

	[[nodiscard]] ClientError
	ConsumeHeaderField(char) noexcept;

	[[nodiscard]] ClientError
	ConsumeHeaderFieldName(std::vector<char> *) noexcept;

	[[nodiscard]] ClientError
	ConsumeHeaderFieldValue(std::vector<char> *) noexcept;

	[[nodiscard]] ClientError
	ConsumeHeaders() noexcept;

	[[nodiscard]] ClientError
	ConsumeSingleSpace() noexcept;

	[[nodiscard]] ClientError
	ConsumeMethod() noexcept;

	[[nodiscard]] ClientError
	ConsumePath() noexcept;

	[[nodiscard]] ClientError
	ConsumeVersion() noexcept;

	void
	Entrypoint();

	[[nodiscard]] ClientError
	HandleRequest() noexcept;

	[[nodiscard]] bool
	RecoverError(ClientError) noexcept;

	[[nodiscard]] bool
	RecoverErrorFileNotFound() noexcept;

	void
	ResetExchangeState() noexcept;

	[[nodiscard]] bool
	RunMessageExchange() noexcept;

	[[nodiscard]] bool
	SendMetadata(const std::string &response, std::size_t contentLength, const MediaType &type) noexcept;

	[[nodiscard]] bool
	ServeDefaultPage() noexcept;

public:
	std::thread thread;
};

} // namespace HTTP
