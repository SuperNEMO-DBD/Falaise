//! \file fecom/calo_hit_parser.hpp
//! \brief Parser for a calo hit
//
// Copyright (c) 2016 by François Mauger <mauger@lpccaen.in2p3.fr>

#ifndef FECOM_CALO_HIT_PARSER_HPP
#define FECOM_CALO_HIT_PARSER_HPP

// Standard library:
#include <string>
#include <fstream>

// Third party:
// - Bayeux:
#include <bayeux/datatools/logger.h>

// This project:
#include <fecom/calo_hit.hpp>

namespace fecom {

  //! \brief Commissioning calo hit parser
  class calo_hit_parser
  {
  public:

    //! Number of header lines
    static const std::size_t NB_CALO_HEADER_LINES = 1;

    //! Constructor
    calo_hit_parser();

    //! Parse
    bool parse(std::istream & in_, calo_hit & hit_);

  private:

    void _parse_header_(const std::string & header_line_,
                        const int index_,
                        calo_hit & hit_);

    void _parse_samples_(const std::string & data_line_,
                         calo_hit & hit_);

  public:

    // Management:
    datatools::logger::priority logging = datatools::logger::PRIO_DEBUG;

  };

} // namespace fecom

#endif // FECOM_CALO_HIT_PARSER_HPP

// Local Variables: --
// Mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
