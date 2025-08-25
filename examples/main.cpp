
#include "asm/elf/elfio.hpp"

int main() {
    elf::elfio writer;

    writer.set_os_abi( ELFOSABI_LINUX );
    writer.set_type( ET_REL );
    writer.set_machine( EM_X86_64 );

    // This is our code
    char text[] = {
        '\xB8', '\x04', '\x00', '\x00', '\x00', // mov eax, 4
        '\xBB', '\x01', '\x00', '\x00', '\x00', // mov ebx, 1
        '\xB9', '\x00', '\x00', '\x00', '\x00', // mov ecx, msg
        '\xBA', '\x0E', '\x00', '\x00', '\x00', // mov edx, 14
        '\xCD', '\x80',                         // int 0x80
        '\xB8', '\x01', '\x00', '\x00', '\x00', // mov eax, 1
        '\xCD', '\x80',                         // int 0x80
        '\x48', '\x65', '\x6C', '\x6C', '\x6F', // msg: db   'Hello, World!', 10
        '\x2C', '\x20', '\x57', '\x6F', '\x72',
        '\x6C', '\x64', '\x21', '\x0A' };
    Elf64_Addr place_to_adjust = 11;

    // Create code section
    elf::section* text_sec = writer.sections.add( ".text" );
    text_sec->set_type( SHT_PROGBITS );
    text_sec->set_flags( SHF_ALLOC | SHF_EXECINSTR );
    text_sec->set_addr_align( 0x10 );
    text_sec->set_data( text, sizeof(text));

    // Create string table section
    elf::section* str_sec = writer.sections.add( ".strtab" );
    str_sec->set_type( SHT_STRTAB );

    // Create string table writer
    elf::string_section_accessor stra( str_sec );
    // Add label name
    Elf32_Word str_index = stra.add_string( "msg" );

    // Create symbol table section
    elf::section* sym_sec = writer.sections.add( ".symtab" );
    sym_sec->set_type(SHT_SYMTAB);
    sym_sec->set_info(1);
    sym_sec->set_addr_align( 0x4 );
    sym_sec->set_entry_size( elf::elfio::get_default_entry_size( SHT_SYMTAB ) );
    sym_sec->set_link( str_sec->get_index() );

    // Create symbol table writer
    elf::symbol_section_accessor syma( writer, sym_sec );
    // Add symbol entry (msg has offset == 29)
    elf::Elf_Word sym_to_adjust = syma.add_symbol(
        str_index, 29, 0, STB_GLOBAL, STT_OBJECT, 0, text_sec->get_index() );
    // Another way to add symbol
    syma.add_symbol( stra, "_start", 0x00000000, 0, STB_WEAK, STT_FUNC, 0,
                     text_sec->get_index() );

    // Create relocation table section
    elf::section* rel_sec = writer.sections.add( ".rel.text" );
    rel_sec->set_type( SHT_REL );
    rel_sec->set_info( text_sec->get_index() );
    rel_sec->set_addr_align( 0x4 );
    rel_sec->set_entry_size( writer.get_default_entry_size(SHT_REL) );
    rel_sec->set_link( sym_sec->get_index() );

    // Create relocation table writer
    elf::relocation_section_accessor rela( writer, rel_sec );
    // Add relocation entry (adjust address at offset 11)
    rela.add_entry( place_to_adjust, sym_to_adjust, R_X86_64_32);

    // Create ELF object file
    writer.save( "hello.o" );
    return 0;
}