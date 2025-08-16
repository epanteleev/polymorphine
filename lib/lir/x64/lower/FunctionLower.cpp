#include "lir/x64/lower/FunctionLower.h"
#include "lir/x64/instruction/LIRAdjustStack.h"
#include "lir/x64/instruction/LIRBranch.h"
#include "lir/x64/instruction/LIRCMove.h"
#include "lir/x64/instruction/LIRCondBranch.h"
#include "lir/x64/instruction/LIRProducerInstruction.h"
#include "lir/x64/instruction/LIRSetCC.h"
#include "lir/x64/instruction/Matcher.h"
#include "lir/x64/instruction/ParallelCopy.h"

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
static aasm::CondType signed_cond_type(const IcmpPredicate predicate) noexcept {
    switch (predicate) {
        case IcmpPredicate::Eq: return aasm::CondType::E;
        case IcmpPredicate::Ne: return aasm::CondType::NE;
        case IcmpPredicate::Gt: return aasm::CondType::G;
        case IcmpPredicate::Ge: return aasm::CondType::GE;
        case IcmpPredicate::Lt: return aasm::CondType::NGE;
        case IcmpPredicate::Le: return aasm::CondType::NG;
        default: die("Unsupported signed condition type in flag2int");
    }
}

/**
 * Converts an unsigned condition to LIRCondType.
 * @param predicate The IcmpPredicate representing the condition.
 * @return The corresponding LIRCondType.
 */
static aasm::CondType unsigned_cond_type(const IcmpPredicate predicate) noexcept {
    switch (predicate) {
        case IcmpPredicate::Eq: return aasm::CondType::E;
        case IcmpPredicate::Ne: return aasm::CondType::NE;
        case IcmpPredicate::Gt: return aasm::CondType::A;
        case IcmpPredicate::Ge: return aasm::CondType::AE;
        case IcmpPredicate::Lt: return aasm::CondType::NAE;
        case IcmpPredicate::Le: return aasm::CondType::NA;
        default: die("Unsupported unsigned condition type in flag2int");
    }
}

static aasm::CondType cond_type(const Value& cond) noexcept {
    if (cond.isa(icmp(signed_v(), signed_v()))) {
        const auto icmp = dynamic_cast<const IcmpInstruction*>(cond.get<ValueInstruction*>());
        assertion(icmp != nullptr, "Expected IcmpInstruction for signed comparison");
        return signed_cond_type(icmp->predicate());
    }

    if (cond.isa(icmp(unsigned_v(), unsigned_v()))) {
        const auto icmp = dynamic_cast<const IcmpInstruction*>(cond.get<ValueInstruction*>());
        assertion(icmp != nullptr, "Expected IcmpInstruction for signed comparison");
        return unsigned_cond_type(icmp->predicate());
    }

    die("Unsupported condition type");
}

static LIRLinkage linkage_from_mir(const FunctionLinkage linkage) noexcept {
    switch (linkage) {
        case FunctionLinkage::EXTERN: return LIRLinkage::EXTERNAL;
        case FunctionLinkage::INTERNAL: return LIRLinkage::INTERNAL;
        default: die("Unsupported function linkage type in LIR");
    }
}

void FunctionLower::setup_arguments() {
    m_bb->ins(LIRAdjustStack::prologue());
    for (const auto& [arg, lir_arg]: std::ranges::zip_view(m_function.args(), m_obj_function->args())) {
        const auto copy = m_bb->ins(LIRProducerInstruction::copy(lir_arg.size(), lir_arg));
        memorize(&arg, copy->def(0));
    }
}

void FunctionLower::traverse_instructions() {
    for (const auto &bb: m_dom_ordering) {
        m_bb = m_bb_mapping.at(bb);
        for (auto &inst: bb->instructions()) {
            inst.visit(*this);
        }
    }
}

void FunctionLower::setup_bb_mapping() {
    for (const auto& bb: m_function.basic_blocks()) {
        if (&bb == m_function.first()) {
            m_bb_mapping.emplace(&bb, m_obj_function->first());
            continue;
        }

        auto lir_bb = m_obj_function->create_mach_block();
        m_bb_mapping.emplace(&bb, lir_bb);
    }
}

