#pragma once

#include <cstring>
#include <iomanip>
#include <iostream>

#include "asm/Assembler.h"
#include "asm/Common.h"

static void print_hex(const std::uint8_t* data, std::size_t size) {
    std::cout << "Hex: ";
    for (std::size_t i = 0; i < size; ++i)
        std::cout << std::hex << std::setw(2) << std::setfill('0')
                  << static_cast<int>(data[i]) << " ";
    std::cout << std::dec << std::endl;
}

class Buff final {
public:
    explicit Buff(std::span<std::uint8_t> buffer) noexcept: m_buffer(buffer) {}

    void emit8(const std::int8_t opcode) {
        m_buffer[m_size++] = opcode;
    }

    void emit16(const std::int16_t opcode) {
        std::memcpy(&m_buffer[m_size], &opcode, sizeof(opcode));
        m_size += sizeof(opcode);
    }

    void emit32(const std::int32_t c) noexcept {
        std::memcpy(&m_buffer[m_size], &c, 4);
        m_size += 4;
    }

    void emit64(const std::int64_t c) noexcept {
        std::memcpy(&m_buffer[m_size], &c, 8);
        m_size += 8;
    }

    void patch32(std::uint32_t offset, std::uint32_t value) noexcept {
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

static_assert(aasm::CodeBuffer<Buff>);

static std::string make_string(const aasm::Assembler &a) {
    std::ostringstream os;
    os << a;
    return os.str();
}

static std::size_t to_byte_buffer(const aasm::Assembler& aasm, std::span<std::uint8_t> buffer) {
    Buff buff{buffer};
    aasm.emit(buff);
    return buff.size();
}

template<std::ranges::range R>
static void check_bytes(const std::vector<std::vector<std::uint8_t>>& codes, const std::vector<std::string>& names, aasm::Assembler(*fn)(std::uint8_t), R&& scales) {
    ASSERT_EQ(codes.size(), names.size());
    ASSERT_GT(codes.size(), 0U) << "No codes provided for testing";

    for (const auto& [idx, scale] : std::ranges::views::enumerate(scales)) {
        aasm::Assembler a = fn(1 << scale);
        std::uint8_t v[aasm::constants::MAX_X86_INSTRUCTION_SIZE]{};
        const auto size = to_byte_buffer(a, v);
        auto& code = codes[idx];

        ASSERT_EQ(size, code.size()) << "Mismatch at scale=" << scale;
        for (std::size_t i = 0; i < code.size(); ++i) {
            ASSERT_EQ(v[i], code[i]) << "Mismatch at index=" << i << " scale=" << scale;
        }

        auto& name = names[idx];
        ASSERT_EQ(name, make_string(a)) << "Mismatch at scale=" << scale;
    }
}

static void check_bytes(const std::vector<std::vector<std::uint8_t>>& codes, const std::vector<std::string>& names, aasm::Assembler(*fn)(std::uint8_t)) {
    return check_bytes(codes, names, fn, std::views::iota(0U, codes.size()));
}

