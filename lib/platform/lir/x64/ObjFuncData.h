#pragma once
#include "MachBlock.h"
#include "Vreg.h"


class ObjFuncData final {
public:
    MachBlock* create_mach_block() {
        const auto creator = [this](std::size_t id) {
            return std::make_unique<MachBlock>(id);
        };

        return m_basic_blocks.push_back<MachBlock>(creator);
    }

    [[nodiscard]]
    MachBlock* first() const {
        return m_basic_blocks.begin().get();
    }

    [[nodiscard]]
    MachBlock* last() const;


    [[nodiscard]]
    std::size_t size() const noexcept {
        return m_basic_blocks.size();
    }

    [[nodiscard]]
    std::string_view name() const noexcept {
        return m_name;
    }

    void print(std::ostream &os) const;

private:
    std::string m_name;
    std::vector<VReg> m_args;
    OrderedSet<MachBlock> m_basic_blocks;
};
