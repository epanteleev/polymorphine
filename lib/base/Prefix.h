#pragma once

#include <cstddef>

[[maybe_unused]]
static char size_prefix(const std::size_t size) {
    switch (size) {
        case 1:  return 'b';
        case 2:  return 'w';
        case 4:  return 'd';
        case 8:  return 'q';
        default: return 'L';
    }
}