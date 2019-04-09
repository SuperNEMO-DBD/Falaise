// Catch
#include "catch.hpp"

#include "falaise/snemo/processing/module.h"

#include "bayeux/dpp/module_manager.h"

#include <memory>

namespace flp = falaise::processing;

// A trivial wrapped module
class TrivialModule {
 public:
  TrivialModule() = default;
  TrivialModule(datatools::properties const&, datatools::service_manager&) : TrivialModule() {}

  flp::status process(datatools::things&) { return flp::status::PROCESS_OK; }
};
FALAISE_REGISTER_MODULE(TrivialModule)

// A module with a config class
class FooConfig {
 public:
  FooConfig() = default;
  FooConfig(datatools::properties const& p) { p.tree_dump(std::cout); }
};

class FooModule {
 public:
  FooModule() = default;
  FooModule(FooConfig const&, datatools::service_manager&) : FooModule() {}

  flp::status process(datatools::things&) { return flp::status::PROCESS_OK; }
};

FALAISE_REGISTER_MODULE(FooModule)


TEST_CASE("Construction and initialization work", "") {
  flp::module<TrivialModule> mod;

  datatools::properties dummyConfig{};
  datatools::service_manager dummyServices{};
  dpp::module_handle_dict_type dummyWhatever{};

  REQUIRE_NOTHROW(mod.initialize(dummyConfig, dummyServices, dummyWhatever));
}

TEST_CASE("Configuration objects work", "") {
  flp::module<FooModule> mod;

  datatools::properties dummyConfig{};
  datatools::service_manager dummyServices{};
  dpp::module_handle_dict_type dummyWhatever{};

  REQUIRE_NOTHROW(mod.initialize(dummyConfig, dummyServices, dummyWhatever));
}

TEST_CASE("Use of reserved keys throws", "") {
  flp::module<TrivialModule> mod;
  datatools::properties badConfig{};
  datatools::service_manager dummyServices{};
  dpp::module_handle_dict_type dummyWhatever{};

  SECTION("Supplying module_label in config throws") {
    badConfig.store("module_label", "not allowed!!");
    REQUIRE_THROWS_AS(mod.initialize(badConfig, dummyServices, dummyWhatever),
                      falaise::processing::reserved_key_error);
  }

  SECTION("Supplying module_type in config throws") {
    badConfig.store("module_type", "not allowed!!");
    REQUIRE_THROWS_AS(mod.initialize(badConfig, dummyServices, dummyWhatever),
                      falaise::processing::reserved_key_error);
  }
}
