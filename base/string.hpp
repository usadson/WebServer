#pragma once

/**
 * Copyright (C) 2020 Tristan. All Rights Reserved.
 * This file is licensed under the BSD 2-Clause license.
 * See the COPYING file for licensing information.
 */

#include <iterator>
#include <ostream>

#include <cstring>

namespace base {

class String {
public:
	inline String(const char *str)
		: str(str), internalLength(strlen(str)) {
	}

	inline constexpr String(const char *str, std::size_t len)
		: str(str), internalLength(len) {
	}

	[[nodiscard]] inline constexpr const char *
	c_str() const noexcept {
		return str;
	}

	[[nodiscard]] inline constexpr const char *
	data() const noexcept {
		return str;
	}

	[[nodiscard]] inline constexpr auto
	cbegin() const noexcept {
		return str;
	}

	[[nodiscard]] inline constexpr auto
	begin() const noexcept {
		return str;
	}

	[[nodiscard]] inline constexpr auto
	cend() const noexcept {
		return str + internalLength;
	}

	[[nodiscard]] inline constexpr auto
	end() const noexcept {
		return str + internalLength;
	}

	[[nodiscard]] inline constexpr std::size_t
	length() const noexcept {
		return internalLength;
	}

private:
	const char *str;
	std::size_t internalLength;
};

} // namespace base

inline std::ostream &
operator<<(std::ostream &stream, const base::String &string) {
	return stream << string.c_str();
}
