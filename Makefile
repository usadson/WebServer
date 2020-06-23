# Copyright (C) 2020 Tristan. All Rights Reserved.
# See the COPYING file for licensing information.

.DEFAULT_GOAL := all

include standard.Makefile

# All the object files. By convention, each .cpp should have a corresponding
# object file. For more information, see the explanation above.
BINARIES = bin/connection/connection.o \
	   bin/http/client.o \
	   bin/http/server.o \
	   bin/http/server_launch_error.o

# The 'all' target will compile all object files and generate the binary
# executable. This is the default target for 'make'.
all: bin/test.txt $(BINARIES) $(TESTING_TARGETS) server

# The 'objects' target will compile all object files, but not generate the
# binary executable.
objects: bin/test.txt $(BINARIES)

# The 'clean' target will remove all binaries generated by the build system.
# This will restore the state to a clean git clone.
clean:
	rm -rf bin
	rm -rf server

# The 'fast' target will build 'all' target in parallel
fast:
	@tools/build-fast.sh

# The 'server' target will build the final binary executable.
server: main.cpp \
	base/logger.hpp \
	http/server.hpp \
	$(BINARIES)
	$(CXX) $(CXXFLAGS) -o $@ main.cpp $(BINARIES) $(LDFLAGS)

# The 'bin/test.txt' will ensure all directories required by the object files
# are present. The bin/test.txt file will be touch'ed so that mkdir will only
# be called once.
#
# When getting a 'directory does not exists' error e.g. after pulling from
# origin, creating the directory yourself can be a hassle, so maybe just
# execute the 'clean' target.
bin/test.txt:
	@mkdir bin
	@mkdir bin/connection
	@mkdir bin/http
	@mkdir bin/test
	@mkdir bin/test/http
	@touch bin/test.txt

bin/connection/connection.o: connection/connection.cpp \
	connection/connection.hpp \
	http/configuration.hpp
	$(CXX) $(CXXFLAGS) -c -o $@ connection/connection.cpp

bin/http/client.o: http/client.cpp \
	http/client.hpp \
	base/logger.hpp \
	http/configuration.hpp \
	http/server.hpp
	$(CXX) $(CXXFLAGS) -c -o $@ http/client.cpp

bin/http/server.o: http/server.cpp \
	http/server.hpp \
	base/logger.hpp \
	http/client.hpp \
	http/configuration.hpp \
	http/server_launch_error.hpp
	$(CXX) $(CXXFLAGS) -c -o $@ http/server.cpp

bin/http/server_launch_error.o: http/server_launch_error.cpp \
	http/server_launch_error.hpp
	$(CXX) $(CXXFLAGS) -c -o $@ http/server_launch_error.cpp


# the 'memory' target will invoke Valgrind, which will run the executable and
# can track memory usage. Memory leaks, double free()'s, use-after-free,
# uninitialised values, etc. can be found by using this tool.
memory:
	valgrind --num-callers=100 \
		 --leak-resolution=high \
		 --leak-check=full \
		 --track-origins=yes \
		 --show-leak-kinds=all \
		 --track-fds=yes \
		 ./server

# the 'cppcheck' target will invoke the cppcheck program. This program
# statically analyzes the code, and might catch bugs and suggest improvements.
cppcheck:
	cppcheck -I. -q --verbose --std=c++17 --enable=all .

# the 'infer' target will invoke the Infer program, so Infer is prerequisite.
# Infer will analyze the software for common bugs.
infer:
	infer run -- make

# the 'infer-clean' target will invoke the Infer program and makes sure older
# binaries will be cleaned up first, so that the complete software will be
# analyzed. Running infer again doesn't require a clean build, so use the
# 'infer' target after running the 'infer-clean' target once.
infer-clean:
	infer run -- make clean all

# A modern linter for C++, made by Facebook.
flint:
	flint++ -r -v .


include test/Makefile
