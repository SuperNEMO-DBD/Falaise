//! \file fecom/tracker_channel_hit_parser.hpp
//! \brief Parser for a tracker hit
//
// Copyright (c) 2016 by François Mauger <mauger@lpccaen.in2p3.fr>
// Copyright (c) 2016 by Guillaume Oliviéro <goliviero@lpccaen.in2p3.fr>

#ifndef FECOM_TRACKER_CHANNEL_HIT_PARSER_HPP
#define FECOM_TRACKER_CHANNEL_HIT_PARSER_HPP

// Standard library:
#include <fstream>
#include <string>

// Third party:
// - Bayeux:
#include <bayeux/datatools/logger.h>

// This project:
#include <fecom/tracker_channel_hit.hpp>
#include <fecom/tracker_hit.hpp>

namespace fecom {

//! \brief Commissioning tracker hit parser
class tracker_channel_hit_parser {
 public:
  //! Number of header lines
  static const std::size_t NB_TRACKER_HEADER_LINES = 0;

  //! Constructor
  tracker_channel_hit_parser();

  //! Parse
  bool parse(std::istream& in_, tracker_channel_hit& hit_);

 private:
  // void _parse_header_(const std::string & header_line_,
  //                     const int index_,
  //                     tracker_channel_hit & hit_);

  void _parse_timestamp_(const std::string& data_line_, tracker_channel_hit& hit_);

 public:
  // Management:
  datatools::logger::priority logging = datatools::logger::PRIO_FATAL;
};

}  // namespace fecom

#endif  // FECOM_TRACKER_CHANNEL_HIT_PARSER_HPP

// Local Variables: --
// Mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
