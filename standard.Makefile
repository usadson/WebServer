# Copyright (C) 2020 Tristan. All Rights Reserved.
# See the COPYING file for licensing information.

# TLS Configuration
TLS_PACKAGE = openssl

GENERAL = -std=c++17 -O3 -g
FLAGS = -fno-rtti
INCLUDES = -I.
WARNINGS = \
	   -Wall \
	   -Wextra \
	   -Wformat=2 \
	   -Wpedantic \
	   -Wshadow

ADDITIONAL_CXXFLAGS ?=
CXXFLAGS += $(GENERAL) $(INCLUDES) $(WARNINGS) $(FLAGS) $(ADDITIONAL_CXXFLAGS)
CXX = clang++
LDFLAGS = `pkg-config --static --libs $(TLS_PACKAGE)`
