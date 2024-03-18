/// \file falaise/snemo/processing/base_tracking_driver.cc

// Ourselves:
#include <falaise/snemo/processing/base_tracking_driver.h>

// Standard library:
#include <sstream>
#include <stdexcept>

// Third party:
// - Boost :
#include <boost/foreach.hpp>
// - Bayeux/datatools :
#include <datatools/properties.h>
// - Bayeux/geomtools :
#include <geomtools/manager.h>

// This project:
#include <falaise/property_set.h>
#include <falaise/quantity.h>
#include <falaise/snemo/datamodels/tracker_clustering_data.h>
#include <falaise/snemo/datamodels/tracker_trajectory_data.h>
#include <falaise/snemo/geometry/gg_locator.h>
#include <falaise/snemo/geometry/locator_helpers.h>
#include <falaise/snemo/geometry/locator_plugin.h>

namespace snemo {

  namespace processing {

    // Constructor
    base_tracking_driver::base_tracking_driver(const std::string & name)
    {
      _id_ = name;
      return;
    }

    base_tracking_driver::~base_tracking_driver()
    {
      if (is_initialized()) {
	DT_THROW(std::logic_error, "Tracking driver must be terminated before destruction");
      }
    };

    datatools::logger::priority base_tracking_driver::get_verbosity() const
    {
      return _verbosity;
    }

    void base_tracking_driver::set_verbosity(datatools::logger::priority verbosity_)
    {
      DT_THROW_IF(verbosity_ == datatools::logger::PRIO_UNDEFINED, std::logic_error,
                  "Invalid verbosity!");
      _verbosity = verbosity_;
      return;
    }

    const std::string & base_tracking_driver::get_id() const
    {
      return _id_;
    }

    bool base_tracking_driver::is_initialized() const
    {
      return _initialized_;
    }
 
    const snemo::processing::detector_description &
    base_tracking_driver::get_detector_description() const
    {
      return _detector_desc_;
    }

    void base_tracking_driver::set_detector_description(const snemo::processing::detector_description & det_desc_)
    {
      _detector_desc_ = det_desc_;
      return;
    }
    
    const geomtools::manager & base_tracking_driver::get_geometry_manager() const
    {
      DT_THROW_IF(not _detector_desc_.has_geometry_manager(), std::logic_error, "No geometry manager is config !");
      return _detector_desc_.get_geometry_manager();
    }

    const snemo::geometry::gg_locator & base_tracking_driver::get_gg_locator() const
    {
      return _detector_desc_.get_gg_locator();
    }

    void base_tracking_driver::reset()
    {
      _initialized_ = false;
      _at_reset(); // Inherited hook
      _reset_();
      return;
    }

    void base_tracking_driver::initialize(const datatools::properties & config_,
					  datatools::service_manager & services_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error, "Already initialized !");
      _initialize_(config_, services_);
      _at_initialize(config_, services_); // Inherited hook
      _initialized_ = true;
      return;
    }

    void base_tracking_driver::_reset_()
    {
      DT_THROW_IF(not is_initialized(), std::logic_error, "Not initialized !");
      _clear_working_arrays(); // Clear working arrays
      _preClusterizer_ = snreco::detail::GeigerTimePartitioner{};
      _cellSelector_.reset();
      return;
    }

