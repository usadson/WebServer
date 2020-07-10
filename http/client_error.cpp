/**
 * Copyright (C) 2020 Tristan. All Rights Reserved.
 * This file is licensed under the BSD 2-Clause license.
 * See the COPYING file for licensing information.
 */

#include "client_error.hpp"

#include <array>

const char *
ClientErrorToString(HTTP::ClientError error) {
	std::array array = {
		"EMPTY_METHOD",
		"FAILED_READ_GENERIC",
		"FAILED_READ_HEADER_FIELD_GENERIC",
		"FAILED_READ_HEADER_FIELD_NAME",
		"FAILED_READ_HEADER_FIELD_VALUE",
		"FAILED_READ_HEADER_NEWLINE",
		"FAILED_READ_METHOD",
		"FAILED_READ_PATH",
		"FAILED_READ_CRLF",
		"FAILED_READ_VERSION",
		"FAILED_WRITE_RESPONSE_BODY",
		"FAILED_WRITE_RESPONSE_METADATA",
		"FILE_NOT_FOUND",
		"INCORRECT_HEADER_FIELD_NAME",
		"INCORRECT_HEADER_FIELD_NEWLINE",
		"INCORRECT_HEADER_FIELD_VALUE",
		"INCORRECT_METHOD",
		"INCORRECT_PATH",
		"INCORRECT_CRLF",
		"INCORRECT_VERSION",
		"INVALID_PATH",
		"NO_ERROR",
		"TOO_MANY_REQUESTS_PER_THIS_CONNECTION",
		"UNEXPECTED_CR_IN_FIELD_NAME",
		"WHITESPACE_EXPECTED"
	};
	return array[static_cast<std::size_t>(error)];
}

std::ostream &
operator<<(std::ostream &stream, HTTP::ClientError error) {
	return stream << ClientErrorToString(error);
}
