#include "lir/x64/lower/FunctionLower.h"
#include "lir/x64/instruction/LIRAdjustStack.h"
#include "lir/x64/instruction/LIRBranch.h"
#include "lir/x64/instruction/LIRCMove.h"
#include "lir/x64/instruction/LIRCondBranch.h"
#include "lir/x64/instruction/LIRInstruction.h"
#include "lir/x64/instruction/LIRProducerInstruction.h"
#include "lir/x64/instruction/LIRSetCC.h"
#include "lir/x64/instruction/Matcher.h"
#include "lir/x64/instruction/ParallelCopy.h"
#include "lir/x64/operand/OperandMatcher.h"

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

/**
 * Determines the condition type from a given Value.
 */
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

static std::pair<Value, std::int64_t> try_fold_field_access_iter(const FieldAccess* field_access) noexcept {
    Value current = field_access;
    std::size_t offset{};
    Value pointer = field_access->pointer();

    while (true) {
        if (current.isa(gfp())) {
            const auto gfp = dynamic_cast<const GetFieldPtr*>(current.get<ValueInstruction*>());
            const auto& access_type = gfp->access_type();
            offset += static_cast<std::int64_t>(access_type->offset_of(gfp->index()));
            current = gfp->pointer();
            pointer = gfp->pointer();
            continue;
        }

        if (current.isa(gep(any_value(), constant()))) {
            const auto gep = dynamic_cast<const GetElementPtr*>(current.get<ValueInstruction*>());
            const auto& access_type = gep->access_type();
            const auto index = gep->index().get<std::int64_t>();
            offset += static_cast<std::int64_t>(access_type->size_of() * index);
            current = gep->pointer();
            pointer = gep->pointer();
            continue;
        }
        break;
    }

    return {pointer, static_cast<std::int64_t>(offset)};
}

/**
 * Calculates the index for a GetFieldPtr instruction.
 */
static std::pair<Value, Value> try_fold_gfp_index(const GetFieldPtr* gfp_inst) noexcept {
    auto [pointer, offset] = try_fold_field_access_iter(gfp_inst);

    const auto& field_type = gfp_inst->access_type()->field(gfp_inst->index());
    const auto index = offset / field_type->size_of();
    return {pointer, Value::i64(static_cast<std::int64_t>(index))};
}

/**
 * Calculates the index for a GetElementPtr instruction.
 */
static std::pair<Value, Value> try_fold_gep_index(const GetElementPtr* gep_inst) noexcept {
    if (const auto& index = gep_inst->index(); !index.isa(constant())) {
        return {gep_inst->pointer(), gep_inst->index()};
    }

    auto [pointer, offset] = try_fold_field_access_iter(gep_inst);
    const auto index = offset / gep_inst->access_type()->size_of();
    return {pointer, Value::i64(static_cast<std::int64_t>(index))};
}

static std::pair<Value, Value> try_fold_field_access(const FieldAccess* field_access) noexcept {
    if (const auto gfp = dynamic_cast<const GetFieldPtr*>(field_access)) {
        return try_fold_gfp_index(gfp);
    }
    if (const auto gep = dynamic_cast<const GetElementPtr*>(field_access)) {
        return try_fold_gep_index(gep);
    }

    std::unreachable();
}

static bool is_pinned(const Instruction& inst) noexcept {
    const auto value_inst = dynamic_cast<const ValueInstruction*>(&inst);
    if (value_inst == nullptr) {
        return true;
    }

    if (inst.isa(load()) || inst.isa(any_terminate())) {
        return true;
    }

    if (dynamic_cast<const FieldAccess*>(value_inst) != nullptr) {
        return false;
    }

    if (value_inst->users().size() > 1) {
        return true;
    }

    return false;
}

LIRFuncData FunctionLower::create_lir_function(const FunctionData &function) {
    std::vector<LIRArg> args;
    args.reserve(function.args().size());

    for (auto [idx, varg]: std::ranges::views::enumerate(function.args())) {
        args.emplace_back(idx, varg.type()->size_of(), varg.attributes());
    }

    return {function.name(), std::move(args)};
}

