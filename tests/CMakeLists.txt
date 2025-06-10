include(../cmake/GoogleTest.cmake)

enable_testing()
include(GoogleTest)

function(add_test_executable target)
    add_executable(${target} ${ARGN})
    target_compile_features(${target} PUBLIC cxx_std_23)

    target_link_libraries(${target} PRIVATE ckoof GTest::gtest GTest::gtest_main)
    gtest_discover_tests(${target})

    add_test(NAME ${target} COMMAND ${target})
endfunction()

add_test_executable(ordered_set_tests ordered_set_tests ordered_set_tests.cpp)