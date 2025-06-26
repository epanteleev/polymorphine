#pragma once

#include <cstddef>
#include "utility/Error.h"


static char size_prefix(std::size_t size) {
    switch (size) {
        case 1: return 'b';
        case 2: return 'w';
        case 4: return 'l';
        case 8: return 'q';
        default: die("wrong size {}", size);
    }
}
