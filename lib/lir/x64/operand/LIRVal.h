#pragma once

#include <expected>
#include <utility>

#include "LIRArg.h"
#include "lir/x64/lir_frwd.h"
#include "utility/Error.h"

/**
 * Represents a value in the Low-Level Intermediate Representation (LIR).
 * This class can represent either an argument or a produced values from instructions.
 */
class LIRVal final {
    enum class Op: std::uint8_t {
        Arg,
        Inst,
        Call
    };

    LIRVal(const std::uint8_t size, const std::uint8_t align, const std::uint8_t index, LIRArg *def) noexcept:
        m_size(size),
        m_align(align),
        m_index(index),
        m_type(Op::Arg) {
        m_variant.m_arg = def;
    }

    LIRVal(const std::uint8_t size, const std::uint8_t align, const std::uint8_t index, LIRProducerInstructionBase *def) noexcept:
        m_size(size),
        m_align(align),
        m_index(index),
        m_type(Op::Inst) {
        m_variant.m_inst = def;
    }

    LIRVal(const std::uint8_t size, const std::uint8_t align, const std::uint8_t index, LIRCall *def) noexcept:
        m_size(size),
        m_align(align),
        m_index(index),
        m_type(Op::Call) {
        m_variant.m_call = def;
    }

public:
    [[nodiscard]]
    std::optional<const LIRArg*> arg() const noexcept {
        if (m_type == Op::Arg) {
            return m_variant.m_arg;
        }

        return std::nullopt;
    }

    [[nodiscard]]
    std::optional<const LIRProducerInstructionBase*> inst() const noexcept {
        if (m_type == Op::Inst) {
            return m_variant.m_inst;
        }

        return std::nullopt;
    }

    template<typename Matcher>
    [[nodiscard]]
    bool isa(Matcher&& matcher) const noexcept {
        return matcher(*this);
    }

    template<typename T>
    decltype(auto) visit(const T& visitor) const {
        switch (m_type) {
            case Op::Arg:  return visitor(*m_variant.m_arg);
            case Op::Inst: return visitor(*m_variant.m_inst);
            case Op::Call: return visitor(*m_variant.m_call);
            default: std::unreachable();
        }
    }

    [[nodiscard]]
    std::uint8_t size() const noexcept {
        return m_size;
    }

    [[nodiscard]]
    std::uint8_t alignment() const noexcept {
        return m_align;
    }

    [[nodiscard]]
    std::uint8_t index() const noexcept {
        return m_index;
    }

    bool operator==(const LIRVal &rhs) const noexcept {
        if (this == &rhs) {
            return true;
        }

        return m_size == rhs.m_size &&
               m_align == rhs.m_align &&
               m_index == rhs.m_index &&
               m_type == rhs.m_type &&
               m_variant.m_arg == rhs.m_variant.m_arg;
    }

    void add_user(LIRInstructionBase *inst) const noexcept;
    void kill_user(LIRInstructionBase *inst) const noexcept;

    void assign_reg(const OptionalGPVReg& reg) const noexcept;

    [[nodiscard]]
    const OptionalGPVReg& assigned_reg() const noexcept;

    [[nodiscard]]
    std::span<LIRInstructionBase* const> users() const noexcept;

    friend std::ostream& operator<<(std::ostream& os, const LIRVal& op) noexcept;

    static LIRVal from(const LIRArg* def) noexcept {
        return {def->size(), def->alignment(), static_cast<std::uint8_t>(def->index()), const_cast<LIRArg *>(def)};
    }

    template<typename T>
    requires std::derived_from<T, LIRProducerInstructionBase> || std::derived_from<T, LIRCall>
    static LIRVal reg(std::uint8_t size, std::uint8_t align, std::uint8_t index, T* def) noexcept {
        return {size, align, index, def};
    }

    static std::expected<LIRVal, Error> try_from(const LIROperand& op);

    static std::span<LIRVal const> defs(const LIRInstructionBase* inst) noexcept;

private:
    [[nodiscard]]
    std::size_t id() const noexcept;

    std::uint8_t m_size;
    std::uint8_t m_align;
    std::uint8_t m_index;
    Op m_type;
    union {
        LIRArg* m_arg;
        LIRProducerInstructionBase* m_inst;
        LIRCall* m_call;
    } m_variant{};
};

std::ostream& operator<<(std::ostream& os, const LIRVal& op) noexcept;

namespace details {
    struct LIRValHash final {
        [[nodiscard]]
        std::size_t operator()(const LIRVal& val) const noexcept {
            return static_cast<std::uint64_t>(val.size()) << 8 | static_cast<std::uint64_t>(val.index());
        }
    };

    struct LIRValEqualTo final {
        bool operator()(const LIRVal& x, const LIRVal& y) const { return x == y; }
    };
}