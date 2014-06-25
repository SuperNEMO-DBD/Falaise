/// \file falaise/snemo/reconstruction/mock_tracker_clustering_driver.cc

// Ourselves:
#include <falaise/snemo/reconstruction/mock_tracker_clustering_driver.h>

// Standard library:
#include <sstream>
#include <stdexcept>
#include <set>

// Third party:
// - Boost :
#include <boost/foreach.hpp>
// - Bayeux/datatools :
#include <datatools/properties.h>
#include <datatools/exception.h>
#include <datatools/logger.h>
#include <datatools/utils.h>
// - Bayeux/geomtools :
#include <geomtools/manager.h>

// This project :
#include <falaise/snemo/datamodels/tracker_clustering_data.h>
#include <falaise/snemo/geometry/gg_locator.h>

namespace snemo {

  namespace reconstruction {

    const std::string mock_tracker_clustering_driver::MTC_ID = "Mock";

    mock_tracker_clustering_driver::mock_tracker_clustering_driver() :
      ::snemo::processing::base_tracker_clusterizer(mock_tracker_clustering_driver::MTC_ID)
    {
      _max_row_distance_   = 2;
      _max_layer_distance_ = 2;
      _max_sum_distance_   = 0;
      return;
    }

    mock_tracker_clustering_driver::~mock_tracker_clustering_driver()
    {
      if (is_initialized()) {
        this->mock_tracker_clustering_driver::reset();
      }
      return;
    }

    // Initialize the driver through configuration properties
    void mock_tracker_clustering_driver::initialize(const datatools::properties & setup_)
    {
      // Invoke initialization at parent level :
      this->snemo::processing::base_tracker_clusterizer::_initialize(setup_);

      if (setup_.has_key("MTC.max_layer_distance")) {
        int value = setup_.fetch_integer("MTC.max_layer_distance");
        DT_THROW_IF(value < 0, std::range_error, "Invalid value for max layer distance !");
        _max_layer_distance_ = value;
      }

      if (setup_.has_key("MTC.max_row_distance")) {
        int value = setup_.fetch_integer("MTC.max_row_distance");
        DT_THROW_IF(value < 0, std::range_error, "Invalid value for max row distance !");
        _max_row_distance_ = value;
      }

      if (setup_.has_key("MTC.max_sum_distance")) {
        int value = setup_.fetch_integer("MTC.max_sum_distance");
        DT_THROW_IF(value < 0, std::range_error, "Invalid value for max sum distance !");
        _max_sum_distance_ = value;
      }

      DT_THROW_IF(_max_row_distance_ + _max_layer_distance_ + _max_sum_distance_ == 0,
                  std::logic_error,
                  "At least one maximum layer/row/sum distance must be non zero !");

      _set_initialized(true);
      return;
    }

    // Reset the clusterizer
    void mock_tracker_clustering_driver::reset()
    {
      DT_THROW_IF (! is_initialized(), std::logic_error,
                   "MockTrackerClusterizer driver is not initialized !");
      _set_initialized(false);
      _max_row_distance_   = 2;
      _max_layer_distance_ = 2;
      _max_sum_distance_   = 0;
      this->snemo::processing::base_tracker_clusterizer::_reset();
      return;
    }

    // int mock_tracker_clustering_driver::_prepare_process (const base_tracker_clusterizer::hit_collection_type & gg_hits_,
    //                                                       const base_tracker_clusterizer::calo_hit_collection_type & calo_hits_,
    //                                                       snemo::datamodel::tracker_clustering_data & clustering_)
    // {
    //   base_tracker_clusterizer::_prepare_process(gg_hits_, calo_hits_, clustering_);
    //   return 0;
    // }

