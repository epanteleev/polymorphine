#pragma once

namespace aasm::details {
    class Lea final {
    public:
        explicit constexpr Lea(const GPReg dst, const Address& src) noexcept:
            m_dst(dst),
            m_src(src) {}

        friend std::ostream& operator<<(std::ostream& os, const Lea& lea);

        template<CodeBuffer Buffer>
        constexpr std::optional<Relocation> emit(Buffer& buffer) const {
            constexpr auto LEA = 0x8D;
            return encode_RM<LEA, LEA>(buffer, 8, m_src, m_dst);
        }

    private:
        GPReg m_dst;
        Address m_src;
    };

    inline std::ostream & operator<<(std::ostream &os, const Lea &lea) {
        return os << "leaq " << lea.m_src << ", %" << lea.m_dst.name(8);
    }
}
