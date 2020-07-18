#pragma once

/**
 * Copyright (C) 2020 Tristan. All Rights Reserved.
 * This file is licensed under the BSD 2-Clause license.
 * See the COPYING file for licensing information.
 */

#include <string>

namespace Security {

// The TLS configuration is how the security should be handled within Wizard.
// This configuration isn't implementation specific, and should be implemented
// and used with that in mind.
struct TLSConfiguration {

	~TLSConfiguration();

	[[nodiscard]] bool
	CreateContext();

	// This is the full path for the certificate file. The certificate roughly
	// speaking specifies the ownership of the domain, to ensure no
	// man-in-the-middle attack can be performed.
	// Read more at https://en.wikipedia.org/wiki/Public_key_certificate
	// Read more at https://en.wikipedia.org/wiki/Man-in-the-middle_attack
	std::string certificateFile;

	// This is the full path for the certificate chain file. As each certificate
	// depends on another (except for root certificates), the final certificate
	// (simply called certificate) should be accompanied by the other related
	// certificates. This file contains those very certificates.
	// Read more at https://en.wikipedia.org/wiki/Chain_of_trust
	std::string chainFile;

	// This is the full path for the private key file. The private key contains
	// information needed to succesfully decrypt a TLS message. As the name
	// suggest, this file should be kept private.
	// Read more at https://en.wikipedia.org/wiki/Public-key_cryptography
	std::string privateKeyFile;

	// This cipher list is a list of ciphers that TLSv1.2 should use.
	// Read more at https://www.openssl.org/docs/man1.1.1/man1/ciphers.html
	// TODO better further reading source
	std::string cipherList;

	// This cipher list is a list of cipher suites that TLSv1.3 should use.
	// Read more at https://www.openssl.org/docs/man1.1.1/man3/SSL_set_ciphersuites.html
	// TODO better further reading source
	std::string cipherSuites;

	// The context is dependent on the TLS implementation, but often represents
	// the compiled configuration, with loaded certificates and all.
	//
	// e.g. OpenSSL: "void *" is actually "SSL_CTX *"
	void *context{ nullptr };

};

} // namespace Security
