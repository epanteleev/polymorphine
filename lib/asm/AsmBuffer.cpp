#include "AsmBuffer.h"

#include <iomanip>
#include <unordered_map>

namespace aasm {
    std::ostream & operator<<(std::ostream &os, const AsmBuffer &asm_buff) {
        const auto pretty_print = static_cast<int>(os.width());

        std::unordered_map<std::uint32_t, std::vector<std::size_t>> instruction_to_label;
        for (const auto& [idx, instruction]: std::ranges::enumerate_view(asm_buff.m_label_table)) {
            auto [fst, _] = instruction_to_label.emplace(instruction, std::vector<std::size_t>{});
            fst->second.push_back(idx);
        }

        for (const auto& [idx, instruction]: std::ranges::enumerate_view(asm_buff.m_instructions)) {
            if (idx > 0) {
                os << std::endl;
            }
            if (const auto it = instruction_to_label.find(idx); it != instruction_to_label.end()) {
                for (const auto& label_idx: it->second) {
                    os << "L" << label_idx << ':' << std::endl;
                }
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
