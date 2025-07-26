#include <ostream>
#include <ranges>

#include "LIRInstructionBase.h"
#include "LIRSetCC.h"
#include "lir/x64/module/LIRBlock.h"

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

        void sub_i(const LIRVal &out, const LIROperand &in1, const LIROperand &in2) override {}

        void mul_i(const LIRVal &out, const LIROperand &in1, const LIROperand &in2) override {}

        void div_i(const LIRVal &out, const LIROperand &in1, const LIROperand &in2) override {}

        void and_i(const LIRVal &out, const LIROperand &in1, const LIROperand &in2) override {}

        void or_i(const LIRVal &out, const LIROperand &in1, const LIROperand &in2) override {}

        void xor_i(const LIRVal &out, const LIROperand &in1, const LIROperand &in2) override {}

        void shl_i(const LIRVal &out, const LIROperand &in1, const LIROperand &in2) override {}

        void shr_i(const LIRVal &out, const LIROperand &in1, const LIROperand &in2) override {

        }

        void setcc_i(const LIRVal &out, LIRCondType cond_type, const LIROperand &in1) override {
            m_os << "setcc_i " << cond_type << " out(" << out << ") in(" << in1 << ')';
        }

        void parallel_copy(const LIRVal &out, std::span<LIRVal const> inputs) override {

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

        void copy_i(const LIRVal &out, const LIROperand &in) override {
            m_os << "copy_i out(" << out << ") in(" << in << ')';
        }

        void jmp(const LIRBlock *bb) override {
            m_os << "jmp ";
            bb->print_short_name(m_os);
        }

        void jcc(LIRCondType cond_type, const LIRBlock *on_true, const LIRBlock *on_false) override {
            m_os << "j" << cond_type << " ";
            on_true->print_short_name(m_os);
            m_os << ", ";
            on_false->print_short_name(m_os);
        }

        void call(const LIRVal &out, std::span<LIRVal const> args) override {

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