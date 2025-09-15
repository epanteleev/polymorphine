#pragma once

#include "asm/x64/Common.h"

namespace aasm::details {
    template<CodeBuffer Buffer>
    class AssembleGlobals final {
    public:
        constexpr explicit AssembleGlobals(Buffer& buffer) noexcept:
            m_buffer(buffer) {}

        constexpr void emit(const NamedSlot& slot) const {
            const auto start = m_buffer.size();
            const auto vis = [&]<typename T>(const T& val) {
                if constexpr (std::same_as<T, std::int64_t>) {
                    assemble_slot_imm(slot.type(), val);

                } else if constexpr (std::same_as<T, std::string>) {
                    assemble_slot_string(val);

                } else {
                    static_assert(false);
                    std::unreachable();
                }
            };

            slot.visit(vis);
        }

    private:
        constexpr void assemble_slot_imm(const SlotType type, const std::int64_t imm) const {
            switch (type) {
                case SlotType::Byte: m_buffer.emit8(aasm::checked_cast<std::uint8_t>(imm)); break;
                case SlotType::Word: m_buffer.emit16(aasm::checked_cast<std::uint16_t>(imm)); break;
                case SlotType::DWord: m_buffer.emit32(aasm::checked_cast<std::uint32_t>(imm)); break;
                case SlotType::QWord: m_buffer.emit64(aasm::checked_cast<std::uint64_t>(imm)); break;
                default: std::unreachable();
            }
        }

        constexpr void assemble_slot_string(const std::string& str) const {
            for (const auto ch: str) {
                m_buffer.emit8(ch);
            }

            m_buffer.emit8(0);
        }

        Buffer& m_buffer;
    };
}