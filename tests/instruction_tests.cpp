#include <gtest/gtest.h>

#include "instruction/Binary.h"
#include "value/LocalValue.h"
#include "value/ArgumentValue.h"

static_assert(IsLocalValueType<ArgumentValue>, "sanity check");
static_assert(IsLocalValueType<ValueInstruction>, "sanity check");
static_assert(IsLocalValueType<Binary>, "sanity check");

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}