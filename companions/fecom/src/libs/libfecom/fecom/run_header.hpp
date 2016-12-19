//! \file fecom/run_header.hpp
//
// Copyright (c) 2016 by François Mauger <mauger@lpccaen.in2p3.fr>

#ifndef FECOM_RUN_HEADER_HPP
#define FECOM_RUN_HEADER_HPP

// Standard library:
#include <string>
#include <iostream>

namespace fecom {

  /// \brief Run header
  struct run_header
  {
    run_header();

    virtual ~run_header();

    bool is_valid() const;

    void reset();

    virtual void tree_dump(std::ostream & out_,
                           const std::string & title_ = "",
                           const std::string & indent_ = "",
                           bool inherit_ = false) const;

  private:

    void _reset_();

  public:

    std::string software_version;
    double      unix_time;
    std::string date;
    std::string time;
    std::string data_type;

  };

} // namespace fecom

#endif // FECOM_RUN_HEADER_HPP

// Local Variables: --
// Mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
