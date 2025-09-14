#pragma once
#include <cstdint>
#include <optional>
#include <string>
#include <utility>
#include <variant>

enum class SlotType: std::uint8_t {
    Byte,
    Word,
    DWord,
    QWord,
    String,
};

inline std::optional<SlotType> to_slot_type(const std::uint8_t size) {
    switch (size) {
        case 1: return SlotType::Byte;
        case 2: return SlotType::Word;
        case 4: return SlotType::DWord;
        case 8: return SlotType::QWord;
        default: return std::nullopt;
    }
}

std::ostream& operator<<(std::ostream& os, const SlotType& type);

template<typename T>
concept SlotVariant = std::integral<T> || std::is_same_v<T, std::string>;

class Slot final {
public:
    template<typename T>
    explicit Slot(T&& data, std::string&& name, const SlotType type) noexcept:
        m_type(type),
        m_name(std::move(name)),
        m_value(std::forward<T>(data)) {}

    [[nodiscard]]
    SlotType type() const noexcept {
        return m_type;
    }

    template<typename Fn>
    decltype(auto) visit(Fn&& fn) const {
        return std::visit(std::forward<Fn>(fn), m_value);
    }

    [[nodiscard]]
    std::uint8_t size() const noexcept;

    [[nodiscard]]
    std::string_view name() const noexcept { return m_name; }

    friend std::ostream &operator<<(std::ostream &os, const Slot &slot);

    void print_description(std::ostream &os) const;

private:
    SlotType m_type;
    std::string m_name;
    std::variant<std::int64_t, std::string> m_value;
};