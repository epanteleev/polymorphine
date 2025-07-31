#pragma once

class Group final {
public:
    explicit Group(LiveInterval&& interval, std::vector<LIRVal>&& values) noexcept:
        m_interval(std::move(interval)),
        m_values(std::move(values)) {}

    void merge_interval(const LiveInterval& other) {
        m_interval.merge_with(other);
    }

    LiveInterval m_interval;
    std::vector<LIRVal> m_values;
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

private:
    std::deque<Group> m_groups;
    LIRValMap<group_iterator> m_group_mapping;
};