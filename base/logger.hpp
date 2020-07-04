#pragma once

/**
 * Copyright (C) 2020 Tristan. All Rights Reserved.
 * This file is licensed under the BSD 2-Clause license.
 * See the COPYING file for licensing information.
 */

#include <iostream>
#include <string>

namespace Logger {

static const char *loggerEndStr = "\x1b[0m\n";

template <typename T>
inline void
Debug(const std::string &source, const T &message) {
	std::cout << "[\x1b[34mDebug\x1b[0m] [\x1b[34m" << source << "\x1b[0m] \x1b[35m" << message << loggerEndStr;
}

template <typename T>
inline void
Error(const std::string &source, const T &message) {
	std::clog << "[\x1b[34mError\x1b[0m] [\x1b[34m" << source << "\x1b[0m] \x1b[31m" << message << loggerEndStr;
}

template <typename T>
inline void
Info(const std::string &source, const T &message) {
	std::cout << "[\x1b[34mInfo\x1b[0m] [\x1b[34m" << source << "\x1b[0m] \x1b[32m" << message << loggerEndStr;
}

template <typename T>
inline void
Log(const std::string &source, const T &message) {
	std::cout << "[\x1b[34mLog\x1b[0m] [\x1b[34m" << source << "\x1b[0m] \x1b[37m" << message << loggerEndStr;
}

template <typename T>
inline void
Severe(const std::string &source, const T &message) {
	std::clog << "[\x1b[34mSevere\x1b[0m] [\x1b[34m" << source << "\x1b[0m] \x1b[31m" << message << loggerEndStr;
}

template <typename T>
inline void
Warning(const std::string &source, const T &message) {
	std::clog << "[\x1b[34mWarning\x1b[0m] [\x1b[34m" << source << "\x1b[0m] \x1b[33m" << message << loggerEndStr;
}

} // namespace Logger
