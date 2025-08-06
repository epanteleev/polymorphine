#pragma once

class Group final {
public:
    explicit Group(LiveInterval&& interval, std::vector<LIRVal>&& values, const std::optional<aasm::GPReg> &fixed_register) noexcept:
        m_interval(std::move(interval)),
        m_values(std::move(values)),
        m_fixed_register(fixed_register) {}

    const LiveInterval& interval() const noexcept {
        return m_interval;
    }

    std::span<const LIRVal> members() const noexcept {
        return m_values;
    }

    const std::optional<aasm::GPReg>& fixed_register() const noexcept {
        return m_fixed_register;
    }

    void add_member(const LIRVal& val, const LiveInterval& other) {
        m_values.push_back(val);
        m_interval.merge_with(other);
    }

    LiveInterval m_interval;
    std::vector<LIRVal> m_values;
    std::optional<aasm::GPReg> m_fixed_register;
};

class LiveIntervalsGroups final: public AnalysisPassResult {
public:
    using group_iterator = std::deque<Group>::iterator;
    using const_group_iterator = std::deque<Group>::const_iterator;

    explicit LiveIntervalsGroups(std::deque<Group>&& groups, LIRValMap<group_iterator>&& group_mapping) noexcept:
        m_groups(std::move(groups)),
        m_group_mapping(std::move(group_mapping)) {}


    std::optional<Group*> try_get_group(const LIRVal& vreg) const {
        if (const auto it = m_group_mapping.find(vreg); it != m_group_mapping.end()) {
            return std::make_optional(&*it->second);
        }

        return std::nullopt;
    }

    const_group_iterator begin() const {
        return m_groups.begin();
    }

    const_group_iterator end() const {
        return m_groups.end();
    }

    friend std::ostream& operator<<(std::ostream& os, const LiveIntervalsGroups& groups);

private:
    std::deque<Group> m_groups;
    LIRValMap<group_iterator> m_group_mapping;
};

inline std::ostream & operator<<(std::ostream &os, const LiveIntervalsGroups &groups) {
    for (const auto& group: groups.m_groups) {
        os << "Group: " << group.interval() << std::endl << " Values: ";
        for (const auto& val: group.members()) {
            os << val << " ";
        }
        const auto& fixed_register = group.fixed_register();
        if (fixed_register.has_value()) {
            os << "Fixed Register: " << fixed_register.value();
        }
        os << "\n";
    }

    return os;
}
