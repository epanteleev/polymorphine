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

        } else if constexpr (std::is_same_v<T, const ArgumentValue *> || std::is_same_v<T, const ValueInstruction *>) {
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
    const auto lhs = get_lir_operand(inst->lhs());
    const auto rhs = get_lir_operand(inst->rhs());
    switch (inst->op()) {
        case BinaryOp::Add: {
            const auto add = m_bb->inst(LIRProducerInstruction::add(lhs, rhs));
            memorize(inst, add->def(0));
            break;
        }
        case BinaryOp::Subtract: {
            const auto sub = m_bb->inst(LIRProducerInstruction::sub(lhs, rhs));
            memorize(inst, sub->def(0));
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
    const auto& cond = cond_branch->condition();
    const auto true_target = m_bb_mapping.at(cond_branch->on_true());
    const auto false_target = m_bb_mapping.at(cond_branch->on_false());

    if (cond.isa(icmp(signed_v(), signed_v()))) {
        const auto icmp = dynamic_cast<const IcmpInstruction*>(cond.get<const ValueInstruction*>());
        assertion(icmp != nullptr, "Expected IcmpInstruction for signed comparison");
        m_bb->inst(LIRCondBranch::jcc(signed_cond_type(icmp->predicate()), true_target, false_target));

    } else if (cond.isa(icmp(unsigned_v(), unsigned_v()))) {
        const auto icmp = dynamic_cast<const IcmpInstruction*>(cond.get<const ValueInstruction*>());
        assertion(icmp != nullptr, "Expected IcmpInstruction for signed comparison");
        m_bb->inst(LIRCondBranch::jcc(unsigned_cond_type(icmp->predicate()), true_target, false_target));

    } else {
        die("Unsupported condition type in cond branch");
    }
}

void FunctionLower::accept(Call *inst) {
    std::vector<LIROperand> args;
    args.reserve(inst->operands().size());

    for (const auto &arg: inst->operands()) {
        const auto arg_vreg = get_lir_operand(arg);
        const auto type = dynamic_cast<const NonTrivialType*>(arg.type());
        assertion(type != nullptr, "Expected NonTrivialType for call argument");

        const auto copy = m_bb->inst(LIRProducerInstruction::copy(type->size_of(), arg_vreg));
        args.emplace_back(copy->def(0));
    }
    const auto cont = m_bb_mapping.at(inst->cont());
    const auto& proto = inst->prototype();
    const auto ret_type = dynamic_cast<const NonTrivialType*>(proto.ret_type());
    assertion(ret_type != nullptr, "Expected NonTrivialType for return type");

    const auto call = m_bb->inst(LIRCall::call(std::string{proto.name()}, ret_type->size_of(), cont, std::move(args)));
    const auto copy_ret = cont->inst(LIRProducerInstruction::copy(ret_type->size_of(), call->def(0)));
    memorize(inst, copy_ret->def(0));
}

void FunctionLower::accept(Return *inst) {
    m_bb->inst(LIRReturn::ret());
}

void FunctionLower::accept(ReturnValue *inst) {
    const auto ret_value = inst->ret_value();
    const auto ret_type = dynamic_cast<const PrimitiveType*>(ret_value.type());
    assertion(ret_type != nullptr, "Expected PrimitiveType for return value");

    const auto ret_val = get_lir_operand(inst->ret_value());
    const auto copy = m_bb->inst(LIRProducerInstruction::copy(ret_type->size_of(), ret_val));
    m_bb->inst(LIRReturn::ret(copy->def(0)));
}

void FunctionLower::accept(Store *store) {
    const auto pointer = store->pointer();
    const auto value = store->value();

    const auto pointer_vreg = get_lir_val(pointer);
    const auto value_vreg = get_lir_operand(value);
    if (pointer.isa(alloc())) {
        m_bb->inst(LIRInstruction::mov(pointer_vreg, value_vreg));

    } else if (pointer.isa(argument())) {
        m_bb->inst(LIRInstruction::store(pointer_vreg, value_vreg));

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
    m_bb->inst(LIRInstruction::cmp(lhs, rhs));
}

void FunctionLower::lower_flag2int(const Unary *inst) {
    const auto cond = inst->operand();
    if (cond.isa(icmp(signed_v(), signed_v()))) {
        const auto icmp = dynamic_cast<const IcmpInstruction*>(cond.get<const ValueInstruction*>());
        assertion(icmp != nullptr, "Expected IcmpInstruction for signed comparison");

        make_setcc(inst, signed_cond_type(icmp->predicate()));

    } else if (cond.isa(icmp(unsigned_v(), unsigned_v()))) {
        const auto icmp = dynamic_cast<const IcmpInstruction*>(cond.get<const ValueInstruction*>());
        assertion(icmp != nullptr, "Expected IcmpInstruction for signed comparison");

        make_setcc(inst, unsigned_cond_type(icmp->predicate()));

    } else {
        die("Unsupported condition type in cond branch");
    }
}

void FunctionLower::lower_load(const Unary *inst) {
    const auto pointer = inst->operand();
    const auto type = dynamic_cast<const PrimitiveType*>(inst->type());
    assertion(type != nullptr, "Expected PrimitiveType for load operation");

    const auto pointer_vreg = get_lir_val(pointer);

    if (pointer.isa(alloc())) {
        const auto copy_inst = m_bb->inst(LIRProducerInstruction::copy(type->size_of(), pointer_vreg));
        memorize(inst, copy_inst->def(0));

    } else if (pointer.isa(argument())) {
        const auto load_inst = m_bb->inst(LIRProducerInstruction::load(type->size_of(), pointer_vreg));
        memorize(inst, load_inst->def(0));

    } else {
        unimplemented();
    }
}

void FunctionLower::make_setcc(const Unary *inst, const LIRCondType cond_type) {
    const auto setcc = m_bb->inst(LIRSetCC::setcc(cond_type));
    memorize(inst, setcc->def(0));
}

void FunctionLower::accept(Unary *inst) {
    switch (inst->op()) {
        case UnaryOp::Flag2Int: lower_flag2int(inst); break;
        case UnaryOp::Load: lower_load(inst); break;
        default: die("Unsupported unary operation: {}", static_cast<int>(inst->op()));
    }
}
