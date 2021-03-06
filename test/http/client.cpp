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

// 	Configuration config;

class ClientTest : public ::testing::Test {
protected:
	ClientTest() : server(HTTP::Configuration(finder, secPolicies, tlsConfig), cgiManager), client(&server) {
		client.connection = std::make_unique<Connection>(&internalData);
	}

	// void TearDown() override {}

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

// Test if the Connection setup is correct
TEST_F(ClientTest, TestConnectionSetup) {
	char c = 0;
	internalData.input.push_back('a');

	ASSERT_TRUE(client.connection->ReadChar(&c));
	ASSERT_EQ(c, 'a');
	ASSERT_EQ(internalData.input.size(), 0);
}

TEST_F(ClientTest, ConsumeMethodNormal) {
	for (const std::string &method : { "GET ", "POST ", "UPDATEREDIRECTREF " }) {
		ensureInputSize(method.length());
		std::copy(std::crbegin(method), std::crend(method), std::begin(internalData.input));

		/* Clear buffer(s) */
		client.currentRequest.method.clear();

		auto error = client.ConsumeMethod();
		ASSERT_EQ_CLIENT_ERROR(error, HTTP::ClientError::NO_ERROR);
	}
}

TEST_F(ClientTest, ConsumeMethodZeroLength) {
	ensureInputSize(1);
	internalData.input[0] = ' ';

	/* Clear buffer(s) */
	client.currentRequest.method.clear();

	auto error = client.ConsumeMethod();
	ASSERT_EQ_CLIENT_ERROR(error, HTTP::ClientError::EMPTY_METHOD);
}

TEST_F(ClientTest, ConsumeMethodRandomToken) {
	for (size_t i = 0; i < 100; i++) {
		static auto &tchar =
			"!#$%&'*+-.^_`|~"
			"0123456789"
			"abcdefghijklmnopqrstuvwxyz"
			"ABCDEFGHIJKLMNOPQRSTUVWXYZ";

		thread_local static std::mt19937 randomGenerator{ std::random_device{}() };
		thread_local static std::uniform_int_distribution<std::string::size_type> pick(0, sizeof(tchar) - 2);
		thread_local static std::uniform_int_distribution<std::string::size_type> randomSize(1, 10);

		auto &buf = internalData.input;
		std::size_t size = randomSize(randomGenerator);
		buf.resize(size + 1);
		buf[0] = ' ';

		std::generate_n(std::begin(buf) + 1, size, [](){
			return tchar[pick(randomGenerator)];
		});

		auto copyOfBuf = buf;
		copyOfBuf.push_back('\0');

		/* Clear buffer(s) */
		client.currentRequest.method.clear();

		auto error = client.ConsumeMethod();
		ASSERT_EQ(error, HTTP::ClientError::NO_ERROR) << "Error: " << ClientErrorToString(error)
			<< "\nConsumer didn't hold find: \"" << copyOfBuf.data()
			<< "\" a correct method (i.e. token i.e. 1*tchar) string, while it is a valid method string.";
	}
}
