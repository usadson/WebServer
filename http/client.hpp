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
	INCORRECT_METHOD,
	INCORRECT_PATH,
	NO_ERROR
};

struct Request {
	std::string method;
	std::string path;
	std::string version;
};

class Client {
public:
	Client(Server *server, int socket) noexcept;

private:
	std::unique_ptr<Connection> connection;
	Server *server;
	Request currentRequest;

	void
	Clean() noexcept;

	[[nodiscard]] ClientError
	ConsumeMethod() noexcept;

	[[nodiscard]] ClientError
	ConsumePath() noexcept;

	void
	Entrypoint();

public:
	std::thread thread;
};

} // namespace HTTP
