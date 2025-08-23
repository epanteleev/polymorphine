#pragma once

#include <cstdlib>
#include <cstring>
#include <string>
#include <limits>

namespace elf {
    template<typename S>
    class string_section_accessor_template final {
    public:
        //------------------------------------------------------------------------------
        //! \brief Constructor
        //! \param section Pointer to the section
        explicit string_section_accessor_template(S *section) noexcept : string_section(section) {}

        //------------------------------------------------------------------------------
        //! \brief Get a string from the section
        //! \param index Index of the string
        //! \return Pointer to the string, or nullptr if not found
        [[nodiscard]]
        const char *get_string(const Elf_Word index) const {
            if (!string_section) {
                return nullptr;
            }

            const char *data = string_section->get_data();
            const auto section_size = static_cast<size_t>(string_section->get_size());

            if (index >= section_size || nullptr == data) {
                return nullptr;
            }

            size_t remaining_size = section_size - index;
            if (remaining_size > section_size) {
                // Check for underflow
                return nullptr;
            }

            const char *str = data + index;
            const auto end = static_cast<const char *>(std::memchr(str, '\0', remaining_size));
            if (end != nullptr && end < str + remaining_size) {
                return str;
            }
            return nullptr;
        }

        //------------------------------------------------------------------------------
        //! \brief Add a string to the section
        //! \param str Pointer to the string
        //! \return Index of the added string
        Elf_Word add_string(const char *str) {
            if (!str) {
                return 0; // Return index of empty string for null input
            }

            if (string_section == nullptr) {
                return 0; // No section to add string to
            }

            // Strings are added to the end of the current section data
            auto current_position = static_cast<Elf_Word>(string_section->get_size());
            if (current_position == 0) {
                char empty_string = '\0';
                string_section->append_data(&empty_string, 1);
                ++current_position;
            }

            // Calculate string length and check for overflow
            size_t str_len = std::strlen(str);
            if (str_len > std::numeric_limits<Elf_Word>::max() - 1) {
                return 0; // String too long
            }

            // Check if appending would overflow section size
            auto append_size = static_cast<Elf_Word>(str_len + 1);
            if (append_size > std::numeric_limits<Elf_Word>::max() - current_position) {
                return 0; // Would overflow section size
            }

            string_section->append_data(str, append_size);
            return current_position;
        }

        Elf_Word add_string(const std::string &str) {
            return add_string(str.c_str());
        }

    private:
        S *string_section; //!< Pointer to the section
    };

    using string_section_accessor = string_section_accessor_template<section>;
    using const_string_section_accessor = string_section_accessor_template<const section>;
} // namespace ELFIO
