#pragma once

#include <cassert>
#include <sys/mman.h>
#include <cstdint>
#include <expected>
#include <memory>

#include "asm/symbol/SymbolTable.h"
#include "lir/x64/asm/jit/JitDataBlob.h"
#include "asm/x64/AsmModule.h"
#include "utility/Error.h"
#include "utility/Sanitizer.h"

template<typename T>
requires std::is_function_v<T>
class JitFunctionFunctor {
public:
    explicit JitFunctionFunctor(T* fn) noexcept:
        m_fn(fn) {}

    template<typename... Args>
    no_usan decltype(auto) operator()(Args... args) const noexcept {
        // Usan: clang's undefined behavior sanitizers checking the function pointer call here.
        // SIGSEGV is raised when calling the function pointer.
        assert(m_fn != nullptr);
        return m_fn(std::forward<Args>(args)...);
    }

private:
    T* m_fn;
};


class JitModule final {
public:
    JitModule(aasm::SymbolTable&& symbol_table, const std::span<std::uint8_t> total_mem, JitDataBlob&& code_blob) noexcept:
        m_symbol_table(std::move(symbol_table)),
        m_total_mem(total_mem),
        m_code_blob(std::move(code_blob)) {}

    ~JitModule() noexcept {
        const auto err = munmap(m_total_mem.data(), m_total_mem.size());
        assert_perror(err);
    }

    /**
     * Finds the start of the code section for a given function name and casts it to a specific type.
     * @tparam T the type to cast the code section to.
     * @return pointer to the code section cast to type T if found, otherwise an error.
     */
    template<typename T>
    requires std::is_function_v<T>
    [[nodiscard]]
    std::expected<JitFunctionFunctor<T>, Error> code_start_as(const std::string& name) const {
        if (const auto code = code_start(name); code.has_value()) {
            return JitFunctionFunctor<T>(reinterpret_cast<T*>(code.value()));
        }

        return std::unexpected(Error::NotFoundError);
    }

    friend std::ostream& operator<<(std::ostream& os, const JitModule& blob);

    static JitModule assembly(const std::unordered_map<const aasm::Symbol*, std::size_t>& external_symbols, aasm::AsmModule&& module);

private:
    /**
     * Finds the start of the code section for a given function name.
     * @return start the address of the code section if found, otherwise an error.
     */
    [[nodiscard]]
    std::expected<std::uint8_t*, Error> code_start(const std::string& name) const {
        const auto sym = m_symbol_table.find(name);
        if (!sym.has_value()) {
            return std::unexpected(Error::NotFoundError);
        }

        return m_code_blob.code_start(sym.value());
    }

    aasm::SymbolTable m_symbol_table;
    std::span<std::uint8_t> m_total_mem;
    JitDataBlob m_code_blob;
};

std::ostream & operator<<(std::ostream &os, const JitModule &blob);