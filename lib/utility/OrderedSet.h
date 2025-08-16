#pragma once

#include <optional>
#include <memory>
#include <vector>
#include <functional>
#include <list>


template<typename T>
class OrderedSet final {
public:
    using iterator = std::list<std::unique_ptr<T>>::iterator;
    using const_iterator = std::list<std::unique_ptr<T>>::const_iterator;

private:
    template<typename iterator>
    class Iterator final {
    public:
        explicit Iterator() = default;

        explicit Iterator(iterator first) noexcept:
            current(first) {}

        Iterator &operator++() {
            ++current;
            return *this;
        }

        Iterator operator++(int) { return ++*this; }

        Iterator &operator--() {
            --current;
            return *this;
        }

        Iterator operator--(int) {
            return --*this;
        }

        bool operator==(const Iterator &other) const { return current == other.current; }
        bool operator!=(const Iterator &other) const { return current != other.current; }
        T &operator*() { return *current->get(); }
        const T &operator*() const { return *current->get(); }
        T *operator->() { return current->get(); }
        T *operator->() const { return current->get(); }

        T* get() { return current->get(); }
    private:
        iterator current;
    };

public:
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

    T &operator[](std::size_t index) {
        return *m_list[index]->get();
    }

    const T &operator[](std::size_t index) const {
        return *m_list[index]->data;
    }

    const T &at(std::size_t index) const {
        return *m_list.at(index)->get();
    }

    T &at(std::size_t index) {
        return *m_list.at(index)->get();
    }

    Iterator<iterator> begin() {
        return Iterator(m_holder.begin());
    }

    Iterator<iterator> end() {
        return Iterator(m_holder.end());
    }

    Iterator<const_iterator> begin() const {
        return Iterator(m_holder.begin());
    }

    Iterator<const_iterator> end() const {
        return Iterator(m_holder.end());
    }

    [[nodiscard]]
    std::size_t size() const noexcept {
        return m_holder.size();
    }

    std::optional<T*> back() const {
        if (size() == 0) {
            return std::nullopt;
        }

        return std::make_optional(m_list.back()->get());
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
    std::vector<iterator> m_list;
    std::list<std::unique_ptr<T>> m_holder;
};
