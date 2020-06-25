/**
 * Copyright (C) 2020 Tristan. All Rights Reserved.
 * This file is licensed under the BSD 2-Clause license.
 * See the COPYING file for licensing information.
 */

#include "client_error.hpp"

#include <array>

std::ostream &
operator<<(std::ostream &stream, HTTP::ClientError error) {
	std::array array = {
		"FAILED_READ_METHOD",
		"FAILED_READ_PATH",
		"FAILED_READ_VERSION",
		"FAILED_WRITE_RESPONSE_METADATA",
		"FAILED_WRITE_RESPONSE_BODY",
		"FILE_NOT_FOUND",
		"INCORRECT_METHOD",
		"INCORRECT_PATH",
		"INCORRECT_VERSION",
		"NO_ERROR"
	};

	return stream << array[static_cast<std::size_t>(error)];
}
