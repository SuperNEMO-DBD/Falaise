/// \file mctools/g4/loggable_support.h
/* Author(s)     : Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date : 2013-06-03
 * Last modified : 2013-06-03
 */

#ifndef MCTOOLS_G4_LOGGABLE_SUPPORT_H
#define MCTOOLS_G4_LOGGABLE_SUPPORT_H 1

// Third party:
// - Bayeux/datatools :
#include <datatools/logger.h>

namespace datatools {
class properties;
}

namespace mctools {

namespace g4 {

/// \brief Base class with logging support
class loggable_support {
 public:
  /// Default constructor
  loggable_support();

  /// Constructor from logging priority level
  explicit loggable_support(datatools::logger::priority);

  datatools::logger::priority get_logging_priority() const;

  /// Destructor
  virtual ~loggable_support();

  void set_logging_priority(const std::string &priority_label_);

  void set_logging_priority(datatools::logger::priority);

  bool is_debug() const;

  void set_debug(bool);

  bool is_verbose() const;

  void set_verbose(bool);

 protected:
  void _initialize_logging_support(const datatools::properties &);

  datatools::logger::priority _logprio() const;

 protected:
  datatools::logger::priority _logging_priority;  //!< Logging priority threshold
};

}  // end of namespace g4

}  // end of namespace mctools

#endif  // MCTOOLS_G4_LOGGABLE_SUPPORT_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// End: --
