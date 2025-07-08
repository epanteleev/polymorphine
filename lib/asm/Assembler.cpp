#include "Assembler.h"

#include <cstring>
#include <ranges>

#include "CPUInstruction.h"
#include "Instruction.h"


namespace aasm {
    class Buff final {
    public:
        Buff(std::span<std::uint8_t> buffer) noexcept: m_buffer(buffer) {}

        void emit8(std::uint8_t opcode) {
            m_buffer[m_size++] = opcode;
        }

        void emit32(const std::uint32_t c) noexcept {
            m_buffer[m_size++] = (c >> 24) & 0xFF;
            m_buffer[m_size++] = (c >> 16) & 0xFF;
            m_buffer[m_size++] = (c >> 8) & 0xFF;
            m_buffer[m_size++] = c & 0xFF;
        }

        [[nodiscard]]
        std::size_t size() const noexcept {
            return m_size;
        }

    private:
        std::span<std::uint8_t> m_buffer;
        std::size_t m_size{};
    };

    static_assert(CodeBuffer<Buff>);

    std::size_t Assembler::to_byte_buffer(std::span<std::uint8_t> buffer) const {
        Buff buff{buffer};
        for (auto& inst: m_instructions) {
            inst.emit(buff);
        }

        return buff.size();
    }
}
