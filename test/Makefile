# Copyright (C) 2020 Tristan. All Rights Reserved.
# See the COPYING file for licensing information.

CXXFLAGS += -I..

test: \
	bin/test/http/server.o \
	bin/test/http/utils.o

bin/test/http/server.o: test/http/server.cpp \
	http/server.cpp \
	http/server.hpp
	$(CXX) $(CXXFLAGS) -o $@ test/http/server.cpp bin/connection/connection.o bin/http/client.o bin/http/server.o bin/http/server_launch_error.o -L/usr/local/lib -lgtest -lpthread

bin/test/http/utils.o: test/http/utils.cpp \
	http/utils.hpp
	$(CXX) $(CXXFLAGS) -o $@ test/http/utils.cpp -L/usr/local/lib -lgtest -lpthread

test-exec:
	bin/test/http/server.o
	bin/test/http/utils.o
