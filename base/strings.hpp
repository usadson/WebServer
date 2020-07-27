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

namespace BadRequestMessages {
	extern const base::String EmptyMethod;
	extern const base::String HeaderFieldNameTooLong;
	extern const base::String HeaderFieldValueTooLong;
	extern const base::String MethodTooLong;
	extern const base::String RequestTargetTooLong;
	extern const base::String TooManyOWSs;
} // namespace BadRequestMessages

namespace StatusLines {
	extern const base::String BadRequest;
	extern const base::String MovedPermanently;
	extern const base::String NotFound;
	extern const base::String OK;
	extern const base::String PayloadTooLarge;
	extern const base::String TooManyRequests;
	extern const base::String URITooLong;
} // namespace StatusLines

} // namespace Strings
