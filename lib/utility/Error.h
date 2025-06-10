#pragma once

#include <source_location>
#include <string>
#include <format>


namespace error {
    [[noreturn]]
    void raise(std::source_location loc, const std::string& msg);

    void assert(bool condition, std::source_location loc, const std::string& msg);
}

#define die(msg) error::raise(std::source_location::current(), msg)
#define assertion(condition, ...) error::assert(condition, std::source_location::current(), std::format(__VA_ARGS__))

#define ENABLE_ASSERTIONS