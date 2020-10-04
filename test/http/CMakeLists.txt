file(GLOB TestingSources *.cpp)
add_executable(httptest ${TestingSources})

target_link_libraries(httptest ObjectFiles ConnectionObjectFileTesting ${OPENSSL_LIBRARIES} ${GTEST_BOTH_LIBRARIES} )

add_test(HTTPTest httptest)
