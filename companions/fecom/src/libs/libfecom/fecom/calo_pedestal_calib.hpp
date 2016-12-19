//! \file fecom/calo_pedestal_calib.hpp
//
// Copyright (c) 2016 by François Mauger <mauger@lpccaen.in2p3.fr>

#ifndef FECOM_CALO_PEDESTAL_CALIB_HPP
#define FECOM_CALO_PEDESTAL_CALIB_HPP

// Standard library:
#include <string>
#include <iostream>

// This project:
#include <fecom/calo_constants.hpp>

namespace fecom {

  /// \brief Calorimeter pedestal calibration data
  struct calo_pedestal_calib
  {

  public:

    /// Default constructor
    calo_pedestal_calib();

    /// Check validity
    bool is_valid() const;

    /// Reset
    void reset();

    /// Smart print
    virtual void tree_dump(std::ostream & out_,
                           const std::string & title_ = "",
                           const std::string & indent_ = "",
                           bool inherit_ = false) const;

    /// Return the offset associated to a cell
    int32_t get_offset(const uint16_t cell_) const;

    /// Set the offset associated to a cell in unit of 1/16 ADC unit
    void set_offset(const uint16_t cell_, const int32_t);

    /// Return the offset associated to a cell
    double foffset(const uint16_t cell_) const;

    /// Print in column mode
    void print_offsets(std::ostream & out_, const uint32_t flags_ = 0) const;

  private:

    void _reset_();

  public:

    uint16_t slot_index;  ///< Board index
    uint8_t  channel;     ///< Channel (0..15)
    uint16_t offset_size; ///< Number of raw samples (64..1024 % 16)
    int32_t  offset[calo_constants::MAX_NUMBER_OF_SAMPLES]; ///< Offset per cell

  };

} // namespace fecom

#endif // FECOM_CALO_PEDESTAL_CALIB_HPP

// Local Variables: --
// Mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