    // Main clustering method
    int mock_tracker_clustering_driver::_process_algo(const base_tracker_clusterizer::hit_collection_type & gg_hits_,
                                                      const base_tracker_clusterizer::calo_hit_collection_type & /* calo_hits_ */,
                                                      snemo::datamodel::tracker_clustering_data & clustering_ )
    {
      namespace sdm = snemo::datamodel;

      // Filling a unique tracker clustering solution:
      sdm::tracker_clustering_solution::handle_type htcs(new sdm::tracker_clustering_solution);
      sdm::tracker_clustering_solution & tc_solution = htcs.grab();
      tc_solution.grab_auxiliaries().update_string(sdm::tracker_clustering_data::clusterizer_id_key(),
                                                   MTC_ID);

      // GG hit loop :
      sdm::calibrated_tracker_hit previous_gg_hit = gg_hits_.begin()->get();
      for (hit_collection_type::const_iterator igg = gg_hits_.begin(); igg != gg_hits_.end(); ++igg) {

        // Get a const reference on the calibrated Geiger hit :
        const sdm::calibrated_tracker_hit & a_gg_hit = igg->get();

        const snemo::geometry::gg_locator & gg_locator = get_gg_locator();
        const geomtools::geom_id & gg_hit_gid = a_gg_hit.get_geom_id();
        DT_THROW_IF(!gg_locator.is_drift_cell_volume(gg_hit_gid),
                    std::logic_error,
                    "Calibrated tracker hit can not be located inside detector !");

        if (!gg_locator.is_drift_cell_volume_in_current_module(gg_hit_gid)) {
          DT_LOG_DEBUG(get_logging_priority(), "Current Geiger cell is not in the module!");
          continue;
        }

        if (!are_neighbours(a_gg_hit.get_geom_id(), previous_gg_hit.get_geom_id())) {
          DT_LOG_TRACE(get_logging_priority(), "New track found !");
          // Create a tracker cluster handle:
          sdm::tracker_cluster::handle_type ht_cluster(new sdm::tracker_cluster);
          ht_cluster.grab().set_cluster_id(tc_solution.get_clusters().size());
          tc_solution.grab_clusters().push_back(ht_cluster);
        }

        // Continue to fill the current track
        sdm::tracker_cluster::handle_type & cluster_handle = tc_solution.grab_clusters().back();
        DT_LOG_TRACE(get_logging_priority (), "Current cluster id " << cluster_handle.get().get_cluster_id());
        cluster_handle.grab().grab_hits().push_back(*igg);
        // hits_ids.insert(a_gg_hit.get_hit_id());
        previous_gg_hit = a_gg_hit;
      }

      // Set a unique Id to this solution:
      tc_solution.set_solution_id(clustering_.get_number_of_solutions());

      if (get_logging_priority() >= datatools::logger::PRIO_TRACE)
        tc_solution.tree_dump(std::clog, "Mock Tracker Clustering solution: ", "DEVEL: ");

      // Add the solution as the default one:
      clustering_.add_solution(htcs, true);

      return 0;
    }

    bool mock_tracker_clustering_driver::are_neighbours(const geomtools::geom_id & tracker_hit_id1_,
                                                        const geomtools::geom_id & tracker_hit_id2_) const
    {
      DT_THROW_IF(! is_initialized(), std::logic_error,
                  "MockTrackerClusterizer driver is not initialized !");

      if (tracker_hit_id1_ == tracker_hit_id2_) return false;

      const snemo::geometry::gg_locator & gg_locator = get_gg_locator();

      // Use Geiger locator for such research Warning: use integer
      // because uint32_t has strange behavior with absolute value
      // cmath::abs
      const int hit1_side  = gg_locator.extract_side (tracker_hit_id1_);
      const int hit1_layer = gg_locator.extract_layer(tracker_hit_id1_);
      const int hit1_row   = gg_locator.extract_row  (tracker_hit_id1_);

      const int hit2_side  = gg_locator.extract_side (tracker_hit_id2_);
      const int hit2_layer = gg_locator.extract_layer(tracker_hit_id2_);
      const int hit2_row   = gg_locator.extract_row  (tracker_hit_id2_);

      // Do not cross the foil
      if (hit1_side != hit2_side) return false;

      // Check neighbouring
      const unsigned int layer_distance = std::abs(hit1_layer - hit2_layer);
      const unsigned int row_distance   = std::abs(hit1_row   - hit2_row);
      if ( _max_layer_distance_ > 0 ) {
        if (layer_distance > _max_layer_distance_) return false;
      }
      if ( _max_row_distance_ > 0 ) {
        if (row_distance > _max_row_distance_) return false;
      }
      if ( _max_sum_distance_ > 0 ) {
        if ((layer_distance + row_distance) > _max_sum_distance_) return false;
      }

      return true;
    }

