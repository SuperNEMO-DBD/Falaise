//! \file fecom/calo_hit.hpp
//
// Copyright (c) 2016 by François Mauger <mauger@lpccaen.in2p3.fr>

#ifndef FECOM_CALO_HIT_HPP
#define FECOM_CALO_HIT_HPP

// Standard library:
#include <string>
#include <iostream>

// - Bayeux:
// - Bayeux/datatools:
#include <bayeux/datatools/utils.h>

// This project:
#include <fecom/base_hit.hpp>
#include <fecom/calo_constants.hpp>

namespace fecom {

	//! \brief Commissioning calorimeter hit, inherit from base hit
	struct calo_hit
    : public base_hit
  {
		/// Default constructor
    calo_hit();

		/// Destructor
    virtual ~calo_hit();

		/// Check if the calo hit is valid
		virtual bool is_valid() const;

		///
		virtual double get_timestamp() const;

		/// Reset
    virtual void reset();

		/// Smart print
    virtual void tree_dump(std::ostream & out_,
                           const std::string & title_ = "",
                           const std::string & indent_ = "",
                           bool inherit_ = false) const;

		/// Get a raw sample
    int16_t get_raw_sample(const uint16_t cell_) const;

		/// Set a raw sample
    void set_raw_sample(const uint16_t cell_, const int16_t);

		/// Print the waveform
    void print_waveform(std::ostream & out_, const uint32_t flags_ = 0) const;

  private:

    void _reset_();

    void _reset_raw_waveform_data_();

  public:

    uint8_t  event_id;                  ///< Debug counter (0..255)
    uint64_t raw_tdc;                   /** Raw timestamp (dynamic 40 bits)
                                         *  Unit: clock period (default: 160 MHz)
                                         */
		double   tdc_cell_step_ns;
		double   tdc_ns;                    ///< Timestampi in ns
    bool     low_threshold;             ///< Low threshold flag
    bool     high_threshold;            ///< High threshold flag
    uint32_t low_threshold_trig_count;  ///< (16 bits)
    int32_t  low_threshold_time_count;  ///< (24 bits)
    uint16_t waveform_data_size;        ///< Number of raw samples (64..1024 % 16)
    uint16_t fcr;                       ///< First cell read (index of the first sample) (0..1023)
    int16_t  raw_waveform_data[calo_constants::MAX_NUMBER_OF_SAMPLES]; ///< Raw samples
    int16_t  raw_baseline;              ///< Raw baseline
		double   baseline_volt;             ///< Baseline in volt
    int16_t  raw_peak;                  ///< ADC amplitude of the peak
		double   peak_volt;                 ///< ADC amplitude of the peak in volt
    int32_t  raw_charge;                ///< Raw integrated charge
		double   charge_picocoulomb;        ///< Integrated charge in pC
    bool     raw_charge_overflow;       ///< Raw charge overflow flag
		uint32_t rising_cell;               ///< Index of the rising cell
		uint32_t rising_offset;             ///< Offset of the rising cell
		double   rising_time_ns;            ///< Rising time in ns
		uint32_t falling_cell;              ///< Index of the falling cell
		uint32_t falling_offset;            ///< Offset of the falling cell
		double   falling_time_ns;           ///< Falling time in ns

		DATATOOLS_SERIALIZATION_DECLARATION()

    friend class calo_hit_parser;
  };

} // namespace fecom

#endif // FECOM_CALO_HIT_HPP

// Local Variables: --
// Mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
