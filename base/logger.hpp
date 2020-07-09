#pragma once

/**
 * Copyright (C) 2020 Tristan. All Rights Reserved.
 * This file is licensed under the BSD 2-Clause license.
 * See the COPYING file for licensing information.
 */

#include <iostream>
#include <string>

namespace LoggerInternals {

	struct LogLevel {
		std::string_view name;
		std::string_view terminalPrefix;
		std::string_view terminalInfix;
	};

	const static LogLevel debug{ "debug", "[\x1b[34mDebug\x1b[0m] [\x1b[34m", "\x1b[0m] \x1b[35m" };
	const static LogLevel error{ "error", "[\x1b[34mError\x1b[0m] [\x1b[34m", "\x1b[0m] \x1b[31m" };
	const static LogLevel info{ "info", "[\x1b[34mInfo\x1b[0m] [\x1b[34m", "\x1b[0m] \x1b[32m" };
	const static LogLevel log{ "log", "[\x1b[34mLog\x1b[0m] [\x1b[34m", "\x1b[0m] \x1b[37m" };
	const static LogLevel severe{ "severe", "[\x1b[34mSevere\x1b[0m] [\x1b[34m", "\x1b[0m] \x1b[31m" };
	const static LogLevel warning{ "warning", "[\x1b[34mWarning\x1b[0m] [\x1b[34m", "\x1b[0m] \x1b[33m" };

	void
	Perform(const LogLevel &, const std::string &, const std::string &);

} // namespace LoggerInternals

namespace Logger {

inline void
Debug(const std::string &source, const std::string &message) {
	LoggerInternals::Perform(LoggerInternals::debug, source, message);
}

inline void
Error(const std::string &source, const std::string &message) {
	LoggerInternals::Perform(LoggerInternals::error, source, message);
}

inline void
Info(const std::string &source, const std::string &message) {
	LoggerInternals::Perform(LoggerInternals::info, source, message);
}

inline void
Log(const std::string &source, const std::string &message) {
	LoggerInternals::Perform(LoggerInternals::log, source, message);
}

inline void
Severe(const std::string &source, const std::string &message) {
	LoggerInternals::Perform(LoggerInternals::severe, source, message);
}

inline void
Warning(const std::string &source, const std::string &message) {
	LoggerInternals::Perform(LoggerInternals::warning, source, message);
}

} // namespace Logger