void FunctionLower::setup_arguments() {
    m_bb->ins(LIRAdjustStack::prologue());

    // rdx & rcx might be used for sal & sar instruction. We do not track these instruction,
    // so that we have to isolate corresponded arguments.
    static constexpr auto RDX_IDX = 2;
    static constexpr auto RCX_IDX = 3;
    for (const auto& [idx, arg, lir_arg]: std::ranges::zip_view(std::ranges::iota_view{0UL}, m_function.args(), m_obj_function.args())) {
        if (arg.attributes().has(Attribute::ByValue)) {
            memorize(&arg, lir_arg);
            continue;
        }

        if (idx != RDX_IDX && idx != RCX_IDX) {
            const auto is_no_live_out = [&](const auto& user) { // TODO better live-out checker.
                return user->owner() == m_function.first() && !user->isa(any_terminate());
            };

            if (std::ranges::all_of(arg.users(), is_no_live_out)) {
                memorize(&arg, lir_arg);
                continue;
            }
        }

        const auto copy = m_bb->ins(LIRProducerInstruction::copy(lir_arg.size(), lir_arg));
        memorize(&arg, copy->def(0));
    }
}

void FunctionLower::traverse_instructions() {
    for (const auto &bb: m_dom_ordering) {
        m_bb = m_bb_mapping.at(bb);
        for (auto &inst: bb->instructions()) {
            if (is_pinned(inst)) {
                inst.visit(*this);
                continue;
            }

            const auto value_inst = dynamic_cast<ValueInstruction*>(&inst);
            // Put the instruction into the set. It will be processed later on demand before its user.
            m_late_schedule_instructions.emplace(value_inst);
        }
    }
}

void FunctionLower::setup_bb_mapping() {
    for (const auto& bb: m_function.basic_blocks()) {
        if (&bb == m_function.first()) {
            m_bb_mapping.emplace(&bb, m_obj_function.first());
            continue;
        }

        auto lir_bb = m_obj_function.create_mach_block();
        m_bb_mapping.emplace(&bb, lir_bb);
    }
}

