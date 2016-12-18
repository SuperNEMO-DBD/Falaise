// fecom/run_header.cpp

// Ourselves:
#include <fecom/run_header.hpp>

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

    out_ << indent_ << io::inherit_last_tag(inherit_)
         << "Validity    : " << std::boolalpha << is_valid() << std::endl;

    return;
  }

} // namespace fecom
