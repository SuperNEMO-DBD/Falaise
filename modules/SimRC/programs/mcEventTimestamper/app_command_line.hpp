
#ifndef FL_SIMRC_APP_COMMAND_LINE_HPP
#define FL_SIMRC_APP_COMMAND_LINE_HPP

// Standard library:
#include <stdexcept>

// Third Party:
#include <boost/program_options.hpp>

// This project:
#include "app_params.hpp"

namespace FLSimRC {

  void do_version(std::ostream & out_);

  void do_help(std::ostream & out_, const boost::program_options::options_description & od_);

  int do_cldialog(int argc_, char * argv_[], app_params & args_);

  struct cl_help_signal : public std::logic_error
  {
    cl_help_signal();
    virtual ~cl_help_signal();
  };
  
} // namespace FLSimRC

#endif // FL_SIMRC_APP_COMMAND_LINE_HPP

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
