#pragma once

#include <string>
#include <vector>
#include <span>
#include <iosfwd>

#include "NonTrivialType.h"
#include "utility/ArithmeticUtils.h"


class StructType final: public NonTrivialType {
public:
    explicit StructType(const std::string_view name, std::vector<const NonTrivialType*>&& field_types, std::vector<std::size_t>&& m_offsets,
        const std::size_t size, const std::size_t alignment) noexcept:
        m_name(name),
        m_field_types(std::move(field_types)),
        m_offsets(std::move(m_offsets)),
        m_size(size),
        m_alignment(alignment) {}

    void visit(type::Visitor &visitor) override {
        visitor.accept(this);
    }

    [[nodiscard]]
    std::size_t offset_of(const std::size_t index) const {
        return m_offsets.at(index);
    }

    [[nodiscard]]
    std::size_t size_of() const override {
        return m_size;
    }

    [[nodiscard]]
    std::size_t align_of() const override {
        return m_alignment;
    }

    [[nodiscard]]
    std::string_view name() const noexcept {
        return m_name;
    }

    [[nodiscard]]
    std::span<const NonTrivialType* const> field_types() const noexcept {
        return m_field_types;
    }

    [[nodiscard]]
    const NonTrivialType* field(const std::size_t index) const {
        return m_field_types.at(index);
    }

    void print_declaration(std::ostream &os) const;

    static StructType make(const std::string_view name, std::vector<const NonTrivialType*>&& field_types) {
        auto [alignments, size, alignment] = compute_offsets(field_types);
        return StructType(name, std::move(field_types), std::move(alignments), size, alignment);
    }

private:
    static std::tuple<std::vector<std::size_t>, std::size_t, std::size_t> compute_offsets(const std::vector<const NonTrivialType*>& field_types) {
        std::vector<std::size_t> offsets;
        offsets.reserve(field_types.size());

        std::size_t current_offset{};
        std::size_t alignment = 1;
        for (const auto& field_type: field_types) {
            const auto field_alignment = field_type->align_of();
            alignment = std::max(alignment, field_alignment);
            offsets.push_back(align_up(current_offset, field_alignment));
            current_offset = current_offset + field_type->size_of();
        }

        return {std::move(offsets), align_up(current_offset, alignment), alignment};
    }

    std::string m_name;
    std::vector<const NonTrivialType*> m_field_types;
    std::vector<std::size_t> m_offsets;
    std::size_t m_size;
    std::size_t m_alignment;
};