#pragma once

#include <vector>

#include "asm/global/Slot.h"
#include "asm/global/SlotType.h"
#include "asm/x64/Relocation.h"
#include "asm/x64/Common.h"
#include "utility/ArithmeticUtils.h"


namespace aasm::details {
    class AssembleSlot final {
        constexpr explicit AssembleSlot(const Slot& slot) noexcept:
            m_slot(slot) {}
        
    public:
        template<CodeBuffer Buffer>
        [[nodiscard]]
        static constexpr std::vector<Relocation> assemble(Buffer& buffer, const Slot& slot) {
            AssembleSlot assembler(slot);
            return assembler.emit(buffer);
        }

    private:
        template<CodeBuffer Buffer>
        constexpr std::vector<Relocation> emit(Buffer& buffer) {
            emit_internal(buffer, m_slot);
            return std::move(m_relocations);
        }

        template<CodeBuffer Buffer>
        constexpr void emit_internal(Buffer& buffer, const Slot& slot) {
            const auto vis = [&]<typename T>(const T& val) {
                if constexpr (std::same_as<T, std::int64_t>) {
                    assemble_slot_imm(buffer, slot, val);

                } else if constexpr (std::same_as<T, std::string>) {
                    assemble_slot_string(buffer, val);

                } else if constexpr (std::same_as<T, std::vector<Slot>>) {
                    for (const auto& s: val) {
                        emit_internal(buffer, s);
                    }

                } else if constexpr (std::same_as<T, const Directive*>) {
                    buffer.emit64(UINT64_MAX);
                    m_relocations.emplace_back(Relocation(RelType::X86_64_GLOB_DAT, buffer.size()-sizeof(std::int64_t), buffer.size(), val->symbol()));

                } else {
                    static_assert(false);
                    std::unreachable();
                }
            };

            slot.visit(vis);
        }

        template<CodeBuffer Buffer>
        static constexpr void assemble_slot_imm(Buffer& buffer, const Slot& slot, const std::int64_t imm) {
            switch (slot.type()) {
                case SlotType::Byte: buffer.emit8(checked_cast<std::uint8_t>(imm)); break;
                case SlotType::Word: buffer.emit16(checked_cast<std::uint16_t>(imm)); break;
                case SlotType::DWord: buffer.emit32(checked_cast<std::uint32_t>(imm)); break;
                case SlotType::QWord: buffer.emit64(imm); break;
                default: std::unreachable();
            }
        }

        template<CodeBuffer Buffer>
        static constexpr void assemble_slot_string(Buffer& buffer, const std::string& str) {
            for (const auto ch: str) {
                buffer.emit8(ch);
            }

            buffer.emit8(0);
            const auto total_size = buffer.size()+1;
            const auto padding = align_up(total_size, 8) - total_size;
            for (std::size_t i{}; i < padding; i++) {
                buffer.emit8(0);
            }
        }

        const Slot& m_slot;
        std::vector<Relocation> m_relocations;
    };
}