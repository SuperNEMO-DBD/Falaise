//! \file fecom/tracker_hit_parser.hpp
//! \brief Parser for a tracker hit
//
// Copyright (c) 2016 by François Mauger <mauger@lpccaen.in2p3.fr>
// Copyright (c) 2016 by Guillaume Oliviéro <goliviero@lpccaen.in2p3.fr>

#ifndef FECOM_TRACKER_HIT_PARSER_HPP
#define FECOM_TRACKER_HIT_PARSER_HPP

// Standard library:
#include <string>
#include <fstream>

// Third party:
// - Bayeux:
#include <bayeux/datatools/logger.h>

// This project:
#include <fecom/tracker_hit.hpp>

namespace fecom {

  //! \brief Commissioning tracker hit parser
  class tracker_hit_parser
  {
  public:

    //! Number of header lines
    static const std::size_t NB_TRACKER_HEADER_LINES = 1;

    //! Constructor
    tracker_hit_parser();

    //! Parse
    bool parse(std::istream & in_, tracker_hit & hit_);

  private:

    void _parse_header_(const std::string & header_line_,
                        const int index_,
                        tracker_hit & hit_);

    void _parse_time_(const std::string & data_line_,
		      tracker_hit & hit_);

  public:

    // Management:
    datatools::logger::priority logging = datatools::logger::PRIO_FATAL;

  };

} // namespace fecom

#endif // FECOM_TRACKER_HIT_PARSER_HPP

// Local Variables: --
// Mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
