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

      bool _is_LTO_ = false; //!< Low Threshold Only flag
      bool _is_LT_  = false; //!< Low Threshold flag
      bool _is_HT_  = false; //!< High Threshold flag
      uint32_t _LT_CT_25_;   //!< Low Threshold Clocktick 25ns
      uint32_t _HT_CT_25_;   //!< High Threshold Clocktick 25ns

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
