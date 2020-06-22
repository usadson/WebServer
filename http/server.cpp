/**
 * Copyright (C) 2020 Tristan. All Rights Reserved.
 * This file is licensed under the BSD 2-Clause license.
 * See the COPYING file for licensing information.
 */

#include "server.hpp"

#include <array>
#include <iostream>

#include <sys/socket.h>
#include <unistd.h>

namespace HTTP {

void
Server::Start() {
	internalThread = std::make_unique<std::thread>(&Server::InternalStart, this);
}

void
Server::InternalStart() {
	if (!CreateServer()) {
		std::cerr << "[HTTPServer] Failed to start!\n";
		return;
	}
}

Server::~Server() noexcept {
	// TODO Stop thread
	for (const auto &cleanFunction : cleanFunctions) {
		cleanFunction(this);
	}
}

bool
Server::CreateServer() noexcept {
	for (const auto &function : functions) {
		ServerLaunchError error = function(this);

		if (error != ServerLaunchError::NO_ERROR) {
			std::cerr << "Failed to start: " << error << '\n';
			return false;
		}
	}

	return true;
}

void
Server::CloseSocket() noexcept {
	if (internalSocket == -1)
		return;

	close(internalSocket);
	internalSocket = -1;
}

ServerLaunchError
Server::CreateSocket() noexcept {
	internalSocket = socket(AF_INET, SOCK_STREAM, 0);

	if (internalSocket == -1)
		return ServerLaunchError::SOCKET_CREATION;

	cleanFunctions.push_back(&Server::CloseSocket);

	return ServerLaunchError::NO_ERROR;
}

ServerLaunchError
Server::ConfigureSocketSetReusable() noexcept {
	int flag = 1;

	if (setsockopt(internalSocket, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(int)) == -1)
		return ServerLaunchError::SOCKET_REUSABLE;

	return ServerLaunchError::NO_ERROR;
}

ServerLaunchError
Server::ConfigureSocketBind() noexcept {
	return ServerLaunchError::NO_ERROR;
}

} // namespace HTTP
