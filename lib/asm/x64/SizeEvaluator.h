#pragma once

#include <cstdint>
#include "asm/global/Directive.h"

namespace aasm {
    class SizeEvaluator final {
    public:
        SizeEvaluator() = default;

        constexpr void emit8(const std::uint8_t) noexcept { m_size++; }
        constexpr void emit16(const std::uint16_t) noexcept { m_size += 2; }
        constexpr void emit32(const std::uint32_t) noexcept { m_size += 4; }
        constexpr void emit64(const std::uint64_t) noexcept { m_size += 8; }

        constexpr void patch32(std::uint32_t, std::int32_t) noexcept {}
        constexpr void patch64(std::uint64_t, std::int64_t) noexcept {}

        [[nodiscard]]
        constexpr std::size_t size() const noexcept {
            return m_size;
        }

    private:
        std::size_t m_size{};
    };

    static_assert(CodeBuffer<SizeEvaluator>);

    class ModuleSizeEvaluator final {
    public:
        static std::size_t module_size_eval(const AsmModule& masm) {
            std::size_t acc{};
            for (const auto& emitter : masm.assembler() | std::views::values) {
                acc += emit(emitter);
            }

            for (const auto& slot: masm.globals() | std::views::values) {
                acc += emit(slot);
            }

            return acc;
        }

    private:
        template<typename E>
        static constexpr std::size_t emit(const E& emitter) {
            SizeEvaluator evaluator;
            (void)(emitter.emit(evaluator));
            return evaluator.size();
        }
    };
}