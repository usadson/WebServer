#pragma once

/**
 * Copyright (C) 2020 Tristan. All Rights Reserved.
 * This file is licensed under the BSD 2-Clause license.
 * See the COPYING file for licensing information.
 */

#include <string>

namespace Security {

struct TLSConfiguration {

	std::string CertificateFile;
	std::string PrivateKeyFile;

};

} // namespace Security
