// loggable_support.cc

// Ourselves:
#include <mctools/g4/loggable_support.h>

// Standard library:
#include <string>

// Third party:
// - Bayeux/datatools:
#include <datatools/properties.h>
#include <datatools/exception.h>

namespace mctools {

  namespace g4 {

    loggable_support::loggable_support()
    {
      _logging_priority = datatools::logger::PRIO_FATAL;
    }

    loggable_support::~loggable_support()
    = default;

    loggable_support::loggable_support(datatools::logger::priority p)
    {
      _logging_priority = p;
    }

    auto loggable_support::_logprio() const -> datatools::logger::priority
    {
      return _logging_priority;
    }

    auto loggable_support::get_logging_priority() const -> datatools::logger::priority
    {
      return _logging_priority;
    }

    void loggable_support::set_debug (bool debug_)
    {
      if (debug_) {
        if (_logging_priority < datatools::logger::PRIO_DEBUG) {
          set_logging_priority(datatools::logger::PRIO_DEBUG);
        }
      } else {
        set_logging_priority(datatools::logger::PRIO_FATAL);
      }
    }

    auto loggable_support::is_debug () const -> bool
    {
      return _logging_priority >= datatools::logger::PRIO_DEBUG;
    }

    void loggable_support::set_verbose (bool verbose_)
    {
      if (verbose_) {
        if (_logging_priority < datatools::logger::PRIO_NOTICE) {
          set_logging_priority(datatools::logger::PRIO_NOTICE);
        }
      } else {
        set_logging_priority(datatools::logger::PRIO_FATAL);
      }
    }

    auto loggable_support::is_verbose () const -> bool
    {
      return _logging_priority >= datatools::logger::PRIO_NOTICE;
    }

    void loggable_support::set_logging_priority(const std::string & priority_label_)
    {
      datatools::logger::priority p = datatools::logger::get_priority(priority_label_);
      DT_THROW_IF(p == datatools::logger::PRIO_UNDEFINED,
                  std::logic_error,
                  "Invalid logging priority label '" << priority_label_ << "' !");
      set_logging_priority(p);
    }

    void loggable_support::set_logging_priority(datatools::logger::priority p)
    {
      _logging_priority = p;
    }

    void loggable_support::_initialize_logging_support(const datatools::properties & config_)
    {
      set_logging_priority(datatools::logger::extract_logging_configuration(config_,
                                                                            datatools::logger::PRIO_FATAL));
   }

  } // end of namespace g4

} // end of namespace mctools
