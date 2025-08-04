#pragma once

#include <cstdint>
#include <expected>
#include <memory>

#include "JitCodeBlob.h"
#include "asm/symbol/SymbolTable.h"
#include "lir/x64/asm/AsmModule.h"
#include "utility/Error.h"

class JitModule final {
public:
    JitModule(std::shared_ptr<aasm::SymbolTable> symbol_table, std::span<std::uint8_t> total_mem, JitCodeBlob&& code_blob) noexcept:
        m_symbol_table(std::move(symbol_table)),
        m_total_mem(total_mem),
        m_code_blob(std::move(code_blob)) {}

    ~JitModule() noexcept {
        const auto err = munmap(m_total_mem.data(), m_total_mem.size());
        assert_perror(err);
    }

    /**
     * Finds the start of the code section for a given function name.
     * @return start address of the code section if found, otherwise an error.
     */
    [[nodiscard]]
    std::expected<std::uint8_t*, Error> code_start(const std::string& name) const {
        const auto sym = m_symbol_table->find(name);
        if (!sym.has_value()) {
            return std::unexpected(Error::NotFoundError);
        }

        return m_code_blob.code_start(sym.value());
    }

    /**
     * Finds the start of the code section for a given function name and casts it to a specific type.
     * @tparam T the type to cast the code section to.
     * @return pointer to the code section cast to type T if found, otherwise an error.
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

    friend std::ostream& operator<<(std::ostream& os, const JitModule& blob);

    static JitModule assembly(const std::unordered_map<const aasm::Symbol*, std::size_t>& external_symbols, const AsmModule& module);

private:
    std::shared_ptr<aasm::SymbolTable> m_symbol_table;
    std::span<std::uint8_t> m_total_mem;
    JitCodeBlob m_code_blob;
};

std::ostream & operator<<(std::ostream &os, const JitModule &blob);