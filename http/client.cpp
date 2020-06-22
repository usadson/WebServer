/**
 * Copyright (C) 2020 Tristan. All Rights Reserved.
 * This file is licensed under the BSD 2-Clause license.
 * See the COPYING file for licensing information.
 */

#include "client.hpp"

#include <cstring>
#include <unistd.h>

namespace HTTP {

Client::Client(int sock) noexcept : internalSocket(sock), thread(&Client::Entrypoint, this) {
}

void
Client::Clean() noexcept {
	close(internalSocket);
	internalSocket = -1;
}

void
Client::Entrypoint() {
	const char *text = "HTTP/1.1 200 OK\r\nContent-Length: 6\r\n\r\nHello!";
	write(internalSocket, text, strlen(text));

	Clean();
}

} // namespace HTTP
