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
get_filename_component(full_path_test_cpp ${CMAKE_CURRENT_SOURCE_DIR}/connection/connection.cpp ABSOLUTE)
message("${full_path_test_cpp}")
list(REMOVE_ITEM Sources "${full_path_test_cpp}")
add_library(ObjectFiles STATIC ${Sources})


# Connection Objects
add_library(ConnectionObjectFileNormal STATIC connection/connection.cpp)
add_library(ConnectionObjectFileTesting STATIC connection/memory_connection.cpp)


# Executable Binary
add_executable(server main.cpp)
target_link_libraries(server ObjectFiles ConnectionObjectFileNormal ${OPENSSL_LIBRARIES})


# Testing
enable_testing()
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})
add_subdirectory(test/http)
