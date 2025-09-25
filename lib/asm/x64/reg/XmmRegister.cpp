#include "XmmRegister.h"

#include <utility>

namespace aasm {
    std::string_view XmmRegister::name(const std::size_t size) const noexcept {
        static constexpr std::string_view xmm_names[] = {
            "xmm0",  "xmm1",  "xmm2",  "xmm3",
            "xmm4",  "xmm5",  "xmm6",  "xmm7",
            "xmm8",  "xmm9",  "xmm10", "xmm11",
            "xmm12", "xmm13", "xmm14", "xmm15",
        };

        static constexpr std::string_view ymm_names[] = {
            "ymm0",  "ymm1",  "ymm2",  "ymm3",
            "ymm4",  "ymm5",  "ymm6",  "ymm7",
            "ymm8",  "ymm9",  "ymm10", "ymm11",
            "ymm12", "ymm13", "ymm14", "ymm15",
        };
        static constexpr std::string_view zmm_names[] = {
            "zmm0",  "zmm1",  "zmm2",  "zmm3",
            "zmm4",  "zmm5",  "zmm6",  "zmm7",
            "zmm8",  "zmm9",  "zmm10", "zmm11",
            "zmm12", "zmm13", "zmm14", "zmm15",
        };

        const auto idx = m_code - xmm0.code();
        switch (size) {
            case 16: return xmm_names[idx];
            case 32: return ymm_names[idx];
            case 64: return zmm_names[idx];
            default: std::unreachable();
        }
    }
}