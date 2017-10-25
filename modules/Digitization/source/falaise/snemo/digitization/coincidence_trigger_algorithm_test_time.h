// snemo/digitization/coincidence_trigger_algorithm_test_time.h
// Author(s): Yves LEMIERE <lemiere@lpccaen.in2p3.fr>
// Author(s): Francois MAUGER <mauger@lpccaen.in2p3.fr>
// Author(s): Guillaume OLIVIERO <goliviero@lpccaen.in2p3.fr>

#ifndef FALAISE_DIGITIZATION_PLUGIN_SNEMO_DIGITIZATION_COINCIDENCE_TRIGGER_ALGORITHM_TEST_TIME_H
#define FALAISE_DIGITIZATION_PLUGIN_SNEMO_DIGITIZATION_COINCIDENCE_TRIGGER_ALGORITHM_TEST_TIME_H

// Standard library :
#include <bitset>
#include <string>
#include <utility>

// Third part :
// Boost :
#include <boost/circular_buffer.hpp>
#include <boost/scoped_ptr.hpp>

// This project :
#include <snemo/digitization/trigger_display_manager.h>
#include <snemo/digitization/trigger_structures.h>

namespace datatools {
class properties;
}

namespace snemo {

namespace digitization {

class coincidence_trigger_algorithm_test_time {
 public:
  /// Trigger display manager is a friend because it can access to members for display
  friend class trigger_display_manager;

  static const uint32_t SIZE_OF_RESERVED_COINCIDENCE_CALO_RECORDS = 5;
  static const uint32_t SIZE_OF_L2_COINCIDENCE_DECISION_GATE = 5;

  /// Default constructor
  coincidence_trigger_algorithm_test_time();

  /// Destructor
  virtual ~coincidence_trigger_algorithm_test_time();

  /// Initializing
  void initialize_simple();

  /// Initializing
  void initialize(const datatools::properties& /*config_*/);

  /// Check if the algorithm is initialized
  bool is_initialized() const;

  /// Reset the object
  void reset();

  /// Reset internal datas
  void reset_data();

  /// Get the finale caraco trigger decision
  bool get_coincidence_decision() const;

  /// General process
  void process(
      const std::pair<trigger_structures::coincidence_calo_record,
                      trigger_structures::tracker_record>
          pair_for_a_clocktick_,
      trigger_structures::coincidence_event_record& a_coincidence_record_,
      trigger_structures::L2_decision& a_L2_decision_record_,
      const boost::scoped_ptr<boost::circular_buffer<trigger_structures::previous_event_record> >&
          previous_event_records_);

 protected:
  /// CAlo tRAcker COincidence (CARACO) process for spatial coincidence between calorimeter and
  /// tracker each 1600ns
  void _process_calo_tracker_coincidence(
      const std::pair<trigger_structures::coincidence_calo_record,
                      trigger_structures::tracker_record>
          a_pair_for_a_clocktick_,
      trigger_structures::coincidence_event_record& a_coincidence_record_,
      trigger_structures::L2_decision& a_L2_decision_record_);

  /// Alpha delayed Pattern Event (APE) process for delayed
  void _process_delayed_coincidence(
      const std::pair<trigger_structures::coincidence_calo_record,
                      trigger_structures::tracker_record>
          a_pair_for_a_clocktick_,
      trigger_structures::coincidence_event_record& a_delayed_coincidence_record_,
      trigger_structures::L2_decision& a_L2_decision_record_,
      const trigger_structures::previous_event_record& a_previous_event_record_);

  /// Process pair record for a clocktick
  void _process(
      const std::pair<trigger_structures::coincidence_calo_record,
                      trigger_structures::tracker_record>
          pair_for_a_clocktick_,
      trigger_structures::coincidence_event_record& a_coincidence_record_,
      trigger_structures::L2_decision& a_L2_decision_record_,
      const boost::scoped_ptr<boost::circular_buffer<trigger_structures::previous_event_record> >&
          previous_event_records_);

 private:
  // Configuration :
  bool _initialized_;  //!< Initialization flag
  trigger_structures::L2_trigger_mode
      _coincidence_decision_;  // Coincidence decision for a clocktick
};

}  // end of namespace digitization

}  // end of namespace snemo

#endif  // FALAISE_DIGITIZATION_PLUGIN_SNEMO_DIGITIZATION_COINCIDENCE_TRIGGER_ALGORITHM_TEST_TIME_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
