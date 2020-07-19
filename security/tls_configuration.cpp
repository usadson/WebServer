/**
 * Copyright (C) 2020 Tristan. All Rights Reserved.
 * This file is licensed under the BSD 2-Clause license.
 * See the COPYING file for licensing information.
 */

#include "tls_configuration.hpp"

#define TLS_LIBRARY_OPENSSL

#include <cstdlib>

#if defined(TLS_LIBRARY_OPENSSL)
#include <openssl/err.h>
#include <openssl/ssl.h>
#else
#error Unsupported TLS Library
#endif

#include "base/logger.hpp"

Security::TLSConfiguration::~TLSConfiguration() {
	SSL_CTX_free(reinterpret_cast<SSL_CTX *>(context));
	EVP_cleanup();
}

bool
Security::TLSConfiguration::CreateContext() {
	SSL_load_error_strings();
	OpenSSL_add_ssl_algorithms();

	auto *ctx = SSL_CTX_new(SSLv23_method());

	if (ctx == nullptr) {
		return false;
	}

	this->context = ctx;

	if (SSL_CTX_use_certificate_file(ctx, certificateFile.c_str(), SSL_FILETYPE_PEM) != 1) {
		Logger::Error("TLSConfiguration::CreateContext", "Failed to load certificate file!");
		ERR_print_errors_fp(stderr);
		return false;
	}

	if (SSL_CTX_use_PrivateKey_file(ctx, privateKeyFile.c_str(), SSL_FILETYPE_PEM) <= 0) {
		Logger::Error("TLSConfiguration::CreateContext", "Failed to load private key file!");
		ERR_print_errors_fp(stderr);
		return false;
	}

	FILE *file = std::fopen(chainFile.c_str(), "r");

	if (file == nullptr) {
		Logger::Error("TLSConfiguration::CreateContext", "Failed to open chain file");
		return false;
	}

	while (X509 *cert = PEM_read_X509(file, nullptr, nullptr, nullptr)) {
		if (!SSL_CTX_add_extra_chain_cert(ctx, cert)) {
			ERR_print_errors_fp(stderr);
			Logger::Error("TLSConfiguration::CreateContext", "Failed to add extra chain certificate");
			fclose(file);
			X509_free(cert);
			return false;
		}
	}

	// Clear errors from PEM_read_X509 since the error stack probably contains
	// an error regarding an EOF in the chain file.
	ERR_clear_error();

	std::fclose(file);

	SSL_CTX_set_min_proto_version(ctx, TLS1_2_VERSION);

	if (!cipherList.empty() &&
		!SSL_CTX_set_cipher_list(ctx, cipherList.c_str())) {
		ERR_print_errors_fp(stderr);
		return false;
	}

	if (!cipherSuites.empty() &&
		!SSL_CTX_set_ciphersuites(ctx, cipherSuites.c_str())) {
		ERR_print_errors_fp(stderr);
		return false;
	}

	ERR_print_errors_fp(stderr);

	return true;
}
