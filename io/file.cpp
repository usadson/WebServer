/**
 * Copyright (C) 2020 Tristan. All Rights Reserved.
 * This file is licensed under the BSD 2-Clause license.
 * See the COPYING file for licensing information.
 */

#include "file.hpp"

#include <fcntl.h>
#include <unistd.h>

#include "posix/fcntl.hpp"
#include "posix/unistd.hpp"

void
IO::File::InternalInit(const char *path) {
	if (fd != -1) {
		psx::close(fd);
	}

	internalPath = path;
	fd = psx::open(path, psx::OpenMode::readOnly);

	if (fd != -1 && psx::fstat(fd, &status) == -1) {
		psx::close(fd);
		fd = -1;
	}
}

IO::File::~File() noexcept {
	if (fd != -1) {
		psx::close(fd);
	}
}