static void insert_copies(ParallelCopy& p_copy) noexcept {
    for (auto [idx, input, target]: std::views::zip(std::views::iota(0), p_copy.inputs(), p_copy.targets())) {
        const auto lir_val = LIRVal::try_from(input);
        assertion(lir_val.has_value(), "Expected LIRVal for ParallelCopy input");

        const auto copy = target->ins_before(target->last(), LIRProducerInstruction::copy(lir_val->size(), input));
        p_copy.in(idx, copy->def(0));
    }
}

void FunctionLower::finalize_parallel_copies() const noexcept {
    for (auto& bb: m_parallel_copy_owners) {
        for (auto& inst: bb->instructions()) {
            if (!inst.isa(parallel_copy())) break;

            insert_copies(dynamic_cast<ParallelCopy&>(inst));
        }
    }
}

LIROperand FunctionLower::get_lir_operand(const Value &val) {
    const auto visitor = [&]<typename T>(const T &v) -> LIROperand {
        if constexpr (std::is_same_v<T, double>) {
            unimplemented();

        } else if constexpr (std::is_same_v<T, std::int64_t> || std::is_same_v<T, std::uint64_t>) {
            return make_constant(*val.type(), v);

        } else if constexpr (std::is_same_v<T, ArgumentValue *> || std::is_same_v<T, ValueInstruction *>) {
            return m_value_mapping.at(LocalValue::from(v));

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
            const auto add = m_bb->ins(LIRProducerInstruction::add(lhs, rhs));
            memorize(inst, add->def(0));
            break;
        }
        case BinaryOp::Subtract: {
            const auto sub = m_bb->ins(LIRProducerInstruction::sub(lhs, rhs));
            memorize(inst, sub->def(0));
            break;
        }
        default: die("Unsupported binary operation: {}", static_cast<int>(inst->op()));
    }
}

void FunctionLower::accept(Branch *branch) {
    const auto target = m_bb_mapping.at(branch->target());
    m_bb->ins(LIRBranch::jmp(target));
}

void FunctionLower::accept(CondBranch *cond_branch) {
    const auto true_target = m_bb_mapping.at(cond_branch->on_true());
    const auto false_target = m_bb_mapping.at(cond_branch->on_false());

    m_bb->ins(LIRCondBranch::jcc(cond_type(cond_branch->condition()), true_target, false_target));
}

void FunctionLower::accept(Call *inst) {
    m_bb->ins(LIRAdjustStack::down_stack());

    std::vector<LIROperand> args;
    args.reserve(inst->operands().size());
    for (const auto &arg: inst->operands()) {
        const auto arg_vreg = get_lir_operand(arg);
        const auto type = dynamic_cast<const NonTrivialType*>(arg.type());
        assertion(type != nullptr, "Expected NonTrivialType for call argument");

        const auto copy = m_bb->ins(LIRProducerInstruction::copy(type->size_of(), arg_vreg));
        args.emplace_back(copy->def(0));
    }
    const auto cont = m_bb_mapping.at(inst->cont());
    const auto& proto = inst->prototype();
    const auto ret_type = dynamic_cast<const NonTrivialType*>(proto.ret_type());
    assertion(ret_type != nullptr, "Expected NonTrivialType for return type");

    const auto call = m_bb->ins(LIRCall::call(std::string{proto.name()}, ret_type->size_of(), cont, std::move(args), linkage_from_mir(proto.linkage())));
    cont->ins(LIRAdjustStack::up_stack());
    const auto copy_ret = cont->ins(LIRProducerInstruction::copy(ret_type->size_of(), call->def(0)));
    memorize(inst, copy_ret->def(0));
}

void FunctionLower::accept(Return *inst) {
    m_bb->ins(LIRAdjustStack::epilogue());
    m_bb->ins(LIRReturn::ret());
}

