#pragma once

/**
 * Copyright (C) 2020 Tristan. All Rights Reserved.
 * This file is licensed under the BSD 2-Clause license.
 * See the COPYING file for licensing information.
 */

#include <thread>

namespace HTTP {

	// Forward-decl for server.hpp
	class Client;

} // namespace HTTP

#include "server.hpp"

namespace HTTP {

class Client {
public:
	Client(Server *server, int socket) noexcept;

private:
	int internalSocket;
	Server *server;

	void
	Clean() noexcept;

	void
	Entrypoint();

public:
	std::thread thread;
};

} // namespace HTTP
