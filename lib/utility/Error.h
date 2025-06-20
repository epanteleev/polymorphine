#pragma once

#include <source_location>
#include <string>
#include <format>

namespace error {
    [[noreturn]]
    void raise(std::source_location loc, const std::string& msg);

    void assert0(bool condition, std::source_location loc, const std::string& msg);
}

enum class Error {
    CastError,
    FunctionExistsError,
};


#define die(msg) error::raise(std::source_location::current(), msg)
#define assertion(condition, ...) error::assert0(condition, std::source_location::current(), std::format(__VA_ARGS__))

#define ENABLE_ASSERTIONS