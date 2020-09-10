#pragma once

/**
 * Copyright (C) 2020 Tristan. All Rights Reserved.
 * This file is licensed under the BSD 2-Clause license.
 * See the COPYING file for licensing information.
 */

#include <iosfwd>

#include <unistd.h>

#include "base/string.hpp"

namespace Security {

struct PoliciesInternalPrivileges {
	gid_t groupID;
	uid_t userID;
};

struct Policies {

	// Sets the value of the "Content-Security-Policy" header.
	// E.g. can tell browsers what sources are allowed.
	// Empty means not sending the header at all.
	//
	// See more at Mozilla Infosec:
	// https://infosec.mozilla.org/guidelines/web_security#content-security-policy
	//
	// Read further at MDN:
	// https://developer.mozilla.org/en-US/docs/Web/HTTP/Headers/Content-Security-Policy/default-src
	//
	// Spec:
	// https://w3c.github.io/webappsec-csp/2/#directive-default-src
	const base::String contentSecurityPolicy{ "default-src 'none'; font-src https://fonts.gstatic.com; img-src 'self'; style-src 'self' 'unsafe-inline' https://fonts.googleapis.com;" };

	// Enables the "X-Frame-Options" header.
	// Prevents browsers from attacks that try to <iframe> the page.
	//
	// See more at: https://infosec.mozilla.org/guidelines/web_security#x-frame-options
	// Spec: https://tools.ietf.org/html/rfc7034
	bool denyIFraming{ true };

	// Disables the 'Referer' header sent when navigating.
	// Effectively: "Referrer-Policy: no-referrer"
	//
	// See more at: https://developer.mozilla.org/en-US/docs/Web/HTTP/Headers/Referrer-Policy
	// Spec: https://w3c.github.io/webappsec-referrer-policy/#referrer-policy-header
	bool disableReferrer{ true };

	// Enables the "X-XSS-Protection" header.
	// Prevents browsers from executing cross site scripts.
	//
	// See more at: https://infosec.mozilla.org/guidelines/web_security#x-xss-protection
	// Informative header (X prefix).
	bool enableXSSProtectionHeader{ true };

	// Enables the "X-Content-Type-Options: no-sniff" header.
	// Prevents browsers from executing scripts accidentally.
	//
	// See more at: https://infosec.mozilla.org/guidelines/web_security#x-content-type-options
	// Spec: https://fetch.spec.whatwg.org/#x-content-type-options-header
	bool enableContentTypeNosniffing{ true };

	// Maximum lifetime of a connection. If the max is reached and the
	// connection is still alive, it will be closed ungracefully.
	//
	// Time is in milliseconds
	// Default is 60000 i.e. 1 minute
	// 0 means unlimited.
	std::size_t maxConnectionLifetime{ 60000 };

	// The maximum length of the header field-name.
	// At this time, the longest registered header field-name is
	// 'Include-Referred-Token-Binding-ID', with a length of 33 characters.
	// The default is rounded up.
	// 0 means unlimited.
	std::size_t maxHeaderFieldNameLength{ 40 };

	// The maximum length of the header field-value.
	// 0 means unlimited.
	std::size_t maxHeaderFieldValueLength{ 255 };

	// The maximum method length.
	// At this time, the longest registered method is 'UPDATEREDIRECTREF', with
	// a length of 17, therefore the default value is 18 (17 + 0 for the null
	// terminator).
	// 0 means unlimited.
	std::size_t maxMethodLength{ 18 };

	// The amount of requests that may be made in a single connection (session).
	// 0 means unlimited.
	std::size_t maxRequestsPerConnection{ 300 };

	// true: (graceful)
	//   after the `maxRequestsCloseImmediately`'s request has been made, close
	//   the connection.
	// false:
	//   on each the request, check if `maxRequestsCloseImmediately` has been
	//   exceeded, if true send a 'HTTP 429 Too Many Requests' page.
	//   Strings::TooManyRequestsPage
	bool maxRequestsCloseImmediately{ false };

	// The maximum request-target (also known as path) length.
	// 0 means unlimited.
	std::size_t maxRequestTargetLength{ 255 };

	// The maximum amount of spaces in header field (between the field-name + ':'
	// and the header field-value).
	// This policy isn't to stop resource exhaustion in the form of memory, like
	// what similar policies try to achieve, but is here to stop attackers from
	// keeping the program in an infinite loop.
	// 0 means unlimited.
	std::size_t maxWhiteSpacesInHeaderField{ 20 };

	PoliciesInternalPrivileges privileges{ 0, 0 };

};

} // namespace Security
