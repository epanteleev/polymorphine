#pragma once

#include <cstring>

#include "JitCodeBlob.h"
#include "lir/x64/asm/ObjModule.h"

class JitAssembler final {
    explicit JitAssembler(std::uint8_t* buffer) noexcept: m_buffer(buffer) {}

public:
    void emit8(const std::uint8_t opcode) {
        m_buffer[m_size++] = opcode;
    }

    void emit16(const std::uint16_t opcode) {
        std::memcpy(&m_buffer[m_size], &opcode, sizeof(opcode));
        m_size += 2;
    }

    void emit32(const std::uint32_t c) noexcept {
        std::memcpy(&m_buffer[m_size], &c, 4);
        m_size += 4;
    }

    void emit64(const std::uint64_t c) noexcept {
        std::memcpy(&m_buffer[m_size], &c, 8);
        m_size += 8;
    }

    void patch32(const std::uint32_t offset, const std::uint32_t value) {
        std::memcpy(&m_buffer[offset], &value, sizeof(value));
    }

    [[nodiscard]]
    std::size_t size() const noexcept {
        return m_size;
    }

    [[nodiscard]]
    std::uint8_t* data() const noexcept {
        return m_buffer;
    }

    static JitCodeBlob assembly(ObjModule&& module);

private:
    static constexpr auto PAGE_SIZE = 4096;

    std::uint8_t* m_buffer;
    std::size_t m_size{0};
};