#pragma once

#include <ostream>
#include <vector>

namespace elf {
    class segment final {
    public:
        explicit segment() = default;

        //------------------------------------------------------------------------------
        // Section info functions
        ELFIO_GET_SET_ACCESS(Elf_Word, type, ph.p_type);
        ELFIO_GET_SET_ACCESS(Elf_Word, flags, ph.p_flags);
        ELFIO_GET_SET_ACCESS(Elf_Xword, align, ph.p_align);
        ELFIO_GET_SET_ACCESS(Elf64_Addr, virtual_address, ph.p_vaddr);
        ELFIO_GET_SET_ACCESS(Elf64_Addr, physical_address, ph.p_paddr);
        ELFIO_GET_SET_ACCESS(Elf_Xword, file_size, ph.p_filesz);
        ELFIO_GET_SET_ACCESS(Elf_Xword, memory_size, ph.p_memsz);
        ELFIO_GET_ACCESS(Elf64_Off, offset, ph.p_offset);

        //------------------------------------------------------------------------------
        //! \brief Get the index of the segment
        //! \return Index of the segment
        Elf_Half get_index() const { return index; }

        //------------------------------------------------------------------------------
        //! \brief Free the data of the segment
        void free_data() const {
            data.reset(nullptr);
        }

        //------------------------------------------------------------------------------
        //! \brief Add a section index to the segment
        //! \param sec_index Index of the section
        //! \param addr_align Alignment of the section
        //! \return Index of the added section
        Elf_Half add_section_index(Elf_Half sec_index, Elf_Xword addr_align) {
            sections.emplace_back(sec_index);
            if (addr_align > get_align()) {
                set_align(addr_align);
            }

            return static_cast<Elf_Half>(sections.size());
        }

        //------------------------------------------------------------------------------
        //! \brief Add a section to the segment
        //! \param psec Pointer to the section
        //! \param addr_align Alignment of the section
        //! \return Index of the added section
        Elf_Half add_section(section *psec, Elf_Xword addr_align) {
            return add_section_index(psec->get_index(), addr_align);
        }

        //------------------------------------------------------------------------------
        //! \brief Get the number of sections in the segment
        //! \return Number of sections in the segment
        Elf_Half get_sections_num() const {
            return static_cast<Elf_Half>(sections.size());
        }

        //------------------------------------------------------------------------------
        //! \brief Get the index of a section at a given position
        //! \param num Position of the section
        //! \return Index of the section
        Elf_Half get_section_index_at(const Elf_Half num) const {
            if (num < sections.size()) {
                return sections[num];
            }

            return static_cast<Elf_Half>(-1);
        }

        //------------------------------------------------------------------------------
        //! \brief Set the offset of the segment
        //! \param value Offset of the segment
        void set_offset(const Elf64_Off &value) {
            ph.p_offset = value;
            is_offset_set = true;
        }

        //------------------------------------------------------------------------------
        //! \brief Check if the offset is initialized
        //! \return True if the offset is initialized, false otherwise
        bool is_offset_initialized() const { return is_offset_set; }

        //------------------------------------------------------------------------------
        //! \brief Get the sections of the segment
        //! \return Vector of section indices
        const std::vector<Elf_Half> &get_sections() const {
            return sections;
        }

        //------------------------------------------------------------------------------
        //! \brief Set the index of the segment
        //! \param value Index of the segment
        void set_index(const Elf_Half &value) { index = value; }

        //------------------------------------------------------------------------------
        //! \brief Save the segment to a stream
        //! \param stream Output stream
        //! \param header_offset Offset of the segment header
        //! \param data_offset Offset of the segment data
        void save(std::ostream &stream, const std::streampos header_offset, std::streampos data_offset) {
            ph.p_offset = data_offset;
            adjust_stream_size(stream, header_offset);
            stream.write(reinterpret_cast<const char *>(&ph), sizeof(ph));
        }

        const char *get_data() const {
            return data.get();
        }

    private:
        Elf64_Phdr ph{}; //!< Segment header
        Elf_Half index{}; //!< Index of the segment
        mutable std::unique_ptr<char[]> data; //!< Pointer to the segment data
        std::vector<Elf_Half> sections; //!< Vector of section indices
        bool is_offset_set{}; //!< Flag indicating if the offset is set
    };
}