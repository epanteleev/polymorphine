#pragma once

class Group final {
public:
    explicit Group(LiveInterval&& interval, std::vector<LIRVal>&& values, const std::optional<GPVReg> &fixed_register) noexcept:
        m_interval(std::move(interval)),
        m_values(std::move(values)),
        m_fixed_register(fixed_register) {}

    void merge_interval(const LiveInterval& other) {
        m_interval.merge_with(other);
    }

    LiveInterval m_interval;
    std::vector<LIRVal> m_values;
    std::optional<GPVReg> m_fixed_register;
};

class LiveIntervalsGroups final: public AnalysisPassResult {
public:
    using group_iterator = std::deque<Group>::iterator;

    explicit LiveIntervalsGroups(std::deque<Group>&& groups, LIRValMap<group_iterator>&& group_mapping) noexcept:
        m_groups(std::move(groups)),
        m_group_mapping(std::move(group_mapping)) {}


    std::optional<Group*> try_get_group(const LIRVal& vreg) const {
        if (const auto it = m_group_mapping.find(vreg); it != m_group_mapping.end()) {
            return std::make_optional(&*it->second);
        }

        return std::nullopt;
    }

    std::optional<GPVReg> try_get_fixed_register(const LIRVal& vreg) const {
        if (const auto it = m_group_mapping.find(vreg); it != m_group_mapping.end()) {
            return it->second->m_fixed_register;
        }

        return std::nullopt;
    }

    std::optional<const LiveInterval*> try_get_group_interval(const LIRVal& vreg) const {
        if (const auto it = m_group_mapping.find(vreg); it != m_group_mapping.end()) {
            return &it->second->m_interval;
        }

        return std::nullopt;
    }

    friend std::ostream& operator<<(std::ostream& os, const LiveIntervalsGroups& groups);

private:
    std::deque<Group> m_groups;
    LIRValMap<group_iterator> m_group_mapping;
};

inline std::ostream & operator<<(std::ostream &os, const LiveIntervalsGroups &groups) {
    for (const auto& group: groups.m_groups) {
        os << "Group: " << group.m_interval << std::endl << " Values: ";
        for (const auto& val: group.m_values) {
            os << val << " ";
        }
        if (group.m_fixed_register.has_value()) {
            os << "Fixed Register: " << group.m_fixed_register.value();
        }
        os << "\n";
    }

    return os;
}
