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

namespace {

  /*
  // These are used to mark properties, but are not needed/used elsewhere, so to be removed
  const std::string & prompt_key() {
    static const std::string _key("prompt");
    return _key;
  }

  // static
  const std::string & delayed_key() {
    static const std::string _key("delayed");
    return _key;
  }

  // static
  const std::string & delayed_id_key() {
    static const std::string _key("delayed.id");
    return _key;
  }

  // static
  const std::string & clusterizer_id_key() {
    static const std::string _key("clusterizer.id");
    return _key;
  }
  */
  // ----- ABOVE TO BE MOVED -----

} // namespace

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

    const snemo::geometry::gg_locator & base_tracking_driver::get_gg_locator() const
    {
      return *_geigerLocator_;
    }

    void base_tracking_driver::reset()
    {
      _initialized_ = false;
      _at_reset(); // Inherited hook
      _reset_();
      return;
    }

    void base_tracking_driver::initialize(const datatools::properties & setup_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error, "Already initialized !");
      _initialize_(setup_);
      _at_initialize(setup_); // Inherited hook
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

    void base_tracking_driver::_initialize_(const datatools::properties & setup_)
    {
      DT_THROW_IF(not has_geometry_manager(), std::logic_error, "Missing geometry manager !");
      DT_THROW_IF(not _geoManager_->is_initialized(), std::logic_error,
                  "Geometry manager is not initialized !");

      // Verbosity:
      if (setup_.has_key("verbosity")) {
        auto lp = datatools::logger::get_priority(setup_.fetch_string("verbosity"));
        set_verbosity(lp);
      }
  
      // Extract the setup of the base tracking :
      falaise::property_set localPS{setup_};
      auto ps = localPS.get<falaise::property_set>("BT", {});

      // Access the Geiger cell locator from geometry plugin :
      auto locator_plugin_name = ps.get<std::string>("locator_plugin_name", "");
      auto snLocator = snemo::geometry::getSNemoLocator(get_geometry_manager(), locator_plugin_name);
      _geigerLocator_ = &(snLocator->geigerLocator());

      // Cell geom_id mask
      auto cell_id_mask_rules = ps.get<std::string>("cell_id_mask_rules", "");
      if (not cell_id_mask_rules.empty()) {
        _cellSelector_.set_id_mgr(get_geometry_manager().get_id_mgr());
        _cellSelector_.initialize(cell_id_mask_rules);
      }

      // Configure time partitioning :
      _preClusterizer_ = snreco::detail::GeigerTimePartitioner(ps.get<falaise::time_t>("TPC.delayed_hit_cluster_time",
										       {10.0, "microsecond"})(),
							       ps.get<bool>("TPC.processing_prompt_hits", true),
							       ps.get<bool>("TPC.processing_delayed_hits", true),
							       ps.get<bool>("TPC.split_chamber", true));
      return;
    }

    void base_tracking_driver::_clear_working_arrays()
    {
      _ignoredHits_.clear();
      _promptClusters_.clear();
      _delayedClusters_.clear();
      _preclusters_.clear();
      return;
    }

    void base_tracking_driver::set_geometry_manager(const geomtools::manager & gmgr_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error, "Already initialized !");
      // Initialization stuff:
      const std::string & geo_setup_label = gmgr_.get_setup_label();
      DT_THROW_IF(geo_setup_label != "snemo::demonstrator" && geo_setup_label != "snemo::tracker_commissioning",
                  std::logic_error, "Invalid geometry setup label '" << geo_setup_label << "' !");
      _geoManager_ = &gmgr_;
      return;
    }

    const geomtools::manager & base_tracking_driver::get_geometry_manager() const
    {
      DT_THROW_IF(not has_geometry_manager(), std::logic_error, "No geometry manager is setup !");
      return *_geoManager_;
    }

    bool base_tracking_driver::has_geometry_manager() const { return _geoManager_ != nullptr; }

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
	_preclusters_.emplace_back(0, false);
	tracking_precluster & pc = _preclusters_.back();
	pc.hits().reserve(prompt_cluster.size());
	for (const auto & hit : prompt_cluster) {
          pc.hits().push_back(pre_cluster_mapping[hit]);
        }
      }
      // Delayed clusters:
      for (const auto & delayed_cluster : odata.delayedClusters) {
	_preclusters_.emplace_back(0, true);
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

    int base_tracking_driver::process(const base_tracking_driver::hit_collection_type & gg_hits_,
				      const base_tracking_driver::calo_hit_collection_type & calo_hits_,
				      snemo::datamodel::tracker_clustering_data & clustering_,
				      snemo::datamodel::tracker_trajectory_data & track_fitting_)
    {
      DT_LOG_DEBUG(get_verbosity(), "Entering...");
      namespace snedm = snemo::datamodel;
      int status = 0;
      DT_THROW_IF(not is_initialized(), std::logic_error, "Clusterizer '" << _id_ << "' is not initialized !");
      _clear_working_arrays();

      // Zero the output data structures:
      clustering_.clear();
      track_fitting_.clear();
      
      // Run pre-processing with preclustering based on time-coincidence to determine what are prompt hits,
      // what are candidate clusters of delayed hits and partitioned on both sides of the source foil :
      status = _prepare_process(gg_hits_, calo_hits_, clustering_, track_fitting_);
      if (status != 0) {
        DT_LOG_ERROR(get_verbosity(), "Pre-processing has failed !");
        return status;
      }
 
      // Algorithm:
      status = process_tracking(_preclusters_, calo_hits_, clustering_, track_fitting_);
      if (status != 0) {
        DT_LOG_ERROR(get_verbosity(), "Tracking algorithm has failed !");
        return status;
      }
      
      status = _post_process(gg_hits_, calo_hits_, clustering_, track_fitting_);
      if (status != 0) {
        DT_LOG_ERROR(get_verbosity(), "Post-processing has failed !");
        return status;
      }
      return status;
    }

      /*
      std::vector<snedm::tracker_clustering_data> prompt_work_clusterings;
      prompt_work_clusterings.reserve(2);
      std::vector<snedm::tracker_trajectory_data> prompt_work_track_fittings;
      prompt_work_track_fittings.reserve(2);

      // Process prompt time-clusters :
      if (_preClusterizer_.classifiesPromptHits()) {
        int iCl = 0;
        // Invoke the clustering algorithms on each set of prompt hits :
        for (const hit_collection_type & prompt_hits : _promptClusters_) {
          DT_LOG_DEBUG(get_verbosity(), "Process tracking for set of prompt hits #" << iCl);
          prompt_work_clusterings.emplace_back(snedm::tracker_clustering_data{});
          prompt_work_track_fittings.emplace_back(snedm::tracker_trajectory_data{});
          status = _process_algo(prompt_hits,
				 calo_hits_,
				 prompt_work_clusterings.back(),
				 prompt_work_track_fittings.back());
          if (status != 0) {
            DT_LOG_ERROR(get_verbosity(),
                         "Processing of prompt hits by '" << _id_ << "' algorithm has failed !");
            return status;
          }
          iCl++;
        }

        if (_promptClusters_.empty()) {
          DT_LOG_DEBUG(get_verbosity(), "No cluster of prompt hits to be processed !");
        } else if (_promptClusters_.size() == 1) {
          // In this case, only one tracking algorithm has been performed on
          // only one side of the tracking chamber or on both sides in a single shot:
	  // Clustering:
          snedm::tracker_clustering_data & prompt_tcd = prompt_work_clusterings[0];
          // clustering_.solutions().reserve(prompt_tcd.size());
          for (size_t isol = 0u; isol < prompt_tcd.size(); isol++) {
            clustering_.append_solution(datatools::make_handle<snedm::tracker_clustering_solution>());
            auto & h_tc_sol = clustering_.solutions().back(); 
            h_tc_sol->set_solution_id(isol);
            h_tc_sol->get_auxiliaries().store_flag(prompt_key());
            const snedm::tracker_clustering_solution & prompt_sol = prompt_tcd.at(isol);
            snedm::tracker_clustering_solution::copy_one_solution_in_other(prompt_sol, *h_tc_sol);
            h_tc_sol->get_auxiliaries().store_string(clusterizer_id_key(), get_id());
          }
	  // // Track fitting:
	  // snedm::tracker_trajectory_data & prompt_ttd = prompt_work_track_fittings[0];
	  // track_fitting_.get_solutions().reserve(prompt_ttd.size());
	  // for (size_t isol = 0u; isol < prompt_ttd.get_number_of_solutions(); isol++) {
	  //   auto h_tt_sol = datatools::make_handle<snedm::tracker_trajectory_solution>();
          //   const snedm::tracker_trajectory_solution & prompt_sol = prompt_ttd.at(isol);
	  //   *h_tt_sol = prompt_sol;
          //   h_tt_sol->set_solution_id(isol);
	  //   h_tt_sol->get_auxiliaries().store_flag(prompt_key());
          //   // snedm::tracker_trajectory_solution::copy_one_solution_in_other(prompt_sol, *h_tt_sol);
	  // }
       } else if (_promptClusters_.size() == 2) {
          // We merge the two tracking in as many as solutions are needed to take into
          // account the combinatory with both sides of the source:
	  // Clustering:
	  snedm::tracker_clustering_data & prompt_cd0 = prompt_work_clusterings[0];
          snedm::tracker_clustering_data & prompt_cd1 = prompt_work_clusterings[1];
          unsigned int nb_prompt_sol0 = prompt_cd0.size();
          unsigned int nb_prompt_sol1 = prompt_cd1.size();
          unsigned int nb_sols = nb_prompt_sol0 * nb_prompt_sol1;
          // Build all combinaisons of solutions from solutions found from both sides
          // of the source:
          for (size_t isol = 0; isol < nb_sols; ++isol) {
	    clustering_.append_solution(datatools::make_handle<snedm::tracker_clustering_solution>());
            auto & h_tc_sol = clustering_.solutions().back();
            h_tc_sol->set_solution_id(isol);
            h_tc_sol->get_auxiliaries().store_flag(prompt_key());
            int isol0 = isol % nb_prompt_sol0;
            int isol1 = isol / nb_prompt_sol0;
            const snedm::tracker_clustering_solution & prompt_sol0 = prompt_cd0.at(isol0);
            const snedm::tracker_clustering_solution & prompt_sol1 = prompt_cd1.at(isol1);
            snedm::tracker_clustering_solution::merge_two_solutions_in_other(prompt_sol0,
									     prompt_sol1,
									     *h_tc_sol);
            h_tc_sol->get_auxiliaries().store_string(clusterizer_id_key(), get_id());
          }
	  // Track fitting:
	  
        } else {
          DT_THROW_IF(true, std::logic_error,
                      "Cannot handle case with more than 2 prompt input clusters!");
        }
      }

      // Process delayed time-clusters :
      if (_preClusterizer_.classifiesDelayedHits()) {
        std::vector<snedm::tracker_clustering_data> delayed_work_clusterings;
        std::vector<snedm::tracker_trajectory_data> delayed_work_track_fittings;
        // Process delayed time-clusters :
        {
          int iCl = 0;
          for (const hit_collection_type & delayed_cluster : _delayedClusters_) {
            snedm::tracker_clustering_data workingTCD;
            snedm::tracker_trajectory_data workingTTD;
            DT_LOG_DEBUG(get_verbosity(), "Process clustering for delayed cluster #" << iCl);
            status = _process_algo(delayed_cluster, calo_hits_, workingTCD, workingTTD);
            if (status != 0) {
              DT_LOG_ERROR(get_verbosity(),
                           "Processing of delayed hits by '" << _id_ << "' algorithm has failed !");
              return status;
            }
            delayed_work_clusterings.push_back(std::move(workingTCD));
            delayed_work_track_fittings.push_back(std::move(workingTTD));
            iCl++;
          }
        }

        for (size_t idelayed_clustering = 0; idelayed_clustering < delayed_work_clusterings.size();
             idelayed_clustering++) {
          snedm::tracker_clustering_data & delayed_cd = delayed_work_clusterings[idelayed_clustering];
          for (size_t idelayed_sol = 0; idelayed_sol < delayed_cd.size(); idelayed_sol++) {
            // Extract the solution from the clustering result:
            const snedm::tracker_clustering_solution &delayed_sol = delayed_cd.at(idelayed_sol);
            // Create a new clustering solution
            clustering_.append_solution(datatools::make_handle<snedm::tracker_clustering_solution>());
            auto & h_tc_sol = clustering_.solutions().back();
            // Give it an unique solution id:
            h_tc_sol->set_solution_id(clustering_.size() + idelayed_sol);
            // Record the delayed time-cluster unique Idd solution:
            h_tc_sol->get_auxiliaries().store_integer(delayed_id_key(), idelayed_clustering);
            snedm::tracker_clustering_solution::copy_one_solution_in_other(delayed_sol, *h_tc_sol);
            h_tc_sol->get_auxiliaries().store_string(clusterizer_id_key(), get_id());
            // Flag it as a delayed clustering solution:
            h_tc_sol->get_auxiliaries().store_flag(delayed_key());
            for (datatools::handle<snedm::tracker_cluster> &icluster : h_tc_sol->get_clusters()) {
              icluster->make_delayed();
            }
          }
        }
      }
      */

      /*
      const bool merge_prompt_delayed_solutions = true;
      if (merge_prompt_delayed_solutions) {
        snedm::TrackerClusteringSolutionHdlCollection & the_solutions = clustering_.solutions();
        for (auto isol = the_solutions.begin(); isol != the_solutions.end(); ++isol) {
          snedm::tracker_clustering_solution & sol_prompt = *(*isol);
          datatools::properties & aux_prompt = sol_prompt.get_auxiliaries();
          if (!aux_prompt.has_flag(prompt_key())) {
            continue;
          }
          for (auto jsol = std::next(isol); jsol != the_solutions.end(); ++jsol) {
            snedm::tracker_clustering_solution & sol_delayed = *(*jsol);
            datatools::properties & aux_delayed = sol_delayed.get_auxiliaries();
            if (!aux_delayed.has_flag(delayed_key())) {
              continue;
            }
            aux_prompt.unset_flag(prompt_key());
            snedm::tracker_clustering_solution::copy_one_solution_in_other(sol_delayed, sol_prompt);
          }
        }
        // Delete all delayed solutions (use erase(remove_if)?)
        for (auto isol = the_solutions.begin(); isol != the_solutions.end();) {
          if ((*isol)->get_auxiliaries().has_flag(delayed_key())) {
            isol = the_solutions.erase(isol);
          } else {
            ++isol;
          }
        }
      }
      */

    void base_tracking_driver::tree_dump(std::ostream & out_,
					 const std::string & title_,
					 const std::string & indent,
					 bool inherit_) const
    {
      if (!title_.empty()) {
        out_ << indent << title_ << std::endl;
      }
      out_ << indent << datatools::i_tree_dumpable::tag << "Logging          : '"
           << datatools::logger::get_priority_label(_verbosity) << "'" << std::endl;
      out_ << indent << datatools::i_tree_dumpable::tag << "Initialized      : " << is_initialized()
           << std::endl;
      out_ << indent << datatools::i_tree_dumpable::tag << "Geometry manager : " << _geoManager_
           << std::endl;
      if (_geoManager_ != nullptr) {
        out_ << indent << datatools::i_tree_dumpable::tag << "Geometry setup label   : '"
             << _geoManager_->get_setup_label() << "'" << std::endl;
        out_ << indent << datatools::i_tree_dumpable::tag << "Geometry setup version : '"
             << _geoManager_->get_setup_version() << "'" << std::endl;
      }
      out_ << indent << datatools::i_tree_dumpable::inherit_tag(inherit_) << "End." << std::endl;
      return;
    }

    // static
    void base_tracking_driver::ocd_support(datatools::object_configuration_description & ocd_,
					   const std::string & prefix_)
    {
      datatools::logger::declare_ocd_logging_configuration(ocd_, "fatal", prefix_ + "BTC.");

      {
        // Description of the 'locator_plugin_name' configuration property :
        datatools::configuration_property_description &cpd = ocd_.add_property_info();
        cpd.set_name_pattern("BTC.locator_plugin_name")
          .set_terse_description("The name of the geometry Geiger locator plugin to be used")
          .set_from("snemo::processing::base_tracking_driver")
          .set_traits(datatools::TYPE_STRING)
          .set_long_description("Empty value means automatic search   \n")
          .add_example("Set a specific value::                                  \n"
                       "                                                        \n"
                       "  TC.locator_plugin_name : string = \"locators_driver\" \n"
                       "                                                        \n");
      }

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
                       "  TPC.processing_prompt_hits : boolean = 1    \n"
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
                       "  TPC.processing_delayed_hits : boolean = 1   \n"
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
                       "  TPC.split_chamber : boolean = 1   \n"
                       "                                    \n");
      }
      return;
    }

  } // end of namespace processing

} // end of namespace snemo
