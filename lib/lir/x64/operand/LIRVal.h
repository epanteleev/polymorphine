#pragma once

#include <expected>

#include "LIRArg.h"
#include "lir/x64/lir_frwd.h"
#include "utility/Error.h"

class LIRVal final {
    enum class Op: std::uint8_t {
        Arg,
        Inst
    };

    LIRVal(std::uint8_t size, std::uint8_t index, const LIRArg *def): m_size(size),
                                                          m_index(index),
                                                          m_type(Op::Arg) {
        m_variant.m_arg = def;
    }

    LIRVal(std::uint8_t size, std::uint8_t index, const LIRInstruction *def): m_size(size),
                                                                      m_index(index),
                                                                      m_type(Op::Inst) {
        m_variant.m_inst = def;
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
    std::optional<const LIRInstruction*> inst() const noexcept {
        if (m_type == Op::Inst) {
            return m_variant.m_inst;
        }

        return std::nullopt;
    }

    template<typename T>
    void visit(const T& visitor) const {
        switch (m_type) {
            case Op::Arg: visitor(*m_variant.m_arg); break;
            case Op::Inst: visitor(*m_variant.m_inst); break;
        }
    }

    [[nodiscard]]
    std::uint8_t size() const noexcept {
        return m_size;
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
               m_index == rhs.m_index &&
               m_type == rhs.m_type &&
               m_variant.m_arg == rhs.m_variant.m_arg;
    }

    static LIRVal from(const LIRArg* def) noexcept {
        return {def->size(), static_cast<std::uint8_t>(def->index()), def};
    }

    static LIRVal reg(std::uint8_t size, std::uint8_t index, LIRInstruction* def) noexcept {
        return {size, index, def};
    }

    static std::expected<LIRVal, Error> try_from(const LIROperand& op);

    friend std::ostream& operator<<(std::ostream& os, const LIRVal& op) noexcept;

private:
    std::uint8_t m_size;
    std::uint8_t m_index;
    Op m_type;
    union {
        const LIRArg* m_arg;
        const LIRInstruction* m_inst;
    } m_variant{};
};

std::ostream& operator<<(std::ostream& os, const LIRVal& op) noexcept;