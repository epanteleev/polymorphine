#pragma once


namespace aasm {
    class AddRR final {
    public:
        constexpr AddRR(const std::uint8_t size, const GPReg src, const GPReg dst) noexcept
            : m_size(size), m_src(src), m_dst(dst) {}

        friend std::ostream& operator<<(std::ostream &os, const AddRR& add);

        template<CodeBuffer Buffer>
        constexpr void emit(Buffer& buffer) const {
            static constexpr std::uint8_t ADD_RR = 0x01;
            static constexpr std::uint8_t ADD_RR_8 = 0x00;
            encode_RR<ADD_RR_8, ADD_RR>(buffer, m_size, m_src, m_dst);
        }

    private:
        std::uint8_t m_size;
        GPReg m_src;
        GPReg m_dst;
    };

    inline std::ostream & operator<<(std::ostream &os, const AddRR &add) {
        return os << "add" << prefix_size(add.m_size) << " %"
                  << add.m_src.name(add.m_size) << ", %"
                  << add.m_dst.name(add.m_size);
    }
}
