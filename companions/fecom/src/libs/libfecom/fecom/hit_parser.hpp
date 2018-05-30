//! \file fecom/hit_parser.hpp
//! \brief Parser for a hit (calorimeter or tracker)
//
// Copyright (c) 2016 by François Mauger <mauger@lpccaen.in2p3.fr>

#ifndef FECOM_HIT_PARSER_HPP
#define FECOM_HIT_PARSER_HPP

// Standard library:
#include <string>
#include <fstream>

// Third party:
// - Bayeux:
#include <bayeux/datatools/logger.h>

// This project:
#include <fecom/tracker_channel_hit_parser.hpp>
#include <fecom/calo_hit_parser.hpp>

namespace fecom {

  //! \brief Commissioning hit parser
  class hit_parser
  {
  public:

    //! Number of header lines
    static const std::size_t NB_HIT_HEADER_LINES = 1;

    //! Constructor
    hit_parser();

    //! Parse
    bool parse(std::istream & in_,
							 calo_hit & a_calo_hit_,
							 tracker_channel_hit & a_tracker_channel_hit_);

  private:

    void _parse_hit_header_(const std::string & header_line_,
														const int index);

  public:

    // Management:
    datatools::logger::priority logging = datatools::logger::PRIO_FATAL;

    // Config :
    calo_hit_parser    _calo_hit_parser_;    //!< Calo hit parser
    tracker_channel_hit_parser _tracker_channel_hit_parser_; //!< Tracker hit parser

    // Working :
    uint64_t               _hit_id_;        //!< Actual hit ID
    base_hit::hitmode_type _hit_mode_type_; //!< Actual hit mode
    uint64_t               _trigger_id_;    //!< Actual trigger ID

  };

} // namespace fecom

#endif // FECOM_HIT_PARSER_HPP

// Local Variables: --
// Mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
