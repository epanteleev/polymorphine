#pragma once

#include <ranges>

#include "LIRBlock.h"
#include "../operand/LIRVal.h"
#include "base/FunctionDataBase.h"


class LIRFuncData final: public FunctionDataBase<LIRBlock, LIRArg> {
public:
    LIRFuncData(std::string_view name, std::vector<LIRArg> &&args) noexcept:
        FunctionDataBase(std::move(args)), m_name(name) {
        create_mach_block();
    }

    LIRBlock* create_mach_block() {
        const auto creator = [this](std::size_t id) {
            return std::make_unique<LIRBlock>(id);
        };

        return m_basic_blocks.push_back<LIRBlock>(creator);
    }

    [[nodiscard]]
    std::string_view name() const noexcept {
        return m_name;
    }

    [[nodiscard]]
    LIRVal arg(const std::size_t index) const {
        return LIRVal::from(&m_args[index]);
    }

    [[nodiscard]]
    auto args() const {
        return m_args | std::views::transform([](const auto &arg) { return LIRVal::from(&arg); } );
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
};
