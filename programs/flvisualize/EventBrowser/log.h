// -*- mode: c++ ; -*-

#ifndef FALAISE_SNEMO_VISUALIZATION_LOG_H
#define FALAISE_SNEMO_VISUALIZATION_LOG_H 1
// Standard Library:
#include <sstream>
#include <iostream>

// Third Party:
// - Boost:
#include <boost/current_function.hpp>

#define FL_LOG_DEVEL(Message)						\
  {                                                                     \
    std::ostringstream _fl_xxx_message;					\
    _fl_xxx_message << Message;						\
    std::ostringstream _fl_xxx_out;					\
    _fl_xxx_out << "[devel:" << BOOST_CURRENT_FUNCTION << ":" << __LINE__ << "] "; \
    _fl_xxx_out << _fl_xxx_message.str();				\
    std::cerr << _fl_xxx_out.str() << std::endl;			\
  }

#endif  // FALAISE_SNEMO_VISUALIZATION_LOG_H
