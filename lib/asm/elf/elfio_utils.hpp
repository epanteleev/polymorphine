#pragma once

#define ELFIO_GET_ACCESS_DECL( TYPE, NAME ) virtual TYPE get_##NAME() const = 0

#define ELFIO_SET_ACCESS_DECL( TYPE, NAME ) \
    virtual void set_##NAME( const TYPE& value ) = 0

#define ELFIO_GET_SET_ACCESS_DECL( TYPE, NAME )       \
    virtual TYPE get_##NAME() const              = 0; \
    virtual void set_##NAME( const TYPE& value ) = 0

#define ELFIO_GET_ACCESS( TYPE, NAME, FIELD ) \
    TYPE get_##NAME() const { return FIELD; }

#define ELFIO_SET_ACCESS( TYPE, NAME, FIELD )     \
    void set_##NAME( const TYPE& value ) override \
    {                                             \
        FIELD = decltype( FIELD )( value );       \
        FIELD = ( *convertor )( FIELD );          \
    }
#define ELFIO_GET_SET_ACCESS( TYPE, NAME, FIELD )                        \
    TYPE get_##NAME() const { return FIELD; }                            \
    void set_##NAME( const TYPE& value )                                 \
    {                                                                    \
        FIELD = decltype( FIELD )( value );                              \
    }

namespace elf {
    //------------------------------------------------------------------------------
    //! \brief Calculate the ELF hash of a name
    //! \param name The name to hash
    //! \return The ELF hash
    inline std::uint32_t elf_hash(const unsigned char *name) {
        std::uint32_t h = 0;
        std::uint32_t g = 0;
        while (*name != '\0') {
            h = (h << 4) + *name++;
            g = h & 0xf0000000;
            if (g != 0)
                h ^= g >> 24;
            h &= ~g;
        }
        return h;
    }

    //------------------------------------------------------------------------------
    //! \brief Calculate the GNU hash of a name
    //! \param s The name to hash
    //! \return The GNU hash
    inline std::uint32_t elf_gnu_hash(const unsigned char *s) {
        std::uint32_t h = 0x1505;
        for (unsigned char c = *s; c != '\0'; c = *++s) {
            h = (h << 5) + h + c;
        }
        return h;
    }

    //------------------------------------------------------------------------------
    //! \brief Convert a value to a hexadecimal string
    //! \param value The value to convert
    //! \return The hexadecimal string
    inline std::string to_hex_string(std::uint64_t value) {
        std::string str;

        while (value) {
            if (auto digit = value & 0xF; digit < 0xA) {
                str = char('0' + digit) + str;
            } else {
                str = char('A' + digit - 0xA) + str;
            }
            value >>= 4;
        }

        return "0x" + str;
    }

    //------------------------------------------------------------------------------
    //! \brief Adjust the size of a stream
    //! \param stream The stream to adjust
    //! \param offset The offset to adjust to
    inline void adjust_stream_size(std::ostream &stream, std::streamsize offset) {
        stream.seekp(0, std::ios_base::end);
        if (stream.tellp() < offset) {
            std::streamsize size = offset - stream.tellp();
            stream.write(std::string(static_cast<std::size_t>(size), '\0').c_str(), size);
        }
        stream.seekp(offset);
    }
}