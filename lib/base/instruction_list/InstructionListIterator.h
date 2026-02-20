#pragma once

#include <iterator>

template<typename iterator, typename T>
class InstructionListIterator final {
public:
    using value_type = T;
    using reference = T&;
    using pointer = T*;
    using const_pointer = pointer;
    using difference_type = std::iterator_traits<iterator>::difference_type;
    using const_reference = reference;

    InstructionListIterator() noexcept = default;

    explicit InstructionListIterator(iterator first) noexcept:
        current(first) {}

    InstructionListIterator &operator++() {
        ++current;
        return *this;
    }

    InstructionListIterator operator++(int) noexcept {
        const InstructionListIterator old = *this;
        ++*this;
        return old;
    }

    InstructionListIterator &operator--() noexcept {
        --current;
        return *this;
    }

    InstructionListIterator operator--(int) noexcept {
        const InstructionListIterator old = *this;
        --*this;
        return old;
    }

    bool operator==(const InstructionListIterator &other) const noexcept { return current == other.current; }
    bool operator!=(const InstructionListIterator &other) const noexcept { return current != other.current; }

    reference operator*() { return *current->get(); }
    const_reference operator*() const { return *current->get(); }

    pointer operator->() { return current->get(); }
    const_pointer operator->() const { return current->get(); }

    pointer get() { return current->get(); }

    [[nodiscard]]
    const_pointer get() const { return current->get(); }

private:
    iterator current;
};
