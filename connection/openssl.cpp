/**
 * Copyright (C) 2020 Tristan. All Rights Reserved.
 * This file is licensed under the BSD 2-Clause license.
 * See the COPYING file for licensing information.
 */

#define CONNECTION_ALLOW_EXTENDED_VISIBILITY

#include "security_internals.hpp"

#include <array>
#include <sstream>

#include <cstdio>
#include <unistd.h>

#include <openssl/err.h>
#include <openssl/ssl.h>

#include "base/logger.hpp"
#include "connection/connection.hpp"

static const char *
GetSSLErrorString(int);

void
ConnectionSecureInternals::Destruct(Connection *connection) {
	auto *ssl = reinterpret_cast<SSL *>(connection->securityContext);
	SSL_shutdown(ssl);
	SSL_free(ssl);
}

bool
ConnectionSecureInternals::Setup(Connection *connection, const HTTP::Configuration &configuration) {
	auto *ctx = SSL_new(reinterpret_cast<SSL_CTX *>(configuration.tlsConfiguration.context));
	connection->securityContext = ctx;
	if (ctx == nullptr) {
		ERR_print_errors_fp(stderr);
		std::stringstream error;
		error << "SSL_new failed. TLS context is " << configuration.tlsConfiguration.context;
		Logger::Error("CSI[OSSL]::Setup", error.str());
		return false;
	}

	if (SSL_set_fd(ctx, connection->internalSocket) == 0) {
		ERR_print_errors_fp(stderr);
		Logger::Error("CSI[OSSL]::Setup", "Failed to set socket (FD)");
		return false;
	}

	int status = SSL_accept(ctx);
	if (status != 1) {
		ERR_print_errors_fp(stderr);
		Logger::Error("CSI[OSSL]::Setup", "Failed to setup TLS communication.");
		std::stringstream error;
		error << "This is what OpenSSL has to say about it: \"";
		error << GetSSLErrorString(SSL_get_error(ctx, status)) << '"';
		Logger::Error("CSI[OSSL]::Setup", error.str());
		error = std::stringstream();
		error << "This is what error has to say about it: \"";
		error << std::strerror(errno) << '"';
		Logger::Error("CSI[OSSL]::Setup", error.str());
		return false;
	}

	status = SSL_do_handshake(ctx);
	if (status != 1) {
		ERR_print_errors_fp(stderr);
		Logger::Error("CSI[OSSL]::Setup", "Failed to perform TLS handshake.");
		std::stringstream error;
		error << "This is what OpenSSL has to say about it: \"";
		error << GetSSLErrorString(SSL_get_error(ctx, status)) << '"';
		Logger::Error("CSI[OSSL]::Setup", error.str());
		error = std::stringstream();
		error << "This is what error has to say about it: \"";
		error << std::strerror(errno) << '"';
		Logger::Error("Connection::Setup", error.str());
		return false;
	}

	return true;
}

bool
ConnectionSecureInternals::ReadChar(const Connection *connection, char *buf) {
	auto status = SSL_read(reinterpret_cast<SSL *>(connection->securityContext), buf, 1);

	if (status == 1) {
		return true;
	}

	std::stringstream error;
	error << "SSL_read failed. securityContext is " << connection->securityContext;
	Logger::Error("CSI[OSSL]::ReadChar", error.str());
	ERR_print_errors_fp(stderr);
	return false;
}

bool
ConnectionSecureInternals::SendFile(Connection *connection, int fd, std::size_t count) {
	std::array<char, 4096> buffer {};
	do {
		ssize_t result = read(fd, buffer.data(), 4096);
		if (result == -1) {
			return false;
		}
		count -= result;
		do {
			ssize_t writeResult = SSL_write(reinterpret_cast<SSL *>(connection->securityContext), buffer.data(), result);
			if (writeResult == -1) {
				connection->hasWriteFailed = true;
				return false;
			}
			result -= writeResult;
		} while (result != 0);
	} while (count != 0);

	return true;
}

int
ConnectionSecureInternals::Write(Connection *connection, const char *str, std::size_t len) {
	return SSL_write(reinterpret_cast<SSL *>(connection->securityContext), str, len);
}

static const char *
GetSSLErrorString(int error) {
	switch (error) {
		case SSL_ERROR_NONE:
			return "SSL_ERROR_NONE";
		case SSL_ERROR_ZERO_RETURN:
			return "SSL_ERROR_ZERO_RETURN";
		case SSL_ERROR_WANT_READ:
			return "SSL_ERROR_WANT_READ or WRITE";
		case SSL_ERROR_WANT_CONNECT:
			return "SSL_ERROR_WANT_CONNECT or ACCEPT";
		case SSL_ERROR_WANT_X509_LOOKUP:
			return "SSL_ERROR_WANT_X509_LOOKUP";
		case SSL_ERROR_SYSCALL:
			return "SSL_ERROR_SYSCALL";
		case SSL_ERROR_SSL:
			return "SSL_ERROR_SSL";
		default:
			return "no string for this OpenSSL error code";
	}
}
