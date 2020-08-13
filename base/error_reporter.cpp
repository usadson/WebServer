/**
 * Copyright (C) 2020 Tristan. All Rights Reserved.
 * This file is licensed under the BSD 2-Clause license.
 * See the COPYING file for licensing information.
 */

#include "error_reporter.hpp"

#include <array>
#include <iosfwd>
#include <sstream>

#include "logger.hpp"

namespace ErrorReporter {

struct ErrorInfo {
	std::string_view name;
	bool log;
};

static std::array<ErrorInfo, 2> settings = {{
	{ "FileNotFound", true }, // FILE_NOT_FOUND
	{ "FileReadInsufficientPermissions", true }, // FILE_READ_INSUFFICIENT_PERMISSIONS
}};

void
ReportError(Error error, const std::string &message) noexcept {
	const auto &info = settings.at(static_cast<std::size_t>(error));

	if (info.log) {
		std::string str;
		str.reserve(3 + info.name.length() + message.length());
		std::stringstream warningLog(str);
		warningLog << '[' << info.name << "] " << message;
		Logger::Warning("ErrorReporter", warningLog.str());
	}
}

} // namespace ErrorReporter
