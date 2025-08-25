#pragma once

#include <string>
#include <functional>
#include <algorithm>
#include <vector>
#include <deque>
#include <fstream>
#include <memory>

#include "elf_types.hpp"
#include "elfio_utils.hpp"
#include "elfio_header.hpp"
#include "elfio_section.hpp"
#include "elfio_segment.hpp"
#include "elfio_strings.hpp"

#define ELFIO_HEADER_ACCESS_GET( TYPE, FNAME ) \
    TYPE get_##FNAME() const { return header ? ( header->get_##FNAME() ) : 0; }

#define ELFIO_HEADER_ACCESS_GET_SET( TYPE, FNAME )     \
    TYPE get_##FNAME() const                           \
    {                                                  \
        return header ? ( header->get_##FNAME() ) : 0; \
    }                                                  \
    void set_##FNAME( TYPE val )                       \
    {                                                  \
        if ( header ) {                                \
            header->set_##FNAME( val );                \
        }                                              \
    }

namespace elf {
    class elfio final {
    public:
        explicit elfio() noexcept:
            sections(this),
            segments(this),
            header(create_header()) {
            create_mandatory_sections();
        }

        //------------------------------------------------------------------------------
        //! \brief Save the ELF file to a file
        //! \param file_name The name of the file to save to
        //! \return True if successful, false otherwise
        bool save(const std::string_view file_name) {
            std::ofstream stream;
            stream.open(file_name.data(), std::ios::out | std::ios::binary);
            if (!stream) {
                return false;
            }

            return save(stream);
        }

        //------------------------------------------------------------------------------
        //! \brief Save the ELF file to a stream
        //! \param stream The output stream to save to
        //! \return True if successful, false otherwise
        bool save(std::ostream &stream) {
            // Define layout specific header fields
            // The position of the segment table is fixed after the header.
            // The position of the section table is variable and needs to be fixed
            // before saving.
            header->set_segments_num(segments.size());
            header->set_segments_offset(
                segments.size() > 0 ? header->get_header_size() : 0);
            header->set_sections_num(sections.size());
            header->set_sections_offset(0);

            // Layout the first section right after the segment table
            current_file_pos = header->get_header_size() + header->get_segment_entry_size() * static_cast<Elf_Xword>(header->get_segments_num());
            calc_segment_alignment();

            bool is_still_good = layout_segments_and_their_sections();
            is_still_good = is_still_good && layout_sections_without_segments();
            is_still_good = is_still_good && layout_section_table();

            is_still_good = is_still_good && save_header(stream);
            is_still_good = is_still_good && save_sections(stream);
            is_still_good = is_still_good && save_segments(stream);

            return is_still_good;
        }

        //------------------------------------------------------------------------------
        // ELF header access functions
        ELFIO_HEADER_ACCESS_GET(unsigned char, class);
        ELFIO_HEADER_ACCESS_GET(unsigned char, elf_version);
        ELFIO_HEADER_ACCESS_GET(unsigned char, encoding);
        ELFIO_HEADER_ACCESS_GET(Elf_Word, version);
        ELFIO_HEADER_ACCESS_GET(Elf_Half, header_size);
        ELFIO_HEADER_ACCESS_GET(Elf_Half, section_entry_size);
        ELFIO_HEADER_ACCESS_GET(Elf_Half, segment_entry_size);

        ELFIO_HEADER_ACCESS_GET_SET(unsigned char, os_abi);
        ELFIO_HEADER_ACCESS_GET_SET(unsigned char, abi_version);
        ELFIO_HEADER_ACCESS_GET_SET(Elf_Half, type);
        ELFIO_HEADER_ACCESS_GET_SET(Elf_Half, machine);
        ELFIO_HEADER_ACCESS_GET_SET(Elf_Word, flags);
        ELFIO_HEADER_ACCESS_GET_SET(Elf64_Addr, entry);
        ELFIO_HEADER_ACCESS_GET_SET(Elf64_Off, sections_offset);
        ELFIO_HEADER_ACCESS_GET_SET(Elf64_Off, segments_offset);
        ELFIO_HEADER_ACCESS_GET_SET(Elf_Half, section_name_str_index);

        [[nodiscard]]
        static Elf_Xword get_default_entry_size(const Elf_Word section_type) noexcept {
            switch (section_type) {
                case SHT_RELA:    return sizeof(Elf64_Rela);
                case SHT_REL:     return sizeof(Elf64_Rel);
                case SHT_SYMTAB:  return sizeof(Elf64_Sym);
                case SHT_DYNAMIC: return sizeof(Elf64_Dyn);
                default: return 0;
            }
        }

    private:
        //------------------------------------------------------------------------------
        //! \brief Check if an offset is within a section
        //! \param offset The offset to check
        //! \param sec Pointer to the section
        //! \return True if the offset is within the section, false otherwise
        static bool is_offset_in_section(Elf64_Off offset, const section *sec) {
            return (offset >= sec->get_offset()) &&
                   (offset < (sec->get_offset() + sec->get_size()));
        }

        //------------------------------------------------------------------------------
        //! \brief Get the virtual address of an offset within a section
        //! \param offset The offset within the section
        //! \param sec Pointer to the section
        //! \return The virtual address of the offset within the section
        static Elf64_Addr get_virtual_addr(const Elf64_Off offset, const section *sec) {
            return sec->get_address() + offset - sec->get_offset();
        }

        [[nodiscard]]
        const section *find_prog_section_for_offset(Elf64_Off offset) const {
            for (const auto &sec: sections) {
                if (sec->get_type() == SHT_PROGBITS &&
                    is_offset_in_section(offset, sec.get())) {
                    return sec.get();
                }
            }
            return nullptr;
        }

        //------------------------------------------------------------------------------
        //! \brief Create an ELF header
        //! \return Unique pointer to the created ELF header
        static std::unique_ptr<elf_header> create_header() {
            return std::make_unique<elf_header>();
        }

        //------------------------------------------------------------------------------
        //! \brief Create a new section
        //! \return Pointer to the created section
        section *create_section(const std::string_view name) {
            sections_.emplace_back(new(std::nothrow) section(name));
            section *new_section = sections_.back().get();
            new_section->set_index(static_cast<Elf_Half>(sections_.size() - 1));
            return new_section;
        }

        //------------------------------------------------------------------------------
        //! \brief Create a new segment
        //! \return Pointer to the created segment
        segment *create_segment() {
            segments_.emplace_back(new(std::nothrow) segment());
            auto *new_segment = segments_.back().get();
            new_segment->set_index(static_cast<Elf_Half>(segments_.size() - 1));
            return new_segment;
        }

        void create_mandatory_sections() {
            auto sec0 = create_section("");
            sec0->set_index(0);
            sec0->set_name_string_offset(0);

            set_section_name_str_index(1);
            auto shstrtab = sections.add(".shstrtab");
            shstrtab->set_type(SHT_STRTAB);
            shstrtab->set_addr_align(1);
        }

        //------------------------------------------------------------------------------
        //! \brief Checks whether the addresses of the section entirely fall within the given segment.
        //! It doesn't matter if the addresses are memory addresses, or file offsets,
        //!  they just need to be in the same address space
        //! \param sect_begin The beginning address of the section
        //! \param sect_size The size of the section
        //! \param seg_begin The beginning address of the segment
        //! \param seg_end The end address of the segment
        //! \return True if the section is within the segment, false otherwise
        static bool is_sect_in_seg(const Elf64_Off sect_begin,
                                   const Elf_Xword sect_size,
                                   const Elf64_Off seg_begin,
                                   const Elf64_Off seg_end) {
            return (seg_begin <= sect_begin) &&
                   (sect_begin + sect_size <= seg_end) &&
                   (sect_begin <
                    seg_end);
        }

        //------------------------------------------------------------------------------
        //! \brief Save the ELF header to a stream
        //! \param stream The output stream to save to
        //! \return True if successful, false otherwise
        bool save_header(std::ostream &stream) const {
            return header->save(stream);
        }

        //------------------------------------------------------------------------------
        //! \brief Save the sections to a stream
        //! \param stream The output stream to save to
        //! \return True if successful, false otherwise
        bool save_sections(std::ostream &stream) const {
            for (const auto &sec: sections_) {
                std::streampos headerPosition =
                        static_cast<std::streamoff>(header->get_sections_offset()) +
                        static_cast<std::streampos>(
                            header->get_section_entry_size()) *
                        sec->get_index();

                sec->save(stream, headerPosition, sec->get_offset());
            }
            return true;
        }

        //------------------------------------------------------------------------------
        //! \brief Save the segments to a stream
        //! \param stream The output stream to save to
        //! \return True if successful, false otherwise
        bool save_segments(std::ostream &stream) const {
            for (const auto &seg: segments_) {
                std::streampos headerPosition =
                        static_cast<std::streamoff>(header->get_segments_offset()) +
                        static_cast<std::streampos>(
                            header->get_segment_entry_size()) *
                        seg->get_index();

                seg->save(stream, headerPosition, seg->get_offset());
            }
            return true;
        }

        [[nodiscard]]
        bool is_section_without_segment(const std::size_t section_index) const {
            bool found = false;
            for (unsigned int j = 0; !found && j < segments.size(); ++j) {
                for (Elf_Half k = 0; !found && k < segments[j]->get_sections_num(); ++k) {
                    found = segments[j]->get_section_index_at(k) == section_index;
                }
            }

            return !found;
        }

        //------------------------------------------------------------------------------
        //! \brief Check if a segment is a subsequence of another segment
        //! \param seg1 Pointer to the first segment
        //! \param seg2 Pointer to the second segment
        //! \return True if seg1 is a subsequence of seg2, false otherwise
        static bool is_subsequence_of(const segment *seg1, const segment *seg2) {
            // Return 'true' if sections of seg1 are a subset of sections in seg2
            const std::vector<Elf_Half> &sections1 = seg1->get_sections();
            const std::vector<Elf_Half> &sections2 = seg2->get_sections();

            bool found = false;
            if (sections1.size() < sections2.size()) {
                found = std::ranges::includes(sections2, sections1);
            }

            return found;
        }

        [[nodiscard]]
        std::vector<segment *> get_ordered_segments() const {
            std::vector<segment*> res;
            res.reserve(segments.size());

            std::deque<segment*> worklist;
            for (const auto &seg: segments) {
                worklist.emplace_back(seg.get());
            }

            // Bring the segments which start at address 0 to the front
            size_t nextSlot{};
            for (size_t i{}; i < worklist.size(); ++i) {
                if (i != nextSlot && worklist[i]->is_offset_initialized() && worklist[i]->get_offset() == 0) {
                    if (worklist[nextSlot]->get_offset() == 0) {
                        ++nextSlot;
                    }
                    std::swap(worklist[i], worklist[nextSlot]);
                    nextSlot+=1;
                }
            }

            while (!worklist.empty()) {
                segment *seg = worklist.front();
                worklist.pop_front();

                size_t i{};
                for (; i < worklist.size(); ++i) {
                    if (is_subsequence_of(seg, worklist[i])) {
                        break;
                    }
                }

                if (i < worklist.size()) {
                    worklist.emplace_back(seg);
                } else {
                    res.emplace_back(seg);
                }
            }

            return res;
        }

        //------------------------------------------------------------------------------
        //! \brief Layout sections without segments
        //! \return True if successful, false otherwise
        bool layout_sections_without_segments() {
            for (std::size_t i = 0; i < sections_.size(); ++i) {
                if (!is_section_without_segment(i)) {
                    continue;
                }

                const auto &sec = sections_[i];
                const Elf_Xword section_align = sec->get_addr_align();
                if (section_align > 1 && current_file_pos % section_align != 0) {
                    current_file_pos += section_align - current_file_pos % section_align;
                }

                if (0 != sec->get_index()) {
                    sec->set_offset(current_file_pos);
                }

                if (SHT_NOBITS != sec->get_type() &&
                    SHT_NULL != sec->get_type()) {
                    current_file_pos += sec->get_size();
                }
            }

            return true;
        }

        //------------------------------------------------------------------------------
        //! \brief Calculate segment alignment
        void calc_segment_alignment() const {
            for (const auto &seg: segments_) {
                for (Elf_Half i = 0; i < seg->get_sections_num(); ++i) {
                    const auto &sect = sections_[seg->get_section_index_at(i)];
                    if (sect->get_addr_align() > seg->get_align()) {
                        seg->set_align(sect->get_addr_align());
                    }
                }
            }
        }

        //------------------------------------------------------------------------------
        //! \brief Layout segments and their sections
        //! \return True if successful, false otherwise
        bool layout_segments_and_their_sections() {
            std::vector section_generated(sections.size(), false);

            for (auto *seg: get_ordered_segments()) {
                Elf_Xword segment_memory = 0;
                Elf_Xword segment_filesize = 0;
                Elf_Xword seg_start_pos = current_file_pos;
                // Special case: PHDR segment
                // This segment contains the program headers but no sections
                if (seg->get_type() == PT_PHDR && seg->get_sections_num() == 0) {
                    seg_start_pos = header->get_segments_offset();
                    segment_memory = segment_filesize =
                                     header->get_segment_entry_size() * static_cast<Elf_Xword>(header->
                                         get_segments_num());
                } else if (seg->is_offset_initialized() && seg->get_offset() == 0) {
                    seg_start_pos = 0;
                    if (seg->get_sections_num() > 0) {
                        segment_memory = segment_filesize = current_file_pos;
                    }
                }
                // New segments with not generated sections
                // have to be aligned
                else if (seg->get_sections_num() > 0 &&
                         !section_generated[seg->get_section_index_at(0)]) {
                    Elf_Xword align = seg->get_align() > 0 ? seg->get_align() : 1;
                    Elf64_Off cur_page_alignment = current_file_pos % align;
                    Elf64_Off req_page_alignment =
                            seg->get_virtual_address() % align;
                    Elf64_Off adjustment = req_page_alignment - cur_page_alignment;

                    current_file_pos += (seg->get_align() + adjustment) % align;
                    seg_start_pos = current_file_pos;
                } else if (seg->get_sections_num() > 0) {
                    seg_start_pos = sections[seg->get_section_index_at(0)]->get_offset();
                }

                // Write segment's data
                if (!write_segment_data(seg, section_generated, segment_memory,
                                        segment_filesize, seg_start_pos)) {
                    return false;
                }

                seg->set_file_size(segment_filesize);

                // If we already have a memory size from loading an elf file (value > 0),
                // it must not shrink!
                // Memory size may be bigger than file size and it is the loader's job to do something
                // with the surplus bytes in memory, like initializing them with a defined value.
                if (seg->get_memory_size() < segment_memory) {
                    seg->set_memory_size(segment_memory);
                }

                seg->set_offset(seg_start_pos);
            }

            return true;
        }

        //------------------------------------------------------------------------------
        //! \brief Layout the section table
        //! \return True if successful, false otherwise
        bool layout_section_table() {
            // Simply place the section table at the end for now
            Elf64_Off alignmentError = current_file_pos % 16;
            current_file_pos += 16 - alignmentError;
            header->set_sections_offset(current_file_pos);
            return true;
        }

        //------------------------------------------------------------------------------
        //! \brief Write segment data
        //! \param seg Pointer to the segment
        //! \param section_generated Vector of section generated flags
        //! \param segment_memory Reference to the segment memory size
        //! \param segment_filesize Reference to the segment file size
        //! \param seg_start_pos The start position of the segment
        //! \return True if successful, false otherwise
        bool write_segment_data(const segment *seg,
                                std::vector<bool> &section_generated,
                                Elf_Xword &segment_memory,
                                Elf_Xword &segment_filesize,
                                const Elf_Xword &seg_start_pos) {
            for (Elf_Half j = 0; j < seg->get_sections_num(); ++j) {
                Elf_Half index = seg->get_section_index_at(j);

                section *sec = sections[index];

                // The NULL section is always generated
                if (SHT_NULL == sec->get_type()) {
                    section_generated[index] = true;
                    continue;
                }

                Elf_Xword section_align = 0;
                // Fix up the alignment
                if (!section_generated[index] && sec->is_address_initialized() && SHT_NOBITS != sec->get_type() &&
                    SHT_NULL != sec->get_type() &&
                    0 != sec->get_size()) {
                    // Align the sections based on the virtual addresses
                    // when possible (this is what matters for execution)
                    Elf64_Off req_offset =
                            sec->get_address() - seg->get_virtual_address();
                    Elf64_Off cur_offset = current_file_pos - seg_start_pos;
                    if (req_offset < cur_offset) {
                        // something has gone awfully wrong, abort!
                        // section_align would turn out negative, seeking backwards and overwriting previous data
                        return false;
                    }
                    section_align = req_offset - cur_offset;

                } else if (!section_generated[index] &&
                           !sec->is_address_initialized()) {
                    // If no address has been specified then only the section
                    // alignment constraint has to be matched
                    Elf_Xword align = sec->get_addr_align();
                    if (align == 0) {
                        align = 1;
                    }
                    Elf64_Off error = current_file_pos % align;
                    section_align = (align - error) % align;

                } else if (section_generated[index]) {
                    // Alignment for already generated sections
                    section_align = sec->get_offset() - seg_start_pos - segment_filesize;
                }

                // Determine the segment file and memory sizes
                // Special case .tbss section (NOBITS) in non TLS segment
                if ((sec->get_flags() & SHF_ALLOC) == SHF_ALLOC &&
                    !((sec->get_flags() & SHF_TLS) == SHF_TLS &&
                      seg->get_type() != PT_TLS && SHT_NOBITS == sec->get_type())) {
                    segment_memory += sec->get_size() + section_align;
                }

                if (SHT_NOBITS != sec->get_type()) {
                    segment_filesize += sec->get_size() + section_align;
                }

                // Nothing to be done when generating nested segments
                if (section_generated[index]) {
                    continue;
                }

                current_file_pos += section_align;

                // Set the section addresses when missing
                if (!sec->is_address_initialized()) {
                    sec->set_address(seg->get_virtual_address() +
                                     current_file_pos - seg_start_pos);
                }

                if (0 != sec->get_index()) {
                    sec->set_offset(current_file_pos);
                }

                if (SHT_NOBITS != sec->get_type()) {
                    current_file_pos += sec->get_size();
                }

                section_generated[index] = true;
            }

            return true;
        }

        //------------------------------------------------------------------------------
    public:
        //! \class Sections
        //! \brief The Sections class provides methods to manipulate sections in an ELF file.
        friend class Sections;

        class Sections {
        public:
            //------------------------------------------------------------------------------
            //! \brief Constructor
            //! \param parent Pointer to the parent elfio object
            explicit Sections(elfio *parent) : parent(parent) {
            }

            //------------------------------------------------------------------------------
            //! \brief Get the number of sections
            //! \return The number of sections
            [[nodiscard]] Elf_Half size() const {
                return static_cast<Elf_Half>(parent->sections_.size());
            }

            section *operator[](const std::size_t index) const {
                if (index >= parent->sections_.size()) {
                    return nullptr;
                }

                return parent->sections_[index].get();
            }

            //------------------------------------------------------------------------------
            //! \brief Get a section by name
            //! \param name The name of the section
            //! \return Pointer to the section, or nullptr if not found
            section *operator[](const std::string_view name) const {
                section *sec = nullptr;

                for (const auto &it: parent->sections_) {
                    if (it->get_name() == name) {
                        sec = it.get();
                        break;
                    }
                }

                return sec;
            }

            //------------------------------------------------------------------------------
            //! \brief Add a new section
            //! \param name The name of the section
            //! \return Pointer to the created section
            [[nodiscard]]
            section *add(const std::string &name) const {
                const auto new_section = parent->create_section(name);
                Elf_Half str_index = parent->get_section_name_str_index();
                section *string_table(parent->sections_[str_index].get());
                string_section_accessor str_writer(string_table);
                Elf_Word pos = str_writer.add_string(name);
                new_section->set_name_string_offset(pos);
                return new_section;
            }

            //------------------------------------------------------------------------------
            //! \brief Get an iterator to the beginning of the sections
            //! \return Iterator to the beginning of the sections
            std::vector<std::unique_ptr<section> >::iterator begin() {
                return parent->sections_.begin();
            }

            //------------------------------------------------------------------------------
            //! \brief Get an iterator to the end of the sections
            //! \return Iterator to the end of the sections
            std::vector<std::unique_ptr<section> >::iterator end() {
                return parent->sections_.end();
            }

            //------------------------------------------------------------------------------
            //! \brief Get a const iterator to the beginning of the sections
            //! \return Const iterator to the beginning of the sections
            [[nodiscard]]
            std::vector<std::unique_ptr<section> >::const_iterator begin() const {
                return parent->sections_.cbegin();
            }

            //------------------------------------------------------------------------------
            //! \brief Get a const iterator to the end of the sections
            //! \return Const iterator to the end of the sections
            [[nodiscard]]
            std::vector<std::unique_ptr<section> >::const_iterator end() const {
                return parent->sections_.cend();
            }

            //------------------------------------------------------------------------------
        private:
            elfio *parent; //!< Pointer to the parent elfio object
        };

        Sections sections; //!< Sections object

        friend class Segments;

        class Segments final {
        public:
            //------------------------------------------------------------------------------
            //! \brief Constructor
            //! \param parent Pointer to the parent elfio object
            explicit Segments(elfio *parent) noexcept : parent(parent) {
            }

            //------------------------------------------------------------------------------
            //! \brief Get the number of segments
            //! \return The number of segments
            [[nodiscard]]
            Elf_Half size() const {
                return static_cast<Elf_Half>(parent->segments_.size());
            }

            //------------------------------------------------------------------------------
            //! \brief Get a segment by index
            //! \param index The index of the segment
            //! \return Pointer to the segment, or nullptr if not found
            segment *operator[](const std::size_t index) const {
                return parent->segments_[index].get();
            }

            //------------------------------------------------------------------------------
            //! \brief Add a new segment
            //! \return Pointer to the created segment
            [[nodiscard]]
            segment *add() const { return parent->create_segment(); }

            //------------------------------------------------------------------------------
            //! \brief Get an iterator to the beginning of the segments
            //! \return Iterator to the beginning of the segments
            std::vector<std::unique_ptr<segment> >::iterator begin() {
                return parent->segments_.begin();
            }

            //------------------------------------------------------------------------------
            //! \brief Get an iterator to the end of the segments
            //! \return Iterator to the end of the segments
            std::vector<std::unique_ptr<segment> >::iterator end() {
                return parent->segments_.end();
            }

            //------------------------------------------------------------------------------
            //! \brief Get a const iterator to the beginning of the segments
            //! \return Const iterator to the beginning of the segments
            [[nodiscard]]
            std::vector<std::unique_ptr<segment> >::const_iterator begin() const {
                return parent->segments_.cbegin();
            }

            //------------------------------------------------------------------------------
            //! \brief Get a const iterator to the end of the segments
            //! \return Const iterator to the end of the segments
            [[nodiscard]]
            std::vector<std::unique_ptr<segment> >::const_iterator end() const {
                return parent->segments_.cend();
            }

            //------------------------------------------------------------------------------
        private:
            elfio *parent; //!< Pointer to the parent elfio object
        };

        Segments segments; //!< Segments object

        //------------------------------------------------------------------------------
    private:
        std::unique_ptr<elf_header> header; //!< Pointer to the ELF header
        std::vector<std::unique_ptr<section>> sections_; //!< Vector of sections
        std::vector<std::unique_ptr<segment>> segments_; //!< Vector of segments
        Elf_Xword current_file_pos{}; //!< Current file position
    };
}

#include "elfio_symbols.hpp"
#include "elfio_relocation.hpp"
#include "elfio_dynamic.hpp"
