#pragma once

#include <cassert>
#include <cstdint>
#include <expected>
#include <string>
#include <unordered_map>
#include <sys/mman.h>

#include "utility/Error.h"

/**
 * JitCodeBlob is a class that represents ready-to-execute JIT-compiled code.
 */
class JitCodeBlob final {
public:
    JitCodeBlob(std::unordered_map<std::string, std::size_t> &&offset_table,
                std::uint8_t* buffer, const std::size_t size) noexcept:
        m_offset_table(std::move(offset_table)),
        m_buffer(buffer),
        m_size(size) {}

    ~JitCodeBlob() noexcept {
        const auto err = munmap(m_buffer, m_size);
        assert_perror(err);
    }

    /**
     * Finds the start of the code section for a given function name.
     * @return start address of the code section if found, otherwise an error.
     */
    [[nodiscard]]
    std::expected<std::uint8_t*, Error> code_start(const std::string& name) const {
        if (const auto it = m_offset_table.find(name); it != m_offset_table.end()) {
            return m_buffer + it->second;
        }

        return std::unexpected(Error::NotFoundError);
    }

    /**
     * Finds the start of the code section for a given function name and casts it to a specific type.
     * @tparam T the type to cast the code section to.
     * @return pointer to the code section casted to type T if found, otherwise an error.
     */
    template<typename T>
    requires std::is_function_v<T>
    [[nodiscard]]
    std::expected<T*, Error> code_start_as(const std::string& name) const {
        if (const auto code = code_start(name); code.has_value()) {
            return reinterpret_cast<T*>(code.value());
        }

        return std::unexpected(Error::NotFoundError);
    }

    friend std::ostream& operator<<(std::ostream& os, const JitCodeBlob& blob);

private:
    std::unordered_map<std::string, std::size_t> m_offset_table;
    std::uint8_t* m_buffer;
    std::size_t m_size{0};
};

std::ostream & operator<<(std::ostream &os, const JitCodeBlob &blob);