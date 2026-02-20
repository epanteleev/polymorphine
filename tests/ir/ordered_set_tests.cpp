#include <ranges>
#include <gtest/gtest.h>

#include "utility/ObjPool.h"

TEST(ObjPool, test1) {
    ObjPool<int> pool;
    ASSERT_EQ(pool.size(), 0);

    const auto id1 = pool.push_back(3);
    ASSERT_EQ(pool.size(), 1);

    const auto id2 = pool.push_back(4);
    ASSERT_EQ(pool.size(), 2);

    ASSERT_EQ(pool[id1], 3);
    ASSERT_EQ(pool[id2], 4);

    const auto v3 = pool.remove(id1);
    ASSERT_EQ(pool.size(), 1);
    ASSERT_EQ(v3, 3);

    const auto v2 = pool.remove(id2);
    ASSERT_EQ(pool.size(), 0);
    ASSERT_EQ(v2, 4);
}

TEST(OrderedSet, iterator1) {
    ObjPool<int> set;
    [[maybe_unused]]
    auto a = set.push_back(3);

    [[maybe_unused]]
    auto b = set.push_back(4);

    auto it = set.begin();
    ASSERT_EQ(*it, 3);
    ++it;
    ASSERT_EQ(*it, 4);
}

TEST(OrderedSet, iterator2) {
    ObjPool<int> set;
    [[maybe_unused]]
    auto a = set.push_back(3);

    [[maybe_unused]]
    auto b = set.push_back(4);

    auto it = --set.end();
    ASSERT_EQ(*it, 4);
    --it;
    ASSERT_EQ(*it, 3);
}

TEST(OrderedSet, iterator3) {
    ObjPool<int> set;
    [[maybe_unused]]
    auto _a = set.push_back(3);
    [[maybe_unused]]
    auto _b = set.push_back(4);
    [[maybe_unused]]
    auto _c = set.push_back(5);

    std::vector<int> values;
    for (auto& elem : set) {
        values.push_back(elem);
    }

    auto list = {3, 4, 5};
    for (auto [a ,b]: std::ranges::views::zip(list, values)) {
        ASSERT_EQ(a, b);
    }

    std::vector<int> set2;
    for (const auto& [a ,b]: std::ranges::views::zip(list, set)) {
        ASSERT_EQ(a, b);
    }
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}