#pragma once

/**
 * Copyright (C) 2020 Tristan. All Rights Reserved.
 * This file is licensed under the BSD 2-Clause license.
 * See the COPYING file for licensing information.
 */

#include <string>

#include <cstddef>
#include <sys/stat.h>

namespace IO {
class File {
	friend class FileResolver;

public:
	inline File(const char *path) noexcept : fd(-1), internalPath(path) {
		InternalInit(path);
	}

	~File() noexcept;

	[[nodiscard]] inline constexpr int
	Handle() const noexcept {
		return fd;
	}

	[[nodiscard]] inline constexpr std::size_t
	Size() const noexcept {
		return status.st_size;
	}

	[[nodiscard]] inline constexpr bool
	IsNormalFile() const noexcept {
		return S_ISREG(status.st_mode);
	}

	[[nodiscard]] inline constexpr bool
	IsDirectory() const noexcept {
		return S_ISDIR(status.st_mode);
	}

	[[nodiscard]] inline constexpr const std::string &
	Path() const noexcept {
		return internalPath;
	}

protected:
	void
	InternalInit(const char *);

private:
	int fd;
	std::string internalPath;
	struct stat status;
};

} // namespace IO
