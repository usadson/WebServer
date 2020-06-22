#pragma once

/**
 * Copyright (C) 2020 Tristan. All Rights Reserved.
 * This file is licensed under the BSD 2-Clause license.
 * See the COPYING file for licensing information.
 */

#include <functional>
#include <memory>
#include <mutex>
#include <thread>
#include <vector>

namespace HTTP {

	// Forward-decl for client.hpp
	class Server;

} // namespace HTTP

#include "http/client.hpp"
#include "http/configuration.hpp"
#include "http/server_launch_error.hpp"

namespace HTTP {

class Server {
public:
	inline explicit Server(const Configuration &configuration) :
		configuration(configuration) {
		CheckConfiguration();
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

	void
	SignalClientDeath(std::reference_wrapper<std::thread>) noexcept;

	inline void
	SignalShutdown() noexcept {
		shutdownSignaled = true;
	}

private:
	Configuration configuration;
	std::unique_ptr<std::thread> internalThread{ nullptr };
	int internalSocket{ -1 };

	std::vector<std::function<void(Server *)>> cleanFunctions;

	std::mutex clientsMutex;
	std::vector<std::unique_ptr<Client>> clients;

	bool shutdownSignaled{ false };

	void
	AcceptClient();

	void
	CheckConfiguration() const;

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
	HandlePollFailure();

	void
	InternalStart();

};

} // namespace HTTP
