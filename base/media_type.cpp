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
MediaTypeFinder::DetectMediaType(const IO::File &file) noexcept {
	const std::string string("test.html");
	auto result = mediaTypes.find(string);
	return result == std::end(mediaTypes) ? genericType : result->second;
}
