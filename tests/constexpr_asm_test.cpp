#include <cstring>
#include <gtest/gtest.h>

#include "asm/Assembler.h"

template<std::size_t N>
class ConstExprBuff final {
public:
    constexpr void emit8(const std::int8_t c) {
        m_buffer[m_size++] = c;
    }

    constexpr void emit16(const std::int16_t c) {
        m_buffer[m_size++] = static_cast<std::uint8_t>(c & 0xFF);
        m_buffer[m_size++] = static_cast<std::uint8_t>((c >> 8) & 0xFF);
    }

    constexpr void emit32(const std::int32_t c) noexcept {
        m_buffer[m_size++] = static_cast<std::uint8_t>(c & 0xFF);
        m_buffer[m_size++] = static_cast<std::uint8_t>((c >> 8) & 0xFF);
        m_buffer[m_size++] = static_cast<std::uint8_t>((c >> 16) & 0xFF);
        m_buffer[m_size++] = static_cast<std::uint8_t>((c >> 24) & 0xFF);
    }

    constexpr void emit64(const std::int64_t c) noexcept {
        emit32(static_cast<std::int32_t>(c & 0xFFFFFFFF));
        emit32(static_cast<std::int32_t>((c >> 32) & 0xFFFFFFFF));
    }

    constexpr void patch32(const std::uint32_t offset, const std::uint32_t value) {
        m_buffer[offset] = static_cast<std::uint8_t>(value & 0xFF);
        m_buffer[offset + 1] = static_cast<std::uint8_t>((value >> 8) & 0xFF);
        m_buffer[offset + 2] = static_cast<std::uint8_t>((value >> 16) & 0xFF);
        m_buffer[offset + 3] = static_cast<std::uint8_t>((value >> 24) & 0xFF);
    }

    [[nodiscard]]
    constexpr std::size_t size() const noexcept {
        return m_size;
    }

    constexpr std::uint8_t operator[](const std::size_t index) const noexcept {
        return m_buffer[index];
    }

private:
    std::array<std::uint8_t, N> m_buffer{};
    std::size_t m_size{};
};

static_assert(aasm::CodeBuffer<ConstExprBuff<4>>);

template<std::size_t N>
consteval ConstExprBuff<N> emit_test1() {
    aasm::Assembler as;
    as.ret();
    as.push(4, 8);
    as.pop(2, aasm::rax);
    as.push(2, aasm::rbx);
    as.mov(4, 0xFF, aasm::rbx);
    as.mov(8, aasm::rax, aasm::rbx);

    ConstExprBuff<N> buff;
    as.emit(buff);
    return std::move(buff);
};

TEST(ConstExpr_Asm, test1) {
    auto v = emit_test1<90>();
    ASSERT_EQ(v[0], 0xC3); // RET instruction
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}