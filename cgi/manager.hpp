#pragma once

/**
 * Copyright (C) 2020 Tristan. All Rights Reserved.
 * This file is licensed under the BSD 2-Clause license.
 * See the COPYING file for licensing information.
 */

#include <map>

#include "cgi/script.hpp"
#include "http/request.hpp"

namespace CGI {

class Manager {
public:
	[[nodiscard]] bool
	Load();

	// If ptr is nullptr, no CGI script was found.
	[[nodiscard]] const Script *
	Lookup(const HTTP::Request &) const noexcept;

private:
	std::map<std::string, Script> scripts;
};

} // namespace CGI
