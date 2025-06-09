#pragma once

#include <source_location>
#include <string>


namespace error {
    [[noreturn]]
    void raise(std::source_location loc, const std::string& msg);
}

#define die(msg) error::raise(std::source_location::current(), msg)