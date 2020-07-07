/**
 * Copyright (C) 2020 Tristan. All Rights Reserved.
 * This file is licensed under the BSD 2-Clause license.
 * See the COPYING file for licensing information.
 */

#include <gtest/gtest.h>

#define TESTING_VISIBILITY public

#include "connection/memory_userdata.hpp"
#include "http/client.hpp"
#include "http/server.hpp"
#include "http/configuration.hpp"
#include "security/policies.hpp"

namespace HTTP {

// 	Configuration config;

class ClientTest : public ::testing::Test {
protected:
	ClientTest() : server(secPolicies), client(&server) {
		client.connection = std::make_unique<Connection>(0, server.config().useTransportSecurity, &internalData);
	}

	// void TearDown() override {}

	Security::Policies secPolicies;
	Server server;
	MemoryUserData internalData;
	Client client;
};

// Test if the Connection setup is correct
TEST_F(ClientTest, TestConnectionSetup) {
	char c = 0;
	internalData.input.push_back('a');

	ASSERT_TRUE(client.connection->ReadChar(&c));
	ASSERT_EQ(c, 'a');
	ASSERT_EQ(internalData.input.size(), 0);
}

} // namespace HTTP

int
main(int argc, char **argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
