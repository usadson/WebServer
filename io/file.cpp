/**
 * Copyright (C) 2020 Tristan. All Rights Reserved.
 * This file is licensed under the BSD 2-Clause license.
 * See the COPYING file for licensing information.
 */

#include "file.hpp"

#include <fcntl.h>
#include <unistd.h>

IO::File::File(const char *path) {
	fd = open(path, O_RDONLY);
	if (fd != -1 && fstat(fd, &status) == -1) {
		close(fd);
		fd = -1;
	}
}

IO::File::~File() noexcept {
	if (fd != -1) {
		close(fd);
	}
}