    void base_tracking_driver::_initialize_(const datatools::properties & config_,
					    datatools::service_manager & services_)
    {
      // Verbosity:
      if (config_.has_key("verbosity")) {
        auto lp = datatools::logger::get_priority(config_.fetch_string("verbosity"));
        set_verbosity(lp);
      }
      if (datatools::logger::is_debug(get_verbosity())) {
	boost::property_tree::ptree popts;
	popts.put("indent", "[debug] ");
	popts.put("title", "Tracking Driver configuration");
	config_.print_tree(std::clog, popts);
      }
 
      // Extract the config of the base tracking :
      falaise::property_set ps{config_};
      
      // Configuration of the detector_description_
      auto servicesPs = ps.get<falaise::property_set>("services", {});
      uint32_t detector_description_flags = detector_description::configuration_flags_from(servicesPs);
      // Ensure geometry:
      detector_description_flags |= detector_description::required_geometry;
      // Ensure gg locator:
      detector_description_flags |= detector_description::required_gg_locator;
      _detector_desc_ = detector_description::make_from_services(services_, detector_description_flags);
      DT_LOG_DEBUG(get_verbosity(), "Detector description:");
      if (datatools::logger::is_debug(get_verbosity())) {
	_detector_desc_.print(std::cerr, "[debug] ");
      }
      DT_THROW_IF(not _detector_desc_.has_geometry_manager(), std::logic_error,
		  "Missing geometry manager !");
      DT_THROW_IF(not _detector_desc_.get_geometry_manager().is_initialized(),
		  std::logic_error,
                  "Geometry manager is not initialized !");
      DT_THROW_IF(not _detector_desc_.has_gg_locator(), std::logic_error,
		  "Missing Geiger locator !");
 
      // Cell geom_id mask
      auto cell_id_mask_rules = ps.get<std::string>("cell_id_mask_rules", "");
      if (not cell_id_mask_rules.empty()) {
        _cellSelector_.set_id_mgr(_detector_desc_.get_geometry_manager().get_id_mgr());
        _cellSelector_.initialize(cell_id_mask_rules);
      }

      // Configure time partitioning :
      _preClusterizer_ =
	snreco::detail::GeigerTimePartitioner(ps.get<falaise::time_t>("TPC.delayed_hit_cluster_time",
								      {10.0, "microsecond"})(),
					      ps.get<bool>("TPC.processing_prompt_hits", true),
					      ps.get<bool>("TPC.processing_delayed_hits", true),
					      ps.get<bool>("TPC.split_chamber", true));
      return;
    }

    void base_tracking_driver::_clear_working_arrays()
    {
      _ignoredHits_.clear();
      _preclusters_.clear();
      return;
    }

    bool base_tracking_driver::has_event_timestamp() const
    {
      return time::is_valid(_eventTimestamp_);
    }

    const snemo::time::time_point & base_tracking_driver::get_event_timestamp() const
    {
      return _eventTimestamp_;
    }

    void base_tracking_driver::set_event_timestamp(const snemo::time::time_point & ets_)
    {
      _eventTimestamp_ = ets_;
      return;
    }

    void base_tracking_driver::reset_event_timestamp()
    {
      time::invalidate(_eventTimestamp_);
      return;
    }
  
