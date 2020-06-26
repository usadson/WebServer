#pragma once

/**
 * Copyright (C) 2020 Tristan. All Rights Reserved.
 * This file is licensed under the BSD 2-Clause license.
 * See the COPYING file for licensing information.
 */

#include <map>
#include <memory>
#include <string_view>

#include "io/file.hpp"

class MediaTypeFinder {
public:
	MediaTypeFinder() noexcept;

	[[nodiscard]] const std::string_view &
	DetectMediaType(const std::unique_ptr<IO::File> &) const noexcept;

private:
	std::map<std::string_view, std::string_view> mediaTypes;
};
