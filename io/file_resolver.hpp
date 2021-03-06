#pragma once

/**
 * Copyright (C) 2020 Tristan. All Rights Reserved.
 * This file is licensed under the BSD 2-Clause license.
 * See the COPYING file for licensing information.
 */

#include <memory>
#include <string>
#include <utility>

#include "http/request.hpp"
#include "io/file.hpp"

namespace IO {

enum class FileResolveStatus {
	OK,
	NOT_FOUND,
	INSUFFICIENT_PERMISSIONS,
	FILE_SYSTEM_OVERLOAD,
};

class FileResolver {
public:
	inline explicit FileResolver(const std::string &rootDirectory) noexcept : root(rootDirectory) {
	}

	[[nodiscard]] std::pair<FileResolveStatus, std::unique_ptr<IO::File>>
	Resolve(const HTTP::Request &) const noexcept;

private:
	std::string root;
};

} // namespace IO
