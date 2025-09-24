#pragma once

namespace aasm::details {
    class AddRR final {
    public:
        constexpr AddRR(const std::uint8_t size, const GPReg src, const GPReg dst) noexcept:
            m_size(size),
            m_src(src),
            m_dst(dst) {}

        friend std::ostream& operator<<(std::ostream &os, const AddRR& add);

        template<CodeBuffer Buffer>
        constexpr void emit(Buffer& buffer) const {
            static constexpr std::array<std::uint8_t, 1> ADD_RR = {0x01};
            static constexpr std::array<std::uint8_t, 1> ADD_RR_8 = {};
            Encoder enc(buffer, ADD_RR_8, ADD_RR);
            enc.encode_MR(m_size, m_src, m_dst);
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

    class AddRI final {
    public:
        explicit AddRI(const std::uint8_t size, const std::int32_t src, const GPReg dst) noexcept:
            m_size(size),
            m_src(src),
            m_dst(dst) {}

        friend std::ostream& operator<<(std::ostream &os, const AddRI& add);

        template<CodeBuffer Buffer>
        constexpr void emit(Buffer& buffer) const {
            static constexpr std::array<std::uint8_t, 1> ADD_RI = {0x81};
            static constexpr std::array<std::uint8_t, 1> ADD_RI_8 = {0x80};
            Encoder enc(buffer, ADD_RI_8, ADD_RI);
            enc.encode_RI32(0, m_size, m_src, m_dst);
        }

    private:
        std::uint8_t m_size;
        std::int32_t m_src;
        GPReg m_dst;
    };

    inline std::ostream & operator<<(std::ostream &os, const AddRI &add) {
        return os << "add" << prefix_size(add.m_size) << " $"
                  << add.m_src << ", %" << add.m_dst.name(add.m_size);
    }

    class AddMI final {
    public:
        constexpr explicit AddMI(const std::uint8_t size, const std::int32_t src, const Address& dst) noexcept:
            m_size(size),
            m_src(src),
            m_dst(dst) {}

        friend std::ostream& operator<<(std::ostream &os, const AddMI& add);

        template<CodeBuffer Buffer>
        [[nodiscard]]
        constexpr std::optional<Relocation> emit(Buffer& buffer) const {
            static constexpr std::array<std::uint8_t, 1> ADD_MI = {0x81};
            static constexpr std::array<std::uint8_t, 1> ADD_MI_8 = {0x80};
            Encoder enc(buffer, ADD_MI_8, ADD_MI);
            return enc.encode_MI32(0, m_size, m_src, m_dst);
        }

    private:
        std::uint8_t m_size;
        std::int32_t m_src;
        Address m_dst;
    };

    inline std::ostream & operator<<(std::ostream &os, const AddMI &add) {
        return os << "add" << prefix_size(add.m_size) << " $"
                  << add.m_src << ", " << add.m_dst;
    }

    class AddRM final {
    public:
        constexpr explicit AddRM(const std::uint8_t size, const Address& src, const GPReg dst) noexcept:
            m_size(size),
            m_dst(dst),
            m_src(src) {}

        friend std::ostream& operator<<(std::ostream &os, const AddRM& add);

        template<CodeBuffer Buffer>
        [[nodiscard]]
        constexpr std::optional<Relocation> emit(Buffer& buffer) const {
            static constexpr std::array<std::uint8_t, 1> ADD_RM = {0x03};
            static constexpr std::array<std::uint8_t, 1> ADD_RM_8 = {0x02};
            Encoder enc(buffer, ADD_RM_8, ADD_RM);
            return enc.encode_RM(m_size, m_src, m_dst);
        }

    private:
        std::uint8_t m_size;
        GPReg m_dst;
        Address m_src;
    };

    inline std::ostream & operator<<(std::ostream &os, const AddRM &add) {
        return os << "add" << prefix_size(add.m_size) << " "
                  << add.m_src << ", %" << add.m_dst.name(add.m_size);
    }

    class AddMR final {
    public:
        constexpr explicit AddMR(const std::uint8_t size, const GPReg src, const Address& dst) noexcept:
            m_size(size),
            m_src(src),
            m_dst(dst) {}

        friend std::ostream& operator<<(std::ostream &os, const AddMR& add);

        template<CodeBuffer Buffer>
        [[nodiscard]]
        constexpr std::optional<Relocation> emit(Buffer& buffer) const {
            static constexpr std::array<std::uint8_t, 1> ADD_MR = {0x01};
            static constexpr std::array<std::uint8_t, 1> ADD_MR_8 = {0x00};
            Encoder enc(buffer, ADD_MR_8, ADD_MR);
            return enc.encode_MR(m_size, m_src, m_dst);
        }

    private:
        std::uint8_t m_size;
        GPReg m_src;
        Address m_dst;
    };

    inline std::ostream & operator<<(std::ostream &os, const AddMR &add) {
        return os << "add" << prefix_size(add.m_size) << " %"
                  << add.m_src.name(add.m_size) << ", " << add.m_dst;
    }
}
