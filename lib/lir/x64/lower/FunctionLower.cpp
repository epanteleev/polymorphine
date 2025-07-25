#include "FunctionLower.h"

#include "lir/x64/instruction/LIRProducerInstruction.h"
#include "lir/x64/instruction/LIRSetCC.h"
#include "mir/mir.h"

/**
 * Creates a LIR constant based on the type and integer value.
 * @param type The type of the constant.
 * @param integer The integer value to be converted into a LIR constant.
 * @return A LirCst representing the constant.
 */
template<std::integral T>
static LirCst make_constant(const Type& type, const T integer) noexcept {
    if (type.isa(i8()) || type.isa(u8())) {
        return LirCst::imm8(static_cast<std::int8_t>(integer));
    }

    if (type.isa(i16()) || type.isa(u16())) {
        return LirCst::imm16(static_cast<std::int16_t>(integer));
    }

    if (type.isa(i32()) || type.isa(u32())) {
        return LirCst::imm32(static_cast<std::int32_t>(integer));
    }

    if (type.isa(i64()) || type.isa(u64())) {
        return LirCst::imm64(static_cast<std::int64_t>(integer));
    }

    die("Unsupported type for constant");
}

/**
 * Converts a signed condition to LIRCondType.
 * @param predicate The IcmpPredicate representing the condition.
 * @return The corresponding LIRCondType.
 */
static LIRCondType signed_cond_type(const IcmpPredicate predicate) noexcept {
    switch (predicate) {
        case IcmpPredicate::Eq: return LIRCondType::E;
        case IcmpPredicate::Ne: return LIRCondType::NE;
        case IcmpPredicate::Gt: return LIRCondType::G;
        case IcmpPredicate::Ge: return LIRCondType::GE;
        case IcmpPredicate::Lt: return LIRCondType::NGE;
        case IcmpPredicate::Le: return LIRCondType::NG;
        default: die("Unsupported signed condition type in flag2int");
    }
}

/**
 * Converts an unsigned condition to LIRCondType.
 * @param predicate The IcmpPredicate representing the condition.
 * @return The corresponding LIRCondType.
 */
static LIRCondType unsigned_cond_type(const IcmpPredicate predicate) noexcept {
    switch (predicate) {
        case IcmpPredicate::Eq: return LIRCondType::E;
        case IcmpPredicate::Ne: return LIRCondType::NE;
        case IcmpPredicate::Gt: return LIRCondType::A;
        case IcmpPredicate::Ge: return LIRCondType::AE;
        case IcmpPredicate::Lt: return LIRCondType::NAE;
        case IcmpPredicate::Le: return LIRCondType::NA;
        default: die("Unsupported unsigned condition type in flag2int");
    }
}

LIROperand FunctionLower::get_lir_operand(const Value &val) {
    const auto visitor = [&]<typename T>(const T &v) -> LIROperand {
        if constexpr (std::is_same_v<T, double>) {
            unimplemented();

        } else if constexpr (std::is_same_v<T, std::int64_t> || std::is_same_v<T, std::uint64_t>) {
            return make_constant(*val.type(), v);

        } else if constexpr (std::is_same_v<T, const ArgumentValue *> || std::is_same_v<T, ValueInstruction *>) {
            const auto local = LocalValue::from(v);
            return m_value_mapping.at(local);

        } else {
            static_assert(false, "Unsupported type in Value variant");
            std::unreachable();
        }
    };

    return val.visit<LIROperand>(visitor);
}

LIRVal FunctionLower::get_lir_val(const Value &val) {
    const auto lir_operand = get_lir_operand(val);
    if (const auto vreg = lir_operand.vreg()) {
        return *vreg;
    }

    die("Expected LIRVal for Value");
}

void FunctionLower::accept(Binary *inst) {
    switch (inst->op()) {
        case BinaryOp::Add: {
            const auto lhs = get_lir_operand(inst->lhs());
            const auto rhs = get_lir_operand(inst->rhs());
            const auto add = m_bb->inst(LIRProducerInstruction::add(lhs, rhs));
            memorize(inst, add->def(0));
            break;
        }
        default: die("Unsupported binary operation: {}", static_cast<int>(inst->op()));
    }
}

