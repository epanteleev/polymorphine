#pragma once

#include <memory>
#include <vector>
#include <list>

template<typename iterator, typename T>
class Iterator final {
public:
    using value_type = T;
    using reference = T&;
    using pointer = T*;
    using const_pointer = pointer;
    using difference_type = std::iterator_traits<iterator>::difference_type;
    using const_reference = reference;

    Iterator() noexcept = default;

    explicit Iterator(iterator first) noexcept:
        current(first) {}

    Iterator &operator++() {
        ++current;
        return *this;
    }

    Iterator operator++(int) noexcept {
        const Iterator old = *this;
        ++*this;
        return old;
    }

    Iterator &operator--() noexcept {
        --current;
        return *this;
    }

    Iterator operator--(int) noexcept {
        const Iterator old = *this;
        --*this;
        return old;
    }

    bool operator==(const Iterator &other) const noexcept { return current == other.current; }
    bool operator!=(const Iterator &other) const noexcept { return current != other.current; }

    reference operator*() { return *current->get(); }
    const_reference operator*() const { return *current->get(); }

    pointer operator->() { return current->get(); }
    const_pointer operator->() const { return current->get(); }

    pointer get() { return current->get(); }
    const_pointer get() const { return current->get(); }

private:
    iterator current;
};

template<typename T>
class OrderedSet final {
public:
    using list_iterator = std::list<std::unique_ptr<T>>::iterator;
    using const_list_iterator = std::list<std::unique_ptr<T>>::const_iterator;

    using value_type = T;
    using reference = T&;
    using const_reference = const T&;
    using iterator = Iterator<list_iterator, T>;
    using const_iterator = Iterator<const_list_iterator, T>;

    OrderedSet() = default;

    std::size_t push_back(std::unique_ptr<T>&& ptr) {
        const auto free_slot = get_free_index();
        const auto s = size();
        m_holder.push_back(std::move(ptr));
        if (free_slot == s) {
            m_list.push_back(--m_holder.end());
            return s;
        }

        m_list[free_slot] = --m_holder.end();
        return free_slot;
    }

    std::size_t insert_before(std::size_t idx, std::unique_ptr<T>&& ptr) {
        auto iter = m_list[idx];
        if (iter == m_holder.end()) {
            return push_back(std::move(ptr));
        }

        const auto free_slot = get_free_index();
        const auto s = size();
        m_holder.insert(iter, std::move(ptr));
        if (free_slot == s) {
            m_list.push_back(--m_holder.end());
            return s;
        }

        m_list[free_slot] = --m_holder.end();
        return free_slot;
    }

    std::unique_ptr<T> remove(std::size_t idx) {
        if (idx >= m_list.size()) {
            return nullptr;
        }

        auto it = m_list[idx];
        if (it == m_holder.end()) {
            return nullptr;
        }

        auto removed = std::move(*it);
        m_holder.erase(it);
        m_list[idx] = m_holder.end();
        m_free_indices.push_back(idx);
        return removed;
    }

    reference operator[](std::size_t index) {
        return *m_list[index]->get();
    }

    const_reference operator[](std::size_t index) const {
        return *m_list[index]->data;
    }

    const_reference at(std::size_t index) const {
        return *m_list.at(index)->get();
    }

    reference at(std::size_t index) {
        return *m_list.at(index)->get();
    }

    iterator begin() noexcept {
        return iterator(m_holder.begin());
    }

    iterator end() noexcept {
        return iterator(m_holder.end());
    }

    const_iterator begin() const noexcept {
        return const_iterator(m_holder.begin());
    }

    const_iterator end() const noexcept {
        return const_iterator(m_holder.end());
    }

    [[nodiscard]]
    std::size_t size() const noexcept {
        return m_holder.size();
    }

    const_iterator back() const noexcept {
        return const_iterator(m_list.back());
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
    std::list<std::unique_ptr<T>> m_holder;
};

static_assert(std::ranges::range<OrderedSet<int>>, "should be");
static_assert(std::ranges::bidirectional_range<OrderedSet<int>>, "should be");