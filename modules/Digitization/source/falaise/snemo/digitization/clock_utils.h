// snemo/digitization/clock_utils.h
// Author(s): Yves LEMIERE <lemiere@lpccaen.in2p3.fr>
// Author(s): Guillaume OLIVIERO <goliviero@lpccaen.in2p3.fr>

#ifndef FALAISE_DIGITIZATION_PLUGIN_SNEMO_DIGITIZATION_CLOCK_UTILS_TP_H
#define FALAISE_DIGITIZATION_PLUGIN_SNEMO_DIGITIZATION_CLOCK_UTILS_TP_H

// Standard Library :
#include <math.h>
#include <limits>

// - Bayeux/datatools:
#include <datatools/clhep_units.h>
#include <datatools/i_tree_dump.h>

// - Bayeux/GSL:
#include <mygsl/rng.h>

// - Boost:
#include <boost/cstdint.hpp>

namespace snemo {

namespace digitization {

class clock_utils {
 public:
  static const uint32_t NUMBER_OF_25_CLOCK_IN_800 = 32;  //!< Number of 25 time clock in 800
  static const uint32_t MAIN_CLOCKTICK = 25;             //!< Main clocktick, 40 MHz => 25ns.
  static const uint32_t TRACKER_CLOCKTICK = 800;         //!< Clocktick for tracker, 800ns.
  static const uint32_t TRIGGER_CLOCKTICK = 1600;        //!< Clocktick for trigger, 1600ns.
  static const uint32_t INVALID_CLOCKTICK =
      std::numeric_limits<uint32_t>::max();  //!< Invalid value for clocktick
  static const uint32_t ACTIVATED_GEIGER_CELLS_NUMBER =
      10;  //!< Number of clocktick 800 where a geiger cell is activated

  static const uint32_t CALO_FEB_SHIFT_CLOCKTICK_NUMBER =
      6;  //!< Number of clocktick which shift the internal clocktick in a caloFEB
  static const uint32_t CALO_CB_SHIFT_CLOCKTICK_NUMBER =
      7;  //!< Number of clocktick which shift the internal clocktick in a caloCB

  // To define with Thierry ? :
  static const uint32_t TRACKER_FEB_SHIFT_CLOCKTICK_NUMBER =
      1;  //!< Number of clocktick 800 ns which shift the internal clocktick in a trackerFEB
  static const uint32_t TRACKER_CB_SHIFT_CLOCKTICK_NUMBER =
      0;  //!< Number of clocktick 800 ns which shift the internal clocktick in a trackerCB

  static const uint32_t TRACKER_FEB_TO_CB_SHIFT_CLOCKTICK_NUMBER =
      1;  //!< Number of clocktick 800 ns to take into account the transit between FEB and CB
  static const uint32_t TRACKER_CB_TO_TB_SHIFT_CLOCKTICK_NUMBER =
      1;  //!< Number of clocktick 800 ns to take into account the transit between CB and TB

  static const uint32_t TRIGGER_COMPUTING_SHIFT_CLOCKTICK_1600NS =
      1;  //!< Shift to take into account the time of computing and when it is published (CT+1)

  static const uint32_t PREVIOUS_EVENT_RECORD_LIVING_NUMBER_OF_CLOCKTICK =
      625;  //!< Number of CT 1600 a previous event record "live" 625 * 1600 = 1 ms

  /// Default constructor
  clock_utils();

  /// Destructor
  virtual ~clock_utils();

  /// Initializing
  void initialize();

  /// Check if the object is initialized
  bool is_initialized() const;

  /// Reset the object :
  void reset();

  /// Get the clocktick reference
  int32_t get_clocktick_ref() const;

  /// Get the shift between 0 and 1600 ns
  double get_shift_1600() const;

  /// Get the clocktick 25ns reference
  int32_t get_clocktick_25_ref() const;

  /// Get the clocktick 800ns reference
  int32_t get_clocktick_800_ref() const;

  /// Get the clocktick 25ns shift
  double get_shift_25() const;

  /// Get the clocktick 800ns shift
  double get_shift_800() const;

  /// Compute a clocktick 25ns into a clocktick 1600ns
  void compute_clocktick_25ns_to_1600ns(const uint32_t clocktick_25ns_,
                                        uint32_t& clocktick_1600ns_) const;
  /// Compute a clocktick 800ns into a clocktick 1600ns
  void compute_clocktick_800ns_to_1600ns(const uint32_t clocktick_800ns_,
                                         uint32_t& clocktick_1600ns_) const;

  /// Display clockticks ref and shifts
  void tree_dump(std::ostream& out_ = std::clog, const std::string& title_ = "",
                 const std::string& indent_ = "", bool inherit_ = false) const;

  /// Compute clockticks reference
  void compute_clockticks_ref(mygsl::rng& prng_);

 protected:
  /// Clocktick shift uniform randomize for 25 and 800ns clockticks
  void _randomize_shift(mygsl::rng& prng_);

 private:
  bool _initialized_;        //!< Initialization flag
  uint32_t _clocktick_ref_;  //!< Clocktick reference (0) for everyone
  double _shift_1600_;       //!< Shift between 0 and 1600 corresponding at the physical decay time
  uint32_t _clocktick_25_ref_;   //!< Clocktick 25 ns reference
  uint32_t _clocktick_800_ref_;  //!< Clocktick 800 ns reference
  double _shift_25_;             //!< 25 ns shift
  double _shift_800_;            //!< 800 ns shift
};

}  // end of namespace digitization

}  // end of namespace snemo

#endif /* FALAISE_DIGITIZATION_PLUGIN_SNEMO_DIGITIZATION_MAPPING_TP_H */

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
