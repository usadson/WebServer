#pragma once

/**
 * Copyright (C) 2020 Tristan. All Rights Reserved.
 * This file is licensed under the BSD 2-Clause license.
 * See the COPYING file for licensing information.
 */
namespace HTTP {

struct Configuration {

	// The amount of clients awaiting in the accept() queue
	std::size_t listenerBacklog;

	uint16_t port;

	bool useTransportSecurity;

};

} // namespace HTTP
