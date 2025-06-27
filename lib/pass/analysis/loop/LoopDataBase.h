#pragma once

#include <vector>
#include <unordered_map>
#include <unordered_set>

#include "pass/Constrains.h"

template<CodeBlock BB>
class LoopBlock final {
public:
    using iterator = typename std::unordered_set<BB*>::iterator;

    LoopBlock(BB* header, BB* exit, std::unordered_set<BB*>&& body):
        m_header(header),
        m_exit(exit),
        m_body(std::move(body)) {}


    auto begin() const {
        return m_body.begin();
    }

    auto end() const {
        return m_body.end();
    }


private:
    BB* m_header;
    BB* m_exit;
    std::unordered_set<BB*> m_body;
};

template<CodeBlock BB>
class LoopDataBase final: public AnalysisPassResult {
public:
    explicit LoopDataBase(std::unordered_map<BB*, std::vector<LoopBlock<BB>>>&& loops):
        m_loops(std::move(loops)) {}

private:
    std::unordered_map<BB*, std::vector<LoopBlock<BB>>> m_loops;
};