    int base_tracking_driver::_prepare_process(const base_tracking_driver::hit_collection_type & gg_hits_,
					       const base_tracking_driver::calo_hit_collection_type & /* calo_hits_ */,
					       snemo::datamodel::tracker_clustering_data & /* clustering_ */,
					       snemo::datamodel::tracker_trajectory_data & /* track_fitting_ */)
    {
      auto logging = get_verbosity();
      // logging = datatools::logger::PRIO_DEBUG;

      // Input data:
      snreco::detail::GeigerHitPtrCollection<hit_type> idata;
      idata.reserve(gg_hits_.size());
      std::map<const hit_type *, hit_handle_type> pre_cluster_mapping;
      // Fill the GeigerTimePartitioner input data model :
      for (const hit_handle_type & gg_handle : gg_hits_) {
        if (not gg_handle.has_data()) {
          continue;
        }
        const geomtools::geom_id & gid = gg_handle->get_geom_id();
        // Drift cell selector:
        if (_cellSelector_.is_initialized() && not _cellSelector_.match(gid)) {
          continue;
        }
        idata.push_back(&(*gg_handle));
        // Mapping between both data models :
        pre_cluster_mapping[&(*gg_handle)] = gg_handle;
      }

      // Invoke pre-clustering algo :
      snreco::detail::GeigerHitTimePartition<hit_type> odata = _preClusterizer_.partition(idata);

      // Populate the input collections :

      // Populate ignored hits :
      _ignoredHits_.reserve(odata.ignoredHits.size());
      DT_LOG_DEBUG(logging, "#ignored hits = " << odata.ignoredHits.size());
      for (const auto & ignored_hit : odata.ignoredHits) {
        _ignoredHits_.push_back(pre_cluster_mapping[ignored_hit]);
      }

      // Populate the collection of input pre-clusters :
      _preclusters_.reserve(odata.promptClusters.size() + odata.delayedClusters.size());
      // Prompt clusters:
      for (const auto & prompt_cluster : odata.promptClusters) {
	_preclusters_.emplace_back(prompt_cluster.at(0)->get_side(), false);
	tracking_precluster & pc = _preclusters_.back();
	pc.hits().reserve(prompt_cluster.size());
	for (const auto & hit : prompt_cluster) {
          pc.hits().push_back(pre_cluster_mapping[hit]);
        }
      }
      // Delayed clusters:
      for (const auto & delayed_cluster : odata.delayedClusters) {
	_preclusters_.emplace_back(delayed_cluster.at(0)->get_side(), true);
	tracking_precluster & pc = _preclusters_.back();
	pc.hits().reserve(delayed_cluster.size());
	for (const auto & hit : delayed_cluster) {
          pc.hits().push_back(pre_cluster_mapping[hit]);
        }
      }

      //XXX
      /*
      // Prompt time clusters :
      _promptClusters_.reserve(odata.promptClusters.size());
      DT_LOG_DEBUG(logging, "#prompt clusters = " << odata.promptClusters.size());
      for (const auto & prompt_cluster : odata.promptClusters) {
        hit_collection_type hc;
        hc.reserve(prompt_cluster.size());
        DT_LOG_DEBUG(logging, "  #hits in prompt clusters = " << prompt_cluster.size());
        for (const auto & hit : prompt_cluster) {
          hc.push_back(pre_cluster_mapping[hit]);
        }
        _promptClusters_.push_back(std::move(hc));
      }

      // Delayed time clusters :
      _delayedClusters_.reserve(odata.delayedClusters.size());
      DT_LOG_DEBUG(logging, "#delayed clusters = " << odata.delayedClusters.size());
      for (const auto & delayed_cluster : odata.delayedClusters) {
        hit_collection_type hc;
        hc.reserve(delayed_cluster.size());
        for (const auto & ihit : delayed_cluster) {
          hc.push_back(pre_cluster_mapping[ihit]);
        }
        _delayedClusters_.push_back(std::move(hc));
      }
      */
      return 0;
    }

    void base_tracking_driver::_post_process_collect_unclustered_hits(const base_tracking_driver::hit_collection_type & gg_hits_,
								      snemo::datamodel::tracker_clustering_data & clustering_)
    {
      namespace snedm = snemo::datamodel;
      for (datatools::handle<snedm::tracker_clustering_solution> &the_solution :
             clustering_.solutions()) {
        std::set<int> clustered_hit_ids;
        for (datatools::handle<snedm::tracker_cluster> & the_cluster : the_solution->get_clusters()) {
          for (datatools::handle<snedm::calibrated_tracker_hit> & the_hit : the_cluster->hits()) {
            clustered_hit_ids.insert(the_hit->get_hit_id());
          }
        }
        for (const datatools::handle<hit_type> & hhit : gg_hits_) {
          int hit_id = hhit->get_hit_id();
          if (clustered_hit_ids.count(hit_id) == 0) {
            // It's unclustered...
            the_solution->get_unclustered_hits().push_back(hhit);
          }
        }
      }
      return;
    }

    int base_tracking_driver::_post_process(const base_tracking_driver::hit_collection_type & gg_hits_,
					    const base_tracking_driver::calo_hit_collection_type & /* calo_hits_ */,
					    snemo::datamodel::tracker_clustering_data & clustering_,
					    snemo::datamodel::tracker_trajectory_data & /* track_fitting_ */)
    {
      _post_process_collect_unclustered_hits(gg_hits_, clustering_);
      return 0;
    }

