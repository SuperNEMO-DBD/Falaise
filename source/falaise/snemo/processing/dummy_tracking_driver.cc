/// \file falaise/snemo/processing/dummy_tracking_driver.cc

// Ourselves:
#include <falaise/snemo/processing/dummy_tracking_driver.h>

// This project:
#include <falaise/snemo/datamodels/tracker_clustering_data.h>
#include <falaise/snemo/datamodels/tracker_trajectory_data.h>

namespace snemo {

  namespace processing {

    dummy_tracking_driver::dummy_tracking_driver()
      : base_tracking_driver("dummy")
    {
     return;
    }

    dummy_tracking_driver::~dummy_tracking_driver()
    {
      return;
    }

    void dummy_tracking_driver::_at_initialize(const datatools::properties & config_,
					       datatools::service_manager & /* services_ */)
    {
      if (datatools::logger::is_debug(get_verbosity())) {
	boost::property_tree::ptree popts;
	popts.put("indent", "[debug] ");
	popts.put("title", "Dummy Tracking Driver setup");
	config_.print_tree(std::clog, popts);
      }
      return;
    }

    void dummy_tracking_driver::_at_reset()
    {
     return;
    }

    int dummy_tracking_driver::process_tracking(const std::vector<tracking_precluster> & preclusters_,
						const base_tracking_driver::calo_hit_collection_type & calo_hits_,
						snemo::datamodel::tracker_clustering_data & clustering_,
						snemo::datamodel::tracker_trajectory_data & track_fitting_)
    {
      DT_LOG_DEBUG(get_verbosity(), "Number of input preclusters = " << preclusters_.size());
      DT_LOG_DEBUG(get_verbosity(), "Number of input calorimeter hits = " << calo_hits_.size());
      
      DT_LOG_DEBUG(get_verbosity(), "Number of output clustering solutions = " << clustering_.size());
      DT_LOG_DEBUG(get_verbosity(), "Number of output track fitting solutions = " << track_fitting_.get_number_of_solutions());
     return 0;
    }

  } // end of namespace processing

} // end of namespace snemo
