#include "Label.h"
#include <ostream>

namespace aasm {
    std::ostream & operator<<(std::ostream &os, const Label &label) {
        return os << 'L' << label.id()+1;
    }
}