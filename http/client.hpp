#pragma once

/**
 * Copyright (C) 2020 Tristan. All Rights Reserved.
 * This file is licensed under the BSD 2-Clause license.
 * See the COPYING file for licensing information.
 */

#include <chrono>
#include <iosfwd>
#include <memory>
#include <string>
#include <thread>
#include <vector>

// From base/media_type.hpp:
struct MediaType;

#include "base/string.hpp"
#include "cgi/script.hpp"
#include "connection/connection.hpp"
#include "http/client_error.hpp"
#include "http/request.hpp"

#ifndef TESTING_VISIBILITY
#define TESTING_VISIBILITY private
#endif

namespace HTTP {

	// Forward-decl from server.hpp
	class Server;

} // namespace HTTP

namespace HTTP {

// Buffers associated with the client, nicely organized.
struct ClientBuffers {
	// Header field name buffer
	std::vector<char> fieldName;

	// Header field value buffer
	std::vector<char> fieldValue;

	ClientBuffers() noexcept;
};

class Client {
public:
	Client(Server *server, int socket) noexcept;

	// For testing purposes
	inline explicit Client(Server *server) : connection(nullptr), server(server) {
	}

TESTING_VISIBILITY:
	ClientBuffers buffers;
	std::unique_ptr<Connection> connection;
	Request currentRequest;

	// This may be changed by InterpretConnectionHeaders
	// HTTP/1.1 enables persistent connections by default.
	// NOTE that this isn't a configuration option, but a state.
	bool persistentConnection{ true };

	Server *server;

	[[nodiscard]] std::size_t
	CalculateMinLengthRequestTargetAbsoluteForm() const noexcept;

	// Checks if the connection should be closed due to security policies.
	[[nodiscard]] bool
	CheckConnectionLifetime() noexcept;

	// Checks if the file is valid (e.g. inside the actual rootDirectory).
	[[nodiscard]] ClientError
	CheckFileLocation(const std::string &) const noexcept;

	[[nodiscard]] ClientError
	CheckUpgradeHTTPS() const noexcept;

	// A before-destructor clean function, ran at the end of 'Entrypoint'.
	void
	Clean() noexcept;

	[[nodiscard]] ClientError
	ConsumeCRLF() noexcept;

	// Effectively runs ConsumeHeaderFieldName and ConsumeHeaderFieldValue, and
	// combines the collected data and pushes them into a pair in
	// 'client.headers'.
	[[nodiscard]] ClientError
	ConsumeHeaderField(char) noexcept;

	// See RFC 7230 § 3.2
	// https://svn.tools.ietf.org/svn/wg/httpbis/specs/rfc7230.html#header.fields
	[[nodiscard]] ClientError
	ConsumeHeaderFieldName() noexcept;

	// See RFC 7230 § 3.2
	// https://svn.tools.ietf.org/svn/wg/httpbis/specs/rfc7230.html#header.fields
	[[nodiscard]] ClientError
	ConsumeHeaderFieldValue() noexcept;

	// Consumes headers. This function effectively repeatedly calls
	// ConsumeHeaderField until time a blank line is encountered.
	// See RFC 7230 § 3
	// https://svn.tools.ietf.org/svn/wg/httpbis/specs/rfc7230.html#http.message
	[[nodiscard]] ClientError
	ConsumeHeaders() noexcept;

	// Checks if the 'Host' header is correct. This function verifies that one,
	// and only one 'Host' header is present, and that the 'Host' header
	// corresponds with the hostname of the server/machine.
	//
	// See RFC 7230 § 5.4
	// https://svn.tools.ietf.org/svn/wg/httpbis/specs/rfc7230.html#rfc.section.5.4.p.8
	[[nodiscard]] ClientError
	CheckHostHeader() noexcept;

	// See RFC 7230 § 5.3
	// https://svn.tools.ietf.org/svn/wg/httpbis/specs/rfc7230.html#request-target
	// NOTE This function only supports relative paths (i.e. paths starting
	// with '/', like '/index.html').
	[[nodiscard]] ClientError
	ConsumeMethod() noexcept;

	// See RFC 7230 § 3.1.1
	// https://svn.tools.ietf.org/svn/wg/httpbis/specs/rfc7230.html#method
	[[nodiscard]] ClientError
	ConsumePath() noexcept;

	// See RFC 7230 § 2.6
	// https://svn.tools.ietf.org/svn/wg/httpbis/specs/rfc7230.html#http.version
	[[nodiscard]] ClientError
	ConsumeVersion() noexcept;

	// The Entrypoint of the client. This is the first effective call after
	// summoning a new thread. This function calls repeatedly RunMessageExchange
	// until EOS or error. Then it calls 'Clean'.
	void
	Entrypoint();

