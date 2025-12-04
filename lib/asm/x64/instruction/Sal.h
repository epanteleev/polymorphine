#pragma once

namespace aasm::details {
    template<typename DST>
    class SalMI_Base {
    public:
        template<typename D = DST>
        constexpr SalMI_Base(const std::uint8_t size, const std::uint8_t src, D&& dst) noexcept:
            m_size(size),
            m_src(src),
            m_dst(std::forward<D>(dst)) {}

        template<CodeBuffer Buffer>
        [[nodiscard]]
        constexpr std::optional<Relocation> emit(Buffer& buffer) const {
            static constexpr std::array<std::uint8_t, 1> SHL = {0xC1};
            static constexpr std::array<std::uint8_t, 1> SHL_8 = {0xC0};
            Encoder enc(buffer, SHL_8, SHL);
            return enc.encode_MI8(4, m_size, m_src, m_dst);
        }

    protected:
        std::uint8_t m_size;
        std::uint8_t m_src;
        DST m_dst;
    };

    class SalRI final: public SalMI_Base<GPReg> {
    public:
        constexpr SalRI(const std::uint8_t size, const std::uint8_t src, const GPReg dst) noexcept:
            SalMI_Base(size, src, dst) {}

        friend std::ostream& operator<<(std::ostream &os, const SalRI&);
    };

    class SalMI final: public SalMI_Base<Address> {
    public:
        constexpr SalMI(const std::uint8_t size, const std::uint8_t src, const Address& dst) noexcept:
            SalMI_Base(size, src, dst) {}

        friend std::ostream& operator<<(std::ostream &os, const SalMI&);
    };

    template<typename DST>
    class SalMR_Base {
    public:
        template<typename D = DST>
        constexpr SalMR_Base(const std::uint8_t size, D&& dst) noexcept:
            m_size(size),
            m_dst(std::forward<D>(dst)) {}

        template<CodeBuffer Buffer>
        [[nodiscard]]
        constexpr std::optional<Relocation> emit(Buffer& buffer) const {
            static constexpr std::array<std::uint8_t, 1> SHL = {0xD3};
            static constexpr std::array<std::uint8_t, 1> SHL_8 = {0xD2};
            Encoder enc(buffer, SHL_8, SHL);
            return enc.encode_MC(m_size, m_dst);
        }

    protected:
        std::uint8_t m_size;
        DST m_dst;
    };

    class SalRR final: public SalMR_Base<GPReg> {
    public:
        constexpr SalRR(const std::uint8_t size, const GPReg dst) noexcept:
            SalMR_Base(size, dst) {}

        friend std::ostream& operator<<(std::ostream &os, const SalRR&);
    };
}