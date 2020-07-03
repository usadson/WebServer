#pragma once

/**
 * Copyright (C) 2020 Tristan. All Rights Reserved.
 * This file is licensed under the BSD 2-Clause license.
 * See the COPYING file for licensing information.
 */

#include <map>
#include <string>

namespace HTTP {

struct Request {
	std::string method;
	std::string path;
	// Version isn't worth/needed storing atm.

	std::map<std::string, std::string> headers;
};

} // namespace HTTP
