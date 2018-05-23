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
// - Bayeux/datatools:
#include <bayeux/datatools/bit_mask.h>
// - Bayeux/geomtools:
#include <bayeux/geomtools/base_hit.h>

namespace snemo {

  namespace datamodel {

    /// \brief Simulated tracker digitized hit.
    class sim_tracker_digi_hit : public geomtools::base_hit
    {

    public:

      static const int64_t INVALID_TIME   = std::numeric_limits<int64_t>::min();
      static const int64_t OVERFLOW_TIME  = std::numeric_limits<int64_t>::min() + 1;
      static const int64_t UNDERFLOW_TIME = std::numeric_limits<int64_t>::max();

      /// Masks to automatically tag the attributes to be stored
      enum more_store_mask_type
	{
	  STORE_ANODE_R0   = datatools::bit_mask::bit03,  //!< Serialization mask for the anode t0
	  STORE_ANODE_R1   = datatools::bit_mask::bit04,  //!< Serialization mask for the anode t1
	  STORE_ANODE_R2   = datatools::bit_mask::bit05,  //!< Serialization mask for the anode t2
	  STORE_ANODE_R3   = datatools::bit_mask::bit06,  //!< Serialization mask for the anode t3
	  STORE_ANODE_R4   = datatools::bit_mask::bit07,  //!< Serialization mask for the anode t4
	  STORE_CATHODE_R5 = datatools::bit_mask::bit08,  //!< Serialization mask for the cathode t5
	  STORE_CATHODE_R6 = datatools::bit_mask::bit09   //!< Serialization mask for the cathode t6
	};

      /// Constructor
      sim_tracker_digi_hit();

      /// Destructor
      virtual ~sim_tracker_digi_hit();

      /// Check validity
      bool is_valid() const;

      /// Reset
      void reset();

      /// Check if the electronic ID is valid
      bool has_elec_id() const;

      /// Reset the electronic ID
      void invalidate_elec_id();

      /// Get a reference on the non-mutable electronic ID
      const geomtools::geom_id & get_elec_id() const;

      /// Get a reference on the mutable electronic ID
      geomtools::geom_id & grab_elec_id();

      /// Set the electronic ID
      void set_elec_id(const geomtools::geom_id & a_gid);

      /// Set the anode t0
      void set_anode_R0(const int64_t);

      /// Reset the anode t0
      void reset_anode_R0();

      /// Check if anode t0 is available
      bool has_anode_R0() const;

      /// Return the anode t0
      int64_t get_anode_R0() const;

      /// Set the anode t1
      void set_anode_R1(const int64_t);

      /// Reset the anode t1
      void reset_anode_R1();

      /// Check if anode t1 is available
      bool has_anode_R1() const;

      /// Return the anode t1
      int64_t get_anode_R1() const;

      /// Set the anode t2
      void set_anode_R2(const int64_t);

      /// Reset the anode t2
      void reset_anode_R2();

      /// Check if anode t2 is available
      bool has_anode_R2() const;

      /// Return the anode t2
      int64_t get_anode_R2() const;

      /// Set the anode t3
      void set_anode_R3(const int64_t);

      /// Reset the anode t3
      void reset_anode_R3();

      /// Check if anode t3 is available
      bool has_anode_R3() const;

      /// Return the anode t3
      int64_t get_anode_R3() const;

      /// Set the anode t4
      void set_anode_R4(const int64_t);

      /// Reset the anode t4
      void reset_anode_R4();

      /// Check if anode t4 is available
      bool has_anode_R4() const;

      /// Return the anode t4
      int64_t get_anode_R4() const;

      /// Set the anode t5
      void set_cathode_R5(const int64_t);

      /// Reset the anode t5
      void reset_cathode_R5();

      /// Check if cathode t5 is available
      bool has_cathode_R5() const;

      /// Return the cathode t5
      int64_t get_cathode_R5() const;

      /// Set the cathode t6
      void set_cathode_R6(const int64_t);

      /// Reset the anode t6
      void reset_cathode_R6();

      /// Check if cathode t6 is available
      bool has_cathode_R6() const;

      /// Return the cathode t6
      int64_t get_cathode_R6() const;

      /// Smart print
      virtual void tree_dump(std::ostream& a_out = std::clog,
			     const std::string& a_title = "",
			     const std::string& a_indent = "",
			     bool a_inherit = false) const;

    private:

      // Data:
      geomtools::geom_id _elec_id_;    //!< The Electronic ID of the Geiger cell
      uint64_t	         _trigger_ID_; //!< Trigger ID number

      int64_t _anode_R0_   = INVALID_TIME; //!< Anode differential time t0 (relative time reference)
      int64_t _anode_R1_   = INVALID_TIME; //!< Anode differential time t1
      int64_t _anode_R2_   = INVALID_TIME; //!< Anode differential time t2
      int64_t _anode_R3_   = INVALID_TIME; //!< Anode differential time t3
      int64_t _anode_R4_   = INVALID_TIME; //!< Anode differential time t4
      int64_t _cathode_R5_ = INVALID_TIME; //!< Cathode time t5
      int64_t _cathode_R6_ = INVALID_TIME; //!< Cathode time t6

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
