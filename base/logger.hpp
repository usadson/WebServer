#pragma once

/**
 * Copyright (C) 2020 Tristan. All Rights Reserved.
 * This file is licensed under the BSD 2-Clause license.
 * See the COPYING file for licensing information.
 */

#include <ostream>

namespace Logger {

inline void Debug(std::string source, std::string message) {
	std::cout << "\x1b[35m[Debug] [" << source << "] " << message << "\x1b[0m\n";
}

inline void Error(std::string source, std::string message) {
	std::clog << "\x1b[31m[Error] [" << source << "] " << message << "\x1b[0m\n";
}

inline void Info(std::string source, std::string message) {
	std::cout << "\x1b[32m[Info] [" << source << "] " << message << "\x1b[0m\n";
}

inline void Log(std::string source, std::string message) {
	std::cout << "[Log] [" << source << "] " << message << '\n';
}

inline void Severe(std::string source, std::string message) {
	std::clog << "\x1b[31m[Severe] [" << source << "] " << message << '\n';
}

inline void Warning(std::string source, std::string message) {
	std::clog << "\x1b[33m[Warning] [" << source << "] " << message << "\x1b[0m\n";
}

} // namespace Logger
