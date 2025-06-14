#include <stack>

#include "PreorderTraverse.h"
#include "BasicBlock.h"
#include "FunctionData.h"
#include "utility/Error.h"
#include "instruction/TerminateInstruction.h"

void PreorderTraverse::run() {
    std::vector visited(m_data->size(), false);
    std::stack<BasicBlock*> stack;
    stack.push(m_data->first());

    BasicBlock* exit{};

    while (!stack.empty()) {
        auto bb = stack.top();
        stack.pop();
        auto last = bb->last();

        if (visited[bb->id()]) {
            continue;
        }

        if (last.as<Return>() || last.as<ReturnValue>()) {
            exit = bb;
            continue;
        }

        visited[bb->id()] = true;
        m_order.push_back(bb);

        for (auto succ: std::ranges::reverse_view(bb->successors())) {
            stack.push(succ);
        }
    }

    if (exit == nullptr) {
        die("No exit block found in function");
    }

    m_order.push_back(exit);
}
