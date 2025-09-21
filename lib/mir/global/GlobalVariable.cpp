#include "mir/global/GlobalVariable.h"
#include "mir/types/NonTrivialType.h"

#include <ostream>

static_assert(!std::is_polymorphic_v<GlobalVariable>, "should be");

void GlobalVariable::print_description(std::ostream &os) const {
    os << '$' << m_name << " = global " << *m_content_type << ' ' << m_value;
}