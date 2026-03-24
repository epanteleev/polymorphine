#pragma once

#include <vector>
#include <list>
#include "Error.h"

template<typename Iterator, typename T>
class ObjPoolIterator final {
public:
    using value_type = T;
    using reference = T&;
    using pointer = T*;
    using const_pointer = const T*;
    using difference_type = std::iterator_traits<Iterator>::difference_type;
    using const_reference = reference;

    ObjPoolIterator() noexcept = default;

    explicit ObjPoolIterator(Iterator first) noexcept:
        current(first) {}

    ObjPoolIterator &operator++() noexcept {
        ++current;
        return *this;
    }

    ObjPoolIterator operator++(int) noexcept {
        const ObjPoolIterator old = *this;
        ++*this;
        return old;
    }

    ObjPoolIterator &operator--() noexcept {
        --current;
        return *this;
    }

    ObjPoolIterator operator--(int) noexcept {
        const ObjPoolIterator old = *this;
        --*this;
        return old;
    }

    bool operator==(const ObjPoolIterator &other) const noexcept { return current == other.current; }
    bool operator!=(const ObjPoolIterator &other) const noexcept { return current != other.current; }

    reference operator*() { return *current; }
    const_reference operator*() const { return *current; }

    pointer operator->() { return current.operator->(); }
    const_pointer operator->() const { return current.operator->(); }

private:
    Iterator current;
};

template<typename T>
class ObjPool final {
public:
    using list_iterator = std::list<T>::iterator;
    using const_list_iterator = std::list<T>::const_iterator;

    using value_type = T;
    using reference = T&;
    using const_reference = const T&;
    using iterator = ObjPoolIterator<list_iterator, T>;
    using const_iterator = ObjPoolIterator<const_list_iterator, const T>;

    ObjPool() = default;

    template<std::convertible_to<T> U>
    [[nodiscard]]
    std::size_t push_back(U&& ptr) {
        const auto free_slot = get_free_index();
        m_holder.push_back(std::forward<U>(ptr));
        auto new_iter = std::prev(m_holder.end());
        if (free_slot == m_list.size()) {
            m_list.push_back(new_iter);
        } else {
            m_list[free_slot] = new_iter;
        }
        return free_slot;
    }

    template<typename... Args>
    std::size_t emplace_back(Args&&... args) {
        const auto free_slot = get_free_index();
        m_holder.emplace_back(std::forward<Args>(args)...);
        auto new_iter = std::prev(m_holder.end());
        if (free_slot == m_list.size()) {
            m_list.push_back(new_iter);
        } else {
            m_list[free_slot] = new_iter;
        }
        return free_slot;
    }

    template<std::convertible_to<T> U>
    [[nodiscard]]
    std::size_t insert_before(std::size_t idx, U&& ptr) {
        assertion(idx < m_list.size(), "must be");
        auto iter = m_list[idx];
        assertion(iter != m_holder.end(), "must be");
        const auto free_slot = get_free_index();
        auto new_iter = m_holder.insert(iter, std::forward<U>(ptr));
        if (free_slot == m_list.size()) {
            m_list.push_back(new_iter);
        } else {
            m_list[free_slot] = new_iter;
        }
        return free_slot;
    }

    T remove(std::size_t idx) {
        assertion(idx < m_list.size(), "must be");

        auto it = m_list[idx];
        assertion(it != m_holder.end(), "must be");

        auto removed = std::move(*it);
        m_holder.erase(it);
        m_list[idx] = m_holder.end();
        m_free_indices.push_back(idx);
        return removed;
    }

    void swap(std::size_t i, std::size_t j) {
        if (i == j) return;

        auto it_i = m_list[i];
        auto it_j = m_list[j];
        assertion(it_i != m_holder.end() && it_j != m_holder.end(), "cannot swap freed slots");

        if (std::next(it_i) == it_j) {
            m_holder.splice(it_i, m_holder, it_j);

        } else if (std::next(it_j) == it_i) {
            m_holder.splice(it_j, m_holder, it_i);
        } else {
            auto next_j = std::next(it_j);
            m_holder.splice(it_i, m_holder, it_j);
            m_holder.splice(next_j, m_holder, it_i);
        }

        std::swap(m_list[i], m_list[j]);
    }

    [[nodiscard]]
    reference operator[](std::size_t index) {
        return *m_list[index];
    }

    [[nodiscard]]
    const_reference operator[](std::size_t index) const {
        return *m_list[index];
    }

    [[nodiscard]]
    const_reference at(std::size_t index) const {
        assertion(index < m_list.size() && m_list[index] != m_holder.end(), "index out of range or freed");
        return *m_list[index];
    }

    [[nodiscard]]
    reference at(std::size_t index) {
        assertion(index < m_list.size() && m_list[index] != m_holder.end(), "index out of range or freed");
        return *m_list[index];
    }

    iterator begin() noexcept {
        return iterator(m_holder.begin());
    }

    iterator end() noexcept {
        return iterator(m_holder.end());
    }

    [[nodiscard]]
    const_iterator begin() const noexcept {
        return const_iterator(m_holder.begin());
    }

    [[nodiscard]]
    const_iterator end() const noexcept {
        return const_iterator(m_holder.end());
    }

    [[nodiscard]]
    std::size_t size() const noexcept {
        return m_holder.size();
    }

    [[nodiscard]]
    std::size_t max_possible_id() const noexcept {
        return m_list.size();
    }

    [[nodiscard]]
    bool empty() const noexcept {
        return m_holder.empty();
    }

    [[nodiscard]]
    const_reference back() const noexcept {
        return m_holder.back();
    }

    [[nodiscard]]
    const_reference front() const noexcept {
        return m_holder.front();
    }

    [[nodiscard]]
    reference back() noexcept {
        return m_holder.back();
    }

    [[nodiscard]]
    reference front() noexcept {
        return m_holder.front();
    }

private:
    std::size_t get_free_index() {
        if (m_free_indices.empty()) {
            return m_list.size();
        }

        const auto index = m_free_indices.back();
        m_free_indices.pop_back();
        return index;
    }

    std::vector<std::size_t> m_free_indices;
    std::vector<list_iterator> m_list;
    std::list<T> m_holder;
};

static_assert(std::ranges::range<ObjPool<int>>, "should be");
static_assert(std::ranges::bidirectional_range<ObjPool<int>>, "should be");
