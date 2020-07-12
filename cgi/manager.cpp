/**
 * Copyright (C) 2020 Tristan. All Rights Reserved.
 * This file is licensed under the BSD 2-Clause license.
 * See the COPYING file for licensing information.
 */

#include "script.hpp"

namespace CGI {

bool
Manager::Load() {
	return true;
}

std::optional<const Script &>
Lookup(const HTTP::Request &request) const noexcept {
	return std::nullopt;
}

} // namespace CGI

