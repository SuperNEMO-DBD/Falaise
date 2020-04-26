/// \file mctools/biasing_manager.cc

// Ourselves:
#include <snsim/biasing_manager.h>

// - Bayeux/datatools :
#include <datatools/exception.h>
#include <datatools/properties.h>
#include <datatools/utils.h>

// This project:
#include <snsim/detector_construction.h>

namespace snsim {

biasing_manager::biasing_manager() { _set_default(); }

biasing_manager::~biasing_manager() = default;

void biasing_manager::set_detector_construction(detector_construction& dc_) {
  DT_THROW_IF(is_initialized(), std::logic_error, "Biasing manager is locked!");
  _dc_ = &dc_;
}

void biasing_manager::_set_default() {
  _initialized_ = false;
  _dc_ = nullptr;
}

auto biasing_manager::is_initialized() const -> bool { return _initialized_; }

void biasing_manager::initialize(const datatools::properties& /*config_*/) {
  DT_THROW_IF(is_initialized(), std::logic_error, "Biasing manager is already initialized!");

  // Do something

  _initialized_ = true;
}

// Reset
void biasing_manager::reset() {
  DT_THROW_IF(!is_initialized(), std::logic_error, "Biasing manager is not initialized!");
  _initialized_ = false;
}

}  // namespace snsim
