/**
 * Copyright (C) 2020 Tristan. All Rights Reserved.
 * This file is licensed under the BSD 2-Clause license.
 * See the COPYING file for licensing information.
 */

#include "media_type.hpp"

#include <algorithm>

static constexpr std::string_view genericType = "application/octet-stream";

MediaTypeFinder::MediaTypeFinder() noexcept : mediaTypes({
	{ "html", { "text/html" } },
}) {
}

const std::string_view &
MediaTypeFinder::DetectMediaType(const std::unique_ptr<IO::File> &file) const noexcept {
	std::string_view string("test.html");

	do {
		auto dot = string.find_first_of('.');
		if (dot == std::string_view::npos)
			return genericType;
		string = string.substr(dot + 1);

		auto result = mediaTypes.find(string);
		if (result != std::end(mediaTypes))
			return result->second;
	} while (true);
}
