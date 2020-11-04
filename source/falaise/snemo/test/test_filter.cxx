// Catch
#include "catch.hpp"

#include "falaise/snemo/processing/filter.h"
#include "falaise/snemo/processing/module.h"

#include "bayeux/dpp/module_manager.h"
#include "bayeux/datatools/multi_properties.h"

#include <memory>

namespace flp = falaise::processing;

// A trivial wrapped filter
class TrivialFilter {
 public:
  TrivialFilter() = default;
  TrivialFilter(falaise::property_set const& p, datatools::service_manager& /*unused*/)
      : yesOrNo_{p.get<bool>("yes_or_no", true)} {}

  bool filter(datatools::things& /*unused*/) { return yesOrNo_; }

 private:
  bool yesOrNo_ = true;
};
FALAISE_REGISTER_FILTER(TrivialFilter)

// Two helper modules to check on branch taken
class YesBranch {
 public:
  YesBranch() = default;
  YesBranch(falaise::property_set const& /*unused*/, datatools::service_manager& /*unused*/) {}

  flp::status process(datatools::things& /*unused*/) { return flp::status::PROCESS_OK; }
};
FALAISE_REGISTER_MODULE(YesBranch)

class NoBranch {
 public:
  NoBranch() = default;
  NoBranch(falaise::property_set const& /*unused*/, datatools::service_manager& /*unused*/) {}

  flp::status process(datatools::things& /*unused*/) { return flp::status::PROCESS_INVALID; }
};
FALAISE_REGISTER_MODULE(NoBranch)

dpp::module_manager makeModuleManager() {
  datatools::multi_properties m("name", "type");
  m.add("YesBranch", "YesBranch", {});
  m.add("NoBranch", "NoBranch", {});
  dpp::module_manager foo;
  foo.load_modules(m);
  foo.initialize_simple();
  return foo;
}

TEST_CASE("Construction and initialization work as expected", "") {
  datatools::service_manager dummyServices{};
  dpp::module_manager foo = makeModuleManager();
  flp::filter<TrivialFilter> mod;
  datatools::properties dummyConfig{};

  SECTION("Empty configuration throws") {
    REQUIRE_THROWS_AS(mod.initialize(dummyConfig, dummyServices, foo.get_modules()),
                      falaise::processing::configuration_error);
  }

  SECTION("No on_accept parameter throws") {
    dummyConfig.store("on_reject", "NoBranch");
    REQUIRE_THROWS_AS(mod.initialize(dummyConfig, dummyServices, foo.get_modules()),
                      falaise::processing::configuration_error);
  }

  SECTION("No on_reject parameter throws") {
    dummyConfig.store("on_accept", "YesBranch");
    REQUIRE_THROWS_AS(mod.initialize(dummyConfig, dummyServices, foo.get_modules()),
                      falaise::processing::configuration_error);
  }

  SECTION("Valid parameter names with bad values throw") {
    dummyConfig.store("on_accept", "YesBranch");
    dummyConfig.store("on_reject", "BadBranch");
    REQUIRE_THROWS_AS(mod.initialize(dummyConfig, dummyServices, foo.get_modules()),
                      falaise::processing::configuration_error);
  }

  SECTION("Valid parameter names with reversed bad values throw") {
    dummyConfig.store("on_accept", "BadBranch");
    dummyConfig.store("on_reject", "NoBranch");
    REQUIRE_THROWS_AS(mod.initialize(dummyConfig, dummyServices, foo.get_modules()),
                      falaise::processing::configuration_error);
  }

  SECTION("Known good parameters and values do not throw") {
    dummyConfig.store("on_accept", "YesBranch");
    dummyConfig.store("on_reject", "NoBranch");
    REQUIRE_NOTHROW(mod.initialize(dummyConfig, dummyServices, foo.get_modules()));
    }
}

TEST_CASE("Branches are chosen correctly", "") {
  datatools::service_manager dummyServices{};
  dpp::module_manager foo = makeModuleManager();
  flp::filter<TrivialFilter> mod;
  datatools::properties dummyConfig{};
  dummyConfig.store("on_accept", "YesBranch");
  dummyConfig.store("on_reject", "NoBranch");

  datatools::things dummyData;

  SECTION("Pass branch chosen correctly") {
    dummyConfig.store("yes_or_no", true);
    REQUIRE_NOTHROW(mod.initialize(dummyConfig, dummyServices, foo.get_modules()));
    REQUIRE(mod.process(dummyData) == flp::status::PROCESS_OK);
  }
  SECTION("Fail branch chosen correctly") {
    dummyConfig.store("yes_or_no", false);
    REQUIRE_NOTHROW(mod.initialize(dummyConfig, dummyServices, foo.get_modules()));
    REQUIRE(mod.process(dummyData) == flp::status::PROCESS_INVALID);
  }
}