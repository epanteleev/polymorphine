#pragma once

#include <cstdint>

namespace aasm {
    class SizeEvaluator final {
        SizeEvaluator() = default;

    public:
        constexpr void emit8(const std::uint8_t) noexcept { m_size++; }
        constexpr void emit16(const std::uint16_t) noexcept { m_size += 2; }
        constexpr void emit32(const std::uint32_t) noexcept { m_size += 4; }
        constexpr void emit64(const std::uint64_t) noexcept { m_size += 8; }

        constexpr void patch32(std::uint32_t, std::int32_t) noexcept {}

        [[nodiscard]]
        constexpr std::size_t size() const noexcept {
            return m_size;
        }

        template<typename E>
        static constexpr std::size_t emit(const E& emitter) {
            SizeEvaluator evaluator;
            emitter.emit(evaluator);
            return evaluator.size();
        }

    private:
        std::size_t m_size{};
    };

    static_assert(CodeBuffer<SizeEvaluator>);
}