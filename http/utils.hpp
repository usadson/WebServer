#pragma once

/**
 * Copyright (C) 2020 Tristan. All Rights Reserved.
 * This file is licensed under the BSD 2-Clause license.
 * See the COPYING file for licensing information.
 */

namespace HTTP::Utils {

	static const std::array unreservedCharacters =
		{ '!', '#', '$', '%', '&', '\'', '*', '+', '-', '.', '^', '_', '`', '|', '~' };

	// The HTTP specification states that we should parse octets as code points
	// part of the USASCII subset.
	//
	// References:
	// https://svn.tools.ietf.org/svn/wg/httpbis/specs/rfc7230.html#rfc.section.3.p.3
	// http://sliderule.mraiow.com/w/images/7/73/ASCII.pdf
	[[nodiscard]] inline constexpr bool
	IsNonUSASCIICharacter(char character) noexcept {
		return static_cast<uint8_t>(character) & 0x80;
	}

	// Some inputs are specified as 'token' definitions. This function makes
	// sure the characters follow the standard.
	//
	// Reference:
	// https://svn.tools.ietf.org/svn/wg/httpbis/specs/rfc7230.html#field.components
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

	// The path of a request is of request-target type. This function ensures
	// that read characters are conforming to the definition of such a type.
	//
	// Reference:
	// https://svn.tools.ietf.org/svn/wg/httpbis/specs/rfc7230.html#request-target
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
