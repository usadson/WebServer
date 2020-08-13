#pragma once

/**
 * Copyright (C) 2020 Tristan. All Rights Reserved.
 * This file is licensed under the BSD 2-Clause license.
 * See the COPYING file for licensing information.
 */

#include <string>

namespace ErrorReporter {

enum class Error {
	FILE_NOT_FOUND,
	FILE_READ_INSUFFICIENT_PERMISSIONS
};

void
ReportError(Error, const std::string &) noexcept;

} // namespace ErrorReporter
