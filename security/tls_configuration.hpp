#pragma once

/**
 * Copyright (C) 2020 Tristan. All Rights Reserved.
 * This file is licensed under the BSD 2-Clause license.
 * See the COPYING file for licensing information.
 */

#include <string>

namespace Security {

struct TLSConfiguration {

	[[nodiscard]] bool
	CreateContext();

	std::string CertificateFile;
	std::string PrivateKeyFile;

	// The underlying object depends on the TLS library.
	// OpenSSL: "void *" is actually "SSL_CTX *"
	void *context{ nullptr };

};

} // namespace Security
