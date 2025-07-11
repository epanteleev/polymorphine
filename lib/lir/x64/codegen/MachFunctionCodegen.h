#pragma once

#include <span>

#include "lir/x64/analysis/Analysis.h"
#include "lir/x64/asm/GPOp.h"
#include "lir/x64/asm/MacroAssembler.h"
#include "lir/x64/instruction/LIRVisitor.h"
#include "lir/x64/module/ObjFuncData.h"


class MachFunctionCodegen final: public LIRVisitor {
    explicit MachFunctionCodegen(const ObjFuncData &data,
                                 const RegisterAllocation &reg_allocation) noexcept:
        m_data(data),
        m_reg_allocation(reg_allocation) {}

public:
    void run() {
        for (const auto& bb: m_data.basic_blocks()) {
            for (auto& inst: bb.instructions()) {
                inst.visit(*this);
            }
        }
    }

    MacroAssembler result() noexcept {
        return std::move(m_as);
    }

    static MachFunctionCodegen create(AnalysisPassCacheMach* cache, const ObjFuncData* data) {
        const auto register_allocation = cache->analyze<LinearScan>(data);

        return MachFunctionCodegen(*data, *register_allocation);
    }

private:
    [[nodiscard]]
    GPOp convert(const LIROperand &val) const;

    void add_i(const LIRVal &out, const LIROperand &in1, const LIROperand &in2) override {

    }

    void sub_i(const LIRVal &out, const LIROperand &in1, const LIROperand &in2) override {

    }

    void mul_i(const LIRVal &out, const LIROperand &in1, const LIROperand &in2) override {

    }

    void div_i(const LIRVal &out, const LIROperand &in1, const LIROperand &in2) override {

    }

    void and_i(const LIRVal &out, const LIROperand &in1, const LIROperand &in2) override {

    }

    void or_i(const LIRVal &out, const LIROperand &in1, const LIROperand &in2) override {

    }

    void xor_i(const LIRVal &out, const LIROperand &in1, const LIROperand &in2) override {

    }

    void shl_i(const LIRVal &out, const LIROperand &in1, const LIROperand &in2) override {

    }

    void shr_i(const LIRVal &out, const LIROperand &in1, const LIROperand &in2) override {

    }

    void cmp_i(const LIROperand &in1, const LIROperand &in2) override {

    }

    void neg_i(const LIRVal &out, const LIROperand &in) override {

    }

    void not_i(const LIRVal &out, const LIROperand &in) override {

    }

    void mov_i(const LIRVal &in1, const LIRVal &in2) override {

    }

    void copy_i(const LIRVal &out, const LIROperand &in) override;

    void jmp(const MachBlock *bb) override {

    }

    void je(const MachBlock *on_true, const MachBlock *on_false) override {

    }

    void jne(const MachBlock *on_true, const MachBlock *on_false) override {

    }

    void jl(const MachBlock *on_true, const MachBlock *on_false) override {

    }

    void jle(const MachBlock *on_true, const MachBlock *on_false) override {

    }

    void jg(const MachBlock *on_true, const MachBlock *on_false) override {

    }

    void jge(const MachBlock *on_true, const MachBlock *on_false) override {

    }

    void parallel_copy(const LIRVal &out, std::span<LIRVal const> inputs) override {

    }

    void call(const LIRVal &out, std::span<LIRVal const> args) override {

    }

    void vcall(std::span<LIRVal const> args) override {

    }

    void icall(const LIRVal &out, const LIRVal &pointer, std::span<LIRVal const> args) override {

    }

    void ivcall(const LIRVal &pointer, std::span<LIRVal const> args) override {

    }

    void ret(std::span<LIROperand const> ret_values) override {

    }

    const ObjFuncData& m_data;
    const RegisterAllocation& m_reg_allocation;
    MacroAssembler m_as{};
};
