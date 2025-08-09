#include "Select.h"

void Select::visit(Visitor &visitor) {
    visitor.accept(this);
}
