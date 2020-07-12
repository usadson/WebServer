/**
 * Copyright (C) 2020 Tristan. All Rights Reserved.
 * This file is licensed under the BSD 2-Clause license.
 * See the COPYING file for licensing information.
 */

#include "cgi/manager.hpp"

namespace CGI {

bool
Manager::Load() {
	return true;
}

const Script *
Manager::Lookup(const HTTP::Request &) const noexcept {
	return nullptr;
}

} // namespace CGI

