/// \file falaise/snemo/datamodels/sim_calo_digi_hit.h
/* Author(s) :    François Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2016-10-11
 * Last modified: 2016-10-11
 *
 * Description: The SuperNEMO simulated calo digitized hit
 */

#ifndef FALAISE_SNEMO_DATAMODELS_SIM_CALO_DIGI_HIT_H
#define FALAISE_SNEMO_DATAMODELS_SIM_CALO_DIGI_HIT_H

// Third party:
// - Bayeux/geomtools:
#include <geomtools/base_hit.h>

namespace snemo {

  namespace datamodel {

    /// \brief Simulated calo digitized hit.
    class sim_calo_digi_hit : public geomtools::base_hit
    {

    public:

      /// Constructor
      sim_calo_digi_hit();

      /// Destructor
      virtual ~sim_calo_digi_hit();

      /// Check validity
      bool is_valid() const;

      /// Check if the electronic ID is valid
      bool has_elec_id() const;

      /// Reset the electronic ID
      void invalidate_elec_id();

      /// Get a reference on the non-mutable electronic ID
      const geomtools::geom_id & get_elec_id() const;

      /// Get a reference on the mutable electronic ID
      geomtools::geom_id & grab_elec_id();

      /// Set the electronic ID
      void set_elec_id(const geomtools::geom_id & an_eid_);

      /// Get a reference on the non-mutable electronic ID
      const uint64_t & get_trigger_id() const;

      /// Set the electronic ID
      void set_trigger_id(const uint64_t & a_tid_);

      /// Get a reference on the non-mutable waveform
      const std::vector<int16_t> & get_waveform() const;

      /// Get a reference on the mutable waveform
      std::vector<int16_t> & grab_waveform();

      /// Check if the calo digi hit is Low Threshold Only
      bool is_lto() const;

      /// Set Low Threshold Only flag
      void set_lto(const bool lto_);

      /// Check if the calo digi hit is Low Threshold
      bool is_lt() const;

      /// Set Low Threshold flag
      void set_lt(const bool lt_);

      /// Check if the calo digi hit is High Threshold
      bool is_ht() const;

      /// Set High Threshold flag
      void set_ht(const bool ht_);

      /// Return the LT Clocktick 25
      uint32_t get_lt_ct_25() const;

      /// Set the LT Clocktick 25
      void set_lt_ct_25(const uint32_t lt_ct_25_);

      /// Return the HT Clocktick 25
      uint32_t get_ht_ct_25() const;

      /// Set the HT Clocktick 25
      void set_ht_ct_25(const uint32_t ht_ct_25_);

      /// Return the timestamp in ADC value
      int64_t get_timestamp() const;

      /// Set the timestamp in ADC value
      void set_timestamp(const int64_t timestamp_);

      /// Return the baseline in ADC value
      int16_t get_baseline() const;

      /// Set the baseline in ADC value
      void set_baseline(const int16_t baseline_);

      /// Return the peak in ADC value
      int16_t get_peak() const;

      /// Set the peak in ADC value
      void set_peak(const int16_t peak_);

      /// Return the charge in ADC value
      int16_t get_charge() const;

      /// Set the charge in ADC value
      void set_charge(const int16_t charge_);

      /// Check if the calo digi hit has his charge overflow
      bool is_charge_overflow() const;

      /// Set the charge overflow flag
      void set_charge_overflow(const bool co_);

      /// Return the rising cell in sample value
      uint32_t get_rising_cell() const;

      /// Set the rising cell in sample value
      void set_rising_cell(const uint32_t rising_cell_);

      /// Return the rising offset (1/256 precision)
      uint32_t get_rising_offset() const;

      /// Set the rising offset (1/256 precision)
      void set_rising_offset(const uint32_t rising_offset_);

      /// Return the falling cell in sample value
      uint32_t get_falling_cell() const;

      /// Set the falling cell in sample value
      void set_falling_cell(const uint32_t falling_cell_);

      /// Return the falling offset (1/256 precision)
      uint32_t get_falling_offset() const;


      /// Set the falling offset (1/256 precision)
      void set_falling_offset(const uint32_t falling_offset_);


      /// Smart print
      virtual void tree_dump(std::ostream & a_out = std::clog,
			     const std::string & a_title = "",
			     const std::string & a_indent = "",
			     bool a_inherit = false) const;

    private:

      // Data:
      geomtools::geom_id _elec_id_; //!< The Electronic ID of the calo channel
      uint64_t _trigger_id_; //!< Trigger ID number

      std::vector<int16_t> _waveform_; //<! Waveform composed by N Samples

      bool _is_lto_ = false; //!< Low Threshold Only flag
      bool _is_lt_  = false; //!< Low Threshold flag
      bool _is_ht_  = false; //!< High Threshold flag
      uint32_t _lt_ct_25_;   //!< Low Threshold Clocktick 25ns
      uint32_t _ht_ct_25_;   //!< High Threshold Clocktick 25ns

      int64_t _timestamp_; //!< LT crossing timestamp
      int16_t _baseline_;  //!< Baseline value in ADC
      int16_t _peak_;      //!< Peak value in ADC
      int16_t _charge_;    //!< Charge value in ADC
      bool    _charge_overflow_ = false; //!< Charge overflow flag

      uint32_t _rising_cell_;    //< Index of the rising cell
      uint32_t _rising_offset_;  //< Offset of the rising cell
      uint32_t _falling_cell_;   //< Index of the falling cell
      uint32_t _falling_offset_; //< Offset of the falling cell

      DATATOOLS_SERIALIZATION_DECLARATION()

    };

  }  // end of namespace datamodel

}  // end of namespace snemo

#endif  // FALAISE_SNEMO_DATAMODELS_SIM_CALO_DIGI_HIT_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
