#pragma once
#include "FixedRegisters.h"
#include "FixedRegistersEval.h"
#include "RegisterAllocation.h"


class LinearScan final {
public:
    using result_type = RegisterAllocation;
    using basic_block = MachBlock;
    static constexpr auto analysis_kind = AnalysisType::LinearScan;

private:
    LinearScan(const ObjFuncData &obj_func_data, const Ordering<MachBlock> &m_ordering, const FixedRegisters& fixed_registers) noexcept:
        m_obj_func_data(obj_func_data),
        m_ordering(m_ordering),
        m_fixed_registers(fixed_registers) {}

public:
    void run() {

    }

    std::unique_ptr<result_type> result() noexcept {
        return {};
    }

    static LinearScan create(AnalysisPassCacheBase<ObjFuncData> *cache, const ObjFuncData *data) {
        const auto preorder = cache->analyze<PreorderTraverseBase<ObjFuncData>>(data);
        const auto fixed_registers = cache->analyze<FixedRegistersEval>(data);
        return {*data, *preorder, *fixed_registers};
    }

private:
    const ObjFuncData& m_obj_func_data;
    const Ordering<MachBlock>& m_ordering;
    const FixedRegisters& m_fixed_registers;
};
