/**
 * Copyright (C) 2020 Tristan. All Rights Reserved.
 * This file is licensed under the BSD 2-Clause license.
 * See the COPYING file for licensing information.
 */

#include "file_resolver.hpp"

#include <iostream>
#include <sstream>

#include <fcntl.h>
#include <unistd.h>

std::unique_ptr<IO::File>
IO::FileResolver::Resolve(const HTTP::Request &request) const noexcept {
	std::stringstream pathBuilder;
	pathBuilder << root;
	pathBuilder << request.path;

	auto file = std::make_unique<IO::File>(pathBuilder.str().c_str());
	if (file->Handle() != -1 && file->IsNormalFile())
		return file;

	if (file->Handle() != -1 && !file->IsDirectory()) {
		return std::unique_ptr<IO::File>{};
	}

	pathBuilder << "/index.html";

	file->InternalInit(pathBuilder.str().c_str());
	if (file->Handle() != -1)
		return file;

	std::cout << "File: " << pathBuilder.str() << " not found!\n";
	return std::unique_ptr<IO::File>{};
}
