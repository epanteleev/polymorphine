#pragma once

#include <utility>

#include "Type.h"
#include "PointerType.h"


class TupleType final: public Type {
public:
    explicit constexpr TupleType(const PrimitiveType* first, const PrimitiveType* second) noexcept:
        m_first(first),
        m_second(second) {}

    void visit(type::Visitor &visitor) override { visitor.accept(this); }

    [[nodiscard]]
    const PrimitiveType* first() const noexcept {
        return m_first;
    }

    [[nodiscard]]
    const PrimitiveType* second() const noexcept {
        return m_second;
    }

    [[nodiscard]]
    const PrimitiveType* inner_type(const std::size_t idx) const noexcept {
        switch (idx) {
            case 0: return m_first;
            case 1: return m_second;
            default: std::unreachable();
        }
    }

    template<std::derived_from<PrimitiveType> F, std::derived_from<PrimitiveType> S>
    static const TupleType *tuple(const F *first, const S *second) noexcept {
        if (const auto f = SignedIntegerType::cast(first)) {
            return tuple2(f, second);
        }
        if (const auto f = UnsignedIntegerType::cast(first)) {
            return tuple2(f, second);
        }
        if (const auto f = FloatingPointType::cast(first)) {
            return tuple2(f, second);
        }
        if (const auto f = PointerType::cast(first)) {
            return tuple2(f, second);
        }

        std::unreachable();
    }

private:
    template<typename F, typename S>
    static const TupleType* tuple0(const F* first, const S* second) noexcept {
        switch (second->size_of()) {
            case 1: {
                static TupleType instance(first, second);
                return &instance;
            }
            case 2: {
                static TupleType instance(first, second);
                return &instance;
            }
            case 4: {
                static TupleType instance(first, second);
                return &instance;
            }
            case 8: {
                static TupleType instance(first, second);
                return &instance;
            }
            default: std::unreachable();
        }
    }

    template<typename F, typename S>
    static const TupleType* tuple1(const F* first, const S* second) noexcept {
        switch (first->size_of()) {
            case 1: return tuple0(first, second);
            case 2: return tuple0(first, second);
            case 4: return tuple0(first, second);
            case 8: return tuple0(first, second);
            default: std::unreachable();
        }
    }

    template<typename F, typename S>
    static const TupleType* tuple2(const F* first, const S* second) noexcept {
        if (const auto s = SignedIntegerType::cast(second)) {
            return tuple1(first, s);
        }
        if (const auto s = UnsignedIntegerType::cast(second)) {
            return tuple1(first, s);
        }
        if (const auto s = FloatingPointType::cast(second)) {
            return tuple1(first, s);
        }
        if (const auto s = PointerType::cast(second)) {
            return tuple1(first, s);
        }

        std::unreachable();
    }

    const PrimitiveType* m_first;
    const PrimitiveType* m_second;
};