void FunctionLower::accept(Branch *branch) {
    const auto target = m_bb_mapping.at(branch->target());
    m_bb->inst(LIRBranch::jmp(target));
}

void FunctionLower::accept(CondBranch *cond_branch) {
    const auto on_true_lir = m_bb_mapping.at(cond_branch->on_true());
    const auto on_false_lir = m_bb_mapping.at(cond_branch->on_false());
    const auto& cond = cond_branch->condition();

    if (cond.isa(icmp(signed_v(), signed_v()))) {


    } else if (cond.isa(icmp(unsigned_v(), unsigned_v()))) {

    } else {
        die("Unsupported condition type in cond branch");
    }
}

void FunctionLower::accept(ReturnValue *inst) {
    const auto ret_val = get_lir_operand(inst->ret_value());
    const auto copy = m_bb->inst(LIRProducerInstruction::copy(ret_val));
    m_bb->inst(LIRReturn::ret(copy->def(0)));
}

void FunctionLower::accept(Store *store) {
    const auto pointer = store->pointer();
    const auto value = store->value();
    if (pointer.isa(alloc())) {
        const auto pointer_vreg = get_lir_val(pointer);
        const auto value_vreg = get_lir_operand(value);
        m_bb->inst(LIRInstruction::mov(pointer_vreg, value_vreg));

    } else {
        unimplemented();
    }
}

void FunctionLower::accept(Alloc *alloc) {
    const auto type = alloc->allocated_type();
    if (type->isa(primitive())) {
        const auto primitive_type = dynamic_cast<const PrimitiveType*>(type);
        assertion(primitive_type != nullptr, "Expected PrimitiveType for allocation");

        const auto size = primitive_type->size_of();
        const auto alloc_inst = m_bb->inst(LIRProducerInstruction::gen(size));
        memorize(alloc, alloc_inst->def(0));

    } else {
        unimplemented();
    }
}

void FunctionLower::accept(IcmpInstruction *icmp) {
    const auto lhs = get_lir_operand(icmp->lhs());
    const auto rhs = get_lir_operand(icmp->rhs());
    const auto cmp = m_bb->inst(LIRProducerInstruction::cmp(lhs, rhs));
    memorize(icmp, cmp->def(0));
}

void FunctionLower::lower_flag2int(const Unary *inst) {
    const auto cond = inst->operand();
    if (cond.isa(icmp(signed_v(), signed_v()))) {
        const auto icmp = dynamic_cast<IcmpInstruction*>(cond.get<ValueInstruction*>());
        assertion(icmp != nullptr, "Expected IcmpInstruction for signed comparison");

        make_setcc(inst, cond, signed_cond_type(icmp->predicate()));

    } else if (cond.isa(icmp(unsigned_v(), unsigned_v()))) {
        const auto icmp = dynamic_cast<IcmpInstruction*>(cond.get<ValueInstruction*>());
        assertion(icmp != nullptr, "Expected IcmpInstruction for signed comparison");

        make_setcc(inst, cond, unsigned_cond_type(icmp->predicate()));

    } else {
        die("Unsupported condition type in cond branch");
    }
}

void FunctionLower::lower_load(const Unary *inst) {
    const auto pointer = inst->operand();
    if (pointer.isa(alloc())) {
        const auto pointer_vreg = get_lir_val(pointer);
        const auto load_inst = m_bb->inst(LIRProducerInstruction::copy(pointer_vreg));
        memorize(inst, load_inst->def(0));

    } else {
        unimplemented();
    }
}

void FunctionLower::make_setcc(const Unary *inst, const Value& cond, LIRCondType cond_type) {
    const auto mapped_cond = get_lir_operand(cond);
    const auto setcc = m_bb->inst(LIRSetCC::setcc(cond_type, mapped_cond));
    memorize(inst, setcc->def(0));
}

void FunctionLower::accept(Unary *inst) {
    switch (inst->op()) {
        case UnaryOp::Flag2Int: lower_flag2int(inst); break;
        case UnaryOp::Load: lower_load(inst); break;
        default: die("Unsupported unary operation: {}", static_cast<int>(inst->op()));
    }
}
