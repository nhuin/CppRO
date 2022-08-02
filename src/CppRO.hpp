#ifndef CPPRO_HPP
#define CPPRO_HPP

#include <cassert>

#ifndef DISABLE_CPPRO_ASSERT
#define CppRO_ASSERT(x) assert(x)
#else
#define CppRO_ASSERT(x) (void)0
#endif

namespace CppRO {}

#endif
