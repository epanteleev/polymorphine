#include <ostream>

#include "GlobalConstant.h"
#include "mir/types/NonTrivialType.h"


void GlobalConstant::print_description(std::ostream &os) const {
    os << '$' << m_name << " = constant " << *m_content_type << ' ' << m_value;
}