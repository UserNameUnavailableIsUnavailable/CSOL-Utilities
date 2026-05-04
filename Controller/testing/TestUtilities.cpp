#include "TestUtilities.hpp"

#define WIDEN_IMPL(x) L##x
#define WIDEN(x) WIDEN_IMPL(x)

std::filesystem::path GetSourceDirectory()
{
#ifndef SOURCE_DIR
std::filesystem::path p(__FILEW__);
return p.parent_path().parent_path();
#else
return WIDEN(SOURCE_DIR);
#endif
}