/**
 * Copyright (C) 2020 Tristan. All Rights Reserved.
 * This file is licensed under the BSD 2-Clause license.
 * See the COPYING file for licensing information.
 */

#include "tls_configuration.hpp"

#define TLS_LIBRARY_OPENSSL

#if defined(TLS_LIBRARY_OPENSSL)
#include <openssl/err.h>
#include <openssl/ssl.h>
#else
#error Unsupported TLS Library
#endif

bool
Security::TLSConfiguration::CreateContext() {
	SSL_load_error_strings();
	OpenSSL_add_ssl_algorithms();

	auto ctx = SSL_CTX_new(SSLv23_method());
	this->context = ctx;

	if (SSL_CTX_use_certificate_file(ctx, CertificateFile.c_str(), SSL_FILETYPE_PEM) != 1) {
		ERR_print_errors_fp(stderr);
		return false;
	}

	if (SSL_CTX_use_PrivateKey_file(ctx, PrivateKeyFile.c_str(), SSL_FILETYPE_PEM) <= 0) {
		ERR_print_errors_fp(stderr);
		return 0;
	}

	SSL_CTX_set_min_proto_version(ctx, TLS1_2_VERSION);

	return true;
}
