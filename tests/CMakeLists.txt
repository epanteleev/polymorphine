include(../cmake/GoogleTest.cmake)
include(CTest)

function(add_test_executable target)
    add_executable(${target} ${target}.cpp
            helpers/Utils.cpp
            helpers/Jit.cpp
    )

    target_compile_features(${target} PUBLIC cxx_std_23)
    target_link_libraries(${target} PRIVATE ckoof GTest::gtest GTest::gtest_main)
    target_include_directories(${target} PRIVATE ${CMAKE_SOURCE_DIR}/lib>)

    gtest_discover_tests(${target})
    add_test(NAME ${target} COMMAND ${target})
endfunction()

add_test_executable(ordered_set_tests)
add_test_executable(asm_test)
add_test_executable(empty_function)
add_test_executable(sanity_check_ir)
add_test_executable(asm_test_1)
add_test_executable(asm_test_setcc)
add_test_executable(fib_test)
add_test_executable(bubble_sort)
add_test_executable(call_test)