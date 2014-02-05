/** \file falaise/snemo/datamodels/raw_tracker_hit.h */
/* Author(s) :    François Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2014-01-30
 * Last modified: 2014-01-31
 *
 * Description: The SuperNEMO raw tracker hit
 */

#ifndef FALAISE_SNEMO_DATAMODEL_RAW_TRACKER_HIT_H
#define FALAISE_SNEMO_DATAMODEL_RAW_TRACKER_HIT_H 1

// Standard library:
#include <bitset>
#include <vector>

// Third party
// - Boost :
#include <boost/cstdint.hpp>
// - Bayeux/geomtools
#include <geomtools/base_hit.h>

namespace snemo {

  namespace datamodel {

    /// \brief The data structure that represents a  SuperNEMO raw tracker hit
    class raw_tracker_hit : public geomtools::base_hit
    {
    public:

      /// \brief Mode
      /**
       *    [0 0]
       *     T B
       */
      enum mode_type {
        MODE_UNDEF          = 0,   //!< Undefined mode
        MODE_BOTTOM_CATHODE = 0x1, //!< Mode with bottom cathode signal [B]
        MODE_TOP_CATHODE    = 0x2, //!< Mode with top cathode signal [T]
        MODE_NEMO3          = MODE_BOTTOM_CATHODE | MODE_TOP_CATHODE, //!< NEMO-3 mode
        MODE_ALTERNATIVE    = MODE_BOTTOM_CATHODE  //!< Alternative mode
      };

      /// \brief Bit masks to describe data
      /**
       *
       *       [0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0]
       *        U U U U U U U 6 5 4 3 2 1 0 M M
       *
       */
      enum data_description_mask {
        DATA_DESC_MODE           = 0x3,    //!< The mode [M]
        DATA_DESC_HAS_ANODE_T0   = 0x4,    //!< The raw data stores the anode t0 [0]
        DATA_DESC_HAS_ANODE_T1   = 0x8,    //!< The raw data stores the anode t1 [1]
        DATA_DESC_HAS_ANODE_T2   = 0x10,   //!< The raw data stores the anode t2 [2]
        DATA_DESC_HAS_ANODE_T3   = 0x20,   //!< The raw data stores the anode t3 [3]
        DATA_DESC_HAS_ANODE_T4   = 0x40,   //!< The raw data stores the anode t4 [4]
        DATA_DESC_HAS_CATHODE_T5 = 0x80,   //!< The raw data stores the cathode t5 [5]
        DATA_DESC_HAS_CATHODE_T6 = 0x100,  //!< The raw data stores the cathode t6 [6]
        DATA_DESC_UNUSED         = 0x1F700 //!< Unused bits [U]
      };

      /// \brief Bit masks to describe the channel address
      /**
       *     module        board
       *     |------->     |------->
       *    [0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0]
       *               |->           |--------->
       *               crate         asic+channel
       *
       */
      enum address_mask {
        ADDRESS_MODULE_ID    = 0x3E000, //!< 5 bits: Module ID  = [0:19]
        ADDRESS_CRATE_ID     = 0x1800,  //!< 2 bits: Crate ID   = [0:2]
        ADDRESS_BOARD_ID     = 0x7C0,   //!< 5 bits: Board ID   = [0:20]
        ADDRESS_CHANNEL_ID   = 0x3F     //!< 6 bits: Channel ID = [0:53]
      };

      /// Check if the hit contains valid data
      bool is_valid() const;

      /// Set the trigger ID
      void set_trigger_id(uint16_t);

      /// Return the trigger ID
      uint16_t get_trigger_id() const;

      /// Set the channel address
      void set_channel_address(uint32_t);

      /// Return the channel address
      uint32_t get_channel_address() const;

      /// Return the module ID
      uint32_t get_module_id() const;

      /// Return the crate ID
      uint32_t get_crate_id() const;

      /// Return the board ID
      uint32_t get_board_id() const;

      /// Return the channel ID
      uint32_t get_channel_id() const;

      /// Set the data description
      void set_data_description(uint16_t);

      /// Return the time_info
      uint16_t get_data_description() const;

      /// Check if the mode is NEMO3
      bool is_mode_nemo3() const;

      /// Check if the mode is ALTERNATIVE
      bool is_mode_alternative() const;

      /// Check if anode signal t0 is available
      bool has_anode_t0() const;

      /// Check if anode signal t1 is available
      bool has_anode_t1() const;

      /// Check if anode signal t2 is available
      bool has_anode_t2() const;

      /// Check if anode signal t3 is available
      bool has_anode_t3() const;

      /// Check if anode signal t4 is available
      bool has_anode_t4() const;

      /// Check if cathode signal t5 is available
      bool has_cathode_t5() const;

      /// Check if cathode signal t6 is available
      bool has_cathode_t6() const;

      /// Set the anode signal t0
      void set_anode_t0(uint64_t);

      /// Return the anode signal t0
      uint64_t get_anode_t0() const;

      /// Set the anode signal t1
      void set_anode_t1(uint16_t);

      /// Return the anode signal t1
      uint16_t get_anode_t1() const;

      /// Set the anode signal t2
      void set_anode_t2(uint16_t);

      /// Return the anode signal t2
      uint16_t get_anode_t2() const;

      /// Set the anode signal t3
      void set_anode_t3(uint16_t);

      /// Return the anode signal t3
      uint16_t get_anode_t3() const;

      /// Set the anode signal t4
      void set_anode_t4(uint16_t);

      /// Return the anode signal t4
      uint16_t get_anode_t4() const;

      /// Set the anode signal t5
      void set_cathode_t5(uint16_t);

      /// Return the cathode signal t5
      uint16_t get_cathode_t5() const;

      /// Set the cathode signal t6
      void set_cathode_t6(uint16_t);

      /// Return the cathode signal t6
      uint16_t get_cathode_t6() const;

      /// Constructor
      raw_tracker_hit();

      /// Destructor
      virtual ~raw_tracker_hit();

      /// Reset
      void reset();

      /// Smart print
      virtual void tree_dump(std::ostream & a_out         = std::clog,
                             const std::string & a_title  = "",
                             const std::string & a_indent = "",
                             bool a_inherit               = false) const;

    protected:

      /// Set default value for attributes
      void _set_defaults();

    private:

      uint16_t _trigger_id_;           //!< Trigger ID
      uint32_t _channel_address_;      //!< Channel address
      uint16_t _data_description_;     //!< Description of the available data
      uint64_t _anode_t0_;             //!< Anode differential signal time t0 (reference)
      uint16_t _anode_t1_;             //!< Anode differential signal time t1
      uint16_t _anode_t2_;             //!< Anode differential signal time t2
      uint16_t _anode_t3_;             //!< Anode differential signal time t3
      uint16_t _anode_t4_;             //!< Anode differential signal time t4
      uint16_t _cathode_t5_;           //!< Cathode signal time t5
      uint16_t _cathode_t6_;           //!< Cathode signal time t6 (only in NEMO-3 mode)

      DATATOOLS_SERIALIZATION_DECLARATION();

    };

  } // end of namespace datamodel

} // end of namespace snemo

#endif // FALAISE_SNEMO_DATAMODEL_RAW_TRACKER_HIT_H
