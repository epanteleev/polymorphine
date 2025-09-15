#pragma once

#include <vector>

/**
 * Removes the element pointed to by the iterator `it` from the vector by swapping it with the last element and popping back.
 * This method does not preserve the order of elements but is more efficient than the standard `remove_if` followed by `erase`.
 *
 * @tparam T The type of elements in the vector.
 * @param vector The vector from which the element will be removed.
 * @param it An iterator pointing to the element to be removed.
 */
template<typename T>
void remove_fast(std::vector<T>& vector, const typename std::vector<T>::iterator& it) noexcept {
    std::swap(*it, vector.back());
    vector.pop_back();
}

/**
 * Removes elements from the vector that satisfy the predicate `fn` by swapping them with the last element and popping back.
 * This method does not preserve the order of elements but is more efficient than the standard `remove_if` followed by `erase`.
 *
 * @tparam T The type of elements in the vector.
 * @tparam Fn A callable type that takes a const reference to an element of type T and returns a bool.
 * @param vector The vector from which elements will be removed.
 * @param fn The predicate function used to determine which elements to remove.
 */
template<typename T, typename Fn>
requires std::invocable<Fn, const T&> && std::same_as<std::invoke_result_t<Fn, const T&>, bool>
void remove_if_fast(std::vector<T>& vector, Fn&& fn) {
    for (auto it = vector.begin(); it != vector.end();) {
        if (fn(*it)) {
            std::swap(*it, vector.back());
            vector.pop_back();
        } else {
            ++it;
        }
    }
}