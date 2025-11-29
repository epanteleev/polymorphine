#pragma once

#include <cstdint>
#include "Error.h"


template<typename T, std::size_t N>
class InplaceVec final {
public:
    using value_type = T;
    using pointer    = T*;
    using iterator   = T*;
    using reference  = T&;
    using const_reference = const T&;
    using size_type  = std::size_t;

    explicit InplaceVec() = default;

    template<typename U = T>
    void push_back(U&& elem) {
        assertion(size() < N, "Must be");
        std::construct_at(at(m_size++), std::forward<U>(elem));
    }

    template<typename... Args>
    void emplace_back(Args&&... args) {
        assertion(size() < N, "Must be");
        std::construct_at(at(m_size++), std::forward<Args>(args)...);
    }

    void pop_back() noexcept {
        assertion(m_size != 0, "Must be");
        std::destroy_at(&m_storage[m_size--]);
    }

    [[nodiscard]]
    reference operator[](const std::size_t idx) noexcept {
        assertion(idx < N, "Must be");
        return *at(idx);
    }

    [[nodiscard]]
    const_reference operator[](const std::size_t idx) const noexcept {
        assertion(idx < N, "Must be");
        return *at(idx);
    }

    pointer data() const noexcept {
        return at(0);
    }

    pointer data() noexcept {
        return at(0);
    }

    [[nodiscard]]
    size_type size() const noexcept {
        return m_size;
    }

    [[nodiscard]]
    bool empty() const noexcept {
        return m_size == 0;
    }

    iterator begin() noexcept {
        return at(0);
    }

    iterator end() noexcept {
        return at(m_size);
    }

    [[nodiscard]]
    std::span<T const> span() const noexcept { //TODO find way to remove
        return {at(0), m_size};
    }

private:
    T* at(const std::size_t idx) noexcept {
        return &reinterpret_cast<T*>(&m_storage)[idx];
    }

    [[nodiscard]]
    const T* at(const std::size_t idx) const noexcept {
        return &reinterpret_cast<const T*>(&m_storage)[idx];
    }

    std::uint8_t m_size{};
    char m_storage[N*sizeof(T)];
};

static_assert(std::ranges::range<InplaceVec<int, 2>>, "should be");
static_assert(std::ranges::bidirectional_range<InplaceVec<int, 2>>, "should be");
static_assert(std::ranges::contiguous_range<InplaceVec<int, 2>>, "should be");
static_assert(std::ranges::sized_range<InplaceVec<int, 2>>, "should be");