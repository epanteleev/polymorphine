#pragma once

#include <vector>
#include <iomanip>
#include <iosfwd>
#include <ranges>

#include "Register.h"
#include "Address.h"
#include "instruction/CPUInstruction.h"
#include "instruction/Jmp.h"

namespace aasm {
    class Assembler final {
        static constexpr auto NO_OFFSET = std::numeric_limits<std::uint32_t>::max();

    public:
        constexpr void ret() {
            m_instructions.emplace_back(Ret());
        }

        constexpr void pop(const std::uint8_t size, const GPReg r) {
            m_instructions.emplace_back(PopR(size, r));
        }

        constexpr void pop(const std::uint8_t size, const Address& addr) {
            m_instructions.emplace_back(PopM(size, addr));
        }

        constexpr void push(const std::uint8_t size, const GPReg r) {
            m_instructions.emplace_back(PushR(size, r));
        }

        constexpr void push(const std::uint8_t size, const Address& addr) {
            m_instructions.emplace_back(PushM(size, addr));
        }

        constexpr void push(const std::uint8_t size, const std::int32_t value) {
            m_instructions.emplace_back(PushI(value, size));
        }

        constexpr void mov(const std::uint8_t size, const GPReg src, const GPReg dst) {
            m_instructions.emplace_back(MovRR(size, src, dst));
        }

        constexpr void mov(const std::uint8_t size, const std::int64_t src, const GPReg dst) {
            m_instructions.emplace_back(MovRI(size, src, dst));
        }

        constexpr void mov(const std::uint8_t size, const GPReg src, const Address& dst) {
            m_instructions.emplace_back(MovMR(size, src, dst));
        }

        constexpr void mov(const std::uint8_t size, const Address& src, GPReg dst) {
            m_instructions.emplace_back(MovRM(size, src, dst));
        }

        constexpr void mov(const std::uint8_t size, const std::int32_t src, const Address& dst) {
            m_instructions.emplace_back(MovMI(size, src, dst));
        }

        constexpr void add(const std::uint8_t size, const GPReg src, const GPReg dst) {
            m_instructions.emplace_back(AddRR(size, src, dst));
        }

        constexpr Label create_label() {
            const auto size = m_label_table.size();
            m_label_table.emplace_back(NO_OFFSET);
            return Label(size);
        }

        constexpr void set_label(const Label& label) {
            if (m_label_table[label.id()] != NO_OFFSET) {
                die("label already set: id=%u", label.id());
            }

            m_label_table[label.id()] = m_instructions.size();
        }

        constexpr void jmp(const Label& label) {
            m_instructions.emplace_back(Jmp(label));
        }

        friend std::ostream &operator<<(std::ostream &os, const Assembler &assembler);

        template<CodeBuffer Buffer>
        constexpr void emit(Buffer& buffer) const {
            std::vector<std::int32_t> offsets_from_start;
            offsets_from_start.reserve(m_instructions.size());

            std::vector<std::vector<std::int32_t>> unresolved_labels;
            unresolved_labels.resize(m_instructions.size());

            const auto visitor = [&]<typename T>(const T &var) {
                if constexpr (std::is_same_v<T, Jmp>) {
                    const auto inst_idx = m_label_table[var.label().id()];
                    if (inst_idx == NO_OFFSET) {
                        die("Label defined, but not set");
                    }

                    if (inst_idx > offsets_from_start.size()) {
                        unresolved_labels[var.label().id()].emplace_back(var.emit_unresolved32(buffer));
                    } else {
                        const auto offset_from_function_start = offsets_from_start[inst_idx];
                        var.emit(buffer, offset_from_function_start - static_cast<std::int64_t>(buffer.size()));
                    }
                } else {
                    var.emit(buffer);
                }
            };

            for (const auto& instruction: m_instructions) {
                offsets_from_start.push_back(buffer.size());
                instruction.visit(visitor);
            }

        }

        [[nodiscard]]
        constexpr std::size_t size() const noexcept {
            return m_instructions.size();
        }

    private:
        std::vector<std::uint32_t> m_label_table;
        std::vector<X64Instruction> m_instructions;
    };

    inline std::ostream & operator<<(std::ostream &os, const Assembler &assembler) {
        const auto pretty_print = static_cast<int>(os.width());
        for (const auto& [idx, instruction]: std::ranges::enumerate_view(assembler.m_instructions)) {
            if (idx > 0) {
                os << std::endl;
            }
            if (pretty_print > 0) {
                os << std::setw(pretty_print) << ' ' << instruction;
            } else {
                os << instruction;
            }
        }

        return os;
    }
}
