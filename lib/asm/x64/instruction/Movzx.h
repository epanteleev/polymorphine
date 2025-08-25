#pragma once

namespace aasm::details {
    class MovzxRR final {
    public:
        explicit constexpr MovzxRR(const std::uint8_t from_size, const std::uint8_t to_size, const GPReg& src, const GPReg& dest) noexcept:
            m_from_size(from_size),
            m_to_size(to_size),
            m_src(src),
            m_dest(dest) {}

        friend std::ostream& operator<<(std::ostream &os, const MovzxRR& movzxrr);


        template<CodeBuffer Buffer>
        constexpr void emit(Buffer& buffer) const {
            static constexpr std::uint8_t MOVZX_RR[] = {0x0F, 0xB7}; // for 8-bit and 16-bit source
            static constexpr std::uint8_t MOVZX_RR_8[] = {0x0F, 0xB6}; // for 8-bit source

            emit_op_prologue(buffer, m_to_size, m_src, m_dest);
            switch (m_from_size) {
                case 1: {
                    switch (m_to_size) {
                        case 2: [[fallthrough]];
                        case 4: [[fallthrough]];
                        case 8: buffer.emit8(MOVZX_RR_8[0]); buffer.emit8(MOVZX_RR_8[1]); break;
                        default: die("Invalid size for instruction: {}", m_to_size);
                    }
                    break;
                }
                case 2: {
                    switch (m_to_size) {
                        case 2: [[fallthrough]];
                        case 4: [[fallthrough]];
                        case 8: buffer.emit8(MOVZX_RR[0]); buffer.emit8(MOVZX_RR[1]); break;
                        default: die("Invalid size for instruction: {}", m_to_size);
                    }
                    break;
                }
                default: die("Invalid size for instruction: {}", m_from_size);
            }
            buffer.emit8(0xC0 | reg3(m_dest) << 3 | reg3(m_src));
        }
    private:
        std::uint8_t m_from_size;
        std::uint8_t m_to_size;
        GPReg m_src;
        GPReg m_dest;
    };

    inline std::ostream & operator<<(std::ostream &os, const MovzxRR &movzxrr) {
        return os << "movz" << prefix_size(movzxrr.m_from_size)
            << prefix_size(movzxrr.m_to_size)
            << " %" << movzxrr.m_src.name(movzxrr.m_from_size)
            << ", %" << movzxrr.m_dest.name(movzxrr.m_to_size);
    }

    class MovzxRM final {
    public:
        explicit constexpr MovzxRM(const std::uint8_t from_size, const std::uint8_t to_size, const Address& src, const GPReg& dest) noexcept:
            m_from_size(from_size),
            m_to_size(to_size),
            m_src(src),
            m_dest(dest) {}

        friend std::ostream& operator<<(std::ostream &os, const MovzxRM& movzxrm);

        template<CodeBuffer Buffer>
        [[nodiscard]]
        constexpr std::optional<Relocation> emit(Buffer& buffer) const {
            static constexpr std::uint8_t MOVZX_RR[] = {0x0F, 0xB7};
            static constexpr std::uint8_t MOVZX_RR_8[] = {0x0F, 0xB6};
            emit_op_prologue(buffer, m_to_size, m_dest, m_src);
            switch (m_from_size) {
                case 1: {
                    switch (m_to_size) {
                        case 2: [[fallthrough]];
                        case 4: [[fallthrough]];
                        case 8: buffer.emit8(MOVZX_RR_8[0]); buffer.emit8(MOVZX_RR_8[1]); break;
                        default: die("Invalid size for instruction: {}", m_to_size);
                    }
                    break;
                }
                case 2: {
                    switch (m_to_size) {
                        case 2: [[fallthrough]];
                        case 4: [[fallthrough]];
                        case 8: buffer.emit8(MOVZX_RR[0]); buffer.emit8(MOVZX_RR[1]); break;
                        default: die("Invalid size for instruction: {}", m_to_size);
                    }
                    break;
                }
                default: die("Invalid size for instruction: {}", m_from_size);
            }
            return m_src.encode(buffer, reg3(m_dest));
        }

    private:
        std::uint8_t m_from_size;
        std::uint8_t m_to_size;
        Address m_src;
        GPReg m_dest;
    };

    inline std::ostream & operator<<(std::ostream &os, const MovzxRM &movzxrm) {
        return os << "movz" << prefix_size(movzxrm.m_from_size)
            << prefix_size(movzxrm.m_to_size)
            << " " << movzxrm.m_src
            << ", %" << movzxrm.m_dest.name(movzxrm.m_to_size);
    }
}
