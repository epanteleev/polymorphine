#pragma once

#include <cstdint>


class VReg final {
    enum class Kind : std::uint8_t {
        Reg,
        Addr
    };

    VReg(std::uint8_t size, Kind kind) noexcept
        : m_size(size), m_kind(kind) {}

public:
    static VReg addr(std::uint8_t size) noexcept {
        return {size, Kind::Addr};
    }

    static VReg reg(std::uint8_t size) noexcept {
        return {size, Kind::Reg};
    }

private:
    std::uint8_t m_size;
    Kind m_kind;
};
