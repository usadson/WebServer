#pragma once

/**
 * Copyright (C) 2020 Tristan. All Rights Reserved.
 * This file is licensed under the BSD 2-Clause license.
 * See the COPYING file for licensing information.
 */

/**
 * This file is for code organizing. Instead of having these C libraries in
 * default namespace, put them inside a proper namespace.
 *
 * We can't use the namespace 'posix', since it is probihited by the C++
 * standard. It is rather stupid, since it is reserved for years but hasn't been
 * used anywhere official AFAIK. Therefore, we'll use 'libposix'.
 */

#include <unistd.h>

namespace psx {

	using ::close;
	using ::read;
	using ::write;

} // namespace psx
