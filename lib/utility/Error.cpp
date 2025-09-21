#include "Error.h"

#include <iostream>
#include <source_location>
#include <stacktrace>

namespace error {
    [[noreturn]]
    void raise(std::source_location loc, const std::string& msg) {
        std::cerr << "Error at " << loc.file_name() << ":" << loc.line() << ": "
                  << msg << std::endl;
        std::cerr << std::stacktrace::current() << std::endl;
        std::terminate();
    }

    [[noreturn]]
    void assert0(const std::string_view condition, const std::source_location loc, const std::string& msg) {
        std::cerr << "Assertion '" << condition << "' failed: " << msg << std::endl;
        raise(loc, msg);
    }

    void setup_terminate_handler() {
        std::set_terminate([] {
            std::cout << std::stacktrace::current() << std::endl;
            std::abort();
        });
    }
}