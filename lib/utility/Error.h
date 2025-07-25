#pragma once

#include <source_location>
#include <string>
#include <format>

namespace error {
    [[noreturn]]
    void raise(std::source_location loc, const std::string& msg);

    [[noreturn]]
    void assert0(bool condition, std::source_location loc, const std::string& msg);

    void setup_terminate_handler();
}

enum class Error {
    CastError,
    NotFoundError,
    FunctionExistsError,
};


#define die(...) error::raise(std::source_location::current(), std::format(__VA_ARGS__))
#define assertion(condition, ...) if (!(condition)) { error::assert0(condition, std::source_location::current(), std::format(__VA_ARGS__)); }
#define unimplemented() die("Unimplemented feature: {}", __func__)

#define ENABLE_ASSERTIONS