#pragma once

#include <cstdint>
#include <elf.h>

namespace elf {

using Elf_Half   = std::uint16_t;
using Elf_Word   = std::uint32_t;
using Elf_Sword  = std::int32_t;
using Elf_Xword  = std::uint64_t;
using Elf_Sxword = std::int64_t;

using Elf32_Addr = std::uint32_t;
using Elf32_Off  = std::uint32_t;
using Elf64_Addr = std::uint64_t;
using Elf64_Off  = std::uint64_t;

using Elf32_Half  = Elf_Half;
using Elf64_Half  = Elf_Half;
using Elf32_Word  = Elf_Word;
using Elf64_Word  = Elf_Word;
using Elf32_Sword = Elf_Sword;
using Elf64_Sword = Elf_Sword;
 // namespace ELFIO

#define ELF_ST_BIND( i )    ( ( i ) >> 4 )
#define ELF_ST_TYPE( i )    ( ( i ) & 0xf )
#define ELF_ST_INFO( b, t ) ( ( ( b ) << 4 ) + ( ( t ) & 0xf ) )

#define ELF_ST_VISIBILITY( o ) ( ( o ) & 0x3 )

}