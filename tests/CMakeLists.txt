
if (BUILD_TESTS)
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -std=c99")
    add_executable(simple_qoo_test unit/simple_qoo_test.c)
    target_link_libraries(simple_qoo_test qoo cmocka)
    enable_testing()
    add_test(simple_qoo_test simple_qoo_test)
endif()



