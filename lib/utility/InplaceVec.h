#pragma once

#include <algorithm>
#include <cstdint>
#include "Error.h"


template<typename T, std::size_t N>
class InplaceVec final {
public:
    using value_type     = T;
    using pointer        = T*;
    using iterator       = T*;
    using const_iterator = const T*;
    using reference      = T&;
    using const_reference = const T&;
    using size_type  = std::size_t;

    constexpr InplaceVec() = default;

    constexpr InplaceVec(std::initializer_list<T> init) {
        for (const auto& elem: init) {
            push_back(elem);
        }
    }

    template<typename U = T>
    constexpr void push_back(U&& elem) {
        assertion(size() < N, "Must be");
        std::construct_at(at(m_size++), std::forward<U>(elem));
    }

    template<typename... Args>
    constexpr void emplace_back(Args&&... args) {
        assertion(size() < N, "Must be");
        std::construct_at(at(m_size++), std::forward<Args>(args)...);
    }

    constexpr void pop_back() noexcept {
        assertion(m_size != 0, "Must be");
        std::destroy_at(&m_storage[m_size--]);
    }

    [[nodiscard]]
    constexpr reference operator[](const std::size_t idx) noexcept {
        assertion(idx < N, "Must be");
        return *at(idx);
    }

    [[nodiscard]]
    constexpr const_reference operator[](const std::size_t idx) const noexcept {
        assertion(idx < N, "Must be");
        return *at(idx);
    }

    constexpr void remove(const_reference val) {
        const auto removed = std::remove(begin(), end(), val);
        if (removed != end()) {
            m_size--;
        }
    }

    [[nodiscard]]
    constexpr const_reference back() const noexcept {
        return *at(size() - 1);
    }

    [[nodiscard]]
    constexpr pointer data() const noexcept {
        return at(0);
    }

    constexpr pointer data() noexcept {
        return at(0);
    }

    [[nodiscard]]
    constexpr size_type size() const noexcept {
        return m_size;
    }

    [[nodiscard]]
    constexpr bool empty() const noexcept {
        return m_size == 0;
    }

    constexpr iterator begin() noexcept {
        return at(0);
    }

    constexpr iterator end() noexcept {
        return at(m_size);
    }

    [[nodiscard]]
    constexpr const_iterator begin() const noexcept {
        return at(0);
    }

    [[nodiscard]]
    constexpr const_iterator end() const noexcept {
        return at(m_size);
    }

    [[nodiscard]]
    constexpr std::span<T const> span() const noexcept { //TODO find way to remove
        return {at(0), m_size};
    }

private:
    constexpr T* at(const std::size_t idx) noexcept {
        return &reinterpret_cast<T*>(&m_storage)[idx];
    }

    [[nodiscard]]
    constexpr const T* at(const std::size_t idx) const noexcept {
        return &reinterpret_cast<const T*>(&m_storage)[idx];
    }

    std::uint8_t m_size{};
    char m_storage[N*sizeof(T)];
};

static_assert(std::ranges::range<InplaceVec<int, 2>>, "should be");
static_assert(std::ranges::bidirectional_range<InplaceVec<int, 2>>, "should be");
static_assert(std::ranges::contiguous_range<InplaceVec<int, 2>>, "should be");
static_assert(std::ranges::sized_range<InplaceVec<int, 2>>, "should be");