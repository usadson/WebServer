/**
 * Copyright (C) 2020 Tristan. All Rights Reserved.
 * This file is licensed under the BSD 2-Clause license.
 * See the COPYING file for licensing information.
 */

#include "client.hpp"

#include <algorithm>
#include <array>
#include <iterator>
#include <map>
#include <memory>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

#include <cctype>
#include <climits>
#include <csignal>
#include <cstdio>
#include <cstdlib>
#include <strings.h>

#include "base/error_reporter.hpp"
#include "base/logger.hpp"
#include "base/media_type.hpp"
#include "base/strings.hpp"
#include "cgi/manager.hpp"
#include "cgi/script.hpp"
#include "http/configuration.hpp"
#include "http/server.hpp"
#include "http/utils.hpp"
#include "io/file.hpp"
#include "io/file_resolver.hpp"
#include "security/policies.hpp"

#define MAGIC_FIELD_NAME_AVG_LENGTH 12
#define MAGIC_FIELD_VALUE_AVG_LENGTH 30
// The following aren't really avg, just a blind guess
#define MAGIC_METHOD_AVG_LENGTH 4
#define MAGIC_PATH_AVG_LENGTH 8

#define MAGIC_METHOD_MAX_BUFFER_SIZE_ON_RESET MAGIC_METHOD_AVG_LENGTH

//#define SIG_IGN  ((__sighandler_t)  1)
#undef SIG_IGN
#define SIG_IGN reinterpret_cast<__sighandler_t>(1)

[[nodiscard]] inline bool
StringStartsWith(const std::string &string, const std::string &prefix) {
#ifdef __cpp_lib_starts_ends_with
	return str.starts_with(str);
#else
	return std::mismatch(std::begin(prefix), std::end(prefix), std::begin(string), std::end(string)).first == std::end(prefix);
#endif
}

