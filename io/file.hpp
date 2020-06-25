#pragma once

/**
 * Copyright (C) 2020 Tristan. All Rights Reserved.
 * This file is licensed under the BSD 2-Clause license.
 * See the COPYING file for licensing information.
 */

#include <cstddef>
#include <sys/stat.h>

namespace IO {
class File {
public:
	File(const char *path);

	~File() noexcept;

	[[nodiscard]] inline constexpr int
	handle() const noexcept {
		return fd;
	}

	[[nodiscard]] inline constexpr std::size_t
	size() const noexcept {
		return status.st_size;
	}

private:
	int fd;
	struct stat status;
};

} // namespace IO
