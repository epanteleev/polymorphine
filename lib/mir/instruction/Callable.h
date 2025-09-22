#pragma once

#include "mir/module/FunctionPrototype.h"


class Callable {
public:
    explicit Callable(const FunctionPrototype *prototype) noexcept:
        m_prototype(prototype) {}

    [[nodiscard]]
    const FunctionPrototype* prototype() const noexcept {
        return m_prototype;
    }

protected:
    const FunctionPrototype *m_prototype;
};