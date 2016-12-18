//! \file fecom/calo_hit_parser.hpp
//! \brief Parser for a calo hit
//
// Copyright (c) 2016 by François Mauger <mauger@lpccaen.in2p3.fr>

#ifndef FECOM_CALO_HIT_PARSER_HPP
#define FECOM_CALO_HIT_PARSER_HPP

namespace fecom {

  //! \brief Commissioning calo hit parser
  class calo_hit_parser
  {
  public:

    //! Number of header lines
    static const std::size_t NB_HEADER_LINES = 2;

    //! Constructor
    calo_hit_parser();

    //! Parse
    bool parse(std::istream & in_, calo_hit & hit_);

  private:

    void _parse_header(const std::string & header_line_, int index_);

    void _parse_samples(const std::string & data_line_);

  };

} // namespace fecom

#endif // FECOM_CALO_HIT_PARSER_HPP

// Local Variables: --
// Mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
