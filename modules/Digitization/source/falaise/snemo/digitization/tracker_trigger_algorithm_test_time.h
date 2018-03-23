// snemo/digitization/tracker_trigger_algorithm_test_time.h
// Author(s): Yves LEMIERE <lemiere@lpccaen.in2p3.fr>
// Author(s): Guillaume OLIVIERO <goliviero@lpccaen.in2p3.fr>

#ifndef FALAISE_DIGITIZATION_PLUGIN_SNEMO_DIGITIZATION_TRACKER_TRIGGER_ALGORITHM_TEST_TIME_H
#define FALAISE_DIGITIZATION_PLUGIN_SNEMO_DIGITIZATION_TRACKER_TRIGGER_ALGORITHM_TEST_TIME_H

// Standard library :
#include <bitset>
#include <string>

// This project :
#include <snemo/digitization/electronic_mapping.h>
#include <snemo/digitization/geiger_ctw_data.h>
#include <snemo/digitization/tracker_sliding_zone.h>
#include <snemo/digitization/tracker_trigger_mem_maker.h>
#include <snemo/digitization/tracker_zone.h>
#include <snemo/digitization/trigger_display_manager.h>
#include <snemo/digitization/trigger_info.h>
#include <snemo/digitization/trigger_structures.h>

namespace datatools {
class properties;
}

namespace snemo {

namespace digitization {

/// \brief Trigger algorithm general process
class tracker_trigger_algorithm_test_time {
 public:
  /// Trigger display manager is a friend because it can access to members for display
  friend class trigger_display_manager;

  /// Default constructor
  tracker_trigger_algorithm_test_time();

  /// Destructor
  virtual ~tracker_trigger_algorithm_test_time();

  /// Set the electronic mapping object
  void set_electronic_mapping(const electronic_mapping& my_electronic_mapping_);

  /// Fill memory 1 for all zones
  void fill_mem1_all(const std::string& filename_);

  /// Fill memory 2 for all zones
  void fill_mem2_all(const std::string& filename_);

  /// Fill memory 3 for all zones
  void fill_mem3_all(const std::string& filename_);

  /// Fill memory 4 for all zones
  void fill_mem4_all(const std::string& filename_);

  /// Fill memory 5 for all zones
  void fill_mem5_all(const std::string& filename_);

  /// Initializing
  void initialize();

  /// Initializing
  void initialize(const datatools::properties& config_);

  /// Check if the algorithm is initialized
  bool is_initialized() const;

  /// Reset the object
  void reset();

  /// Return the board id from the bitset of 100 bits
  uint32_t get_board_id(const std::bitset<geiger::tp::FULL_SIZE>& my_bitset_) const;

  /// Convert the electronic ID of active geiger cells into geometric ID
  void build_hit_cells_gids_from_ctw(const geiger_ctw& my_geiger_ctw_,
                                     std::vector<geomtools::geom_id>& hit_cells_gids_) const;

  /// Fill the geiger cells matrix
  void fill_matrix(const std::vector<geomtools::geom_id>& hit_cells_gids_);

  /// ASCII display for the geiger cells matrix
  void display_matrix() const;

  /// Get the Geiger matrix for a CT 1600 ns
  const trigger_structures::geiger_matrix get_geiger_matrix_for_a_clocktick() const;

  /// Reset the geiger cells matrix
  void reset_matrix();

  /// Reset bitsets in zone and sliding zone
  void reset_zones_informations();

  /// Build one sliding zone information for a clocktick
  void build_sliding_zone(unsigned int side_, unsigned int szone_id_);

  /// Build all sliding zones with memories mem1 and mem2 for projections
  void build_sliding_zones(tracker_trigger_mem_maker::mem1_type& mem1_,
                           tracker_trigger_mem_maker::mem2_type& mem2_);

  /// Build one zone information for a clocktick
  void build_zone(unsigned int side_, unsigned int zone_id_);

  /// Build all zones responses for a clocktick
  void build_zones();

  /// Build the vertical information for a zone
  void build_in_out_pattern(tracker_zone& zone_, tracker_trigger_mem_maker::mem3_type& mem3_);

  /// Build the horizeontal information for a zone
  void build_left_mid_right_pattern(tracker_zone& zone_,
                                    tracker_trigger_mem_maker::mem4_type& mem4_,
                                    tracker_trigger_mem_maker::mem5_type& mem5_);

  /// Build near source information for a zone
  void build_near_source_pattern(tracker_zone& zone_);

  /// Build tracker record for each clocktick
  void build_tracker_record(trigger_structures::tracker_record& a_tracker_record_);

  /// Print all tracker with zones boundaries
  void print_zones(std::ostream& out_) const;

  /// General process
  void process(const std::vector<datatools::handle<geiger_ctw> > geiger_ctw_list_per_clocktick_,
               trigger_structures::tracker_record& a_tracker_record_);

 protected:
  /// Process for a clocktick
  void _process_for_a_clocktick(
      const std::vector<datatools::handle<geiger_ctw> > geiger_ctw_list_per_clocktick_,
      trigger_structures::tracker_record& a_tracker_record_);

  /// Protected general process
  void _process(const std::vector<datatools::handle<geiger_ctw> > geiger_ctw_list_per_clocktick_,
                trigger_structures::tracker_record& a_tracker_record_);

 private:
  // Configuration :
  bool _initialized_;                              //!< Initialization
  const electronic_mapping* _electronic_mapping_;  //!< Convert geometric ID into electronic ID flag

  tracker_trigger_mem_maker::mem1_type _sliding_zone_vertical_memory_;
  tracker_trigger_mem_maker::mem2_type _sliding_zone_horizontal_memory_;
  tracker_trigger_mem_maker::mem3_type _zone_vertical_memory_;
  tracker_trigger_mem_maker::mem4_type _zone_horizontal_memory_;
  tracker_trigger_mem_maker::mem5_type _zone_vertical_for_horizontal_memory_;

  // Data :
  trigger_structures::geiger_matrix _a_geiger_matrix_for_a_clocktick_;
  tracker_zone _zones_[trigger_info::NSIDES][trigger_info::NZONES];
  tracker_sliding_zone _sliding_zones_[trigger_info::NSIDES][trigger_info::NSLZONES];
};

}  // end of namespace digitization

}  // end of namespace snemo

#endif  // FALAISE_DIGITIZATION_PLUGIN_SNEMO_DIGITIZATION_TRACKER_TRIGGER_ALGORITHM_TEST_TIME_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
