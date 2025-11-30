include(cmake/GoogleTest)
include(CTest)

function(add_test_executable target path)
    add_executable(${target} ${path}
            helpers/Utils.cpp
            helpers/Jit.cpp
    )

    target_compile_features(${target} PUBLIC cxx_std_23)
    target_link_libraries(${target} PRIVATE polymorphine GTest::gtest GTest::gtest_main)
    target_include_directories(${target} PRIVATE ${CMAKE_SOURCE_DIR}/lib> .)

    gtest_discover_tests(${target})
    add_test(NAME ${target} COMMAND ${target})
    if (GCOVR_FOUND)
        add_dependencies(coverage ${target})
    endif()
endfunction()

add_test_executable(ordered_set_tests    ir/ordered_set_tests.cpp)
add_test_executable(sanity_check_ir      ir/sanity_check_ir.cpp)
add_test_executable(sanity_check_ir1     ir/sanity_check_ir1.cpp)
add_test_executable(sanity_check_ir2     ir/sanity_check_ir2.cpp)
add_test_executable(int_div_test         ir/int_div_test.cpp)
add_test_executable(test_ir_cmp          ir/test_cmp.cpp)
add_test_executable(convertion_test      ir/convertion_test.cpp)
add_test_executable(array_access_test    ir/array/array_access_test.cpp)
add_test_executable(empty_function_test  ir/empty_function_test.cpp)

add_test_executable(global_constant_test ir/global/global_constant_test.cpp)
add_test_executable(global_variable_test ir/global/global_variable_test.cpp)

add_test_executable(call_test      ir/call/call_test.cpp)
add_test_executable(call_test1     ir/call/call_test1.cpp)
add_test_executable(call_conv_test ir/call/call_conv_test.cpp)

add_test_executable(struct_test        ir/struct/struct_test.cpp)
add_test_executable(struct_access_test ir/struct/struct_access_test.cpp)

add_test_executable(asm_test        asm/asm_test.cpp)
add_test_executable(asm_test_1      asm/asm_test_1.cpp)
add_test_executable(asm_test_2      asm/asm_test_2.cpp)
add_test_executable(sse_asm_test    asm/sse_asm_test.cpp)
add_test_executable(asm_test_setcc  asm/asm_test_setcc.cpp)
add_test_executable(reg_map_test    asm/reg_map_test.cpp)
add_test_executable(reg_set_test    asm/reg_set_test.cpp)

add_test_executable(fib_test    algo/test_fib.cpp)
add_test_executable(bubble_sort algo/test_bubble_sort.cpp)
add_test_executable(memcpy_test algo/test_memcpy.cpp)
add_test_executable(swap_test   algo/test_swap.cpp)