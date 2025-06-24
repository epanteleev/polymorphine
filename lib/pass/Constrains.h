#pragma once

#include <span>

template<typename T>
concept CodeBlock = requires(T t)
{
    { t.successors() } -> std::same_as<std::span<T *const>>;
    { t.predecessors() } -> std::same_as<std::span<T *const>>;
    { t.id() } -> std::convertible_to<std::size_t>;
};

template<typename T>
concept Function = requires(T t)
{
      typename T::code_block_type;
};
