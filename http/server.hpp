#pragma once

/**
 * Copyright (C) 2020 Tristan. All Rights Reserved.
 * This file is licensed under the BSD 2-Clause license.
 * See the COPYING file for licensing information.
 */

#include <functional>
#include <optional>
#include <thread>

#include "configuration.hpp"
#include "server_launch_error.hpp"

namespace HTTP {

class Server {
public:
	inline Server(const Configuration &configuration) noexcept :
		configuration(configuration) {
	}

	~Server() noexcept;

	// Will create a thread for this server to run in, and starts the thread.
	void
	Start();

	inline void
	Join() const {
		if (internalThread == nullptr)
			return;
		return internalThread->join();
	}

private:
	Configuration configuration;
	std::unique_ptr<std::thread> internalThread{ nullptr };
	int internalSocket{ -1 };

	const std::array<std::function<ServerLaunchError(Server *)>, 4> functions = {
		&Server::CreateSocket,
		&Server::ConfigureSocketSetReusable,
		&Server::ConfigureSocketBind,
		&Server::ConfigureSocketListen,
	};

	std::vector<std::function<void(Server *)>> cleanFunctions;

	void
	CloseSocket() noexcept;

	[[nodiscard]] ServerLaunchError
	ConfigureSocketBind() noexcept;

	[[nodiscard]] ServerLaunchError
	ConfigureSocketListen() noexcept;

	[[nodiscard]] ServerLaunchError
	ConfigureSocketSetReusable() noexcept;

	[[nodiscard]] bool
	CreateServer() noexcept;

	[[nodiscard]] ServerLaunchError
	CreateSocket() noexcept;

	void
	InternalStart();
};

} // namespace HTTP
