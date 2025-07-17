
include(../cmake/GoogleTest.cmake)

include(GoogleTest)


function(add_test_executable target)
    add_executable(${target} ${ARGN})
    target_compile_features(${target} PUBLIC cxx_std_23)
    target_compile_options(${target} PUBLIC -fsanitize=address)

    target_link_libraries(${target} PRIVATE ckoof GTest::gtest GTest::gtest_main)
    gtest_discover_tests(${target})

    add_test(NAME ${target} COMMAND ${target})
endfunction()

add_test_executable(ordered_set_tests ordered_set_tests ordered_set_tests.cpp)
add_test_executable(asm_test asm_test asm_test.cpp)
add_test_executable(empty_function empty_function empty_function.cpp)
add_test_executable(sanity_check_ir sanity_check_ir sanity_check_ir.cpp)
add_test_executable(constexpr_asm_test.cpp constexpr_asm_test constexpr_asm_test.cpp)
add_test_executable(asm_test_1.cpp asm_test_1 asm_test_1.cpp)