#pragma once

#include "MachBlock.h"
#include "../operand/LIRVReg.h"
#include "base/FunctionDataBase.h"


class ObjFuncData final: public FunctionDataBase<MachBlock, LIRArg> {
public:
    ObjFuncData(std::string_view name, std::vector<LIRArg> &&args) noexcept: FunctionDataBase(std::move(args)),
                                                                             m_name(name) {
        create_mach_block();
    }

    MachBlock* create_mach_block() {
        const auto creator = [this](std::size_t id) {
            return std::make_unique<MachBlock>(id);
        };

        return m_basic_blocks.push_back<MachBlock>(creator);
    }

    [[nodiscard]]
    std::string_view name() const noexcept {
        return m_name;
    }

    void print(std::ostream &os) const {
        os << m_name << '(';
        for (auto& arg : m_args) {
            os << " " << arg;
        }
        os << ") ";
        print_blocks(os);
    }

private:
    std::string m_name;
    std::vector<LIRArg> m_args;
};
