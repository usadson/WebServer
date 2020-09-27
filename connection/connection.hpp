#pragma once

/**
 * Copyright (C) 2020 Tristan. All Rights Reserved.
 * This file is licensed under the BSD 2-Clause license.
 * See the COPYING file for licensing information.
 */

#include <iosfwd>
#include <iostream>
#include <string>

#include <netinet/in.h>
#include <sys/socket.h>

#include "base/string.hpp"

// ForwardDecl from http/configuration.hpp
namespace HTTP {
struct Configuration;
} // namespace HTTP

class Connection {
public:
#ifndef CONNECTION_MEMORY_VARIANT
	inline Connection(int socket, bool useTransportSecurity, void *userData=nullptr) noexcept
	: userData(userData), internalSocket(socket), useTransportSecurity(useTransportSecurity) {
#ifndef HTTP_SERVER_FORCE_IPV4
		struct sockaddr_in6 address;
#else
		struct sockaddr_in address;
#endif
		socklen_t len = sizeof(address);
		if (getpeername(socket, reinterpret_cast<struct sockaddr *>(&address), &len) == 0) {
#ifndef HTTP_SERVER_FORCE_IPV4
			std::cout << "IPv6 Address: " << std::hex;
			for (std::size_t i = 0; i < 16; i++) {
				std::cout << static_cast<int>(address.sin6_addr.s6_addr[i]);

				if (i != 15) {
					std::cout << '.';
				}
			}
			std::cout << std::dec << '\n';
			[&address, this]() mutable {
				isLocalhost = false;
				for (std::size_t i = 0; i < 15; i++) {
					if (address.sin6_addr.s6_addr[i] != 0x00)
						return;
				}
				if (address.sin6_addr.s6_addr[15] != 0x01)
					return;
				isLocalhost = true;
			}();
			if (!isLocalhost) {
				[&address, this]() mutable {
					isLocalhost = false;
					for (std::size_t i = 0; i < 10; i++) {
						if (address.sin6_addr.s6_addr[i] != 0x00)
							return;
					}
					if (address.sin6_addr.s6_addr[10] != 0xff ||
						address.sin6_addr.s6_addr[11] != 0xff ||
						address.sin6_addr.s6_addr[12] != 0x7f ||
						address.sin6_addr.s6_addr[13] != 0x00 ||
						address.sin6_addr.s6_addr[14] != 0x00 ||
						address.sin6_addr.s6_addr[15] != 0x01
					)
						return;
					isLocalhost = true;
				}();
			}
#else
			std::cout << "IPv4 Address: ";
			for (std::size_t i = 0; i < 4; i++) {
				std::cout << address.sin_addr.s_addr[i];
			}
			std::cout << '\n';
#endif
		}
	}
#else
	inline explicit Connection(void *userData=nullptr) noexcept
		: userData(userData) {
	}
#endif /* CONNECTION_MEMORY_VARIANT */

	~Connection() noexcept;

	// Will read a single character from the source, either directly from the
	// socket or from the TLS wrapper.
	//
	// This function, being part of the network stack, can fail. There can be
	// many reasons for failure, but in general it means that the connection
	// has either been closed, or a catastrophic failure has occurred.
	//
	// The [bool] return value specifies whether or not the read was succesful.
	// The [char *] is an output field for the character.
	//
	// NOTE: If the [bool] return value is false, the value of [char *] is
	// undefined.
	// NOTE: If [char *] is nullptr, false is always returned.
	[[nodiscard]] bool
	ReadChar(char *) const noexcept;

	[[nodiscard]] bool
	SendFile(int fd, std::size_t count) noexcept;

	// Will setup the connection. If the configuration specifies the use of TLS,
	// it will setup this as well.
	//
	// Won't/can't modify the [configuration] parameter.
	//
	// Returns success status
	[[nodiscard]] bool
	Setup(const HTTP::Configuration &) noexcept;

	// Writes the contents of the string. Doesn't include the null termination
	// character.
	//
	// Won't/can't modify the [string] parameter.
	//
	// Returns success status
	[[nodiscard]] bool
	WriteBaseString(const base::String &) noexcept;

	[[nodiscard]] inline constexpr bool
	IsLocalhost() const noexcept {
		return isLocalhost;
	}

	// Is used by memory_connection.hpp but isn't used in the normal
	// implementation.
	void *userData;

	// Will be modified by Setup and its descendants if TLS is enabled.
	void *securityContext{ nullptr };

	bool isLocalhost{ false };

	#ifndef CONNECTION_MEMORY_VARIANT
#ifndef CONNECTION_ALLOW_EXTENDED_VISIBILITY
private:
#endif
	bool hasWriteFailed{ false };
	const int internalSocket;
	const bool useTransportSecurity;
#endif /* CONNECTION_MEMORY_VARIANT */
};