    int base_tracking_driver::process(const snemo::datamodel::calibrated_data & cd_,
				      snemo::datamodel::tracker_clustering_data & clustering_,
				      snemo::datamodel::tracker_trajectory_data & track_fitting_)
    {
      DT_LOG_DEBUG(get_verbosity(), "Entering...");
      namespace snedm = snemo::datamodel;
      const base_tracking_driver::hit_collection_type & gg_hits = cd_.tracker_hits();
      const base_tracking_driver::calo_hit_collection_type & calo_hits = cd_.calorimeter_hits();
      int status = 0;
      DT_THROW_IF(not is_initialized(), std::logic_error, "Clusterizer '" << _id_ << "' is not initialized !");
      _clear_working_arrays();

      // Zero the output data structures:
      clustering_.clear();
      track_fitting_.clear();
      
      // Run pre-processing with preclustering based on time-coincidence to determine what are prompt hits,
      // what are candidate clusters of delayed hits and partitioned on both sides of the source foil :
      DT_LOG_DEBUG(get_verbosity(), "Input CD data before pre-processing: ");
      DT_LOG_DEBUG(get_verbosity(), "|-- Number of input tracker hits = " << gg_hits.size());
      DT_LOG_DEBUG(get_verbosity(), "`-- Number of input calorimeter hits = " << calo_hits.size());
      status = _prepare_process(gg_hits, calo_hits, clustering_, track_fitting_);
      if (status != 0) {
        DT_LOG_ERROR(get_verbosity(), "Pre-processing has failed !");
        return status;
      }
 
      DT_LOG_DEBUG(get_verbosity(), "Input working data before processing: ");
      DT_LOG_DEBUG(get_verbosity(), "`-- Number of input tracker preclusters = " << _preclusters_.size());
      if (datatools::logger::is_debug(get_verbosity())) {
	std::cerr << "[debug] " << "Input tracker preclusters:\n";
	for (auto ipc = 0u; ipc < _preclusters_.size(); ipc++) {
	  std::cerr << "[debug] ";
	  std::string itemTag; 
	  if (ipc+1 == _preclusters_.size()) {
	    itemTag = "    ";
	    std::cerr << "`-- ";
	  } else {
	    itemTag = "|   ";
	    std::cerr << "|-- ";
	  }
	  std::cerr << "Input precluster #" << ipc << ':' << '\n';
	  _preclusters_[ipc].print(std::cerr, "[debug] " + itemTag);
	}
      }
      // Algorithm:
      status = process_tracking(_preclusters_, calo_hits, clustering_, track_fitting_);
      if (status != 0) {
        DT_LOG_ERROR(get_verbosity(), "Tracking algorithm has failed !");
        return status;
      }

      DT_LOG_DEBUG(get_verbosity(), "Output data before post-processing: ");
      DT_LOG_DEBUG(get_verbosity(), "|-- Number of output clustering solutions = " << clustering_.size());
      DT_LOG_DEBUG(get_verbosity(), "`-- Number of output track fitting solutions = " << track_fitting_.get_number_of_solutions());

      status = _post_process(gg_hits, calo_hits, clustering_, track_fitting_);
      if (status != 0) {
        DT_LOG_ERROR(get_verbosity(), "Post-processing has failed !");
        return status;
      }
      DT_LOG_DEBUG(get_verbosity(), "Output data after post-processing: ");
      DT_LOG_DEBUG(get_verbosity(), "|-- Number of output clustering solutions = " << clustering_.size());
      DT_LOG_DEBUG(get_verbosity(), "`-- Number of output track fitting solutions = " << track_fitting_.get_number_of_solutions());
      return status;
    }

