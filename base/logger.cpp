/**
 * Copyright (C) 2020 Tristan. All Rights Reserved.
 * This file is licensed under the BSD 2-Clause license.
 * See the COPYING file for licensing information.
 */

#include "logger.hpp"

namespace LoggerInternals {

	void
	Perform(const LogLevel &level, const std::string &source, const std::string &message) {
		std::cout << level.terminalPrefix << source << level.terminalInfix << message << "\x1b[0m\n";
	}

} // namespace LoggerInternals
