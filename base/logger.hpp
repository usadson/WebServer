#pragma once

/**
 * Copyright (C) 2020 Tristan. All Rights Reserved.
 * This file is licensed under the BSD 2-Clause license.
 * See the COPYING file for licensing information.
 */

#include <ostream>

namespace Logger {

inline void Error(std::string source, std::string message) {
	std::clog << "[Error] [" << source << "] " << message << '\n';
}

inline void Info(std::string source, std::string message) {
	std::cout << "[Info] [" << source << "] " << message << '\n';
}

inline void Log(std::string source, std::string message) {
	std::cout << "[Log] [" << source << "] " << message << '\n';
}

inline void Severe(std::string source, std::string message) {
	std::clog << "[Severe] [" << source << "] " << message << '\n';
}

inline void Warning(std::string source, std::string message) {
	std::clog << "[Warning] [" << source << "] " << message << '\n';
}

} // namespace Logger
