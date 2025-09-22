#pragma once

namespace aasm::details {
    class CMovRR final {
    public:
        constexpr explicit CMovRR(const std::uint8_t size, const GPReg src, const GPReg dst, const CondType cond) noexcept:
            m_src(src),
            m_dst(dst),
            m_size(size),
            m_cond(cond) {}

        friend std::ostream& operator<<(std::ostream& os, const CMovRR& inst);

        template<CodeBuffer Buffer>
        constexpr void emit(Buffer& buffer) const {
            static constexpr std::uint8_t CMOV_RR = 0x0F;
            EncodeUtils::emit_op_prologue(buffer, m_size, m_dst, m_src);
            switch (m_size) {
                case 2: [[fallthrough]];
                case 4: [[fallthrough]];
                case 8: buffer.emit8(CMOV_RR); break;
                default: die("Invalid size for mov instruction: {}", m_size);
            }
            buffer.emit8(0x40 | static_cast<std::uint8_t>(m_cond));
            buffer.emit8(0xC0 | reg3(m_dst) << 3 | reg3(m_src));
        }

    private:
        GPReg m_src;
        GPReg m_dst;
        std::uint8_t m_size;
        CondType m_cond;
    };

    inline std::ostream & operator<<(std::ostream &os, const CMovRR &inst) {
        return os << "cmov" << inst.m_cond << " %" << inst.m_src.name(inst.m_size) << ", %" << inst.m_dst.name(inst.m_size);
    }

    class CMovRM final {
    public:
        constexpr explicit CMovRM(const std::uint8_t size, const Address& src, const GPReg dst, const CondType cond) noexcept:
            m_dst(dst),
            m_src(src),
            m_size(size),
            m_cond(cond) {}

        friend std::ostream& operator<<(std::ostream& os, const CMovRM& inst);

        template<CodeBuffer Buffer>
        [[nodiscard]]
        constexpr std::optional<Relocation> emit(Buffer& buffer) const {
            static constexpr std::uint8_t CMOV_RM = 0x0F;
            EncodeUtils::emit_op_prologue(buffer, m_size, m_dst, m_src);
            switch (m_size) {
                case 2: [[fallthrough]];
                case 4: [[fallthrough]];
                case 8: buffer.emit8(CMOV_RM); break;
                default: die("Invalid size for cmov instruction: {}", m_size);
            }
            buffer.emit8(0x40 | static_cast<std::uint8_t>(m_cond));
            return m_src.encode(buffer, reg3(m_dst), 0);
        }

    private:
        GPReg m_dst;
        Address m_src;
        std::uint8_t m_size;
        CondType m_cond;
    };

    inline std::ostream & operator<<(std::ostream &os, const CMovRM &inst) {
        return os << "cmov" << inst.m_cond << " " << inst.m_src << ", %" << inst.m_dst.name(inst.m_size);
    }
}
