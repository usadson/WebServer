#pragma once

/**
 * Copyright (C) 2020 Tristan. All Rights Reserved.
 * This file is licensed under the BSD 2-Clause license.
 * See the COPYING file for licensing information.
 */

#include <iosfwd>

namespace Security {

struct Policies {

	// The maximum method length.
	// At this time, the longest registered method is 'UPDATEREDIRECTREF', with
	// a length of 17, therefore the default value is 18 (17 + 0 for the null
	// terminator).
	// 0 means unlimited.
	std::size_t maxMethodLength{ 18 };

	// The amount of requests that may be made in a single connection (session).
	// 0 means unlimited.
	std::size_t maxRequestsPerConnection{ 300 };

	// true: (graceful)
	//   after the `maxRequestsCloseImmediately`'s request has been made, close
	//   the connection.
	// false:
	//   on each the request, check if `maxRequestsCloseImmediately` has been
	//   exceeded, if true send a 'HTTP 429 Too Many Requests' page.
	//   Strings::TooManyRequestsPage
	bool maxRequestsCloseImmediately{ false };

	// The maximum request-target (also known as path) length.
	// 0 means unlimited.
	std::size_t maxRequestTargetLength{ 255 };

};

} // namespace Security
