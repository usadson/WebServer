/**
 * Copyright (C) 2020 Tristan. All Rights Reserved.
 * This file is licensed under the BSD 2-Clause license.
 * See the COPYING file for licensing information.
 */

#include "process.hpp"

constexpr uid_t superuserUID = 0;
constexpr gid_t superuserGID = 0;

namespace Security {

	PrivilegesStatus
	Process::DropPrivileges(gid_t group, uid_t user) noexcept {
		if (setgid(group) == -1) {
			return PrivilegesStatus::UNABLE_DROP_GROUP;
		}

		if (setuid(user) == -1) {
			return PrivilegesStatus::UNABLE_DROP_USER;
		}

		if (setuid(superuserUID) == 0) {
			return PrivilegesStatus::SWITCHABLE_TO_SUPERUSER;
		}

		if (setgid(superuserGID) == 0) {
			return PrivilegesStatus::SWITCHABLE_TO_SUPERUSER_GROUP;
		}

		return PrivilegesStatus::OK;
	}

} // namespace Security
