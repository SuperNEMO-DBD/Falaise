// No clang-format in linkdefs!
// clang-format off
#include "Things2Root.h"

#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;

#pragma link C++ nestedclass;    // added for namespace
#pragma link C++ nestedtypedef;  // added for namespace

#pragma link C++ class std::vector<std::vector<int16_t> >+;
#pragma link C++ class std::vector<std::vector<int64_t> >+;

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
