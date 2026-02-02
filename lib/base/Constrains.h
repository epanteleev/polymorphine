#pragma once

#include <span>

/**
 * Concept that defines the requirements for a basic block.
 */
template<typename T>
concept CodeBlock = requires(T t)
{
    { t.successors() } -> std::same_as<std::span<T *const>>;
    { t.predecessors() } -> std::same_as<std::span<T *const>>;
    { t.id() } -> std::convertible_to<std::size_t>;
};
