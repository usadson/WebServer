/**
 * Copyright (C) 2020 Tristan. All Rights Reserved.
 * This file is licensed under the BSD 2-Clause license.
 * See the COPYING file for licensing information.
 */

#include <gtest/gtest.h>

#define TESTING

#include "http/utils.hpp"

namespace HTTP::Utils {

	TEST(Utils, IsNonUSASCIICharacter) {
		for (char c = 0; c < 127; c++) {
			ASSERT_FALSE(IsNonUSASCIICharacter(c));
		}

		for (char c = 0; c < 127; c++) {
			ASSERT_TRUE(IsNonUSASCIICharacter(c + 0x80));
		}
	}

	TEST(Utils, IsTokenCharacter) {
		std::array invalidCharacters = {
			'"', '(', ')', ',', '/', ':', ';', '<', '=', '>', '?', '@', '[', '\\', ']', '{', '}'
		};
		for (uint16_t c = 0; c <= 255; c++) {
			if (c > 0x20 && std::find(std::begin(invalidCharacters), std::end(invalidCharacters), c) == std::end(invalidCharacters) && c < 0x7F) {
				ASSERT_TRUE(IsTokenCharacter(c))
					<< "Character '" << static_cast<char>(c) << "' isn't treated as a token character!";
			} else {
				ASSERT_FALSE(IsTokenCharacter(c))
					<< "Character '" << static_cast<char>(c) << "' (" << c << ") is wrongfully treated as a token character!";
			}
		}
	}

} // namespace HTTP

