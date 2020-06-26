#pragma once

/**
 * Copyright (C) 2020 Tristan. All Rights Reserved.
 * This file is licensed under the BSD 2-Clause license.
 * See the COPYING file for licensing information.
 */

#include <map>
#include <string_view>

#include "io/file.hpp"

class MediaTypeFinder {
public:
	MediaTypeFinder() noexcept;

	[[nodiscard]] const std::string_view &
	DetectMediaType(const IO::File &) noexcept;

private:
	std::map<std::string, std::string_view> mediaTypes;
};
