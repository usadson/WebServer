#pragma once

/**
 * Copyright (C) 2020 Tristan. All Rights Reserved.
 * This file is licensed under the BSD 2-Clause license.
 * See the COPYING file for licensing information.
 */

#include <iosfwd>

namespace Security {

struct Policies {

	// The amount of requests that may be made in a single connection (session).
	// 0 means unlimited.
	std::size_t maxRequestsPerConnection{ 10 };

};

} // namespace Security
