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
}