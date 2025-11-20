#include <gtest/gtest.h>

#include "mir/mir.h"


TEST(Struct, Point) {
    const auto struct_type = StructType::make("Point", {SignedIntegerType::i32(), UnsignedIntegerType::u64()});
    ASSERT_EQ(struct_type.size_of(), 16);
    ASSERT_EQ(struct_type.align_of(), 8);
    ASSERT_EQ(struct_type.offset_of(0), 0);
    ASSERT_EQ(struct_type.offset_of(1), 8);
}

TEST(Struct, Rect) {
    const auto point = StructType::make("Point", {SignedIntegerType::i32(), UnsignedIntegerType::u64()});
    const auto rect = StructType::make("Rect", {&point, &point});
    ASSERT_EQ(rect.size_of(), 32);
    ASSERT_EQ(rect.align_of(), 8);
    ASSERT_EQ(rect.offset_of(0), 0);
    ASSERT_EQ(rect.offset_of(1), 16);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}