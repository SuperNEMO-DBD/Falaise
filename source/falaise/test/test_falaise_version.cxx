// Catch
#include "catch.hpp"

#include "falaise/version.h"

TEST_CASE("compile/run time versions match", "") {
  REQUIRE(falaise::version::get_major() == FALAISE_VERSION_MAJOR);
  REQUIRE(falaise::version::get_minor() == FALAISE_VERSION_MINOR);
  REQUIRE(falaise::version::get_patch() == FALAISE_VERSION_PATCH);
}

TEST_CASE("compile/run time git state matches", "") {
  #define TFV_AS_STR(var) #var
  #define XTFV_AS_STR(val) TFV_AS_STR(val)
  REQUIRE(falaise::version::get_commit() == XTFV_AS_STR(FALAISE_VERSION_COMMIT));

#ifdef FALAISE_VERSION_IS_DIRTY
  bool expectedDirty{true};
#else
  bool expectedDirty{false};
#endif
  REQUIRE(falaise::version::is_dirty() == expectedDirty);
}
