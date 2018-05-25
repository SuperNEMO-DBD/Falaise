/// \file falaise/snemo/datamodels/sim_tracker_digi_hit.h
/* Author(s) :    François Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2016-10-11
 * Last modified: 2016-10-11
 *
 * Description: The SuperNEMO simulated tracker digitized hit
 */

#ifndef FALAISE_SNEMO_DATAMODELS_SIM_TRACKER_DIGI_HIT_H
#define FALAISE_SNEMO_DATAMODELS_SIM_TRACKER_DIGI_HIT_H

// Third party:
// - Boost :
#include <boost/serialization/access.hpp>
// Third party:
// - Bayeux/datatools:
#include <datatools/i_serializable.h>
#include <datatools/i_tree_dump.h>
// - Bayeux/geomtools:
#include <bayeux/geomtools/base_hit.h>

namespace snemo {

  namespace datamodel {

    /// \brief Simulated tracker digitized hit.
    class sim_tracker_digi_hit  : public geomtools::base_hit
    {

    public:

      static const int64_t INVALID_TIME   = std::numeric_limits<int64_t>::min();
      static const int64_t OVERFLOW_TIME  = std::numeric_limits<int64_t>::min() + 1;
      static const int64_t UNDERFLOW_TIME = std::numeric_limits<int64_t>::max();

      /// Constructor
      sim_tracker_digi_hit();

      /// Destructor
      virtual ~sim_tracker_digi_hit();

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

      /// Set the anode t0
      void set_anode_R0(const int64_t);

      /// Return the anode t0
      int64_t get_anode_R0() const;

      /// Set the anode t1
      void set_anode_R1(const int64_t);

      /// Return the anode t1
      int64_t get_anode_R1() const;

      /// Set the anode t2
      void set_anode_R2(const int64_t);

      /// Return the anode t2
      int64_t get_anode_R2() const;

      /// Set the anode t3
      void set_anode_R3(const int64_t);

      /// Return the anode t3
      int64_t get_anode_R3() const;

      /// Set the anode t4
      void set_anode_R4(const int64_t);

      /// Return the anode t4
      int64_t get_anode_R4() const;

      /// Set the anode t5
      void set_cathode_R5(const int64_t);

      /// Return the cathode t5
      int64_t get_cathode_R5() const;

      /// Set the cathode t6
      void set_cathode_R6(const int64_t);

      /// Return the cathode t6
      int64_t get_cathode_R6() const;

      /// Smart print
      virtual void tree_dump(std::ostream& a_out = std::clog,
			     const std::string& a_title = "",
			     const std::string& a_indent = "",
			     bool a_inherit = false) const;

    protected:

      void _set_defaults();

    private:

      // Data:
      geomtools::geom_id _elec_id_;    //!< The Electronic ID of the Geiger cell anodic channel
      uint64_t           _trigger_id_; //!< Trigger ID number

      int64_t _anode_R0_   = INVALID_TIME; //!< Anode timestamp register R0
      int64_t _anode_R1_   = INVALID_TIME; //!< Anode timestamp register R1
      int64_t _anode_R2_   = INVALID_TIME; //!< Anode timestamp register R2
      int64_t _anode_R3_   = INVALID_TIME; //!< Anode timestamp register R3
      int64_t _anode_R4_   = INVALID_TIME; //!< Anode timestamp register R4
      int64_t _cathode_R5_ = INVALID_TIME; //!< Cathode timestamp register R5
      int64_t _cathode_R6_ = INVALID_TIME; //!< Cathode timestamp register R6

      DATATOOLS_SERIALIZATION_DECLARATION()
    };

  }  // end of namespace datamodel

}  // end of namespace snemo

#endif  // FALAISE_SNEMO_DATAMODELS_SIM_TRACKER_DIGI_HIT_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
