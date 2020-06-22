#pragma once

/**
 * Copyright (C) 2020 Tristan. All Rights Reserved.
 * This file is licensed under the BSD 2-Clause license.
 * See the COPYING file for licensing information.
 */

#include <thread>

namespace HTTP {

class Client {
public:
	explicit Client(int socket) noexcept;

private:
	int internalSocket;
	std::thread thread;

	void
	Clean() noexcept;

	void
	Entrypoint();
};

} // namespace HTTP
