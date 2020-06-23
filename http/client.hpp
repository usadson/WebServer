#pragma once

/**
 * Copyright (C) 2020 Tristan. All Rights Reserved.
 * This file is licensed under the BSD 2-Clause license.
 * See the COPYING file for licensing information.
 */

#include <thread>

#include "connection/connection.hpp"

namespace HTTP {

	// Forward-decl from server.hpp
	class Server;

} // namespace HTTP

namespace HTTP {

enum class ClientError {
	FAILED_READ_METHOD,
	FAILED_READ_PATH,
	FAILED_READ_VERSION,
	FAILED_WRITE_RESPONSE,
	INCORRECT_METHOD,
	INCORRECT_PATH,
	INCORRECT_VERSION,
	NO_ERROR
};

struct Request {
	std::string method;
	std::string path;
	// Version isn't worth/needed storing atm.
};

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
	RecoverError() noexcept;

	void
	ResetExchangeState() noexcept;

	[[nodiscard]] bool
	RunMessageExchange() noexcept;

public:
	std::thread thread;
};

} // namespace HTTP