void FunctionLower::accept(ReturnValue *inst) {
    const auto& ret_value = inst->ret_value();
    const auto ret_type = dynamic_cast<const PrimitiveType*>(ret_value.type());
    assertion(ret_type != nullptr, "Expected PrimitiveType for return value");

    const auto ret_val = get_lir_operand(inst->ret_value());
    const auto copy = m_bb->ins(LIRProducerInstruction::copy(ret_type->size_of(), ret_val));
    m_bb->ins(LIRAdjustStack::epilogue());
    m_bb->ins(LIRReturn::ret(copy->def(0)));
}

void FunctionLower::accept(Phi *inst) {
    m_parallel_copy_owners.emplace(m_bb);

    std::vector<LIROperand> incoming_values;
    incoming_values.reserve(inst->incoming().size());

    std::vector<LIRBlock*> incoming_targets;
    incoming_targets.reserve(inst->incoming().size());

    for (const auto [target, incoming]: std::views::zip(inst->incoming(), inst->operands())) {
        incoming_values.emplace_back(get_lir_val(incoming));
        incoming_targets.push_back(m_bb_mapping.at(target));
    }
    const auto parallel_copy = m_bb->ins(ParallelCopy::copy(std::move(incoming_values), std::move(incoming_targets)));
    memorize(inst, parallel_copy->def(0));
}

void FunctionLower::accept(Store *store) {
    const auto pointer = store->pointer();
    const auto value = store->value();

    const auto pointer_vreg = get_lir_val(pointer);
    const auto value_vreg = get_lir_operand(value);
    if (pointer.isa(alloc())) {
        m_bb->ins(LIRInstruction::mov(pointer_vreg, value_vreg));

    } else if (pointer.isa(argument())) {
        m_bb->ins(LIRInstruction::store(pointer_vreg, value_vreg));

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
        const auto alloc_inst = m_bb->ins(LIRProducerInstruction::gen(size));
        memorize(alloc, alloc_inst->def(0));

    } else {
        unimplemented();
    }
}

void FunctionLower::accept(IcmpInstruction *icmp) {
    const auto lhs = get_lir_operand(icmp->lhs());
    const auto rhs = get_lir_operand(icmp->rhs());
    m_bb->ins(LIRInstruction::cmp(lhs, rhs));
}

void FunctionLower::accept(Select *select) {
    const auto& on_true = select->on_true();
    const auto& on_false = select->on_false();
    const auto on_true_lir = get_lir_operand(on_true);
    const auto on_false_lir = get_lir_operand(on_false);

    const auto cond_ty = cond_type(select->condition());
    if (on_true.isa(integral(1)) && on_false.isa(integral(0))) {
        make_setcc(select, cond_ty);

    } else if (on_true.isa(integral(0)) && on_false.isa(integral(1))) {
        make_setcc(select, aasm::invert(cond_ty));

    } else {
        const auto cmov = m_bb->ins(LIRCMove::cmov(cond_ty, on_true_lir, on_false_lir));
        memorize(select, cmov->def(0));
    }
}

void FunctionLower::lower_load(const Unary *inst) {
    const auto pointer = inst->operand();
    const auto type = dynamic_cast<const PrimitiveType*>(inst->type());
    assertion(type != nullptr, "Expected PrimitiveType for load operation");

    const auto pointer_vreg = get_lir_val(pointer);

    if (pointer.isa(alloc())) {
        const auto copy_inst = m_bb->ins(LIRProducerInstruction::copy(type->size_of(), pointer_vreg));
        memorize(inst, copy_inst->def(0));

    } else if (pointer.isa(argument())) {
        const auto load_inst = m_bb->ins(LIRProducerInstruction::load(type->size_of(), pointer_vreg));
        memorize(inst, load_inst->def(0));

    } else {
        unimplemented();
    }
}

void FunctionLower::make_setcc(const ValueInstruction *inst, const aasm::CondType cond_type) {
    const auto setcc = m_bb->ins(LIRSetCC::setcc(cond_type));
    memorize(inst, setcc->def(0));
}

void FunctionLower::accept(Unary *inst) {
    switch (inst->op()) {
        case UnaryOp::Flag2Int: make_setcc(inst, cond_type(inst->operand())); break;
        case UnaryOp::Load: lower_load(inst); break;
        default: die("Unsupported unary operation: {}", static_cast<int>(inst->op()));
    }
}