    void base_tracking_driver::tree_dump(std::ostream & out_,
					 const std::string & title_,
					 const std::string & indent_,
					 bool inherit_) const
    {
      if (!title_.empty()) {
        out_ << indent_ << title_ << std::endl;
      }
      out_ << indent_ << datatools::i_tree_dumpable::tag << "Logging : '"
           << datatools::logger::get_priority_label(_verbosity) << "'" << std::endl;
      out_ << indent_ << datatools::i_tree_dumpable::tag << "Initialized : " << std::boolalpha << is_initialized()
           << std::endl;
      out_ << indent_ << datatools::i_tree_dumpable::inherit_tag(inherit_) << "ID : '" << _id_ << "'"
           << std::endl;
      return;
    }

    // static
    void base_tracking_driver::ocd_support(datatools::object_configuration_description & ocd_,
					   const std::string & prefix_)
    {
      datatools::logger::declare_ocd_logging_configuration(ocd_, "fatal", prefix_ + "BTC.");

      // {
      //   // Description of the 'locator_plugin_name' configuration property :
      //   datatools::configuration_property_description &cpd = ocd_.add_property_info();
      //   cpd.set_name_pattern("BTC.locator_plugin_name")
      //     .set_terse_description("The name of the geometry Geiger locator plugin to be used")
      //     .set_from("snemo::processing::base_tracking_driver")
      //     .set_traits(datatools::TYPE_STRING)
      //     .set_long_description("Empty value means automatic search   \n")
      //     .add_example("Set a specific value::                                  \n"
      //                  "                                                        \n"
      //                  "  TC.locator_plugin_name : string = \"locators_driver\" \n"
      //                  "                                                        \n");
      // }

      {
        // Description of the 'TPC.delayed_hit_cluster_time' configuration property :
        datatools::configuration_property_description &cpd = ocd_.add_property_info();
        cpd.set_name_pattern("TPC.delayed_hit_cluster_time")
          .set_terse_description("The minimum time to consider a Geiger hit as delayed")
          .set_from("snemo::processing::base_tracking_driver")
          .set_traits(datatools::TYPE_REAL)
          .set_default_value_real(10 * CLHEP::microsecond, "us")
          .add_example("Set the default value::                          \n"
                       "                                                 \n"
                       "  TPC.delayed_hit_cluster_time : real = 10 us    \n"
                       "                                                 \n");
      }

      {
        // Description of the 'TPC.processing_prompt_hits' configuration property :
        datatools::configuration_property_description &cpd = ocd_.add_property_info();
        cpd.set_name_pattern("TPC.processing_prompt_hits")
          .set_terse_description("Flag to process prompt Geiger hits")
          .set_from("snemo::processing::base_tracking_driver")
          .set_traits(datatools::TYPE_BOOLEAN)
          .set_default_value_boolean(true)
          .add_example("Set the default value::                       \n"
                       "                                              \n"
                       "  TPC.processing_prompt_hits : boolean = true \n"
                       "                                              \n");
      }

      {
        // Description of the 'TPC.processing_delayed_hits' configuration property :
        datatools::configuration_property_description &cpd = ocd_.add_property_info();
        cpd.set_name_pattern("TPC.processing_delayed_hits")
          .set_terse_description("Flag to process delayed Geiger hits")
          .set_from("snemo::processing::base_tracking_driver")
          .set_traits(datatools::TYPE_BOOLEAN)
          .set_default_value_boolean(true)
          .add_example("Set the default value::                       \n"
                       "                                              \n"
                       "  TPC.processing_delayed_hits : boolean = true \n"
                       "                                              \n");
      }

      {
        // Description of the 'TPC.split_chamber' configuration property :
        datatools::configuration_property_description &cpd = ocd_.add_property_info();
        cpd.set_name_pattern("TPC.split_chamber")
          .set_terse_description(
                                 "Flag to process distinctly Geiger hits from different sides of the tracking chamber")
          .set_from("snemo::processing::base_tracking_driver")
          .set_traits(datatools::TYPE_BOOLEAN)
          .set_default_value_boolean(false)
          .add_example("Split chamber in 2 sides::          \n"
                       "                                    \n"
                       "  TPC.split_chamber : boolean = true \n"
                       "                                    \n");
      }
      return;
    }

  } // end of namespace processing

} // end of namespace snemo
