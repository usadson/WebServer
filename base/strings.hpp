#pragma once

/**
 * Copyright (C) 2020 Tristan. All Rights Reserved.
 * This file is licensed under the BSD 2-Clause license.
 * See the COPYING file for licensing information.
 */

#include "string.hpp"

namespace Strings {

extern const base::String DefaultWebPage;
extern const base::String NotFoundPage;
extern const base::String TooManyRequestsPage;

namespace BadRequests {
	extern const base::String EmptyMethod;
} // namespace BadRequests

namespace StatusLines {
	extern const base::String BadRequest;
	extern const base::String MovedPermanently;
	extern const base::String NotFound;
	extern const base::String OK;
	extern const base::String TooManyRequests;
} // namespace StatusLines

} // namespace Strings
