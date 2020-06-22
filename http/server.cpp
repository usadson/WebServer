/**
 * Copyright (C) 2020 Tristan. All Rights Reserved.
 * This file is licensed under the BSD 2-Clause license.
 * See the COPYING file for licensing information.
 */

#include "server.hpp"

#include <initializer_list>
#include <iostream>

#include <cerrno>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include "http/configuration.hpp"
#include "http/server_launch_error.hpp"

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

void
Server::CloseSocket() noexcept {
	if (internalSocket == -1) {
		return;
	}

	close(internalSocket);
	internalSocket = -1;
}

ServerLaunchError
Server::ConfigureSocketBind() noexcept {
	struct sockaddr_in address;
	address.sin_family = AF_INET;
	address.sin_port = htons(configuration.port);
	address.sin_addr.s_addr = htonl(INADDR_ANY);

	if (bind(internalSocket, reinterpret_cast<struct sockaddr *>(&address), sizeof(address)) == -1) {
		switch (errno) {
			case EADDRINUSE:
				return ServerLaunchError::SOCKET_BIND_PORT_IN_USE;
			case EACCES:
				return ServerLaunchError::SOCKET_BIND_PERMISSIONS;
			default:
				// Only uncommon errors should be associated with this
				// ServerLaunchError.
				return ServerLaunchError::SOCKET_BIND_UNKNOWN;
		}
	}

	return ServerLaunchError::NO_ERROR;
}

ServerLaunchError
Server::ConfigureSocketListen() noexcept {
	if (listen(internalSocket, configuration.listenerBacklog) == -1) {
		return ServerLaunchError::SOCKET_LISTEN;
	}

	return ServerLaunchError::NO_ERROR;
}

ServerLaunchError
Server::ConfigureSocketSetReusable() noexcept {
	int flag = 1;

	if (setsockopt(internalSocket, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(int)) == -1) {
		return ServerLaunchError::SOCKET_REUSABLE;
	}

	return ServerLaunchError::NO_ERROR;
}

bool
Server::CreateServer() noexcept {
	for (auto function : { &Server::CreateSocket, &Server::ConfigureSocketSetReusable, &Server::ConfigureSocketBind, &Server::ConfigureSocketListen }) {
		ServerLaunchError error = (this->*function)();

		if (error != ServerLaunchError::NO_ERROR) {
			std::cerr << "Failed to create server!\nError: " << error << '\n';
			return false;
		}
	}

	return true;
}

ServerLaunchError
Server::CreateSocket() noexcept {
	internalSocket = socket(AF_INET, SOCK_STREAM, 0);

	if (internalSocket == -1) {
		return ServerLaunchError::SOCKET_CREATION;
	}

	cleanFunctions.push_back(&Server::CloseSocket);

	return ServerLaunchError::NO_ERROR;
}

} // namespace HTTP
