#include <algorithm>
#include <ranges>
#include <gtest/gtest.h>

#include "asm/reg/RegSet.h"
#include "utility/Error.h"

TEST(RegSet, empty) {
    aasm::GPRegSet reg_map;
    ASSERT_TRUE(reg_map.empty());
}

TEST(RegSet, traverse) {
    aasm::GPRegSet reg_map;
    ASSERT_EQ(reg_map.size(), 0);

    reg_map.emplace(aasm::r14);
    ASSERT_EQ(reg_map.size(), 1);

    reg_map.emplace(aasm::rbx);
    ASSERT_EQ(reg_map.size(), 2);

    const std::array regs = {
        aasm::rbx,
        aasm::r14,
    };

    std::size_t idx{};
    for (const auto& reg: reg_map) {
        ASSERT_EQ(reg, regs[idx]) << "Unexpected register at index: " << idx;
        ++idx;
    }
}

TEST(RegSet, traverse_reverse) {
    aasm::GPRegSet reg_map;
    ASSERT_EQ(reg_map.size(), 0);

    reg_map.emplace(aasm::r14);
    ASSERT_EQ(reg_map.size(), 1);

    reg_map.emplace(aasm::rbx);
    ASSERT_EQ(reg_map.size(), 2);

    std::array regs = {
        aasm::r14,
        aasm::rbx,
    };

    std::size_t idx{};
    for (const auto& reg: std::ranges::reverse_view(reg_map)) {
        ASSERT_EQ(reg, regs[idx]) << "Unexpected register at index: " << idx;
        ++idx;
    }
}

int main(int argc, char **argv) {
    error::setup_terminate_handler();
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
