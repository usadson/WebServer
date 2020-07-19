#pragma once

/**
 * Copyright (C) 2020 Tristan. All Rights Reserved.
 * This file is licensed under the BSD 2-Clause license.
 * See the COPYING file for licensing information.
 */

#include <string_view>

namespace Strings {

extern const std::string_view DefaultWebPage;
extern const std::string_view NotFoundPage;
extern const std::string_view TooManyRequestsPage;

namespace BadRequests {
	extern const std::string_view EmptyMethod;
} // namespace BadRequests

namespace StatusLines {
	extern const std::string_view BadRequest;
	extern const std::string_view MovedPermanently;
	extern const std::string_view NotFound;
	extern const std::string_view OK;
	extern const std::string_view TooManyRequests;
} // namespace StatusLines

} // namespace Strings
