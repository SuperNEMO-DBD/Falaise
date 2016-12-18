//! \file wctools/run_header.hpp
//! \brief Wavecatcher run header
//
// Copyright (c) 2016 by François Mauger <mauger@lpccaen.in2p3.fr>

#ifndef WCTOOLS_RUN_HEADER_HPP
#define WCTOOLS_RUN_HEADER_HPP

// Standard library:
#include <string>

namespace wctools {

  //! \brief File header
  struct run_header
  {
    static const std::size_t NBLINES = 4;
    std::string sw_version;      //!< Software version
    int         wc_system_type;  //!< WaveCatcher system type
    std::size_t nb_channels;     //!< WaveCatcher number of channels
    double      gain;            //!< Gain
    std::size_t nb_data_samples; //!< Number of data samples
    std::size_t nb_acq_channels; //!< Number of acquired channels
    std::string adc_unit;        //!< ADC unit
    double      sampling_period; //!< Sampling period
    std::string tdc_unit;        //!< TDC unit
    int         inl_correction;  //!< INL correction
  };

} // namespace wctools

#endif // WCTOOLS_RUN_HEADER_HPP

// Local Variables: --
// Mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
