/**
 * Copyright (C) 2020 Tristan. All Rights Reserved.
 * This file is licensed under the BSD 2-Clause license.
 * See the COPYING file for licensing information.
 */

#define CONNECTION_MEMORY_VARIANT

#include <algorithm>
#include <initializer_list>
#include <iterator>
#include <memory>
#include <ostream>
#include <random>
#include <string>

#include <cstddef>

#include <gtest/gtest.h>

#define TESTING_VISIBILITY public
#define ASSERT_EQ_CLIENT_ERROR(a, b) ASSERT_EQ(a, b) << ClientErrorToString(a) << " should be " << ClientErrorToString(b);

#include "base/media_type.hpp"
#include "cgi/manager.hpp"
#include "connection/connection.hpp"
#include "connection/memory_userdata.hpp"
#include "http/client.hpp"
#include "http/client_error.hpp"
#include "http/configuration.hpp"
#include "http/server.hpp"
#include "security/policies.hpp"
#include "security/tls_configuration.hpp"

class PoliciesTest : public ::testing::Test {
protected:
	PoliciesTest() : server(HTTP::Configuration(finder, secPolicies, tlsConfig), cgiManager), client(&server) {
		client.connection = std::make_unique<Connection>(&internalData);
	}

	CGI::Manager cgiManager;
	MediaTypeFinder finder;
	Security::Policies secPolicies;
	Security::TLSConfiguration tlsConfig;
	HTTP::Server server;
	MemoryUserData internalData{};
	HTTP::Client client;

	void
	ensureInputSize(std::size_t size) {
		internalData.input.resize(size);
	}
};

TEST_F(PoliciesTest, ConsumeMethod_MaxMethodLength_Unlimited) {
	secPolicies.maxMethodLength = 0;
	std::string method("VERYVERYLONGSTRINGASTESTOFMETHODwecanalsoIncludeLowerCaSE / HTTP/1.1\r\n");
	ensureInputSize(method.length());
	std::copy(std::crbegin(method), std::crend(method), std::begin(internalData.input));
	auto error = client.ConsumeMethod();
	ASSERT_EQ_CLIENT_ERROR(error, HTTP::ClientError::NO_ERROR);
}

TEST_F(PoliciesTest, ConsumeMethod_MaxMethodLength_WithinBounds) {
	secPolicies.maxMethodLength = 4;
	std::string method("GET / HTTP/1.1\r\n");
	ensureInputSize(method.length());
	std::copy(std::crbegin(method), std::crend(method), std::begin(internalData.input));
	auto error = client.ConsumeMethod();
	ASSERT_EQ_CLIENT_ERROR(error, HTTP::ClientError::NO_ERROR);
}

TEST_F(PoliciesTest, ConsumeMethod_MaxMethodLength_OutOfBounds) {
	secPolicies.maxMethodLength = 3;
	std::string method("GET / HTTP/1.1\r\n");
	ensureInputSize(method.length());
	std::copy(std::crbegin(method), std::crend(method), std::begin(internalData.input));
	auto error = client.ConsumeMethod();
	ASSERT_EQ_CLIENT_ERROR(error, HTTP::ClientError::POLICY_TOO_LONG_METHOD);
}

int
main(int argc, char **argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
