#pragma once

#include <bitset>
#include <cstddef>

namespace bitutils {
    template <std::size_t N>
    [[nodiscard]]
    std::size_t find_next_set_bit(const std::bitset<N>& bitset, const std::size_t start) noexcept {
        if (start >= N) {
            return N; // Invalid start index
        }
        for (std::size_t i = start; i < N; ++i) {
            if (bitset.test(i)) return i;
        }

        return N; // No set bit found
    }

    template <std::size_t N>
    [[nodiscard]]
    std::size_t find_prev_set_bit(const std::bitset<N>& bitset, const std::size_t start) noexcept {
        if (start >= N) {
            return N; // Invalid start index
        }
        for (std::size_t i = start+1; i > 0; --i) {
            if (bitset.test(i-1)) return i-1;
        }
        return N; // No set bit found
    }
}