	// Extract things like the query parameters from the path.
	[[nodiscard]] ClientError
	ExtractComponentsFromPath() noexcept;

	// This function handles the FILE_NOT_FOUND ClientError. It is called from
	// RecoverError.
	[[nodiscard]] bool
	HandleFileNotFound() noexcept;

	// This function handles the parsed HTTP message, and runs the correct calls
	// to respond to this request.
	//
	// 'Handling' refers to the interpreting and respoding to requests.
	[[nodiscard]] ClientError
	HandleRequest() noexcept;

	void
	InterpretConnectionHeaders() noexcept;

	// Mark the connection as to-be-closed. It will eventually be closed after a
	// new run of Entrypoint's while loop.
	//
	// This function is useful when the request an user made was malformed.
	// These problems arise when it receives incorrect data, and this problem is
	// immediately handled. This means that when RunMessageExchange runs, the
	// data in the buffer probably is from the previous request, where the
	// consumer left off.
	void
	MarkConnectionClosing() noexcept;

	// This function is called after a subroutine encounters an error. Some
	// errors can be handled gracefully (e.g. FILE_NOT_FOUND), but some can't.
	[[nodiscard]] bool
	RecoverError(ClientError) noexcept;

	[[nodiscard]] bool
	RecoverErrorBadRequest(const base::String &) noexcept;

	// Handles the FILE_NOT_FOUND ClientError. Called by RecoverError.
	[[nodiscard]] bool
	RecoverErrorFileNotFound() noexcept;

	// Handles the FILE_READ_INSUFFICIENT_PERMISSIONS ClientError.
	// Called by RecoverError.
	[[nodiscard]] bool
	RecoverErrorFileReadInsufficientPermissions() noexcept;

	// Clears the 'currentRequest' member variable.
	void
	ResetExchangeState() noexcept;

	// This function calls the correct subroutines exchange functions. Called
	// by 'Entrypoint', on failure, RecoverError is called and false is returned.
	[[nodiscard]] bool
	RunMessageExchange() noexcept;

	// Sends the HTTP metadata. (See below for more information.)
	[[nodiscard]] bool
	SendMetadata(const base::String &response, std::size_t contentLength, const MediaType &type, const char *additionalMetaData = nullptr) noexcept;

	// Run the CGI algorithm.
	[[nodiscard]] bool
	ServeCGI(const CGI::Script *) noexcept;

	// When the homepage of the site couldn't be found. (See below for more
	// information.)
	[[nodiscard]] bool
	ServeDefaultPage() noexcept;

	// Send a response with a body defined in the base/strings.xpp files.
	[[nodiscard]] bool
	ServeStringRequest(const base::String &, const MediaType &, const base::String &body) noexcept;

	[[nodiscard]] ClientError
	ValidateCurrentRequestPath() noexcept;

public:
	// This value is encremented if Security::Policies::maxRequestsCloseImmediately
	// is true: ResetExchangeState
	// is false: HandleRequest
	std::size_t requestCount{0};
	std::thread thread;
	std::chrono::time_point<std::chrono::high_resolution_clock> startingTimePoint;
};

///////////////////////////////////////////////////////////////////////////////
//                            HTTP Response Message                          //
///////////////////////////////////////////////////////////////////////////////
// A HTTP Response message consists of three parts:
// - A status line (RFC 7230 § 3.1.2 [2])
// - Headers (RFC 7230 § 3.2 [3])
// - An optional message body (RFC 7230 § 3.3 [4])
//
// The first two are considered part of HTTP message metadata. These describe
// mainly how the server handled the request, and describes how the response
// body should be interpreted.
//
// https://svn.tools.ietf.org/svn/wg/httpbis/specs/rfc7230.html#http.message
// https://svn.tools.ietf.org/svn/wg/httpbis/specs/rfc7230.html#status.line
// https://svn.tools.ietf.org/svn/wg/httpbis/specs/rfc7230.html#header.fields
// https://svn.tools.ietf.org/svn/wg/httpbis/specs/rfc7230.html#message.body
//
///////////////////////////////////////////////////////////////////////////////
//                                  Homepage                                 //
///////////////////////////////////////////////////////////////////////////////
// The homepage of a website is the main page of the site.
//
// For example: https://duckduckgo.com/
//
// The lookup for request-target "/", with DIR as the PUBLIC_HTML directory, is
// as follows:
// DIR/index.html
// DIR/index.*
// CGI script
// default page
//
// The default page of the site is an example test page. This document is
// embedded in the executable. This file is there to avoid users facing a scary
// 404 error, which beginner sysadmins might understand as a configuration
// error.

} // namespace HTTP
