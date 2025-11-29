#pragma once

#include "lir/x64/lir_frwd.h"
#include "base/FunctionDataBase.h"
#include "lir/x64/global/GlobalData.h"
#include "lir/x64/module/LIRBlock.h"
#include "lir/x64/operand/LIRVal.h"


class LIRFuncData final: public FunctionDataBase<LIRBlock, LIRArg> {
public:
    LIRFuncData(const std::string_view name, std::vector<LIRArg> &&args, std::vector<LIRVal>&& lir_args) noexcept:
        FunctionDataBase(std::move(args)),
        m_name(name),
        m_lir_args(std::move(lir_args)) {
        create_mach_block();
    }

    LIRBlock* create_mach_block() {
        const auto id = m_basic_blocks.push_back(std::make_unique<LIRBlock>());
        m_basic_blocks[id].set_id(id);
        return &m_basic_blocks[id];
    }

    [[nodiscard]]
    std::string_view name() const noexcept {
        return m_name;
    }

    [[nodiscard]]
    LIRVal arg(const std::size_t index) const noexcept {
        return m_lir_args[index];
    }

    [[nodiscard]]
    std::span<LIRVal const> args() const noexcept {
        return m_lir_args;
    }

    [[nodiscard]]
    LIRBlock* last() const;

    [[nodiscard]]
    LIRAdjustStack* prologue() const;

    [[nodiscard]]
    LIRAdjustStack* epilogue() const;

    [[nodiscard]]
    std::expected<const LIRNamedSlot*, Error> add_slot(const std::string_view name, LIRNamedSlot&& value) {
        return m_global_data.add_slot(name, std::move(value));
    }

    [[nodiscard]]
    const GlobalData& global_data() const noexcept {
        return m_global_data;
    }

    [[nodiscard]]
    GlobalData& global_data() noexcept {
        return m_global_data;
    }

    friend std::ostream &operator<<(std::ostream &os, const LIRFuncData &fd);

private:
    std::string m_name;
    std::vector<LIRVal> m_lir_args;
    GlobalData m_global_data;
};