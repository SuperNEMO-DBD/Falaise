// fecom/run_header.cpp

// Ourselves:
#include <fecom/run_header.hpp>

// Third party:
// - Bayeux:
#include <bayeux/datatools/utils.h>

// This project:
#include <fecom/utils.hpp>

namespace fecom {

  run_header::run_header()
  {
    _reset_();
    return;
  }

  run_header::~run_header()
  {
    return;
  }

  bool run_header::is_valid() const
  {
    if (software_version.empty()) return false;
    if (!datatools::is_valid(unix_time)) return false;
    if (date.empty()) return false;
    if (time.empty()) return false;
    if (data_type.empty()) return false;
    return true;
  }

  void run_header::reset()
  {
    _reset_();
    return;
  }

  void run_header::_reset_()
  {
    return;
  }

  void run_header::tree_dump(std::ostream & out_,
                                      const std::string & title_,
                                      const std::string & indent_,
                                      bool inherit_) const
  {
    if (!title_.empty()) {
      out_ << indent_ << title_ << std::endl;
    }

    out_ << indent_ << io::tag()
         << "Software version : '" << software_version << "'" << std::endl;

    out_ << indent_ << io::tag()
         << "Unix time : [" << unix_time << "]" << std::endl;

    out_ << indent_ << io::tag()
         << "Date : [" << date << "]" << std::endl;

    out_ << indent_ << io::tag()
         << "Time : [" << time << "]" << std::endl;

    out_ << indent_ << io::tag()
         << "Data type : '" << data_type << "'" << std::endl;

    out_ << indent_ << io::inherit_last_tag(inherit_)
         << "Validity  : " << std::boolalpha << is_valid() << std::endl;

    return;
  }

} // namespace fecom
