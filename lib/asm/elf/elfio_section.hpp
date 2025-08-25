#pragma once

#include <string>
#include <ostream>
#include <limits>
#include <utility>

namespace elf {
    class section final {
    public:
        explicit section(const std::string_view name_) noexcept :
            name(name_) {}

        // Section info functions
        ELFIO_GET_SET_ACCESS(Elf_Word, type, header.sh_type);
        ELFIO_GET_SET_ACCESS(Elf_Xword, flags, header.sh_flags);
        ELFIO_GET_SET_ACCESS(Elf_Xword, size, header.sh_size);
        ELFIO_GET_SET_ACCESS(Elf_Word, link, header.sh_link);
        ELFIO_GET_SET_ACCESS(Elf_Word, info, header.sh_info);
        ELFIO_GET_SET_ACCESS(Elf_Xword, addr_align, header.sh_addralign);
        ELFIO_GET_SET_ACCESS(Elf_Xword, entry_size, header.sh_entsize);
        ELFIO_GET_SET_ACCESS(Elf_Word, name_string_offset, header.sh_name);
        ELFIO_GET_ACCESS(Elf64_Addr, address, header.sh_addr);

        /**
         * @brief Get the index of the section.
         * @return Index of the section.
         */
        Elf_Half get_index() const { return index; }

        /**
         * @brief Get the name of the section.
         * @return Name of the section.
         */
        const std::string &get_name() const { return name; }

        /**
         * @brief Set the address of the section.
         * @param value Address of the section.
         */
        void set_address(const Elf64_Addr &value) {
            header.sh_addr = value;
            is_address_set = true;
        }

        /**
         * @brief Check if the address is initialized.
         * @return True if initialized, false otherwise.
         */
        bool is_address_initialized() const { return is_address_set; }

        /**
         * @brief Get the data of the section.
         * @return Pointer to the data.
         */
        const char *get_data() const {
            return data.get();
        }

        /**
         * @brief Free the data of the section.
         */
        void free_data() const {
            data.reset(nullptr);
        }

        /**
         * @brief Set the data of the section.
         * @param raw_data Pointer to the raw data.
         * @param size Size of the data.
         */
        void set_data(const char *raw_data, const Elf_Xword size) {
            if (get_type() != SHT_NOBITS) {
                data = std::unique_ptr<char[]>(new(std::nothrow) char[size]);
                data_size = size;
                std::copy_n(raw_data, size, data.get());
            }

            set_size(data_size);
        }

        /**
         * @brief Set the data of the section.
         * @param str_data String containing the data.
         */
        void set_data(const std::string &str_data) {
            return set_data(str_data.c_str(), static_cast<Elf_Word>(str_data.size()));
        }

        /**
         * @brief Append data to the section.
         * @param raw_data Pointer to the raw data.
         * @param size Size of the data.
         */
        void append_data(const char *raw_data, const Elf_Xword size) {
            insert_data(get_size(), raw_data, size);
        }

        /**
         * @brief Append data to the section.
         * @param str_data String containing the data.
         */
        void append_data(const std::string &str_data) {
            return append_data(str_data.c_str(), static_cast<Elf_Word>(str_data.size()));
        }

        /**
         * @brief Insert data into the section at a specific position.
         * @param pos Position to insert the data.
         * @param raw_data Pointer to the raw data.
         * @param size Size of the data.
         */
        void insert_data(const Elf_Xword pos, const char *raw_data, const Elf_Xword size) {
            if (get_type() == SHT_NOBITS) {
                return;
            }
            // Check for valid position
            if (pos > get_size()) {
                return; // Invalid position
            }

            // Check for integer overflow in size calculation
            Elf_Xword new_size = get_size() + size;
            if (new_size <= data_size) {
                char *d = data.get();
                std::copy_backward(d + pos, d + get_size(),
                                   d + get_size() + size);
                std::copy_n(raw_data, size, d + pos);
            } else {
                // Calculate new size with overflow check
                Elf_Xword new_data_size = data_size;
                if (new_data_size >
                    std::numeric_limits<Elf_Xword>::max() / 2) {
                    return; // Multiplication would overflow
                }
                new_data_size *= 2;
                if (size > std::numeric_limits<Elf_Xword>::max() - new_data_size) {
                    return; // Addition would overflow
                }
                new_data_size += size;

                // Check if the size would overflow size_t
                if (new_data_size > std::numeric_limits<size_t>::max()) {
                    return; // Size would overflow size_t
                }

                std::unique_ptr<char[]> new_data(
                    new(std::nothrow) char[new_data_size]);

                if (nullptr != new_data) {
                    char *d = data.get();
                    std::copy_n(d, pos, new_data.get());
                    std::copy_n(raw_data, size,
                                new_data.get() + pos);
                    std::copy(d + pos, d + get_size(),
                              new_data.get() + pos + size);
                    data = std::move(new_data);
                    data_size = new_data_size;
                } else {
                    return; // Allocation failed
                }
            }
            set_size(new_size);
        }

        /**
         * @brief Insert data into the section at a specific position.
         * @param pos Position to insert the data.
         * @param str_data String containing the data.
         */
        void insert_data(Elf_Xword pos, const std::string &str_data) {
            return insert_data(pos, str_data.c_str(), (Elf_Word) str_data.size());
        }

        ELFIO_GET_SET_ACCESS(Elf64_Off, offset, header.sh_offset);

        /**
         * @brief Set the index of the section.
         * @param value Index of the section.
         */
        void set_index(const Elf_Half &value) { index = value; }

        /**
         * @brief Save the section to a stream.
         * @param stream Output stream.
         * @param header_offset Offset of the header.
         * @param data_offset Offset of the data.
         */
        void save(std::ostream &stream, const std::streampos header_offset, const std::streampos data_offset) {
            if (0 != get_index()) {
                header.sh_offset = static_cast<Elf64_Off>(data_offset);
            }

            save_header(stream, header_offset);
            if (get_type() != SHT_NOBITS && get_type() != SHT_NULL && get_size() != 0 && data != nullptr) {
                save_data(stream, data_offset);
            }
        }

    private:
        /**
         * @brief Save the header of the section to a stream.
         * @param stream Output stream.
         * @param header_offset Offset of the header.
         */
        void save_header(std::ostream &stream, const std::streampos header_offset) const {
            adjust_stream_size(stream, header_offset);
            stream.write(reinterpret_cast<const char *>(&header),
                         sizeof(header));
        }

        /**
         * @brief Save the data of the section to a stream.
         * @param stream Output stream.
         * @param data_offset Offset of the data.
         */
        void save_data(std::ostream &stream, const std::streampos data_offset) const {
            adjust_stream_size(stream, data_offset);
            stream.write(get_data(), get_size());
        }

        std::string name; /**< Name of the section. */
        Elf_Half index{}; /**< Index of the section. */
        Elf64_Shdr header{}; /**< Section header. */
        mutable std::unique_ptr<char[]> data; /**< Pointer to the data. */
        mutable Elf_Xword data_size{}; /**< Size of the data. */
        bool is_address_set = false; /**< Flag indicating if the address is set. */
    };
} // namespace ELFIO
