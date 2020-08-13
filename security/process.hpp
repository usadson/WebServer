#pragma once

/**
 * Copyright (C) 2020 Tristan. All Rights Reserved.
 * This file is licensed under the BSD 2-Clause license.
 * See the COPYING file for licensing information.
 */

#include <unistd.h>

namespace Security {

	enum class PrivilegesStatus {
		OK,
		SWITCHABLE_TO_SUPERUSER,
		SWITCHABLE_TO_SUPERUSER_GROUP,
		UNABLE_DROP_GROUP,
		UNABLE_DROP_USER,
	};

	namespace Process {

		[[nodiscard]] PrivilegesStatus
		DropPrivileges(gid_t group, uid_t user) noexcept;

	} // namespace Process

} // namespace Security
