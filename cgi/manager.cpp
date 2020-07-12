/**
 * Copyright (C) 2020 Tristan. All Rights Reserved.
 * This file is licensed under the BSD 2-Clause license.
 * See the COPYING file for licensing information.
 */

#include "cgi/manager.hpp"

namespace CGI {

bool
Manager::Load() {
	scripts.insert({ std::string("/cgi"), { "/opt/test.sh", "Test CGI Script" } });
	return true;
}

const Script *
Manager::Lookup(const HTTP::Request &request) const noexcept {
	auto iterator = scripts.find(request.path);

	if (iterator == std::end(scripts)) {
		return nullptr;
	}

	return &iterator->second;
}

} // namespace CGI

