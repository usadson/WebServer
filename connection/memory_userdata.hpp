#pragma once

/**
 * Copyright (C) 2020 Tristan. All Rights Reserved.
 * This file is licensed under the BSD 2-Clause license.
 * See the COPYING file for licensing information.
 */

#include <iosfwd>
#include <vector>

struct MemoryUserData {
	std::vector<char> input{};
	std::vector<char> output{};
	bool writeSendFile{};
};
