#pragma once

#include <cstring>
#include <gtest/gtest.h>

#include "asm/asm.h"

[[maybe_unused]]
void print_hex(const std::uint8_t* data, std::size_t size);

class Utils final {
public:
    explicit Utils(std::span<std::uint8_t> buffer) noexcept: m_buffer(buffer) {}

    void emit8(const std::uint8_t opcode) {
        m_buffer[m_size++] = opcode;
    }

    void emit16(const std::uint16_t opcode) {
        std::memcpy(&m_buffer[m_size], &opcode, sizeof(opcode));
        m_size += sizeof(opcode);
    }

    void emit32(const std::uint32_t c) noexcept {
        std::memcpy(&m_buffer[m_size], &c, 4);
        m_size += 4;
    }

    void emit64(const std::uint64_t c) noexcept {
        std::memcpy(&m_buffer[m_size], &c, 8);
        m_size += 8;
    }

    void patch32(std::uint32_t offset, const std::uint32_t value) noexcept {
        std::memcpy(&m_buffer[offset], &value, sizeof(value));
    }

    void patch64(std::uint64_t offset, const std::uint64_t value) noexcept {
        std::memcpy(&m_buffer[offset], &value, sizeof(value));
    }

    [[nodiscard]]
    std::size_t size() const noexcept {
        return m_size;
    }

private:
    std::span<std::uint8_t> m_buffer;
    std::size_t m_size{};
};

static_assert(aasm::CodeBuffer<Utils>);

std::string make_string(const aasm::AsmBuffer &a);

static std::size_t to_byte_buffer(const aasm::AsmBuffer& aasm, std::span<std::uint8_t> buffer) {
    Utils buff{buffer};
    aasm.emit(buff);
    return buff.size();
}

template<std::ranges::range R>
[[maybe_unused]]
static void check_bytes(const std::vector<std::vector<std::uint8_t>>& codes, const std::vector<std::string>& names, aasm::AsmEmitter(*fn)(std::uint8_t), R&& scales) {
    ASSERT_EQ(codes.size(), names.size());
    ASSERT_GT(codes.size(), 0U) << "No codes provided for testing";

    for (const auto& [idx, scale] : std::ranges::views::enumerate(scales)) {
        aasm::AsmEmitter a = fn(1 << scale);
        const auto asm_buffer = a.to_buffer();
        std::uint8_t v[aasm::constants::MAX_X86_INSTRUCTION_SIZE]{};
        const auto size = to_byte_buffer(asm_buffer, v);
        auto& code = codes[idx];

        ASSERT_EQ(size, code.size()) << "Mismatch at scale=" << scale;
        for (std::size_t i = 0; i < code.size(); ++i) {
            ASSERT_EQ(v[i], code[i]) << "Mismatch at index=" << i << " scale=" << scale;
        }

        auto& name = names[idx];
        ASSERT_EQ(name, make_string(asm_buffer)) << "Mismatch at scale=" << scale;
    }
}

[[maybe_unused]]
static void check_bytes(const std::vector<std::vector<std::uint8_t>>& codes, const std::vector<std::string>& names, aasm::AsmEmitter(*fn)(std::uint8_t)) {
    return check_bytes(codes, names, fn, std::views::iota(0U, codes.size()));
}

template<std::signed_integral T>
[[maybe_unused]]
static constexpr T add_overflow(T a, T b) {
    T sum;
    __builtin_add_overflow(a, b, &sum);
    return sum;
}