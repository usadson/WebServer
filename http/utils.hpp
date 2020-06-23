#pragma once

/**
 * Copyright (C) 2020 Tristan. All Rights Reserved.
 * This file is licensed under the BSD 2-Clause license.
 * See the COPYING file for licensing information.
 */

namespace HTTP::Utils {

	static const std::array unreservedCharacters =
		{ '!', '#', '$', '%', '&', '\'', '*', '+', '-', '.', '^', '_', '`', '|', '~' };

	[[nodiscard]] inline constexpr bool
	IsNonUSASCIICharacter(char character) noexcept {
		return static_cast<uint8_t>(character) & 0x80;
	}

	[[nodiscard]] inline constexpr bool
	IsTokenCharacter(char character) {
		if (IsNonUSASCIICharacter(character))
			return false;

		if ((character >= '0' && character <= '9') ||
			(character >= 'A' && character <= 'Z') ||
			(character >= 'a' && character <= 'z')) {
			return true;
		}

		if (std::find(std::begin(unreservedCharacters),
					  std::end(unreservedCharacters), character)
			!= std::end(unreservedCharacters)) {
			return true;
		}

		return false;
	}

	[[nodiscard]] inline constexpr bool
	IsPathCharacter(char character) {
		if (IsNonUSASCIICharacter(character))
			return false;

		if (character == '\0')
			return false;

		// Path validation/sanitization is going to be done by the request
		// handler, not the parser.
		return true;
	}

} // namespace HTTP
