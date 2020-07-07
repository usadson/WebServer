#pragma once

/**
 * Copyright (C) 2020 Tristan. All Rights Reserved.
 * This file is licensed under the BSD 2-Clause license.
 * See the COPYING file for licensing information.
 */

#include <string>

namespace Strings {

extern const std::string DefaultWebPage;
extern const std::string NotFoundPage;
extern const std::string TooManyRequestsPage;

namespace BadRequests {
	extern const std::string EmptyMethod;
} // namespace BadRequests

namespace StatusLines {
	extern const std::string BadRequest;
	extern const std::string NotFound;
	extern const std::string OK;
	extern const std::string TooManyRequests;
}

} // namespace Strings
