#pragma once
#include <string_view>

class TransformPass {
public:
    virtual ~TransformPass() = default;
    virtual void run() noexcept = 0;

    [[nodiscard]]
    virtual std::string_view name() const noexcept = 0;
};
