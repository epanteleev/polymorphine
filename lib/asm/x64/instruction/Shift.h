#pragma once

namespace aasm::details {
    template<typename DST, std::size_t MODRM>
    class Shift_Base {
    public:
        template<typename D = DST>
        constexpr Shift_Base(const std::uint8_t size, const std::uint8_t src, D&& dst) noexcept:
            m_size(size),
            m_src(src),
            m_dst(std::forward<D>(dst)) {}

        template<CodeBuffer Buffer>
        [[nodiscard]]
        constexpr std::optional<Relocation> emit(Buffer& buffer) const {
            static constexpr std::array<std::uint8_t, 1> SHL = {0xC1};
            static constexpr std::array<std::uint8_t, 1> SHL_8 = {0xC0};
            Encoder enc(buffer, SHL_8, SHL);
            return enc.encode_MI8(MODRM, m_size, m_src, m_dst);
        }

    protected:
        std::uint8_t m_size;
        std::uint8_t m_src;
        DST m_dst;
    };

    class SalRI final: public Shift_Base<GPReg, 4> {
    public:
        constexpr SalRI(const std::uint8_t size, const std::uint8_t src, const GPReg dst) noexcept:
            Shift_Base(size, src, dst) {}

        friend std::ostream& operator<<(std::ostream &os, const SalRI&);
    };

    class SalMI final: public Shift_Base<Address, 4> {
    public:
        constexpr SalMI(const std::uint8_t size, const std::uint8_t src, const Address& dst) noexcept:
            Shift_Base(size, src, dst) {}

        friend std::ostream& operator<<(std::ostream &os, const SalMI&);
    };

    class SarRI final: public Shift_Base<GPReg, 7> {
    public:
        constexpr SarRI(const std::uint8_t size, const std::uint8_t src, const GPReg dst) noexcept:
            Shift_Base(size, src, dst) {}

        friend std::ostream& operator<<(std::ostream &os, const SarRI&);
    };

    class SarMI final: public Shift_Base<Address, 7> {
    public:
        constexpr SarMI(const std::uint8_t size, const std::uint8_t src, const Address& dst) noexcept:
            Shift_Base(size, src, dst) {}

        friend std::ostream& operator<<(std::ostream &os, const SarMI&);
    };

    class ShrRI final: public Shift_Base<GPReg, 5> {
    public:
        constexpr ShrRI(const std::uint8_t size, const std::uint8_t src, const GPReg dst) noexcept:
            Shift_Base(size, src, dst) {}

        friend std::ostream& operator<<(std::ostream &os, const ShrRI&);
    };

    class ShrMI final: public Shift_Base<Address, 5> {
    public:
        constexpr ShrMI(const std::uint8_t size, const std::uint8_t src, const Address& dst) noexcept:
            Shift_Base(size, src, dst) {}

        friend std::ostream& operator<<(std::ostream &os, const ShrMI&);
    };

    template<typename DST, std::size_t MODRM>
    class ShiftMR_Base {
    public:
        template<typename D = DST>
        constexpr ShiftMR_Base(const std::uint8_t size, D&& dst) noexcept:
            m_size(size),
            m_dst(std::forward<D>(dst)) {}

        template<CodeBuffer Buffer>
        [[nodiscard]]
        constexpr std::optional<Relocation> emit(Buffer& buffer) const {
            static constexpr std::array<std::uint8_t, 1> SHL = {0xD3};
            static constexpr std::array<std::uint8_t, 1> SHL_8 = {0xD2};
            Encoder enc(buffer, SHL_8, SHL);
            return enc.encode_MC(MODRM, m_size, m_dst);
        }

    protected:
        std::uint8_t m_size;
        DST m_dst;
    };

    class SalRR final: public ShiftMR_Base<GPReg, 4> {
    public:
        constexpr SalRR(const std::uint8_t size, const GPReg dst) noexcept:
            ShiftMR_Base(size, dst) {}

        friend std::ostream& operator<<(std::ostream &os, const SalRR&);
    };

    class SarRR final: public ShiftMR_Base<GPReg, 7> {
    public:
        constexpr SarRR(const std::uint8_t size, const GPReg dst) noexcept:
            ShiftMR_Base(size, dst) {}

        friend std::ostream& operator<<(std::ostream &os, const SarRR&);
    };

    class ShrRR final: public ShiftMR_Base<GPReg, 5> {
    public:
        constexpr ShrRR(const std::uint8_t size, const GPReg dst) noexcept:
            ShiftMR_Base(size, dst) {}

        friend std::ostream& operator<<(std::ostream &os, const ShrRR&);
    };
}