#include <ranges>
#include <gtest/gtest.h>

#include "../lib/utility/OrderedSet.h"

template<typename T>
struct Elem {
    std::size_t m_id;
    T value;

    [[nodiscard]]
    std::size_t id() const {
        return m_id;
    }
};

TEST(OrderedSet, test1) {
    OrderedSet<Elem<int>> set;
    set.push_back<Elem<int>>(3);
    set.push_back<Elem<int>>(4);

    ASSERT_EQ(set[0].value, 3);
    ASSERT_EQ(set[1].value, 4);
}

TEST(OrderedSet, iterator1) {
    OrderedSet<Elem<int>> set;
    set.push_back<Elem<int>>(3);
    set.push_back<Elem<int>>(4);

    auto it = set.begin();
    ASSERT_EQ(it->value, 3);
    ++it;
    ASSERT_EQ(it->value, 4);
}

TEST(OrderedSet, iterator2) {
    OrderedSet<Elem<int>> set;
    set.push_back<Elem<int>>(3);
    set.push_back<Elem<int>>(4);

    auto it = set.end();
    ASSERT_EQ(it->value, 4);
    --it;
    ASSERT_EQ(it->value, 3);
}


/*
TEST(OrderedSet, iterator3) {
    OrderedSet<Elem<int>> set;
    set.push_back<Elem<int>>(3);
    set.push_back<Elem<int>>(4);
    set.push_back<Elem<int>>(5);

    std::vector<int> values;
    for (auto& elem : set) {
        values.push_back(elem.value);
    }

    auto list = {3, 4, 5};
    for (auto [a ,b]: std::ranges::views::zip(list, values)) {
        ASSERT_EQ(a, b);
    }

    std::vector<Elem<int>> set2;
    for (auto& [a ,b]: std::ranges::views::zip(list, set)) {
        ASSERT_EQ(a, b);
    }
}*/

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}