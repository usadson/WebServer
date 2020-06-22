# Copyright (C) 2020 Tristan. All Rights Reserved.
# See the COPYING file for licensing information.

CXXFLAGS += -I..

test: \
	bin/test/http/server.o

bin/test/http/server.o: test/http/server.cpp \
	http/server.cpp \
	http/server.hpp
	$(CXX) $(CXXFLAGS) -o $@ test/http/server.cpp bin/http/client.o bin/http/server.o bin/http/server_launch_error.o -L/usr/local/lib -lgtest -lpthread

test-exec:
	bin/test/http/server.o
