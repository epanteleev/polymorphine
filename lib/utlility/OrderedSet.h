#pragma once

#include <memory>
#include <vector>


template<typename T>
concept HasId = requires(T t) {
    { t.id() } -> std::convertible_to<std::size_t>;
};

template<HasId T>
class OrderedSet final {
    class Node final {
    public:
        explicit Node (std::unique_ptr<T> &&node) : data(std::move(node)) {}

        T& operator*() {
            return *data;
        }

        const T& operator*() const {
            return *data;
        }

        friend class OrderedSet;
    private:
        Node * prev{};
        Node * next{};
        std::unique_ptr<T> data;
    };
public:
    using iterator = typename std::vector<Node *>::iterator;

    OrderedSet() = default;
    ~OrderedSet() {
        for (auto * node : m_list) {
            delete node;
        }
    }

    template<HasId U, typename... Args>
    T* push_back(Args&&... args) {
        if (m_head == nullptr) {
            m_head = new Node(std::make_unique<U>(0, std::forward<Args>(args)...));
            m_tail = m_head;
            m_list.push_back(m_head);
            return m_head->data.get();
        }
        const auto index = get_free_index();
        auto * node = new Node(std::make_unique<U>(index, std::forward<Args>(args)...));
        m_tail->next = node;
        m_tail = node;
        m_list.push_back(node);
        return m_tail->data.get();
    }

    template<HasId U, typename... Args>
    T* push_front(Args&&... args) {
        if (m_head == nullptr) {
            m_head = new Node(std::make_unique<U>(0, std::forward<Args>(args)...));
            m_tail = m_head;
            m_list.push_front(m_head);
            return m_head->data.get();
        }
        const auto index = get_free_index();
        auto node = new Node(std::make_unique<U>(index, std::forward<Args>(args)...));
        node->next = m_head;
        m_head->prev = node;
        m_head = node;
        return m_head->data.get();
    }

    void remove(T* item) {
        const auto id = item->id();
        m_list.at(id)->data.reset();
        m_free_indices.push_back(id);
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

private:
    std::vector<std::size_t> m_free_indices;
    std::vector<Node *> m_list;
    Node * m_head{};
    Node * m_tail{};
};
