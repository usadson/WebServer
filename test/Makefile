# Copyright (C) 2020 Tristan. All Rights Reserved.
# See the COPYING file for licensing information.

CXXFLAGS += -I..

TEST_LDFLAGS = $(LDFLAGS) -L/usr/local/lib -lgtest -lpthread
TEST_GENERAL_DEPENDENCIES = \
	bin/base/error_reporter.o \
	bin/base/logger.o \
	bin/base/media_type.o \
	bin/base/strings.o \
	bin/cgi/manager.o \
	bin/http/client.o \
	bin/http/client_error.o \
	bin/http/server.o \
	bin/http/server_launch_error.o \
	bin/io/file.o \
	bin/io/file_resolver.o \
	bin/security/tls_configuration.o

test: \
	bin/test/http/client.o \
	bin/test/http/policies.o \
	bin/test/http/utils.o

bin/test/http/client.o: test/http/client.cpp \
	http/client.cpp \
	base/media_type.hpp \
	cgi/manager.hpp \
	connection/connection.hpp \
	connection/memory_userdata.hpp \
	http/client.hpp \
	http/client_error.hpp \
	http/configuration.hpp \
	http/server.hpp \
	security/policies.hpp \
	bin/connection/memory_connection.o
	$(CXX) $(CXXFLAGS) -o $@ test/http/client.cpp $(TEST_GENERAL_DEPENDENCIES) bin/connection/memory_connection.o $(TEST_LDFLAGS)

bin/test/http/policies.o: test/http/policies.cpp \
	http/client.cpp \
	base/media_type.hpp \
	cgi/manager.hpp \
	connection/connection.hpp \
	connection/memory_userdata.hpp \
	http/client.hpp \
	http/client_error.hpp \
	http/configuration.hpp \
	http/server.hpp \
	security/policies.hpp \
	bin/connection/memory_connection.o
	$(CXX) $(CXXFLAGS) -o $@ test/http/policies.cpp $(TEST_GENERAL_DEPENDENCIES) bin/connection/memory_connection.o $(TEST_LDFLAGS)

bin/test/http/server.o: test/http/server.cpp \
	http/server.cpp \
	http/server.hpp
	$(CXX) $(CXXFLAGS) -o $@ test/http/server.cpp bin/connection/connection.o bin/http/client.o bin/http/server.o bin/http/server_launch_error.o -L/usr/local/lib -lgtest -lpthread

bin/test/http/utils.o: test/http/utils.cpp \
	http/utils.hpp
	$(CXX) $(CXXFLAGS) -o $@ test/http/utils.cpp -L/usr/local/lib -lgtest -lpthread

test-exec:
	bin/test/http/client.o
	bin/test/http/utils.o
