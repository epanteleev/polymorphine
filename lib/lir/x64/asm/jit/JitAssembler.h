#pragma once

#include <cassert>
#include <cstring>
#include <memory>
#include <ranges>

#include "JitCodeBlob.h"
#include "lir/x64/asm/AsmEmitter.h"
#include "lir/x64/asm/ObjModule.h"

class JitAssembler final {
    explicit JitAssembler(std::uint8_t* buffer) noexcept: m_buffer(buffer) {}

public:
    void emit8(const std::int8_t opcode) {
        m_buffer[m_size++] = opcode;
    }

    void emit16(const std::int16_t opcode) {
        std::memcpy(&m_buffer[m_size], &opcode, sizeof(opcode));
        m_size += 2;
    }

    void emit32(const std::int32_t c) noexcept {
        std::memcpy(&m_buffer[m_size], &c, 4);
        m_size += 4;
    }

    void emit64(const std::int64_t c) noexcept {
        std::memcpy(&m_buffer[m_size], &c, 8);
        m_size += 8;
    }

    [[nodiscard]]
    std::size_t size() const noexcept {
        return m_size;
    }

    [[nodiscard]]
    std::uint8_t* data() const noexcept {
        return m_buffer;
    }

    static JitCodeBlob assembly(const ObjModule& masm);

private:
    static constexpr std::uint8_t PAGE_SIZE = 4096;

    class SizeEvaluator final {
    public:
        void emit8(const std::int8_t) noexcept { m_size++; }
        void emit16(const std::int16_t) noexcept { m_size += 2; }
        void emit32(const std::int32_t) noexcept { m_size += 4; }
        void emit64(const std::int64_t) noexcept { m_size += 8; }

        [[nodiscard]]
        std::size_t size() const noexcept {
            return m_size;
        }

        static std::size_t eval(const ObjModule& masm) {
            std::size_t acc = 0;
            for (const auto& emitter : masm.emitters() | std::views::values) {
                SizeEvaluator size_evaluator;
                emitter.emit(size_evaluator);
                acc += size_evaluator.size();
            }

            return acc;
        }

    private:
        std::size_t m_size{};
    };

    std::uint8_t* m_buffer;
    std::size_t m_size{0};
};