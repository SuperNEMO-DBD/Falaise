// Catch
#include "catch.hpp"

#include "falaise/snemo/processing/module.h"

#include "bayeux/dpp/module_manager.h"

namespace flp = falaise::processing;


// A trivial wrapped module
class TrivialModule {
 public:
  TrivialModule() = default;
  TrivialModule(std::string const&, datatools::properties const&, datatools::service_manager&) : TrivialModule() {}

  flp::status process(datatools::things&) {
    return flp::status::PROCESS_OK;
  }
};
FALAISE_REGISTER_MODULE(TrivialModule, "TrivialModule")

// A module with a config class
class FooConfig {
 public:
  FooConfig() = default;
  FooConfig(datatools::properties const& p) { p.tree_dump(std::cout); }
};

class FooModule {
 public:
  FooModule() = default;
  FooModule(std::string const&, FooConfig const&, datatools::service_manager&) : FooModule() {}

  flp::status process(datatools::things&) {
    return flp::status::PROCESS_OK;
  }
};

FALAISE_REGISTER_MODULE(FooModule, "FooModule")


TEST_CASE("Construction and initialization work", "") {
  flp::module<TrivialModule> mod;

  datatools::properties dummyConfig{};
  datatools::service_manager dummyServices{};
  dpp::module_handle_dict_type dummyWhatever{};

  mod.initialize(dummyConfig, dummyServices, dummyWhatever);
}

TEST_CASE("Configuration objects work", "") {
  flp::module<FooModule> mod;

  datatools::properties dummyConfig{};
  datatools::service_manager dummyServices{};
  dpp::module_handle_dict_type dummyWhatever{};

  mod.initialize(dummyConfig, dummyServices, dummyWhatever);
}


