#pragma once

#include <cassert>
#include <cstdint>
#include <expected>
#include <string>
#include <unordered_map>
#include <sys/mman.h>

#include "utility/Error.h"

enum class Error;

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

    [[nodiscard]]
    std::expected<std::uint8_t*, Error> code_start(const std::string& name) const {
        if (const auto it = m_offset_table.find(name); it != m_offset_table.end()) {
            return m_buffer + it->second;
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
