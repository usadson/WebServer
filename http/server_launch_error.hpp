#pragma once

/**
 * Copyright (C) 2020 Tristan. All Rights Reserved.
 * This file is licensed under the BSD 2-Clause license.
 * See the COPYING file for licensing information.
 */

#include <ostream>

namespace HTTP {

enum ServerLaunchError {
	NO_ERROR,
	SOCKET_BIND_UNKNOWN,
	SOCKET_BIND_PORT_IN_USE,
	SOCKET_BIND_PERMISSIONS,
	SOCKET_CREATION,
	SOCKET_REUSABLE,
};

} // namespace HTTP

std::ostream &
operator<<(std::ostream &, HTTP::ServerLaunchError);
