/// \file mctools/biasing_manager.cc

// Ourselves:
#include <mctools/g4/biasing_manager.h>

// - Bayeux/datatools :
#include <datatools/properties.h>
#include <datatools/utils.h>
#include <datatools/exception.h>

// This project:
#include <mctools/g4/detector_construction.h>

namespace mctools {

  namespace g4 {

    biasing_manager::biasing_manager()
    {
      _set_default();
   }

    biasing_manager::~biasing_manager()
    = default;

    void biasing_manager::set_detector_construction(detector_construction & dc_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error,
                  "Biasing manager is locked!");
      _dc_ = &dc_;
   }

    void biasing_manager::_set_default()
    {
      _initialized_ = false;
      _dc_ = nullptr;
   }

    auto biasing_manager::is_initialized() const -> bool
    {
      return _initialized_;
    }

    void biasing_manager::initialize(const datatools::properties & /*config_*/)
    {
      DT_THROW_IF(is_initialized(), std::logic_error,
                  "Biasing manager is already initialized!");

      // Do something

      _initialized_ = true;
   }

    // Reset
    void biasing_manager::reset()
    {
      DT_THROW_IF(!is_initialized(), std::logic_error,
                  "Biasing manager is not initialized!");
      _initialized_ = false;
   }

  } // end of namespace g4

} // end of namespace mctools
