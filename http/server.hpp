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

	const std::array<std::function<HTTP::ServerLaunchError(Server *)>, 2> functions = {
		&Server::CreateSocket,
		&Server::ConfigureSocket,
	};

	void
	CloseSocket() noexcept;

	[[nodiscard]] bool
	CreateServer() noexcept;

	[[nodiscard]] ServerLaunchError
	CreateSocket() noexcept;

	[[nodiscard]] ServerLaunchError
	ConfigureSocket() noexcept;

	void
	InternalStart();
};

} // namespace HTTP
