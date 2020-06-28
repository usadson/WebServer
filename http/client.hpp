#pragma once

/**
 * Copyright (C) 2020 Tristan. All Rights Reserved.
 * This file is licensed under the BSD 2-Clause license.
 * See the COPYING file for licensing information.
 */

#include <memory>
#include <thread>

#include "connection/connection.hpp"
#include "http/client_error.hpp"
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
	ServeDefaultPage() noexcept;

public:
	std::thread thread;
};

} // namespace HTTP
