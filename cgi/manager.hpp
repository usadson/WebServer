#pragma once

/**
 * Copyright (C) 2020 Tristan. All Rights Reserved.
 * This file is licensed under the BSD 2-Clause license.
 * See the COPYING file for licensing information.
 */

#include <optional>

#include "cgi/script.hpp"
#include "http/request.hpp"

namespace CGI {

class Manager {
public:
	[[nodiscard]] bool
	Load();

	[[nodiscard]] std::optional<const Script &>
	Lookup(const HTTP::Request &) const noexcept;
};

} // namespace CGI
