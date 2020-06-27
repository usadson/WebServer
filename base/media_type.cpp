/**
 * Copyright (C) 2020 Tristan. All Rights Reserved.
 * This file is licensed under the BSD 2-Clause license.
 * See the COPYING file for licensing information.
 */

#include "media_type.hpp"

#include <algorithm>
#include <sstream>

#include "base/logger.hpp"

static const std::string genericType = "application/octet-stream";

MediaTypeFinder::MediaTypeFinder() noexcept
	: mediaTypes({
		// NOTE This list must be kept minimal. There is no need to have every
		//      extension in this list. It will only cause overhead and provide no
		//      real advantage.
		//
		// If you need to have an extension in this list, you can add it, but keep
		// the advice above in mind.
		{ "html", "text/html" },
		{ "json", "application/json" },
		{ "jpg", "image/jpeg" },
		{ "png", "image/png" },
	}) {
}

const std::string &
MediaTypeFinder::DetectMediaType(const std::unique_ptr<IO::File> &file) const noexcept {
	std::string string(file->Path());

	do {
		auto dot = string.find_first_of('.');
		if (dot == std::string::npos) {
			std::stringstream info;
			info << "Unrecognized extension: \"" << file->Path() << '"';
			Logger::Debug("MediaTypeFinder", info.str());
			return genericType;
		}
		string = string.substr(dot + 1);

		auto result = mediaTypes.find(string);
		if (result != std::end(mediaTypes))
			return result->second;
	} while (true);
}
