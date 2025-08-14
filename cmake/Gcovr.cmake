
find_program(GCOVR_EXECUTABLE NAMES gcovr)
if (NOT GCOVR_EXECUTABLE)
    message(STATUS "gcovr executable not found. Please install gcovr.")
else ()
    message(STATUS "Found gcovr: ${GCOVR_EXECUTABLE}")

    # Set up a custom target for generating coverage reports
    add_custom_target(coverage
            COMMAND ${GCOVR_EXECUTABLE} -r ${CMAKE_SOURCE_DIR} --sort=uncovered-percent --sort-reverse --html --html-details -o ${CMAKE_BINARY_DIR}/Testing/coverage_report.html
            COMMAND echo "Coverage report generated at ${CMAKE_BINARY_DIR}/Testing/coverage_report.html"
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
            COMMENT "Generating coverage report with gcovr"
            VERBATIM
    )
    set(GCOVR_FOUND TRUE)
endif ()