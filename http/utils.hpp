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
	IsTokenCharacter(char character) {
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

} // namespace HTTP
