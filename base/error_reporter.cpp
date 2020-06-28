/**
 * Copyright (C) 2020 Tristan. All Rights Reserved.
 * This file is licensed under the BSD 2-Clause license.
 * See the COPYING file for licensing information.
 */

#include "error_reporter.hpp"

#include <array>

#include "logger.hpp"

namespace ErrorReporter {

struct ErrorInfo {
	std::string name;
	bool log;
};

static std::array<ErrorInfo, 1> settings = {{
	{ "FileNotFound", true }, // FILE_NOT_FOUND
}};

void
ReportError(Error error, const std::string &message) noexcept {
	const auto &info = settings.at(static_cast<std::size_t>(error));

	if (info.log) {
		Logger::Warning("ErrorReported", '[' + info.name + "] " + message);
	}
}

} // namespace ErrorReporter
