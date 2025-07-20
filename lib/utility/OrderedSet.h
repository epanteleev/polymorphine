#pragma once

#include <deque>
#include <optional>
#include <memory>
#include <vector>
#include <functional>


template<typename T>
class OrderedSet final {
public:
    template<typename U>
    using creator = std::function<std::unique_ptr<U>(std::size_t)>;
    using iterator = typename std::deque<std::unique_ptr<T>>::iterator;
    using const_iterator = typename std::deque<std::unique_ptr<T>>::const_iterator;

private:

    template<typename iterator>
    class Iterator final {
    public:
        Iterator() = default;
        explicit Iterator (iterator first) : current(first) {}

        Iterator& operator++ () {
            ++current;
            return *this;
        }

        Iterator operator++ (int) { return ++*this; }

        Iterator& operator-- () {
            --current;
            return *this;
        }

        Iterator operator-- (int) { return --*this; }

        bool operator== (const Iterator& other) const { return current == other.current; }
        bool operator!= (const Iterator& other) const { return current != other.current; }
        T& operator* () { return *current->get(); }
        const T& operator* () const { return *current->get(); }
        T* operator-> () { return current->get(); }
        T* operator-> () const { return current->get(); }

        T* get() { return current->get(); }
    private:
        iterator current;
    };

public:
    OrderedSet() = default;

    template<typename U>
    U* push_back(const creator<U>& fn) {
        const auto free_slot = get_free_index();
        auto value = fn(free_slot);
        auto ret = value.get();

        const auto s = size();
        m_holder.push_back(std::move(value));
        if (free_slot == s) {
            m_list.push_back(--m_holder.end());
        } else {
            m_list[free_slot] = --m_holder.end();
        }

        return ret;
    }

    template<typename U>
    T* push_front(const creator<U>& fn) {
        const auto free_slot = get_free_index();
        auto value = fn(free_slot);
        auto ret = value.get();

        const auto s = size();
        m_holder.push_front(std::move(value));
        if (free_slot == s) {
            m_list.push_back(--m_holder.end());
        } else {
            m_list[free_slot] = --m_holder.end();
        }

        return ret;
    }

    void remove(T* item) {
        const auto id = item->id();
        auto node = m_list.at(id);
        m_free_indices.push_back(id);
        delete node;
    }

    T &operator[](std::size_t index) {
        return *m_list[index]->get();
    }

    const T &operator[](std::size_t index) const {
        return *m_list[index]->data;
    }

    const T &at(std::size_t index) const {
        return *m_list.at(index)->data;
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
    std::deque<std::unique_ptr<T>> m_holder;
};