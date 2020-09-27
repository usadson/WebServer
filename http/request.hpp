#pragma once

/**
 * Copyright (C) 2020 Tristan. All Rights Reserved.
 * This file is licensed under the BSD 2-Clause license.
 * See the COPYING file for licensing information.
 */

#include <map>
#include <string>
#include <vector>

namespace HTTP {

struct Request {
	std::vector<char> method;

	std::string path;
	std::string query;

	std::uint8_t versionMinor;

	// Version isn't worth/needed storing atm.

	std::multimap<std::string, std::string> headers;

	// Is method head
	[[nodiscard]] inline bool
	IsHead() {
		const char head[] = "HEAD";
		return std::mismatch(std::begin(method), std::end(method),
							 std::begin(head), std::begin(head) + 4)
				.first == std::end(method);
	}
};

} // namespace HTTP
