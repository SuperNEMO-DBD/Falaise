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
		/// Default constructor
		run_header();

		/// Destructor
    virtual ~run_header();

		/// Check if the run header is valid
    bool is_valid() const;

		/// Reset
    void reset();

		/// Smart print
    virtual void tree_dump(std::ostream & out_,
                           const std::string & title_ = "",
                           const std::string & indent_ = "",
                           bool inherit_ = false) const;

  private:

    void _reset_();

  public:

    std::string software_version; ///< The parsed software version
    double      unix_time;        ///< The parsed unix time
    std::string date;             ///< The parsed date
    std::string time;             ///< The parsed time
    std::string data_type;        ///< The parsed data type

  };

} // namespace fecom

#endif // FECOM_RUN_HEADER_HPP

// Local Variables: --
// Mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
