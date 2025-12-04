#pragma once

namespace aasm::details {
    template<typename SRC>
    class CMov_Base {
    public:
        template<typename S = SRC>
        constexpr explicit CMov_Base(const std::uint8_t size, S&& src, const GPReg dst, const CondType cond) noexcept:
            m_size(size),
            m_cond(cond),
            m_dst(dst),
            m_src(std::forward<S>(src)) {}

        template<CodeBuffer Buffer>
        [[nodiscard]]
        constexpr std::optional<Relocation> emit(Buffer& buffer) const {
            static constexpr std::uint8_t CMOV_RM = 0x0F;
            EncodeUtils::emit_op_prologue(buffer, m_size, m_dst, m_src);
            switch (m_size) {
                case 2: [[fallthrough]];
                case 4: [[fallthrough]];
                case 8: buffer.emit8(CMOV_RM); break;
                default: die("Invalid size for mov instruction: {}", m_size);
            }
            buffer.emit8(0x40 | static_cast<std::uint8_t>(m_cond));
            if constexpr (std::is_same_v<SRC, GPReg>) {
                buffer.emit8(0xC0 | m_dst.encode() << 3 | m_src.encode());
                return std::nullopt;

            } else if constexpr (std::is_same_v<SRC, Address>) {
                return m_src.encode(buffer, m_dst.encode(), 0);

            } else {
                static_assert(false, "Unsupported source type for CMov instruction");
                std::unreachable();
            }
        }

    protected:
        std::uint8_t m_size;
        CondType m_cond;
        GPReg m_dst;
        SRC m_src;
    };

    class CMovRR final: public CMov_Base<GPReg> {
    public:
        constexpr explicit CMovRR(const std::uint8_t size, const GPReg src, const GPReg dst, const CondType cond) noexcept:
            CMov_Base(size, src, dst, cond) {}

        friend std::ostream& operator<<(std::ostream& os, const CMovRR& inst);
    };

    class CMovRM final: public CMov_Base<Address> {
    public:
        constexpr explicit CMovRM(const std::uint8_t size, const Address& src, const GPReg dst, const CondType cond) noexcept:
            CMov_Base(size, src, dst, cond) {}

        friend std::ostream& operator<<(std::ostream& os, const CMovRM& inst);
    };
}