#pragma once

#include <memory>
#include <vector>


template<typename  T>
class OrderedSet final {
    class Node final {
    public:
        explicit Node (std::unique_ptr<T> &&node) : data(std::move(node)) {}

        friend class OrderedSet;
    private:
        Node * prev{};
        Node * next{};
        std::unique_ptr<T> data;
    };

    class Iterator final {
    public:
        Iterator() = default;
        explicit Iterator (Node* first) : current(first) {}

        Iterator& operator++ () {
            current = current->next;
            return *this;
        }

        Iterator operator++ (int) {
            return ++*this;
        }

        Iterator& operator-- () {
            current = current->prev;
            return *this;
        }

        Iterator operator-- (int) {
            return --*this;
        }

        bool operator== (const Iterator& other) const {
            return current == other.current;
        }

        bool operator!= (const Iterator& other) const {
            return current != other.current;
        }

        T& operator* () {
            return *current->data;
        }

        const T& operator* () const {
            return *current->data;
        }

        T* operator-> () {
            return current->data.get();
        }

        T* operator-> () const {
            return current->data.get();
        }

        T* get() { return current->data.get(); }
    private:
        Node* current;
    };
public:
    template<typename U>
    using creator = std::function<std::unique_ptr<U>(std::size_t)>;
    using iterator = Iterator;
    using const_iterator = Iterator;

    OrderedSet() = default;
    ~OrderedSet() {
        for (auto * node : m_list) {
            delete node;
        }
    }

    template<typename U>
    U* push_back(const creator<U>& fn) {
        m_size++;
        if (m_head == nullptr) {
            return add_first(fn);
        }
        if (m_size == 2) {
            delete m_tail;
            m_tail = m_head;
        }

        const auto index = get_free_index();
        auto * node = new Node(fn(index));
        m_tail->next = node;
        node->prev = m_tail;
        m_tail = node;
        m_list.push_back(node);
        return static_cast<U*>(m_tail->data.get());
    }

    template<typename U>
    T* push_front(const creator<U>& fn) {
        m_size++;
        if (m_head == nullptr) {
            return add_first(fn);
        }
        if (m_size == 2) {
            delete m_tail;
            m_tail = m_head;
        }

        const auto index = get_free_index();
        auto * node = new Node(fn(index));
        node->next = m_head;
        m_head->prev = node;
        m_head = node;
        return static_cast<U*>(m_head->data.get());
    }

    void remove(T* item) {
        m_size--;
        const auto id = item->id();
        auto node = m_list.at(id);
        m_free_indices.push_back(id);
        delete node;
    }

    T &operator[](std::size_t index) {
        return *m_list[index]->data;
    }

    const T &operator[](std::size_t index) const {
        return *m_list[index]->data;
    }

    const T &at(std::size_t index) const {
        return *m_list.at(index)->data;
    }

    iterator begin() {
        return Iterator(m_head);
    }

    iterator end() {
        return Iterator(m_tail);
    }

    const_iterator begin() const {
        return Iterator(m_head);
    }

    const_iterator end() const {
        return Iterator(m_tail);
    }

    [[nodiscard]]
    std::size_t size() const {
        return m_size;
    }

private:
    template<typename U>
    U* add_first(const creator<U>& fn) {
        m_head = new Node(fn(0));
        m_tail = new Node(nullptr);

        m_head->next = m_tail;
        m_tail->prev = m_head;

        m_list.push_back(m_head);
        return static_cast<U*>(m_head->data.get());
    }

    std::size_t get_free_index() {
        if (m_free_indices.empty()) {
            return m_list.size();
        }

        const auto index = m_free_indices.back();
        m_free_indices.pop_back();
        return index;
    }

    std::vector<std::size_t> m_free_indices;
    std::vector<Node *> m_list;
    Node * m_head{};
    Node * m_tail{};
    std::size_t m_size{};
};