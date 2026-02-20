#pragma once

#include <memory>

#include "utility/ObjPool.h"
#include "InstructionListIterator.h"

template<typename Inst>
class InstructionList final {
public:
    using list_type = ObjPool<std::unique_ptr<Inst>>;
    using list_iterator = list_type::iterator;
    using list_const_iterator = list_type::const_iterator;

    using upointer = std::unique_ptr<Inst>;

    using value_type = Inst;
    using reference = Inst&;
    using const_reference = const Inst&;

    using iterator = InstructionListIterator<list_iterator, Inst>;
    using const_iterator = InstructionListIterator<list_const_iterator, Inst>;
    
    [[nodiscard]]
    std::size_t push_back(upointer&& ptr) {
        return m_instructions.push_back(std::move(ptr)); 
    }

    [[nodiscard]]
    std::size_t insert_before(std::size_t pos, upointer ptr) noexcept {
        return m_instructions.insert_before(pos, std::move(ptr));
    }

    std::unique_ptr<Inst> remove(std::size_t idx) {
        return m_instructions.remove(idx);
    }

    [[nodiscard]]
    const_reference at(std::size_t index) const {
        return *m_instructions.at(index).get();
    }

    [[nodiscard]]
    reference at(std::size_t index) {
        return *m_instructions.at(index).get();
    }

    [[nodiscard]]
    std::size_t size() const noexcept {
        return m_instructions.size();
    }

    [[nodiscard]]
    reference back() const noexcept {
        return *m_instructions.back().get();
    }

    [[nodiscard]]
    bool empty() const noexcept {
        return m_instructions.empty();
    }

    [[nodiscard]]
    iterator begin() noexcept {
        return iterator(m_instructions.begin());
    }

    [[nodiscard]]
    iterator end() noexcept {
        return iterator(m_instructions.end());
    }

    [[nodiscard]]
    const_iterator begin() const noexcept {
        return const_iterator(m_instructions.begin());
    }

    [[nodiscard]]
    const_iterator end() const noexcept {
        return const_iterator(m_instructions.end());
    }
    
private:
    ObjPool<std::unique_ptr<Inst>> m_instructions;
};