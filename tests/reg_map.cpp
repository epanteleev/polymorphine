#include <gtest/gtest.h>

#include <ranges>
#include "asm/reg/RegMap.h"
#include "utility/Error.h"

TEST(RegMap, empty) {
    aasm::GPRegMap<int> reg_map;
    ASSERT_TRUE(reg_map.empty());
}

TEST(RegMap, size) {
    aasm::GPRegMap<int> reg_map;
    ASSERT_EQ(reg_map.size(), 0);

    reg_map.try_emplace(aasm::rax, 42);
    ASSERT_EQ(reg_map.size(), 1);

    reg_map.try_emplace(aasm::rbx, 84);
    ASSERT_EQ(reg_map.size(), 2);

    const std::array pairs = {
        std::make_pair(aasm::rax, 42),
        std::make_pair(aasm::rbx, 84)
    };
    std::size_t idx{};
    for (const auto& [reg, value]: reg_map) {
        ASSERT_EQ(value, pairs[idx].second) << " at index: " << idx;
        ASSERT_EQ(reg, pairs[idx].first) << " at index: " << idx;
        ++idx;
    }
    ASSERT_EQ(idx, 2);
}

TEST(RegMap, gaps) {
    const std::array pairs = {
        std::make_pair(aasm::rax, 42),
        std::make_pair(aasm::r15, 84)
    };
    aasm::GPRegMap<int> reg_map;

    reg_map.try_emplace(aasm::rax, 42);
    reg_map.try_emplace(aasm::r15, 84);

    std::size_t idx{};
    for (const auto& [reg, value]: reg_map) {
        ASSERT_EQ(value, pairs[idx].second) << " at index: " << idx;
        ASSERT_EQ(reg, pairs[idx].first) << " at index: " << idx;
        ++idx;
    }
    ASSERT_EQ(idx, 2);
}

TEST(RegMap, gaps1) {
    const std::array pairs = {
        std::make_pair(aasm::rdx, 42),
        std::make_pair(aasm::r14, 84)
    };
    aasm::GPRegMap<int> reg_map;

    reg_map.try_emplace(aasm::rdx, 42);
    reg_map.try_emplace(aasm::r14, 84);

    std::size_t idx{};
    for (const auto& [reg, value]: reg_map) {
        ASSERT_EQ(value, pairs[idx].second) << " at index: " << idx;
        ASSERT_EQ(reg, pairs[idx].first) << " at index: " << idx;
        ++idx;
    }
    ASSERT_EQ(idx, 2);
}

TEST(RegMap, try_emplace) {
    aasm::GPRegMap<int> reg_map;
    auto [it, inserted] = reg_map.try_emplace(aasm::rax, 42);
    ASSERT_TRUE(inserted);
    ASSERT_EQ(it->first, aasm::rax);
    ASSERT_EQ(it->second, 42);

    auto [it2, inserted2] = reg_map.try_emplace(aasm::rax, 84);
    ASSERT_FALSE(inserted2);
    ASSERT_EQ(it2->first, aasm::rax);
    ASSERT_EQ(it2->second, 42); // Should not change the value

    for (auto rev: std::ranges::reverse_view(reg_map)) {
        ASSERT_EQ(rev.first, aasm::rax);
        ASSERT_EQ(rev.second, 42);
    }
}

TEST(RegMap, try_emplace_vec) {
    aasm::GPRegMap<std::vector<int>> reg_map;
    auto [it, inserted] = reg_map.try_emplace(aasm::rax, std::vector{1, 2, 3});
    ASSERT_TRUE(inserted);
    ASSERT_EQ(it->first, aasm::rax);
    ASSERT_EQ(it->second, std::vector({1, 2, 3}));

    auto [it2, inserted2] = reg_map.try_emplace(aasm::rax, std::vector{4, 5});
    ASSERT_FALSE(inserted2);
    it2->second.push_back(42);
    ASSERT_EQ(it2->first, aasm::rax);
    ASSERT_EQ(it2->second, std::vector({1, 2, 3, 42})); // Should append to the existing vector
}

int main(int argc, char **argv) {
    error::setup_terminate_handler();
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
