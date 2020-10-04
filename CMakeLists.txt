cmake_minimum_required(VERSION 3.15)
project(WebServer)
set(CMAKE_CXX_STANDARD 17)


# Equivalent to -I. in GCC/Clang
include_directories(.)
add_compile_options(-Wall -Wextra -pedantic -Wformat=2)


# External Libraries
find_package(OpenSSL REQUIRED)

IF (NOT APPLE AND NOT MSVC)
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
ENDIF()


# Object Files
file(GLOB Sources "*/*.cpp")
add_library(ObjectFiles STATIC ${Sources})


# Executable Binary
add_executable(server main.cpp)
target_link_libraries(server ObjectFiles ${OPENSSL_LIBRARIES})
