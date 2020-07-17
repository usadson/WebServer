/**
 * Copyright (C) 2020 Tristan. All Rights Reserved.
 * This file is licensed under the BSD 2-Clause license.
 * See the COPYING file for licensing information.
 */

#include <cstddef> // for std::size_t

#include "connection/connection.hpp"
#include "http/configuration.hpp"

namespace ConnectionSecureInternals {

void
Destruct(Connection *connection);

bool
Setup(Connection *connection, const HTTP::Configuration &configuration);

bool
ReadChar(const Connection *connection, char *buf);

bool
SendFile(Connection *connection, int fd, std::size_t count);

int
Write(Connection *connection, const char *str, std::size_t len);

} // namespace ConnectionSecureInternals
