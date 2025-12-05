#pragma once

namespace aasm::details {
    template<typename DST>
    class TestRM_Base {
    public:
        template<typename D = DST>
        constexpr TestRM_Base(const std::uint8_t size, const GPReg src, D&& dst) noexcept:
            m_size(size),
            m_src(src),
            m_dst(std::forward<D>(dst)) {}

        template<CodeBuffer Buffer>
        [[nodiscard]]
        constexpr std::optional<Relocation> emit(Buffer& buffer) const {
            static constexpr std::array<std::uint8_t, 1> TEST = {0x85};
            static constexpr std::array<std::uint8_t, 1> TEST_8 = {0x84};
            Encoder enc(buffer, TEST_8, TEST);
            return enc.encode_MR(m_size, m_src, m_dst);
        }

    protected:
        std::uint8_t m_size;
        GPReg m_src;
        DST m_dst;
    };

    class TestRR final: public TestRM_Base<GPReg> {
    public:
        constexpr TestRR(const std::uint8_t size, const GPReg src, const GPReg dst) noexcept:
            TestRM_Base(size, src, dst) {}

        friend std::ostream& operator<<(std::ostream &os, const TestRR& test);
    };

    class TestMR final: public TestRM_Base<Address> {
    public:
        constexpr explicit TestMR(const std::uint8_t size, const GPReg src, const Address& dst) noexcept:
            TestRM_Base(size, src, dst) {}

        friend std::ostream& operator<<(std::ostream &os, const TestMR& test);
    };

    template<typename DST>
    class TestRI_R {
    public:
        template<typename D = DST>
        explicit constexpr TestRI_R(const std::uint8_t size, const std::int32_t src, D&& dst) noexcept:
            m_size(size),
            m_src(src),
            m_dst(std::forward<D>(dst)) {}

        template<CodeBuffer Buffer>
        [[nodiscard]]
        constexpr std::optional<Relocation> emit(Buffer& buffer) const {
            static constexpr std::array<std::uint8_t, 1> TEST_RI = {0x81};
            static constexpr std::array<std::uint8_t, 1> TEST_RI_8 = {0x80};
            Encoder enc(buffer, TEST_RI_8, TEST_RI);
            return enc.encode_MI32(0, m_size, m_src, m_dst);
        }

    protected:
        std::uint8_t m_size;
        std::int32_t m_src;
        DST m_dst;
    };

    class TestRI final: public TestRI_R<GPReg> {
    public:
        explicit TestRI(const std::uint8_t size, const std::int32_t src, const GPReg dst) noexcept:
            TestRI_R(size, src, dst) {}

        friend std::ostream& operator<<(std::ostream &os, const TestRI& test);
    };

    class TestMI final: public TestRI_R<Address> {
    public:
        constexpr explicit TestMI(const std::uint8_t size, const std::int32_t src, const Address& dst) noexcept:
            TestRI_R(size, src, dst) {}

        friend std::ostream& operator<<(std::ostream &os, const TestMI& test);
    };

    class TestRM final {
    public:
        constexpr explicit TestRM(const std::uint8_t size, const Address& src, const GPReg dst) noexcept:
            m_size(size),
            m_dst(dst),
            m_src(src) {}

        friend std::ostream& operator<<(std::ostream &os, const TestRM& test);

        template<CodeBuffer Buffer>
        [[nodiscard]]
        constexpr std::optional<Relocation> emit(Buffer& buffer) const {
            static constexpr std::array<std::uint8_t, 1> TEST_RM = {0x03};
            static constexpr std::array<std::uint8_t, 1> TEST_RM_8 = {0x02};
            Encoder enc(buffer, TEST_RM_8, TEST_RM);
            return enc.encode_RM(m_size, m_src, m_dst);
        }

    private:
        std::uint8_t m_size;
        GPReg m_dst;
        Address m_src;
    };
}