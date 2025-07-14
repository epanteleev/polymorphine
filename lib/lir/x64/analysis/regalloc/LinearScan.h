#pragma once

#include "FixedRegisters.h"
#include "FixedRegistersEval.h"
#include "RegisterAllocation.h"

class LinearScan final {
public:
    using result_type = RegisterAllocation;
    using basic_block = LIRBlock;
    static constexpr auto analysis_kind = AnalysisType::LinearScan;

private:
    LinearScan(const LIRFuncData &obj_func_data, const Ordering<LIRBlock> &m_ordering, const FixedRegisters& fixed_registers) noexcept:
        m_obj_func_data(obj_func_data),
        m_ordering(m_ordering),
        m_fixed_registers(fixed_registers) {}

public:
    void run() {
        allocate_fixed_registers();
    }

    std::unique_ptr<result_type> result() noexcept {
        return std::make_unique<RegisterAllocation>(std::move(m_reg_allocation));
    }

    static LinearScan create(AnalysisPassCacheBase<LIRFuncData> *cache, const LIRFuncData *data) {
        const auto preorder = cache->analyze<PreorderTraverseBase<LIRFuncData>>(data);
        const auto fixed_registers = cache->analyze<FixedRegistersEval>(data);
        return {*data, *preorder, *fixed_registers};
    }

private:
    void allocate_fixed_registers() {
        for (const auto& [lir, rax_reg]: m_fixed_registers) {
            m_reg_allocation.emplace(lir, rax_reg);
        }
    }

    const LIRFuncData& m_obj_func_data;
    const Ordering<LIRBlock>& m_ordering;
    const FixedRegisters& m_fixed_registers;

    LIRValMap<GPVReg> m_reg_allocation{};
};
