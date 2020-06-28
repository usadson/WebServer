#pragma once

/**
 * Copyright (C) 2020 Tristan. All Rights Reserved.
 * This file is licensed under the BSD 2-Clause license.
 * See the COPYING file for licensing information.
 */

#include <memory>
#include <string>

#include "http/request.hpp"
#include "io/file.hpp"

namespace IO {
class FileResolver {
public:
	inline explicit FileResolver(const std::string &rootDirectory) noexcept : root(rootDirectory) {
	}

	[[nodiscard]] std::unique_ptr<IO::File>
	Resolve(const HTTP::Request &) const noexcept;

private:
	std::string root;
};

} // namespace IO