static void insert_copies(ParallelCopy& p_copy) noexcept {
    for (auto [idx, input, target]: std::views::zip(std::views::iota(0UL), p_copy.inputs(), p_copy.targets())) {
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

        } else if constexpr (std::is_same_v<T, ArgumentValue *>) {
            return m_value_mapping.at(LocalValue::from(v));

        } else if constexpr (std::is_same_v<T, ValueInstruction *>) {
            const auto lir_val_iter = m_value_mapping.find(LocalValue::from(v));
            if (lir_val_iter != m_value_mapping.end()) {
                return lir_val_iter->second;
            }

            assertion(m_late_schedule_instructions.contains(v), "invariant");
            schedule_late(v);
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
    if (const auto vreg = lir_operand.as_vreg()) {
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
        case BinaryOp::BitwiseXor: {
            const auto xxor = m_bb->ins(LIRProducerInstruction::xxor(lhs, rhs));
            memorize(inst, xxor->def(0));
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
    try_schedule_late(cond_branch->condition());
    const auto true_target = m_bb_mapping.at(cond_branch->on_true());
    const auto false_target = m_bb_mapping.at(cond_branch->on_false());

    m_bb->ins(LIRCondBranch::jcc(cond_type(cond_branch->condition()), true_target, false_target));
}

void FunctionLower::accept(Call *inst) {
    m_bb->ins(LIRAdjustStack::down_stack());

    std::vector<LIROperand> args;
    args.reserve(inst->operands().size());
    const auto proto = inst->prototype();
    for (const auto &[arg_idx, arg]: std::ranges::enumerate_view(inst->operands())) {
        if (!proto->attribute(arg_idx).has(Attribute::ByValue)) {
            // Primitive type arguments are passed directly via registers or stack.
            args.emplace_back(lower_primitive_type_argument(arg));
            continue;
        }

        const auto arg_vreg = get_lir_operand(arg);
        const auto alloc = dynamic_cast<Alloc*>(arg.get<ValueInstruction*>());
        const auto allocated_type = alloc->allocated_type();

        const auto gen = m_bb->ins(LIRProducerInstruction::gen(allocated_type->size_of()));
        for (std::size_t offset{}; offset < allocated_type->size_of(); offset += 8) { //FIXME handle <8 bytes
            const auto load = m_bb->ins(LIRProducerInstruction::load_from_stack(8, arg_vreg.as_vreg().value(), LirCst::imm64(offset/8)));
            m_bb->ins(LIRInstruction::store_on_stack(gen->def(0), LirCst::imm64(offset/8), load->def(0)));
        }

        args.emplace_back(gen->def(0));
    }

    const auto cont = m_bb_mapping.at(inst->cont());
    const auto ret_type = dynamic_cast<const NonTrivialType*>(proto->ret_type());
    assertion(ret_type != nullptr, "Expected NonTrivialType for return type");

    const auto call = m_bb->ins(LIRCall::call(std::string{proto->name()}, ret_type->size_of(), cont, std::move(args), proto->linkage()));
    cont->ins(LIRAdjustStack::up_stack());
    const auto copy_ret = cont->ins(LIRProducerInstruction::copy(ret_type->size_of(), call->def(0)));
    memorize(inst, copy_ret->def(0));
}

void FunctionLower::accept(Return *inst) {
    m_bb->ins(LIRAdjustStack::epilogue());
    m_bb->ins(LIRReturn::ret());
}

void FunctionLower::accept(ReturnValue *inst) {
    const auto& ret_value = inst->first();
    const auto ret_type = dynamic_cast<const PrimitiveType*>(ret_value.type());
    assertion(ret_type != nullptr, "Expected PrimitiveType for return value");

    const auto copy_first = m_bb->ins(LIRProducerInstruction::copy(ret_type->size_of(), get_lir_operand(inst->first())));
    if (const auto second = inst->second(); !second) {
        m_bb->ins(LIRAdjustStack::epilogue());
        m_bb->ins(LIRReturn::ret(copy_first->def(0)));

    } else {
        const auto second_type = dynamic_cast<const PrimitiveType*>(second->type());
        assertion(second_type != nullptr, "Expected PrimitiveType for return value");

        const auto copy_second = m_bb->ins(LIRProducerInstruction::copy(second_type->size_of(), get_lir_operand(*second)));
        m_bb->ins(LIRAdjustStack::epilogue());
        m_bb->ins(LIRReturn::ret(copy_first->def(0), copy_second->def(0)));
    }
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

    const auto value_vreg = get_lir_operand(value);
    if (pointer.isa(any_stack_alloc())) {
        const auto pointer_vreg = get_lir_val(pointer);
        m_bb->ins(LIRInstruction::mov(pointer_vreg, value_vreg));
        return;
    }

    if (pointer.isa(field_access())) {
        const auto gep = dynamic_cast<FieldAccess*>(pointer.get<ValueInstruction*>());
        const auto [src, idx] = try_fold_field_access(gep);
        const auto src_vreg = get_lir_val(src);
        const auto idx_lir_op = get_lir_operand(idx);
        if (src.isa(any_stack_alloc())) {
            m_bb->ins(LIRInstruction::store_on_stack(src_vreg, idx_lir_op, value_vreg));
            return;
        }
        m_bb->ins(LIRInstruction::mov_by_idx(src_vreg, idx_lir_op, value_vreg));
        return;
    }

    if (pointer.isa(argument())) {
        const auto pointer_vreg = get_lir_val(pointer);
        m_bb->ins(LIRInstruction::store(pointer_vreg, value_vreg));
        return;
    }

    unimplemented();
}

void FunctionLower::accept(Alloc *alloc) {
    const auto size = alloc->allocated_type()->size_of();
    const auto alloc_inst = m_bb->ins(LIRProducerInstruction::gen(size));
    memorize(alloc, alloc_inst->def(0));
}

void FunctionLower::accept(IcmpInstruction *icmp) {
    const auto lhs = get_lir_operand(icmp->lhs());
    const auto rhs = get_lir_operand(icmp->rhs());
    m_bb->ins(LIRInstruction::cmp(lhs, rhs));
}

void FunctionLower::accept(GetElementPtr *gep) {
    const auto pointer = get_lir_val(gep->pointer());
    const auto index = get_lir_operand(gep->index());
    const auto type = dynamic_cast<const PrimitiveType*>(gep->type());
    assertion(type != nullptr, "Expected PrimitiveType for GEP operation");
    const auto gep_inst = m_bb->ins(LIRProducerInstruction::lea(type->size_of(), pointer, index));
    memorize(gep, gep_inst->def(0));
}

void FunctionLower::accept(GetFieldPtr *gfp) {
    unimplemented();
}

void FunctionLower::accept(Select *select) {
    const auto& on_true = select->on_true();
    const auto& on_false = select->on_false();
    const auto on_true_lir = get_lir_operand(on_true);
    const auto on_false_lir = get_lir_operand(on_false);
    try_schedule_late(select->condition());

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

void FunctionLower::accept(IntDiv *div) {
    const auto lhs_val = div->lhs();
    const auto lhs = get_lir_operand(lhs_val);
    const auto copy = m_bb->ins(LIRProducerInstruction::copy(lhs.size(), lhs));
    const auto copy_def = copy->def(0);

    const auto rhs = get_lir_operand(div->rhs());
    const auto type = lhs_val.type();
    const LIRProducerInstruction* idiv;
    if (type->isa(signed_type())) {
        idiv = m_bb->ins(LIRProducerInstruction::idiv(copy_def, rhs));

    } else if (type->isa(unsigned_type())) {
        idiv = m_bb->ins(LIRProducerInstruction::udiv(copy_def, rhs));

    } else {
        die("Unsupported type for IntDiv");
    }

    if (const auto quotient = div->quotient(); !quotient->users().empty()) {
        const auto quotient_type = dynamic_cast<const PrimitiveType*>(quotient->type());
        const auto copy_quotient = m_bb->ins(LIRProducerInstruction::copy(quotient_type->size_of(), idiv->def(0)));
        memorize(quotient, copy_quotient->def(0));
    }

    if (const auto remain = div->remain(); !remain->users().empty()) {
        const auto remain_type = dynamic_cast<const PrimitiveType*>(remain->type());
        const auto copy_remain = m_bb->ins(LIRProducerInstruction::copy(remain_type->size_of(), idiv->def(1)));
        memorize(remain, copy_remain->def(0));
    }
}

void FunctionLower::accept(Unary *inst) {
    switch (inst->op()) {
        case UnaryOp::Flag2Int: {
            const auto cond = inst->operand();
            try_schedule_late(cond);
            make_setcc(inst, cond_type(cond));
            break;
        }
        case UnaryOp::Load: lower_load(inst); break;
        case UnaryOp::SignExtend: {
            const auto operand = get_lir_operand(inst->operand());
            const auto type = dynamic_cast<const PrimitiveType*>(inst->type());
            assertion(type != nullptr, "Expected PrimitiveType for SignExtend operation");

            const auto movsx = m_bb->ins(LIRProducerInstruction::movsx(type->size_of(), operand));
            memorize(inst, movsx->def(0));
            break;
        }
        case UnaryOp::ZeroExtend: {
            const auto operand = get_lir_operand(inst->operand());
            const auto type = dynamic_cast<const PrimitiveType*>(inst->type());
            assertion(type != nullptr, "Expected PrimitiveType for ZeroExtend operation");

            const auto movzx = m_bb->ins(LIRProducerInstruction::movzx(type->size_of(), operand));
            memorize(inst, movzx->def(0));
            break;
        }
        case UnaryOp::Trunk: {
            const auto operand = get_lir_operand(inst->operand());
            const auto type = dynamic_cast<const PrimitiveType*>(inst->type());
            assertion(type != nullptr, "Expected PrimitiveType for Trunk operation");

            const auto trunc = m_bb->ins(LIRProducerInstruction::trunc(type->size_of(), operand));
            memorize(inst, trunc->def(0));
            break;
        }
        case UnaryOp::Bitcast: {
            const auto operand = get_lir_operand(inst->operand());
            const auto type = dynamic_cast<const PrimitiveType*>(inst->type());
            assertion(type != nullptr, "Expected PrimitiveType for Bitcast operation");

            const auto copy = m_bb->ins(LIRProducerInstruction::copy(type->size_of(), operand));
            memorize(inst, copy->def(0));
            break;
        }
        default: die("Unsupported unary operation: {}", static_cast<int>(inst->op()));
    }
}

void FunctionLower::lower_load(const Unary *inst) {
    const auto pointer = inst->operand();
    const auto type = dynamic_cast<const PrimitiveType*>(inst->type());
    assertion(type != nullptr, "Expected PrimitiveType for load operation");

    if (pointer.isa(any_stack_alloc())) {
        const auto pointer_vreg = get_lir_val(pointer);
        const auto copy_inst = m_bb->ins(LIRProducerInstruction::copy(type->size_of(), pointer_vreg));
        memorize(inst, copy_inst->def(0));
        return;
    }

    if (pointer.isa(field_access())) {
        const auto gep = dynamic_cast<FieldAccess*>(pointer.get<ValueInstruction*>());
        const auto [src, idx] = try_fold_field_access(gep);
        const auto src_vreg = get_lir_val(src);
        const auto idx_lir_op = get_lir_operand(idx);

        if (src.isa(any_stack_alloc())) {
            const auto load_inst = m_bb->ins(LIRProducerInstruction::load_from_stack(type->size_of(), src_vreg, idx_lir_op));
            memorize(inst, load_inst->def(0));
            return;
        }

        const auto load_inst = m_bb->ins(LIRProducerInstruction::load_by_idx(type->size_of(), src_vreg, idx_lir_op));
        memorize(inst, load_inst->def(0));
        return;
    }

    if (pointer.isa(argument())) {
        const auto pointer_vreg = get_lir_val(pointer);
        const auto load_inst = m_bb->ins(LIRProducerInstruction::load(type->size_of(), pointer_vreg));
        memorize(inst, load_inst->def(0));
        return;
    }

    unimplemented();
}

LIRVal FunctionLower::lower_primitive_type_argument(const Value& arg) {
    const auto type = dynamic_cast<const PrimitiveType*>(arg.type());
    assertion(type != nullptr, "Expected NonTrivialType for call argument");

    if (arg.isa(any_stack_alloc())) {
        // Escaped stack allocation. We need to load stack address.
        const auto arg_vreg = get_lir_operand(arg);
        const auto lea = m_bb->ins(LIRProducerInstruction::load_stack_addr(type->size_of(), arg_vreg.as_vreg().value(), LirCst::imm64(0UL)));
        return lea->def(0);
    }

    if (arg.isa(field_access())) {
        const auto gep = dynamic_cast<FieldAccess*>(arg.get<ValueInstruction*>());
        const auto [src, idx] = try_fold_field_access(gep);
        const auto src_vreg = get_lir_val(src);
        const auto idx_lir_op = get_lir_operand(idx);
        if (src.isa(any_stack_alloc())) {
            const auto lea_stack_val = m_bb->ins(LIRProducerInstruction::load_stack_addr(type->size_of(), src_vreg, idx_lir_op));
            return lea_stack_val->def(0);
        }

        const auto lea = m_bb->ins(LIRProducerInstruction::lea(type->size_of(), src_vreg, idx_lir_op));
        return lea->def(0);
    }

    const auto arg_vreg = get_lir_operand(arg);
    const auto copy = m_bb->ins(LIRProducerInstruction::copy(type->size_of(), arg_vreg));
    return copy->def(0);
}

void FunctionLower::make_setcc(const ValueInstruction *inst, const aasm::CondType cond_type) {
    const auto setcc = m_bb->ins(LIRSetCC::setcc(cond_type));
    memorize(inst, setcc->def(0));
}

void FunctionLower::try_schedule_late(const Value &cond) {
    if (!cond.is<ValueInstruction*>()) {
        return;
    }

    const auto inst = cond.get<ValueInstruction*>();
    try_schedule_late(inst);
}

void FunctionLower::try_schedule_late(ValueInstruction *inst) {
    if (!m_late_schedule_instructions.contains(inst)) {
        return;
    }

    schedule_late(inst);
}

void FunctionLower::schedule_late(ValueInstruction *inst) {
    m_late_schedule_instructions.erase(inst);
    inst->visit(*this);
}