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
	setInput(std::string input) {
		internalData.input.resize(input.length());
		std::copy(std::crbegin(input), std::crend(input), std::begin(internalData.input));
	}
};

// Function: ConsumeMethod
// Policy:   maxMethodLength
// Error:    POLICY_TOO_LONG_METHOD
TEST_F(PoliciesTest, ConsumeMethod_MaxMethodLength_Unlimited) {
	secPolicies.maxMethodLength = 0;
	setInput("VERYVERYLONGSTRINGASTESTOFMETHODwecanalsoIncludeLowerCaSE / HTTP/1.1\r\n");
	auto error = client.ConsumeMethod();
	ASSERT_EQ_CLIENT_ERROR(error, HTTP::ClientError::NO_ERROR);
}

TEST_F(PoliciesTest, ConsumeMethod_MaxMethodLength_WithinBounds) {
	secPolicies.maxMethodLength = 4;
	setInput("GET / HTTP/1.1\r\n");
	auto error = client.ConsumeMethod();
	ASSERT_EQ_CLIENT_ERROR(error, HTTP::ClientError::NO_ERROR);
}

TEST_F(PoliciesTest, ConsumeMethod_MaxMethodLength_OutOfBounds) {
	secPolicies.maxMethodLength = 3;
	setInput("GET / HTTP/1.1\r\n");
	auto error = client.ConsumeMethod();
	ASSERT_EQ_CLIENT_ERROR(error, HTTP::ClientError::POLICY_TOO_LONG_METHOD);
}

// Function: ConsumePath
// Policy:   maxRequestTargetLength
// Error:    POLICY_TOO_LONG_REQUEST_TARGET
TEST_F(PoliciesTest, ConsumePath_MaxRequestTargetLength_Unlimited) {
	secPolicies.maxRequestTargetLength = 0;
	setInput("GET / HTTP/1.1\r\n");
	auto error = client.ConsumePath();
	ASSERT_EQ_CLIENT_ERROR(error, HTTP::ClientError::NO_ERROR);
}

TEST_F(PoliciesTest, ConsumePath_MaxRequestTargetLength_WithinBounds) {
	secPolicies.maxRequestTargetLength = 20;
	setInput("GET / HTTP/1.1\r\n");
	auto error = client.ConsumePath();
	ASSERT_EQ_CLIENT_ERROR(error, HTTP::ClientError::NO_ERROR);
}

TEST_F(PoliciesTest, ConsumePath_MaxRequestTargetLength_OutOfBounds) {
	secPolicies.maxRequestTargetLength = 3;
	setInput("GET / HTTP/1.1\r\n");
	auto error = client.ConsumePath();
	ASSERT_EQ_CLIENT_ERROR(error, HTTP::ClientError::POLICY_TOO_LONG_REQUEST_TARGET);
}

// Function: ConsumeHeaderFieldName
// Policy:   maxHeaderFieldNameLength
// Error:    POLICY_TOO_LONG_HEADER_FIELD_NAME
TEST_F(PoliciesTest, ConsumeHeaderFieldName_maxHeaderFieldNameLength_Unlimited) {
	secPolicies.maxHeaderFieldNameLength = 0;
	setInput("This-Is-A-Header-Name-With-Allowed-Characters-And-Is-Very-Long: value\r\n");
	auto error = client.ConsumeHeaderFieldName();
	ASSERT_EQ_CLIENT_ERROR(error, HTTP::ClientError::NO_ERROR);
}

TEST_F(PoliciesTest, ConsumeHeaderFieldName_maxHeaderFieldNameLength_WithinBounds) {
	secPolicies.maxHeaderFieldNameLength = 20;
	setInput("Header-Field-Name: value\r\n");
	auto error = client.ConsumeHeaderFieldName();
	ASSERT_EQ_CLIENT_ERROR(error, HTTP::ClientError::NO_ERROR);
}

TEST_F(PoliciesTest, ConsumeHeaderFieldName_maxHeaderFieldNameLength_OutOfBounds) {
	secPolicies.maxHeaderFieldNameLength = 3;
	setInput("This-Is-A-Header-Name-With-Allowed-Characters-And-Is-Very-Long: value\r\n");
	auto error = client.ConsumeHeaderFieldName();
	ASSERT_EQ_CLIENT_ERROR(error, HTTP::ClientError::POLICY_TOO_LONG_HEADER_FIELD_NAME);
}

int
main(int argc, char **argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
