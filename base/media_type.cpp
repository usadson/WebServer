/**
 * Copyright (C) 2020 Tristan. All Rights Reserved.
 * This file is licensed under the BSD 2-Clause license.
 * See the COPYING file for licensing information.
 */

/* NOTE Don't auto-format this file! */

#include "media_type.hpp"

#include <iterator>
#include <sstream>
#include <utility>

#include "base/logger.hpp"
#include "io/file.hpp"

static MediaType genericType = { "application", "octet-stream" };

namespace MediaTypes {
	// NOTE When adding members to this namespace, make sure to register it in
	// MediaTypeFinder::MediaTypeFinder (call MediaType::SetCompleteType()).

	MediaType HTML { "text", "html" };
	MediaType TEXT { "text", "plain" };
}

MediaTypeFinder::MediaTypeFinder()
	: mediaTypes({
		// NOTE This list must be kept minimal. There is no need to have every
		//      extension in this list. Providing rarely used and obsolete
		//      media types only causes overhead, because media type lookup
		//      should be snappy.
		// NOTE If you need to have an extension in this list, you can add it, but
		//      keep the advice above in mind.
		{ "css",   { "text", "css" } },                     // RFC 2318
		{ "html",  { "text", "html" } },                    // https://html.spec.whatwg.org/#text/html
		// Microsoft uses image/x-icon, which isn't approved by IANA. Most web
		// servers use the correct media type (as seen below), except nginx.
		{ "ico",   { "image", "vnd.microsoft.icon" } },     // https://www.iana.org/assignments/media-types/image/vnd.microsoft.icon
		{ "js",    { "application", "javascript", true } }, // RFC 4329
		{ "json",  { "application", "json" } },               // RFC 8259
		{ "jpg",   { "image", "jpeg" } },                   // RFC 2046
		{ "otf",   { "font", "otf" } },                     // RFC 8081
		{ "png",   { "image", "png" } },                    // RFC 2083
		{ "svg",   { "image", "svg+xml" } },                // https://www.w3.org/TR/SVG/mimereg.html
		{ "ttf",   { "font", "ttf" } },                     // RFC 8081
		{ "txt",   { "text", "plain" } },                   // RFC 2046, 3676 & 5147
		{ "woff",  { "font", "woff" } },                    // RFC 8081
		{ "woff2", { "font", "woff" } },                    // RFC 8081
		// The debate of application/xml vs text/xml. As every UA knows about
		// both of them, it isn't really necessary. Every server has their own
		// opinion about it, so there isn't really a standard to follow. RFC
		// 7303 doesn't specify a better one — as opposed to JSON — only that
		// text/xml is for user-readable XML, and application/xml is for non-
		// user-readable XML.
		{ "xml",  { "application", "xml" } },               // RFC 7303
		{ "zip",  { "application", "zip" } },               // https://www.iana.org/assignments/media-types/application/zip
	}) {
	genericType.SetCompleteType();
	for (auto &pair : mediaTypes) {
		pair.second.SetCompleteType();
	}
	MediaTypes::HTML.SetCompleteType();
	MediaTypes::TEXT.SetCompleteType();
}

const MediaType &
MediaTypeFinder::DetectMediaType(
	const std::unique_ptr<IO::File> &file) const noexcept {
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
		if (result != std::end(mediaTypes)) {
			return result->second;
		}
	} while (true);
}
