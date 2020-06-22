#pragma once

/**
 * Copyright (C) 2020 Tristan. All Rights Reserved.
 * This file is licensed under the BSD 2-Clause license.
 * See the COPYING file for licensing information.
 */

#include <iosfwd>

#include <cstdint>

namespace HTTP {

struct Configuration {

	// The amount of clients awaiting in the accept() queue
	std::size_t listenerBacklog;

	// The amount of time should pass between poll() calls to the main server
	// socket.
	//
	// Time is in milliseconds.
	//
	// This is not really a performance issue, more a preference issue.
	int pollAcceptTimeout{ 1000 };

	uint16_t port;

	bool useTransportSecurity;

};

} // namespace HTTP
