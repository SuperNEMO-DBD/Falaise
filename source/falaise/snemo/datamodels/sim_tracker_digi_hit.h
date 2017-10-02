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
class sim_tracker_digi_hit : public geomtools::base_hit {
 public:
  static const int16_t INVALID_TIME = std::numeric_limits<int16_t>::min();
  static const int16_t OVERFLOW_TIME = std::numeric_limits<int16_t>::min() + 1;
  static const int16_t UNDERFLOW_TIME = std::numeric_limits<int16_t>::max();

  /// Masks to automatically tag the attributes to be stored
  enum more_store_mask_type {
    STORE_ANODE_T0 = datatools::bit_mask::bit03,    //!< Serialization mask for the anode t0
    STORE_ANODE_T1 = datatools::bit_mask::bit04,    //!< Serialization mask for the anode t1
    STORE_ANODE_T2 = datatools::bit_mask::bit05,    //!< Serialization mask for the anode t2
    STORE_ANODE_T3 = datatools::bit_mask::bit06,    //!< Serialization mask for the anode t3
    STORE_ANODE_T4 = datatools::bit_mask::bit07,    //!< Serialization mask for the anode t4
    STORE_CATHODE_T5 = datatools::bit_mask::bit08,  //!< Serialization mask for the cathode t5
    STORE_CATHODE_T6 = datatools::bit_mask::bit09   //!< Serialization mask for the cathode t6
  };

  /// Constructor
  sim_tracker_digi_hit();

  /// Destructor
  virtual ~sim_tracker_digi_hit();

  /// Check validity
  bool is_valid() const;

  /// Reset
  void reset();

  /// Set the anode t0
  void set_anode_t0(const int16_t);

  /// Reset the anode t0
  void reset_anode_t0();

  /// Check if anode t0 is available
  bool has_anode_t0() const;

  /// Return the anode t0
  int16_t get_anode_t0() const;

  /// Set the anode t1
  void set_anode_t1(const int16_t);

  /// Reset the anode t1
  void reset_anode_t1();

  /// Check if anode t1 is available
  bool has_anode_t1() const;

  /// Return the anode t1
  int16_t get_anode_t1() const;

  /// Set the anode t2
  void set_anode_t2(const int16_t);

  /// Reset the anode t2
  void reset_anode_t2();

  /// Check if anode t2 is available
  bool has_anode_t2() const;

  /// Return the anode t2
  int16_t get_anode_t2() const;

  /// Set the anode t3
  void set_anode_t3(const int16_t);

  /// Reset the anode t3
  void reset_anode_t3();

  /// Check if anode t3 is available
  bool has_anode_t3() const;

  /// Return the anode t3
  int16_t get_anode_t3() const;

  /// Set the anode t4
  void set_anode_t4(const int16_t);

  /// Reset the anode t4
  void reset_anode_t4();

  /// Check if anode t4 is available
  bool has_anode_t4() const;

  /// Return the anode t4
  int16_t get_anode_t4() const;

  /// Set the anode t5
  void set_cathode_t5(const int16_t);

  /// Reset the anode t5
  void reset_cathode_t5();

  /// Check if cathode t5 is available
  bool has_cathode_t5() const;

  /// Return the cathode t5
  int16_t get_cathode_t5() const;

  /// Set the cathode t6
  void set_cathode_t6(const int16_t);

  /// Reset the anode t6
  void reset_cathode_t6();

  /// Check if cathode t6 is available
  bool has_cathode_t6() const;

  /// Return the cathode t6
  int16_t get_cathode_t6() const;

  /// Smart print
  virtual void tree_dump(std::ostream& a_out = std::clog, const std::string& a_title = "",
                         const std::string& a_indent = "", bool a_inherit = false) const;

 private:
  int16_t _anode_t0_ = INVALID_TIME;    //!< Anode differential time t0 (relative time reference)
  int16_t _anode_t1_ = INVALID_TIME;    //!< Anode differential time t1
  int16_t _anode_t2_ = INVALID_TIME;    //!< Anode differential time t2
  int16_t _anode_t3_ = INVALID_TIME;    //!< Anode differential time t3 (cathode bottom)
  int16_t _anode_t4_ = INVALID_TIME;    //!< Anode differential time t4 (cathode top)
  int16_t _cathode_t5_ = INVALID_TIME;  //!< Cathode time t5 (cathode bottom)
  int16_t _cathode_t6_ = INVALID_TIME;  //!< Cathode time t6 (cathode top)

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
