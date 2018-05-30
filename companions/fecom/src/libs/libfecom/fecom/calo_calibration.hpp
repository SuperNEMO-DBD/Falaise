//! \file fecom/calo_calibration.hpp
//
// Copyright (c) 2016 by François Mauger <mauger@lpccaen.in2p3.fr>

#ifndef FECOM_CALO_CALIBRATION_HPP
#define FECOM_CALO_CALIBRATION_HPP

// Standard library:
#include <string>
#include <map>

// Third party:
// #include <bayeux/datatools/i_tree_dump.h>
#include <bayeux/datatools/logger.h>
#include <bayeux/datatools/i_serializable.h>

// This project:
#include <fecom/calo_constants.hpp>
#include <fecom/calo_channel_id.hpp>
#include <fecom/calo_pedestal_calib.hpp>

namespace fecom {

  /// \brief Calorimeter calibration manager
  struct calo_calibration
		: public datatools::i_serializable
  {

  public:

    /// Default constructor
    calo_calibration();

		/// Load pedestals from a file
    void load_pedestals(const std::string & filename_);

		/// Clear pedestals
    void clear_pedestals();

    /// Internal header data
    struct calo_pedestal_header
    {
      int16_t start_cell    = -1;
      int16_t stop_cell     = -1;
      int16_t start_channel = -1;
      int16_t stop_channel  = -1;
      int16_t board_id = -1;
    };

    /// Internal parsing data
    struct calo_pedestal_parsing_data
    {
      calo_pedestal_header header;
    };

		/// Smart print
    void tree_dump(std::ostream & out_,
                   const std::string & title_ = "",
                   const std::string & indent_ = "",
                   bool inherit_ = false) const;

  private:

    //! Main parsing method
    bool _parse_pedestals_(std::istream & in_,
                           calo_pedestal_parsing_data & parsing_data_);

    //! Header parsing method
    void _parse_pedestals_header_(const std::string & header_line_,
                                  calo_pedestal_header & header_);

    //! Offset per channel parsing method
    void _parse_pedestals_per_channel_(const std::string & offsets_line_,
                                       const int channel_id_,
                                       calo_pedestal_parsing_data & parsing_data_);


  public:

		// Management :
    datatools::logger::priority logging = datatools::logger::PRIO_FATAL; ///< Logger

    std::map<calo_channel_id, calo_pedestal_calib> calo_pedestals; ///< Map between channel and pedestal calibration

		DATATOOLS_SERIALIZATION_DECLARATION()
  };

} // namespace fecom

#endif // FECOM_CALO_CALIBRATION_HPP

// Local Variables: --
// Mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