    // static
    void mock_tracker_clustering_driver::init_ocd(datatools::object_configuration_description & ocd_)
    {

      // Invoke OCD support from parent class :
      ::snemo::processing::base_tracker_clusterizer::ocd_support(ocd_);

      {
        // Description of the 'MTC.max_layer_distance' configuration property :
        datatools::configuration_property_description & cpd
          = ocd_.add_property_info();
        cpd.set_name_pattern("MTC.max_layer_distance")
          .set_from("snemo::reconstruction::mock_tracker_clustering_driver")
          .set_terse_description("Maximum layer distance between two neighbour hits")
          .set_traits(datatools::TYPE_INTEGER)
          .set_mandatory(false)
          .set_long_description("Value ``0`` disables this test.")
          .set_default_value_integer(2)
          .add_example("Use the default value::                \n"
                       "                                       \n"
                       "  MTC.max_layer_distance : integer = 2 \n"
                       "                                       \n"
                       )
          ;
      }

      {
        // Description of the 'MTC.max_row_distance' configuration property :
        datatools::configuration_property_description & cpd
          = ocd_.add_property_info();
        cpd.set_name_pattern("MTC.max_row_distance")
          .set_from("snemo::reconstruction::mock_tracker_clustering_driver")
          .set_terse_description("Maximum row distance between two neighbour hits")
          .set_traits(datatools::TYPE_INTEGER)
          .set_mandatory(false)
          .set_long_description("Value ``0`` disables this test.")
          .set_default_value_integer(2)
          .add_example("Use the default value::                \n"
                       "                                       \n"
                       "  MTC.max_row_distance : integer = 2 \n"
                       "                                       \n"
                       )
          ;
      }

      {
        // Description of the 'MTC.max_sum_distance' configuration property :
        datatools::configuration_property_description & cpd
          = ocd_.add_property_info();
        cpd.set_name_pattern("MTC.max_sum_distance")
          .set_from("snemo::reconstruction::mock_tracker_clustering_driver")
          .set_terse_description("Maximum row+layer distance between two neighbour hits")
          .set_traits(datatools::TYPE_INTEGER)
          .set_mandatory(false)
          .set_long_description("Value ``0`` disables this test.")
          .set_default_value_integer(0)
          .add_example("Use the default value::                \n"
                       "                                       \n"
                       "  MTC.max_sum_distance : integer = 3   \n"
                       "                                       \n"
                       )
          ;
      }
    }

  }  // end of namespace reconstruction

}  // end of namespace snemo

/* OCD support */
#include <datatools/object_configuration_description.h>
DOCD_CLASS_IMPLEMENT_LOAD_BEGIN(snemo::reconstruction::mock_tracker_clustering_driver,ocd_)
{
  ocd_.set_class_name("snemo::reconstruction::mock_tracker_clustering_driver");
  ocd_.set_class_description("A driver class for the mock tracker clustering algorithm");
  ocd_.set_class_library("Falaise_MockTrackerClusterizer");
  ocd_.set_class_documentation("This driver manager for the mock tracker clustering clustering algorithm.\n"
                               "This algorithm is only used for simulated data.                          \n"
                               );

  // Invoke specific OCD support :
  ::snemo::reconstruction::mock_tracker_clustering_driver::init_ocd(ocd_);

  ocd_.set_validation_support(true);
  ocd_.lock();
  return;
}
DOCD_CLASS_IMPLEMENT_LOAD_END() // Closing macro for implementation
DOCD_CLASS_SYSTEM_REGISTRATION(snemo::reconstruction::mock_tracker_clustering_driver,
                               "snemo::reconstruction::mock_tracker_clustering_driver")
