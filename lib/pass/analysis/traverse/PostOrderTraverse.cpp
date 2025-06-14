
#include "PostOrderTraverse.h"

#include <algorithm>

void PostOrderTraverse::run() {
    std::ranges::reverse_copy(m_preorder, std::begin(m_order));
}

PostOrderTraverse PostOrderTraverse::create(AnalysisPassCache* cache, const FunctionData *data) {
    auto preorder = cache->concurrent_analyze<PreorderTraverse>(data);
    preorder.wait();
    return PostOrderTraverse(data, *preorder.get());
}
