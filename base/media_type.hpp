#pragma once

/**
 * Copyright (C) 2020 Tristan. All Rights Reserved.
 * This file is licensed under the BSD 2-Clause license.
 * See the COPYING file for licensing information.
 */

#include <map>
#include <memory>
#include <optional>
#include <string>

#include <cstring>

// From io/file.hpp:
namespace IO { class File; }

struct MediaType {
	friend class MediaTypeFinder;

	inline MediaType(const char *type, const char *subtype, bool includeCharset)
		 : completeType(std::nullopt), type(type), subtype(subtype),
		   includeCharset(includeCharset) {
	}

	inline MediaType(const char *type, const char *subtype)
		 : MediaType(type, subtype, strcmp(type, "text") == 0) {
	}

	[[nodiscard]] const std::string &
	Complete() const {
		return completeType.value();
	}

	[[nodiscard]] bool
	IncludeCharset() const noexcept {
		return includeCharset;
	}

	[[nodiscard]] const std::string_view &
	Subtype() const noexcept {
		return subtype;
	}

	[[nodiscard]] const std::string_view &
	Type() const noexcept {
		return type;
	}

protected:
	// Exception-safe completeType generation.
	void
	SetCompleteType() {
		std::string s;
		s.reserve(type.length() + 1 + subtype.length());
		s += type;
		s += '/';
		s += subtype;
		completeType = std::move(s);
	}

private:
	std::optional<std::string> completeType;
	const std::string_view type;
	const std::string_view subtype;
	const bool includeCharset;
};

namespace MediaTypes {
	extern MediaType HTML;
	extern MediaType TEXT;
} // namespace MediaTypes

class MediaTypeFinder {
public:
	MediaTypeFinder();

	[[nodiscard]] const MediaType &
	DetectMediaType(const std::unique_ptr<IO::File> &) const noexcept;

private:
	std::map<std::string, MediaType> mediaTypes;
};
