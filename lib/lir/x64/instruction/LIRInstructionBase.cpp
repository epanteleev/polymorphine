#include <ostream>
#include <ranges>

#include "lir/x64/instruction/LIRInstructionBase.h"
#include "lir/x64/module/LIRBlock.h"

#include "asm/x64/instruction/CondType.h"
#include "asm/x64/reg/RegSet.h"

namespace {
    class LIRInstructionPrinter final: public LIRVisitor {
    public:
        explicit LIRInstructionPrinter(std::ostream& os): m_os(os) {}

    private:
        void gen(const LIRVal &out) override {
            m_os << "gen out(" << out << ')';
        }

        void add_i(const LIRVal &out, const LIROperand &in1, const LIROperand &in2) override {
            m_os << "add_i out(" << out << ") in(" << in1 << ", " << in2 << ')';
        }

        void sub_i(const LIRVal &out, const LIROperand &in1, const LIROperand &in2) override {
            m_os << "sub_i out(" << out << ") in(" << in1 << ", " << in2 << ')';
        }

        void mul_i(const LIRVal &out, const LIROperand &in1, const LIROperand &in2) override {}

        void div_i(const LIRVal &out, const LIROperand &in1, const LIROperand &in2) override {}

        void and_i(const LIRVal &out, const LIROperand &in1, const LIROperand &in2) override {}

        void or_i(const LIRVal &out, const LIROperand &in1, const LIROperand &in2) override {}

        void xor_i(const LIRVal &out, const LIROperand &in1, const LIROperand &in2) override {
            m_os << "xor_i out(" << out << ") in(" << in1 << ", " << in2 << ')';
        }

        void shl_i(const LIRVal &out, const LIROperand &in1, const LIROperand &in2) override {}

        void shr_i(const LIRVal &out, const LIROperand &in1, const LIROperand &in2) override {

        }

        void setcc_i(const LIRVal &out, const aasm::CondType cond_type) override {
            m_os << "setcc_i " << cond_type << " out(" << out << ")";
        }

        void cmov_i(const aasm::CondType cond_type, const LIRVal& out, const LIROperand& in1, const LIROperand& in2) override {
            m_os << "cmov" << cond_type << "_i" << " out(" << out << ") in1(" << in1 << ") in2(" << in2 << ')';
        }

        void parallel_copy(const LIRVal &out, std::span<LIRVal const> inputs) override {
            m_os << "parallel_copy out(" << out << ") inputs(";
            for (auto [idx, input]: std::ranges::views::enumerate(inputs)) {
                if (idx != 0) {
                    m_os << ", ";
                }
                m_os << input;
            }
            m_os << ')';
        }

        void cmp_i(const LIROperand &in1, const LIROperand &in2) override {
            m_os << "cmp_i in1(" << in1 << ") in2(" << in2 << ')';
        }

        void neg_i(const LIRVal &out, const LIROperand &in) override {

        }

        void not_i(const LIRVal &out, const LIROperand &in) override {

        }

        void mov_i(const LIRVal &in0, const LIROperand &in) override {
            m_os << "mov_i in(" << in0 << ") in(" << in << ')';
        }

        void mov_by_idx_i(const LIRVal &out, const LIROperand &index, const LIROperand &in2) override {
            m_os << "mov_by_idx_i out(" << out << ") index(" << index << ") in2(" << in2 << ')';
        }

        void store_i(const LIRVal &pointer, const LIROperand &value) override {
            m_os << "store_i pointer(" << pointer << ") value(" << value << ')';
        }

        void print_adjust_stack(const std::string_view name, const aasm::GPRegSet& reg_set, const std::size_t caller_overflow_area_size) const noexcept {
            m_os << name << " [";
            for (const auto reg: reg_set) {
                m_os << reg.name(8) << ' ';
            }

            m_os << "] stack_size(" << caller_overflow_area_size << ')';
        }

        void up_stack(const aasm::GPRegSet& reg_set, const std::size_t caller_overflow_area_size) override {
            print_adjust_stack("up_stack", reg_set, caller_overflow_area_size);
        }

        void down_stack(const aasm::GPRegSet& reg_set, const std::size_t caller_overflow_area_size) override {
            print_adjust_stack("down_stack", reg_set, caller_overflow_area_size);
        }

        void prologue(const aasm::GPRegSet &reg_set, std::size_t caller_overflow_area_size) override {
            print_adjust_stack("prologue", reg_set, caller_overflow_area_size);
        }

        void epilogue(const aasm::GPRegSet &reg_set, std::size_t caller_overflow_area_size) override {
            print_adjust_stack("epilogue", reg_set, caller_overflow_area_size);
        }

        void copy_i(const LIRVal &out, const LIROperand &in) override {
            m_os << "copy_i out(" << out << ") in(" << in << ')';
        }

        void load_i(const LIRVal &out, const LIRVal &pointer) override {
            m_os << "load_i out(" << out << ") pointer(" << pointer << ')';
        }

        void load_by_idx_i(const LIRVal &out, const LIRVal &pointer, const LIROperand &index) override {
            m_os << "load_by_idx_i out(" << out << ") index(" << index << ") pointer(" << pointer << ')';
        }

        void lea_i(const LIRVal &out, const LIRVal &pointer, const LIROperand &index) override {
            m_os << "lea_i out(" << out << ") pointer(" << pointer << ") index(" << index << ')';
        }

        void jmp(const LIRBlock *bb) override {
            m_os << "jmp ";
            bb->print_short_name(m_os);
        }

        void jcc(const aasm::CondType cond_type, const LIRBlock *on_true, const LIRBlock *on_false) override {
            m_os << "j" << cond_type << " ";
            on_true->print_short_name(m_os);
            m_os << ", ";
            on_false->print_short_name(m_os);
        }

        void call(const LIRVal &out, const std::string_view name, std::span<LIRVal const> args, LIRLinkage linkage) override {
            m_os << "call " << name << " out(" << out << ") args(";
            for (auto [idx, arg]: std::ranges::views::enumerate(args)) {
                if (idx != 0) {
                    m_os << ", ";
                }

                m_os << arg;
            }
            m_os << ")";
        }

        void vcall(std::span<LIRVal const> args) override {

        }

        void icall(const LIRVal &out, const LIRVal &pointer, std::span<LIRVal const> args) override {

        }

        void ivcall(const LIRVal &pointer, std::span<LIRVal const> args) override {

        }

        void ret(std::span<LIRVal const> ret_values) override {
            m_os << "ret " << "in[";
            for (auto [idx, v_ret]: std::ranges::views::enumerate(ret_values)) {
                if (idx != 0) m_os << ", ";
                m_os << v_ret;
            }
            m_os << "]";
        }

        std::ostream& m_os;
    };
}

void LIRInstructionBase::print(std::ostream &os) const {
    LIRInstructionPrinter printer(os);
    const auto me = const_cast<LIRInstructionBase*>(this);
    me->visit(printer);
}