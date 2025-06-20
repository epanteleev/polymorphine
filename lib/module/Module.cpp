#include "module/Module.h"


void Module::print(std::ostream &os) const {
    for (auto& f : m_functions) {
        f.print(os);
    }
}
