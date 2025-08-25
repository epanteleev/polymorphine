#pragma once

#include <cstdint>
#include <expected>
#include <memory>
#include <unordered_map>

#include "asm/symbol/Symbol.h"
#include "asm/symbol/SymbolTable.h"
#include "utility/Error.h"

/**
 * Represents a block of JIT-compiled code for one function.
 */
class JitCodeChunk final {
public:
    explicit JitCodeChunk(const std::size_t _offset, const std::size_t _size) noexcept:
        offset(_offset), size(_size) {}

    const std::size_t offset; // Offset from code blob start
    const std::size_t size; // Size of the code in bytes
};

/**
 * JitCodeBlob is a class that represents ready-to-execute JIT-compiled code.
 */
class JitCodeBlob final {
public:
    JitCodeBlob(std::unordered_map<const aasm::Symbol*, JitCodeChunk> &&offset_table,
        const std::span<std::uint8_t> code_buffer) noexcept:
        m_offset_table(std::move(offset_table)),
        m_code_buffer(code_buffer) {}

    /**
     * Finds the start of the code section for a given function name.
     * @return start the address of the code section if found, otherwise an error.
     */
    [[nodiscard]]
    std::expected<std::uint8_t*, Error> code_start(const aasm::Symbol* name) const {
        if (const auto it = m_offset_table.find(name); it != m_offset_table.end()) {
            return m_code_buffer.data() + it->second.offset;
        }

        return std::unexpected(Error::NotFoundError);
    }

    friend std::ostream& operator<<(std::ostream& os, const JitCodeBlob& blob);

private:
    std::unordered_map<const aasm::Symbol*, JitCodeChunk> m_offset_table;
    std::span<std::uint8_t> m_code_buffer;
};

std::ostream & operator<<(std::ostream &os, const JitCodeBlob &blob);