namespace HTTP {

ClientBuffers::ClientBuffers() noexcept {
	fieldName.reserve(MAGIC_FIELD_NAME_AVG_LENGTH);
	fieldName.reserve(MAGIC_FIELD_VALUE_AVG_LENGTH);
}

Client::Client(Server *server, int sock) noexcept :
	connection(std::make_unique<Connection>(sock, server->config().useTransportSecurity)),
	server(server), thread(&Client::Entrypoint, this) {
}

ClientError
Client::CheckFileLocation(const std::string &path) const noexcept {
	std::array<char, PATH_MAX> dest{};
	if (realpath(path.c_str(), dest.data()) == nullptr) {
		return ClientError::CHECK_FILE_LOCATION_VERIFICATION_FAILURE;
	}

	if (!StringStartsWith(dest.data(), server->config().rootDirectory)) {
		return ClientError::CHECK_FILE_LOCATION_OUTSIDE_ROOT_DIRECTORY;
	}

	return ClientError::NO_ERROR;
}

ClientError
Client::CheckUpgradeHTTPS() const noexcept {
	if (server->config().upgradeToHTTPS) {
		return ClientError::UPGRADE_TO_HTTPS;
	}

	return ClientError::NO_ERROR;
}

void
Client::Clean() noexcept {
	connection = nullptr;

	server->SignalClientDeath(thread);
}

ClientError
Client::ConsumeCRLF() noexcept {
	char cr; // NOLINT(cppcoreguidelines-init-variables)
	char lf; // NOLINT(cppcoreguidelines-init-variables)
	if (!connection->ReadChar(&cr) || !connection->ReadChar(&lf)) {
		return ClientError::FAILED_READ_CRLF;
	}

	if (cr != '\r' || lf != '\n') {
		return ClientError::INCORRECT_CRLF;
	}

	return ClientError::NO_ERROR;
}

ClientError
Client::ConsumeHeaderField(char firstCharacter) noexcept {
	ClientError subroutineError;

	/* Consume field-name */
	buffers.fieldName.push_back(firstCharacter);
	subroutineError = ConsumeHeaderFieldName();
	if (subroutineError != ClientError::NO_ERROR) {
		return subroutineError;
	}

	const auto maxOWS = server->config().securityPolicies.maxWhiteSpacesInHeaderField;
	std::size_t owsCount = 0;

	/* Consume OWS (Optional Whitespaces) */
	while (true) {
		char character; // NOLINT(cppcoreguidelines-init-variables)

		if (!connection->ReadChar(&character)) {
			return ClientError::FAILED_READ_HEADER_FIELD_GENERIC;
		}

		if (character != ' ' && character != '\t') {
			buffers.fieldValue.push_back(character);
			break;
		}

		if (maxOWS != 0 && ++owsCount > maxOWS) {
			return ClientError::POLICY_TOO_MANY_OWS;
		}
	}

	/* Consume header-value */
	subroutineError = ConsumeHeaderFieldValue();
	if (subroutineError != ClientError::NO_ERROR) {
		return subroutineError;
	}

	/* Store in strings */
	buffers.fieldName.push_back('\0');
	buffers.fieldValue.push_back('\0');

	/* Trim end of OWS's. */
	auto spaceIterator = std::find(std::begin(buffers.fieldValue), std::end(buffers.fieldValue), ' ');
	auto tabIterator = std::find(std::begin(buffers.fieldValue), std::end(buffers.fieldValue), '\t');

	auto endIterator = spaceIterator < tabIterator ? spaceIterator : tabIterator;

	currentRequest.headers.insert({
		std::string(buffers.fieldName.data()),
		std::string(std::begin(buffers.fieldValue), endIterator - 1)
	});

	// Clear buffers (this won't reset the capacity, e.g. the buffer itself).
	buffers.fieldName.clear();
	buffers.fieldValue.clear();

	return ClientError::NO_ERROR;
}

ClientError
Client::ConsumeHeaderFieldValue() noexcept {
	/* obs-fold (optional line folding) isn't supported. */
	while (true) {
		/* Set next character */
		char character; // NOLINT(cppcoreguidelines-init-variables)

		if (!connection->ReadChar(&character)) {
			return ClientError::FAILED_READ_HEADER_FIELD_VALUE;
		}

		if (character == '\r') {
			if (!connection->ReadChar(&character)) {
				return ClientError::FAILED_READ_HEADER_NEWLINE;
			}
			if (character != '\n') {
				return ClientError::INCORRECT_HEADER_FIELD_NEWLINE;
			}
			return ClientError::NO_ERROR;
		}

		auto uc = static_cast<unsigned char>(character);
		if ((uc >= 0x21 && uc <= 0x7E) || // VCHAR
			(uc >= 0x80 && uc <= 0xFF) || // obs-text
			character == ' ' || character == '\t') {	// SP / HTAB
			buffers.fieldValue.push_back(character);
		} else {
			return ClientError::INCORRECT_HEADER_FIELD_VALUE;
		}

		const auto max = server->config().securityPolicies.maxHeaderFieldValueLength;
		if (max != 0 && buffers.fieldValue.size() == max) {
			return ClientError::POLICY_TOO_LONG_HEADER_FIELD_VALUE;
		}
	}
}

ClientError
Client::ConsumeHeaderFieldName() noexcept {
	static const std::array unreservedCharacters
		= { '!', '#', '$', '%', '&', '\'', '*', '+', '-', '.', '^', '_', '`', '|', '~' };

	while (true) {
		char character; // NOLINT(cppcoreguidelines-init-variables)

		if (!connection->ReadChar(&character)) {
			return ClientError::FAILED_READ_HEADER_FIELD_NAME;
		}

		if (character == ':') {
			return ClientError::NO_ERROR;
		}

		if (std::find(std::begin(unreservedCharacters),
					  std::end(unreservedCharacters), character)
			!= std::end(unreservedCharacters) ||
			(character >= '0' && character <= '9') ||
			(character >= 'A' && character <= 'Z') ||
			(character >= 'a' && character <= 'z')) {
			buffers.fieldName.push_back(std::tolower(character));
		} else {
			return ClientError::INCORRECT_HEADER_FIELD_NAME;
		}

		const auto max = server->config().securityPolicies.maxHeaderFieldNameLength;
		if (max != 0 && buffers.fieldName.size() == max) {
			return ClientError::POLICY_TOO_LONG_HEADER_FIELD_NAME;
		}
	}
}

ClientError
Client::ConsumeHeaders() noexcept {
	while (true) {
		char character; // NOLINT(cppcoreguidelines-init-variables)
		if (!connection->ReadChar(&character)) {
			return ClientError::FAILED_READ_HEADER_FIELD_NAME;
		}

		if (character == '\r') {
			if (!connection->ReadChar(&character)) {
				return ClientError::FAILED_READ_HEADER_NEWLINE;
			}
			if (character != '\n') {
				return ClientError::UNEXPECTED_CR_IN_FIELD_NAME;
			}
			return ClientError::NO_ERROR;
		}

		auto error = ConsumeHeaderField(character);
		if (error != ClientError::NO_ERROR) {
			return error;
		}
	}
}

ClientError
Client::ConsumeMethod() noexcept {
	std::vector<char> &buffer = this->currentRequest.method;

	// Reserve 4 octets because GET & POST fit in 4 octets, so no reallocation
	// is needed.
	buffer.reserve(MAGIC_METHOD_AVG_LENGTH);

	while (true) {
		char character = 0;

		if (!connection->ReadChar(&character)) {
			return ClientError::FAILED_READ_METHOD;
		}

		if (character == ' ') {
			if (buffer.empty()) {
				return ClientError::EMPTY_METHOD;
			}
			return ClientError::NO_ERROR;
		}

		// Character validation
		if (!Utils::IsTokenCharacter(character)) {
			return ClientError::INCORRECT_METHOD;
		}

		buffer.push_back(character);

		const auto max = server->config().securityPolicies.maxMethodLength;
		if (max != 0 && buffer.size() == max) {
			return ClientError::POLICY_TOO_LONG_METHOD;
		}
	}
}

ClientError
Client::ConsumePath() noexcept {
	std::vector<char> buffer;
	buffer.reserve(MAGIC_PATH_AVG_LENGTH);

	while (true) {
		char character = 0;

		if (!connection->ReadChar(&character)) {
			return ClientError::FAILED_READ_PATH;
		}

		if (character == ' ') {
			this->currentRequest.path = std::string(std::begin(buffer), std::end(buffer));
			return ClientError::NO_ERROR;
		}

		// Character validation
		if (!Utils::IsPathCharacter(character)) {
			return ClientError::INCORRECT_PATH;
		}

		buffer.push_back(character);

		const auto max = server->config().securityPolicies.maxRequestTargetLength;
		if (max != 0 && buffer.size() == max) {
			return ClientError::POLICY_TOO_LONG_REQUEST_TARGET;
		}
	}
}

ClientError
Client::ConsumeVersion() noexcept {
	std::array<char, 7> expectedChars = {
		'H', 'T', 'T', 'P', '/', '1', '.'
	};

	for (std::size_t i = 0; i < 8; i++) {
		char character;
		if (!connection->ReadChar(&character)) {
			return ClientError::FAILED_READ_VERSION;
		}

		if (i == 7 ?
			(!Utils::IsNumericCharacter(character)) :
			(character != expectedChars[i])) {
			return ClientError::INCORRECT_VERSION;
		}
	}

	// Not storing the version atm.

	return ClientError::NO_ERROR;
}

void
Client::Entrypoint() {
	// Ignore SIGPIPE ~= accessing closed connection
	std::signal(SIGPIPE, SIG_IGN);

	if (!connection->Setup(server->config())) {
		Logger::Error("Client::Entrypoint", "Failed to setup connection!");
		Clean();
		return;
	}

	bool previousRequestSuccess; // NOLINT(cppcoreguidelines-init-variables)

	do {
		previousRequestSuccess = RunMessageExchange();
		ResetExchangeState();
	} while (previousRequestSuccess && persistentConnection);

	Clean();
}

ClientError
Client::ExtractComponentsFromPath() noexcept {
	std::string path(currentRequest.path);
	auto questionMark = path.find('?');

	if (questionMark == std::string::npos) {
		return ClientError::NO_ERROR;
	}

	if (questionMark != path.rfind('?')) {
		// Multiple ?'s in path
		return ClientError::INVALID_PATH_MULTIPLE_QUESTION_MARKS;
	}

	currentRequest.path = path.substr(0, questionMark);
	currentRequest.query = path.substr(questionMark + 1, path.length() - 1 - questionMark);

	return ClientError::NO_ERROR;
}

ClientError
Client::HandleRequest() noexcept {
	const auto maxRequests = server->config().securityPolicies.maxRequestsPerConnection;
	if (!server->config().securityPolicies.maxRequestsCloseImmediately &&
		maxRequests != 0 && ++requestCount > maxRequests) {
		return ClientError::TOO_MANY_REQUESTS_PER_THIS_CONNECTION;
	}

	const auto file = server->fileResolver.Resolve(currentRequest);

	if (!file) {
		return ClientError::FILE_NOT_FOUND;
	}

	auto error = CheckFileLocation(file->Path());
	if (error != ClientError::NO_ERROR) {
		return error;
	}

	if (!SendMetadata(Strings::StatusLines::OK, file->Size(), server->config().mediaTypeFinder.DetectMediaType(file))) {
		return ClientError::FAILED_WRITE_RESPONSE_METADATA;
	}

	if (!currentRequest.IsHead() &&
		!connection->SendFile(file->Handle(), file->Size())) {
		perror("HandleRequest");
		return ClientError::FAILED_WRITE_RESPONSE_BODY;
	}

	return ClientError::NO_ERROR;
}

void
Client::InterpretConnectionHeaders() noexcept {
	if (persistentConnection) {
		auto header = currentRequest.headers.find("connection");
		if (header != std::end(currentRequest.headers) &&
			strcasecmp(header->second.c_str(), "close") == 0) {
			MarkConnectionClosing();
		}
	}
}

void
Client::MarkConnectionClosing() noexcept {
	persistentConnection = false;
}

bool
Client::RecoverError(ClientError error) noexcept {
	static const std::string indexPathTarget("/index.html");
	const CGI::Script *script = nullptr;

	switch (error) {
		case ClientError::FILE_NOT_FOUND:
			script = server->cgi().Lookup(currentRequest);
			if (script != nullptr) {
				return ServeCGI(script);
			}

			if (StringStartsWith(indexPathTarget, currentRequest.path)) {
				return ServeDefaultPage();
			}
			ErrorReporter::ReportError(ErrorReporter::Error::FILE_NOT_FOUND, "Path='" + currentRequest.path + '\'');
			return RecoverErrorFileNotFound();
		case ClientError::EMPTY_METHOD:
			return RecoverErrorBadRequest(Strings::BadRequestMessages::EmptyMethod);

		case ClientError::INCORRECT_HEADER_FIELD_NAME:
			return RecoverErrorBadRequest("invalid header field-name");
		case ClientError::INCORRECT_HEADER_FIELD_NEWLINE:
			return RecoverErrorBadRequest("expected newline (CRLF) after header field");
		case ClientError::INCORRECT_HEADER_FIELD_VALUE:
			return RecoverErrorBadRequest("invalid header field-value");

		case ClientError::INCORRECT_METHOD:
			return RecoverErrorBadRequest("invalid method: not a token as per RFC 7230 section 3.2.6");
		case ClientError::INCORRECT_PATH:
			return RecoverErrorBadRequest("incorrect request-target");
		case ClientError::INCORRECT_CRLF:
			return RecoverErrorBadRequest("request-line should end with a newline (CRLF)");
		case ClientError::INCORRECT_VERSION:
			return RecoverErrorBadRequest("invalid HTTP version as per RFC 7230 section 2.6");

		case ClientError::INVALID_PATH_EMPTY:
			return RecoverErrorBadRequest("request-target was empty");
		case ClientError::INVALID_PATH_NOT_ABSOLUTE:
			return RecoverErrorBadRequest("only absolute-path request-target supported");

		case ClientError::POLICY_TOO_LONG_HEADER_FIELD_NAME:
			return ServeStringRequest(Strings::StatusLines::PayloadTooLarge, MediaTypes::TEXT, Strings::BadRequestMessages::HeaderFieldNameTooLong);
		case ClientError::POLICY_TOO_LONG_HEADER_FIELD_VALUE:
			return ServeStringRequest(Strings::StatusLines::PayloadTooLarge, MediaTypes::TEXT, Strings::BadRequestMessages::HeaderFieldValueTooLong);
		case ClientError::POLICY_TOO_LONG_METHOD:
			return ServeStringRequest(Strings::StatusLines::PayloadTooLarge, MediaTypes::TEXT, Strings::BadRequestMessages::MethodTooLong);
		case ClientError::POLICY_TOO_LONG_REQUEST_TARGET:
			return ServeStringRequest(Strings::StatusLines::URITooLong, MediaTypes::TEXT, Strings::BadRequestMessages::RequestTargetTooLong);
		case ClientError::POLICY_TOO_MANY_OWS:
			return ServeStringRequest(Strings::StatusLines::PayloadTooLarge, MediaTypes::TEXT, Strings::BadRequestMessages::TooManyOWSs);

		case ClientError::TOO_MANY_REQUESTS_PER_THIS_CONNECTION:
			return ServeStringRequest(Strings::StatusLines::TooManyRequests, MediaTypes::HTML, Strings::TooManyRequestsPage);;
		case ClientError::UPGRADE_TO_HTTPS:
		{
			std::string metadata = "Location: https://" + server->config().hostname + currentRequest.path + "\r\n";
			// Notify to close connection
			return SendMetadata(Strings::StatusLines::MovedPermanently, 0, MediaTypes::HTML, metadata.c_str()) && false;
		}
		default:
			break;
	}

	std::stringstream test;
	test << "Error Occurred: " << error;
	Logger::Info("HTTPClient::RecoverError", test.str());
	return false;
}

bool
Client::RecoverErrorBadRequest(const base::String &message) noexcept {
	// TODO this works, but isn't really nice.
	const char prefix[] = "Malformed request: ";
	std::vector<char> buf(19 + message.length() + 1);
	std::copy(std::begin(prefix), std::end(prefix) - 1, std::begin(buf));
	std::copy(std::begin(message), std::end(message), std::begin(buf) + sizeof(prefix) - 1);
	buf.at(message.length() + 19) = '\0';

	// Because the request parsing has abruptly failed, the connection is
	// useless.
	MarkConnectionClosing();

	return ServeStringRequest(Strings::StatusLines::BadRequest, MediaTypes::TEXT, base::String(buf.data(), buf.size()));
}

bool
Client::RecoverErrorFileNotFound() noexcept {
	return ServeStringRequest(Strings::StatusLines::NotFound, MediaTypes::HTML, Strings::NotFoundPage);
}

void
Client::ResetExchangeState() noexcept {
	const auto maxRequests = server->config().securityPolicies.maxRequestsPerConnection;
	if (server->config().securityPolicies.maxRequestsCloseImmediately && maxRequests != 0 && ++requestCount >= maxRequests) {
		// Close the connection.
		MarkConnectionClosing();
		return;
	}

	/* Keep capacity but remove contents */
	if (this->currentRequest.method.capacity() > MAGIC_METHOD_MAX_BUFFER_SIZE_ON_RESET) {
		// How can we optimize this?
		// I want to reserve() but backwards.
		this->currentRequest.method.resize(MAGIC_METHOD_MAX_BUFFER_SIZE_ON_RESET, 0);
		this->currentRequest.method.clear();
	} else {
		this->currentRequest.method.clear();
	}

	// Clear the headers
	this->currentRequest.headers.clear();

	// Clear the rest of the std::string's
	this->currentRequest.path.clear();
	this->currentRequest.query.clear();
}

bool
Client::RunMessageExchange() noexcept {
	auto error = ConsumeMethod();
	if (error != ClientError::NO_ERROR) {
		return RecoverError(error);
	}

	error = ConsumePath();
	if (error != ClientError::NO_ERROR) {
		return RecoverError(error);
	}

	error = ValidateCurrentRequestPath();
	if (error != ClientError::NO_ERROR) {
		return RecoverError(error);
	}

	error = ExtractComponentsFromPath();
	if (error != ClientError::NO_ERROR) {
		return RecoverError(error);
	}

	error = ConsumeVersion();
	if (error != ClientError::NO_ERROR) {
		return RecoverError(error);
	}

	error = ConsumeCRLF();
	if (error != ClientError::NO_ERROR) {
		return RecoverError(error);
	}

	error = ConsumeHeaders();
	if (error != ClientError::NO_ERROR) {
		return RecoverError(error);
	}

	error = CheckUpgradeHTTPS();
	if (error != ClientError::NO_ERROR) {
		return RecoverError(error);
	}

	InterpretConnectionHeaders();

	error = HandleRequest();
	if (error != ClientError::NO_ERROR) {
		return RecoverError(error);
	}

	return true;
}

bool
Client::SendMetadata(const base::String &response, std::size_t contentLength, const MediaType &mediaType, const char *additionalMetaData) noexcept {
	const std::string contentLengthValue = std::to_string(contentLength);
	const bool useHSTS = server->config().useTransportSecurity && !server->config().hsts.empty();
	const std::size_t additionalMetaDataLen = additionalMetaData == nullptr ? 0 : strlen(additionalMetaData);
	const std::string &mediaTypeValue = mediaType.Complete();

	const size_t size =
		 response.length() +
		 20 + contentLengthValue.length() +
		 12 + server->config().serverProductName.length() +
		 (persistentConnection ? 26 : 21) +
		 (useHSTS ? 31 + server->config().hsts.length() : 0) +
		 18 + mediaTypeValue.length() +
		 (mediaType.IncludeCharset() ? 18 : 2) +
		 (additionalMetaData != nullptr ? additionalMetaDataLen : 0) +
		 2;

	std::vector<char> metadata;
	metadata.reserve(size);
	metadata.insert(std::end(metadata), std::cbegin(response), std::cend(response));

	const char contentLengthName[] = "\r\nContent-Length: ";
	metadata.insert(std::end(metadata), std::cbegin(contentLengthName), std::cend(contentLengthName));
	metadata.insert(std::end(metadata), std::cbegin(contentLengthValue), std::cend(contentLengthValue));

	const char serverHeaderName[] = "\r\nServer: ";
	metadata.insert(std::end(metadata), std::cbegin(serverHeaderName), std::cend(serverHeaderName));
	metadata.insert(std::end(metadata), std::cbegin(server->config().serverProductName), std::cend(server->config().serverProductName));

	const char connectionHeaderAlive[] = "\r\nConnection: keep-alive";
	const char connectionHeaderClose[] = "\r\nConnection: close";
	if (persistentConnection) {
		metadata.insert(std::end(metadata), std::cbegin(connectionHeaderAlive), std::cend(connectionHeaderAlive));
	} else {
		metadata.insert(std::end(metadata), std::cbegin(connectionHeaderClose), std::cend(connectionHeaderClose));
	}

	if (useHSTS) {
		const char stsHeader[] = "\r\nStrict-Transport-Security: ";
		metadata.insert(std::end(metadata), std::cbegin(stsHeader), std::cend(stsHeader));
		metadata.insert(std::end(metadata), std::cbegin(server->config().hsts), std::cend(server->config().hsts));
	}

	const char contentTypeName[] = "\r\nContent-Type: ";
	metadata.insert(std::end(metadata), std::cbegin(contentTypeName), std::cend(contentTypeName));
	metadata.insert(std::end(metadata), std::cbegin(mediaTypeValue), std::cend(mediaTypeValue));

	const char crlf[] = "\r\n";
	const char charset[] = ";charset=utf-8\r\n";
	if (mediaType.IncludeCharset()) {
		metadata.insert(std::end(metadata), std::begin(charset), std::end(charset));
	} else {
		metadata.insert(std::end(metadata), std::begin(crlf), std::end(crlf));
	}

	if (additionalMetaData) {
		metadata.insert(std::end(metadata), additionalMetaData, additionalMetaData + additionalMetaDataLen);
	}

	metadata.insert(std::end(metadata), std::begin(crlf), std::end(crlf));
	std::stringstream report;
	report << "Report. Used " << metadata.size() << " octets out of " << metadata.capacity() << " allocated with size=" << size;
	Logger::Debug(__PRETTY_FUNCTION__, report.str());

	return connection->WriteBaseString(base::String(metadata.data(), metadata.size()));
}

bool
Client::ServeCGI(const CGI::Script *) noexcept {
	return true;
}

bool
Client::ServeDefaultPage() noexcept {
	return ServeStringRequest(Strings::StatusLines::OK, MediaTypes::HTML, Strings::DefaultWebPage);
}

bool
Client::ServeStringRequest(const base::String &responseLine,
						   const MediaType &type,
						   const base::String &body) noexcept {
	if (!SendMetadata(responseLine, body.length(), type)) {
		return false;
	}

	if (currentRequest.IsHead()) {
		return true;
	}

	return connection->WriteBaseString(body);
}

ClientError
Client::ValidateCurrentRequestPath() const noexcept {
	if (currentRequest.path.empty()) {
		return ClientError::INVALID_PATH_EMPTY;
	}

	// We should preferably support:
	//   - * for the OPTIONS method.
	//   - the 'absolute-form' request-target type
	//   -
	if (currentRequest.path[0] != '/') {
		return ClientError::INVALID_PATH_NOT_ABSOLUTE;
	}

	return ClientError::NO_ERROR;
}

} // namespace HTTP
