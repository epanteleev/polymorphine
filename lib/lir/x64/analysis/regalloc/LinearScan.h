#pragma once

#include "FixedRegisters.h"
#include "FixedRegistersEval.h"
#include "RegisterAllocation.h"
#include "lir/x64/analysis/intervals/LiveIntervals.h"
#include "lir/x64/analysis/intervals/LiveIntervalsEval.h"
#include "lir/x64/operand/OperandMatcher.h"

class LinearScan final {
public:
    using result_type = RegisterAllocation;
    using basic_block = LIRBlock;
    static constexpr auto analysis_kind = AnalysisType::LinearScan;

private:
    LinearScan(const LIRFuncData &obj_func_data, const FixedRegisters& fixed_registers, const LiveIntervals& intervals) noexcept:
        m_obj_func_data(obj_func_data),
        m_fixed_registers(fixed_registers),
        m_intervals(intervals) {}

public:
    void run() {
        allocate_fixed_registers();
        setup_unhandled_intervals();
        do_register_allocation();
    }

    std::unique_ptr<result_type> result() noexcept {
        return std::make_unique<RegisterAllocation>(std::move(m_reg_allocation), m_local_area_size);
    }

    static LinearScan create(AnalysisPassManagerBase<LIRFuncData> *cache, const LIRFuncData *data) {
        const auto fixed_registers = cache->analyze<FixedRegistersEval>(data);
        const auto intervals = cache->analyze<LiveIntervalsEval>(data);

        return {*data, *fixed_registers, *intervals};
    }

private:
    void allocate_fixed_registers() {
        for (const auto& [lir, rax_reg]: m_fixed_registers) {
            m_reg_allocation.emplace(lir, rax_reg);
        }
    }

    void setup_unhandled_intervals() {
        m_unhandled_intervals.reserve(m_intervals.intervals().size());
        for (auto& [lir_val, intervals]: m_intervals.intervals()) {
            for (auto interval: intervals) {
                m_unhandled_intervals.emplace_back(interval, lir_val);
            }
        }

        const auto ordering = [](const IntervalEntry& a, const IntervalEntry& b) -> bool {
            return a.m_interval.start() < b.m_interval.start();
        };

        std::ranges::sort(m_unhandled_intervals, ordering);
    }

    void do_register_allocation() {
        for (auto& [unhandled_interval, vreg]: m_unhandled_intervals) {
            if (vreg.isa(cmp())) {
                // Produces flag registers, which are not allocated.
                continue;
            }

            if (vreg.isa(gen())) {
                m_reg_allocation.try_emplace(vreg, aasm::Address(aasm::rbp, 0, -vreg.size()));
                m_local_area_size += vreg.size();
                continue;
            }

            m_reg_allocation.try_emplace(vreg, aasm::rcx);
        }
    }

    struct IntervalEntry final {
        IntervalEntry(const Interval& interval, const LIRVal vreg) noexcept:
            m_interval(interval),
            m_vreg(vreg) {}

        Interval m_interval;
        LIRVal m_vreg;
    };

    const LIRFuncData& m_obj_func_data;
    const FixedRegisters& m_fixed_registers;
    const LiveIntervals& m_intervals;

    LIRValMap<GPVReg> m_reg_allocation{};
    // Size of the 'gen' values in the local area.
    std::int32_t m_local_area_size{0};

    std::vector<IntervalEntry> m_unhandled_intervals{};
    std::vector<IntervalEntry> m_unactive_intervals{};
    std::vector<IntervalEntry> m_active_intervals{};
};
