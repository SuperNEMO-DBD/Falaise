/// \file falaise/snemo/processing/base_tracker_clusterizer.cc

// Ourselves:
#include <falaise/snemo/processing/base_tracker_clusterizer.h>

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
#include <falaise/snemo/geometry/gg_locator.h>
#include <falaise/snemo/geometry/locator_helpers.h>
#include <falaise/snemo/geometry/locator_plugin.h>

namespace snemo {

namespace processing {

// Constructor
base_tracker_clusterizer::base_tracker_clusterizer(const std::string &name) {
  id_ = name;
  _set_initialized(false);
  _set_defaults();
}

base_tracker_clusterizer::~base_tracker_clusterizer() = default;

void base_tracker_clusterizer::_set_defaults() {
  _logging_priority = datatools::logger::PRIO_WARNING;
  geoManager_ = nullptr;
  geigerLocator_ = nullptr;
}

void base_tracker_clusterizer::_reset() {
  _set_initialized(false);

  // Clear working arrays:
  _clear_working_arrays();
  preClusterer_ = snreco::detail::GeigerTimePartitioner{};
  cellSelector_.reset();

  // Reset configuration params:
  _set_defaults();
}

datatools::logger::priority base_tracker_clusterizer::get_logging_priority() const {
  return _logging_priority;
}

void base_tracker_clusterizer::set_logging_priority(datatools::logger::priority priority_) {
  DT_THROW_IF(priority_ == datatools::logger::PRIO_UNDEFINED, std::logic_error,
              "Invalid logging priority level !");
  _logging_priority = priority_;
}

const std::string &base_tracker_clusterizer::get_id() const { return id_; }

bool base_tracker_clusterizer::is_initialized() const { return isInitialized_; }

void base_tracker_clusterizer::_set_initialized(bool i_) { isInitialized_ = i_; }

const snemo::geometry::gg_locator &base_tracker_clusterizer::get_gg_locator() const {
  return *geigerLocator_;
}

void base_tracker_clusterizer::_initialize(const datatools::properties &setup_) {
  DT_THROW_IF(is_initialized(), std::logic_error, "Already initialized !");
  DT_THROW_IF(!has_geometry_manager(), std::logic_error, "Missing geometry manager !");
  DT_THROW_IF(!geoManager_->is_initialized(), std::logic_error,
              "Geometry manager is not initialized !");

  // Extract the setup of the base tracker fitter :
  falaise::property_set localPS{setup_};
  auto ps = localPS.get<falaise::property_set>("BTC", {});

  // Logging priority:
  auto lp = datatools::logger::get_priority(ps.get<std::string>("logging.priority", "warning"));
  set_logging_priority(lp);

  // Get the Geiger cell locator from geometry plugins :
  auto locator_plugin_name = ps.get<std::string>("locator_plugin_name", "");
  auto snLocator = snemo::geometry::getSNemoLocator(get_geometry_manager(), locator_plugin_name);
  geigerLocator_ = &(snLocator->geigerLocator());

  // Cell geom_id mask
  auto cell_id_mask_rules = ps.get<std::string>("cell_id_mask_rules", "");
  if (!cell_id_mask_rules.empty()) {
    cellSelector_.set_id_mgr(get_geometry_manager().get_id_mgr());
    cellSelector_.initialize(cell_id_mask_rules);
  }

  // Configure time partitioning :
  preClusterer_ = snreco::detail::GeigerTimePartitioner(
      ps.get<falaise::time_t>("TPC.delayed_hit_cluster_time", {10.0, "microsecond"})(),
      ps.get<bool>("TPC.processing_prompt_hits", true),
      ps.get<bool>("TPC.processing_delayed_hits", true), ps.get<bool>("TPC.split_chamber", false));
}

void base_tracker_clusterizer::_clear_working_arrays() {
  ignoredHits_.clear();
  promptClusters_.clear();
  delayedClusters_.clear();
}

void base_tracker_clusterizer::set_geometry_manager(const geomtools::manager &gmgr_) {
  DT_THROW_IF(is_initialized(), std::logic_error, "Already initialized !");
  // Initialization stuff:
  const std::string &geo_setup_label = gmgr_.get_setup_label();
  DT_THROW_IF(
      geo_setup_label != "snemo::demonstrator" && geo_setup_label != "snemo::tracker_commissioning",
      std::logic_error, "Invalid geometry setup label '" << geo_setup_label << "' !");

  geoManager_ = &gmgr_;
}

const geomtools::manager &base_tracker_clusterizer::get_geometry_manager() const {
  DT_THROW_IF(!has_geometry_manager(), std::logic_error, "No geometry manager is setup !");
  return *geoManager_;
}

bool base_tracker_clusterizer::has_geometry_manager() const { return geoManager_ != nullptr; }

int base_tracker_clusterizer::_prepare_process(
    const base_tracker_clusterizer::hit_collection_type &gg_hits,
    const base_tracker_clusterizer::calo_hit_collection_type & /* calo_hits_ */,
    snemo::datamodel::tracker_clustering_data & /* clustering_ */) {
  // Input data
  snreco::detail::GeigerHitPtrCollection<hit_type> idata;
  idata.reserve(gg_hits.size());
  std::map<const hit_type *, hit_handle_type> pre_cluster_mapping;

  // Fill the TrackerPreClustering input data model :
  for (const hit_handle_type &gg_handle : gg_hits) {
    if (!gg_handle.has_data()) {
      continue;
    }
    const geomtools::geom_id &gid = gg_handle->get_geom_id();
    // Drift cell selector
    if (cellSelector_.is_initialized() && !cellSelector_.match(gid)) {
      continue;
    }
    idata.push_back(&(*gg_handle));
    // Mapping between both data models (what, What, WHAT?) :
    pre_cluster_mapping[&(*gg_handle)] = gg_handle;
  }

  // Invoke pre-clusterizing algo :
  snreco::detail::GeigerHitTimePartition<hit_type> odata = preClusterer_.partition(idata);

  // Repopulate the collections of pre-clusters :

  // Ignored hits :
  ignoredHits_.reserve(odata.ignoredHits.size());
  for (const auto &ignored_hit : odata.ignoredHits) {
    ignoredHits_.push_back(pre_cluster_mapping[ignored_hit]);
  }

  // Prompt time clusters :
  promptClusters_.reserve(odata.promptClusters.size());
  for (const auto &prompt_cluster : odata.promptClusters) {
    hit_collection_type hc;
    hc.reserve(prompt_cluster.size());
    for (const auto &hit : prompt_cluster) {
      hc.push_back(pre_cluster_mapping[hit]);
    }
    promptClusters_.push_back(std::move(hc));
  }

  // Delayed time clusters :
  delayedClusters_.reserve(odata.delayedClusters.size());
  for (const auto &delayed_cluster : odata.delayedClusters) {
    hit_collection_type hc;
    hc.reserve(delayed_cluster.size());
    for (const auto &ihit : delayed_cluster) {
      hc.push_back(pre_cluster_mapping[ihit]);
    }
    delayedClusters_.push_back(std::move(hc));
  }
  return 0;
}

void base_tracker_clusterizer::_post_process_collect_unclustered_hits(
    const base_tracker_clusterizer::hit_collection_type &gg_hits_,
    snemo::datamodel::tracker_clustering_data &clustering_) {
  namespace snedm = snemo::datamodel;

  for (datatools::handle<snedm::tracker_clustering_solution> &the_solution :
       clustering_.get_solutions()) {
    std::set<int> clustered_hit_ids;
    for (datatools::handle<snedm::tracker_cluster> &the_cluster : the_solution->get_clusters()) {
      for (datatools::handle<snedm::calibrated_tracker_hit> &the_hit : the_cluster->get_hits()) {
        clustered_hit_ids.insert(the_hit->get_hit_id());
      }
    }

    for (const datatools::handle<hit_type> &hhit : gg_hits_) {
      int hit_id = hhit->get_hit_id();
      if (clustered_hit_ids.count(hit_id) == 0) {
        // It's unclustered...
        the_solution->get_unclustered_hits().push_back(hhit);
      }
    }
  }
}

int base_tracker_clusterizer::_post_process(
    const base_tracker_clusterizer::hit_collection_type &gg_hits_,
    const base_tracker_clusterizer::calo_hit_collection_type & /* calo_hits_ */,
    snemo::datamodel::tracker_clustering_data &clustering_) {
  _post_process_collect_unclustered_hits(gg_hits_, clustering_);
  return 0;
}

int base_tracker_clusterizer::process(
    const base_tracker_clusterizer::hit_collection_type &gg_hits_,
    const base_tracker_clusterizer::calo_hit_collection_type &calo_hits_,
    snemo::datamodel::tracker_clustering_data &clustering_) {
  namespace snedm = snemo::datamodel;
  int status = 0;
  DT_THROW_IF(!is_initialized(), std::logic_error,
              "Clusterizer '" << id_ << "' is not initialized !");
  _clear_working_arrays();

  clustering_.invalidate_solutions();

  // Run pre-processing based on time-coincidence to determine what are prompt hits,
  // what are candidate clusters of delayed hits :
  status = _prepare_process(gg_hits_, calo_hits_, clustering_);
  if (status != 0) {
    DT_LOG_ERROR(get_logging_priority(), "Pre-processing based on time-coincidence has failed !");
    return status;
  }

  std::vector<snedm::tracker_clustering_data> prompt_work_clusterings;
  prompt_work_clusterings.reserve(2);

  // Process prompt time-clusters :
  if (preClusterer_.classifiesPromptHits()) {
    // Invoke the clustering algorithms on each prompt clusters :
    for (const hit_collection_type &prompt_clusters : promptClusters_) {
      prompt_work_clusterings.emplace_back(snedm::tracker_clustering_data{});
      status = _process_algo(prompt_clusters, calo_hits_, prompt_work_clusterings.back());
      if (status != 0) {
        DT_LOG_ERROR(get_logging_priority(),
                     "Processing of prompt hits by '" << id_ << "' algorithm has failed !");
        return status;
      }
    }

    if (promptClusters_.empty()) {
      DT_LOG_DEBUG(get_logging_priority(), "No cluster of prompt hits to be processed !");
    } else if (promptClusters_.size() == 1) {
      // In this case, only one clustering algorithm has been performed on
      // only one side of the tracking chamber or on both sides in a single shot:
      snedm::tracker_clustering_data &prompt_cd = prompt_work_clusterings[0];
      clustering_.get_solutions().reserve(prompt_cd.get_number_of_solutions());

      for (size_t isol = 0; isol < prompt_cd.get_number_of_solutions(); isol++) {
        auto h_tc_sol = datatools::make_handle<snedm::tracker_clustering_solution>();
        h_tc_sol->set_solution_id(isol);
        h_tc_sol->get_auxiliaries().store_flag(snedm::tracker_clustering_data::prompt_key());
        const snedm::tracker_clustering_solution &prompt_sol = prompt_cd.get_solution(isol);
        snedm::tracker_clustering_solution::copy_one_solution_in_one(prompt_sol, *h_tc_sol);

        if (prompt_sol.get_auxiliaries().has_key(
                snedm::tracker_clustering_data::clusterizer_id_key())) {
          h_tc_sol->get_auxiliaries().store_string(
              snedm::tracker_clustering_data::clusterizer_id_key(),
              prompt_sol.get_auxiliaries().fetch_string(
                  snedm::tracker_clustering_data::clusterizer_id_key()));
        }

        clustering_.add_solution(h_tc_sol);
      }
    } else if (promptClusters_.size() == 2) {
      // We merge the two clusterings in as many as solutions are needed to take into
      // account the combinatory with both sides of the source:
      snedm::tracker_clustering_data &prompt_cd0 = prompt_work_clusterings[0];
      snedm::tracker_clustering_data &prompt_cd1 = prompt_work_clusterings[1];
      unsigned int nb_prompt_sol0 = prompt_cd0.get_number_of_solutions();
      unsigned int nb_prompt_sol1 = prompt_cd1.get_number_of_solutions();
      unsigned int nb_sols = nb_prompt_sol0 * nb_prompt_sol1;

      // Build all combinaisons of solutions from solutions found from both sides
      // of the source:
      for (size_t isol = 0; isol < nb_sols; ++isol) {
        auto h_tc_sol = datatools::make_handle<snedm::tracker_clustering_solution>();
        h_tc_sol->set_solution_id(isol);
        h_tc_sol->get_auxiliaries().store_flag(snedm::tracker_clustering_data::prompt_key());
        int isol0 = isol % nb_prompt_sol0;
        int isol1 = isol / nb_prompt_sol0;
        const snedm::tracker_clustering_solution &prompt_sol0 = prompt_cd0.get_solution(isol0);
        const snedm::tracker_clustering_solution &prompt_sol1 = prompt_cd1.get_solution(isol1);
        snedm::tracker_clustering_solution::merge_two_solutions_in_ones(prompt_sol0, prompt_sol1,
                                                                        *h_tc_sol);
        if (prompt_sol0.get_auxiliaries().has_key(
                snedm::tracker_clustering_data::clusterizer_id_key())) {
          h_tc_sol->get_auxiliaries().store_string(
              snedm::tracker_clustering_data::clusterizer_id_key(),
              prompt_sol0.get_auxiliaries().fetch_string(
                  snedm::tracker_clustering_data::clusterizer_id_key()));
        }

        clustering_.add_solution(h_tc_sol);
      }

    } else {
      DT_THROW_IF(true, std::logic_error,
                  "Cannot handle case with more than 2 prompt input clusters!");
    }
  }

  // Process delayed time-clusters :
  if (preClusterer_.classifiesDelayedHits()) {
    std::vector<snedm::tracker_clustering_data> delayed_work_clusterings;
    // Process delayed time-clusters :
    {
      for (const hit_collection_type &delayed_cluster : delayedClusters_) {
        snedm::tracker_clustering_data workingTCD;
        status = _process_algo(delayed_cluster, calo_hits_, workingTCD);
        if (status != 0) {
          DT_LOG_ERROR(get_logging_priority(),
                       "Processing of delayed hits by '" << id_ << "' algorithm has failed !");
          return status;
        }
        delayed_work_clusterings.push_back(std::move(workingTCD));
      }
    }

    for (size_t idelayed_clustering = 0; idelayed_clustering < delayed_work_clusterings.size();
         idelayed_clustering++) {
      snedm::tracker_clustering_data &delayed_cd = delayed_work_clusterings[idelayed_clustering];
      for (size_t idelayed_sol = 0; idelayed_sol < delayed_cd.get_number_of_solutions();
           idelayed_sol++) {
        // Extract the solution from the clustering result:
        const snedm::tracker_clustering_solution &delayed_sol =
            delayed_cd.get_solution(idelayed_sol);
        // Create a new clustering solution
        auto h_tc_sol = datatools::make_handle<snedm::tracker_clustering_solution>();
        // Give it an unique solution id:
        h_tc_sol->set_solution_id(clustering_.get_solutions().size() + idelayed_sol);
        // Record the delayed time-cluster unique Idd solution:
        h_tc_sol->get_auxiliaries().store_integer(snedm::tracker_clustering_data::delayed_id_key(),
                                                  idelayed_clustering);
        snedm::tracker_clustering_solution::copy_one_solution_in_one(delayed_sol, *h_tc_sol);
        if (delayed_sol.get_auxiliaries().has_key(
                snedm::tracker_clustering_data::clusterizer_id_key())) {
          h_tc_sol->get_auxiliaries().store_string(
              snedm::tracker_clustering_data::clusterizer_id_key(),
              delayed_sol.get_auxiliaries().fetch_string(
                  snedm::tracker_clustering_data::clusterizer_id_key()));
        }
        // Flag it as a delayed clustering solution:
        h_tc_sol->get_auxiliaries().store_flag(snedm::tracker_clustering_data::delayed_key());
        for (datatools::handle<snedm::tracker_cluster> &icluster : h_tc_sol->get_clusters()) {
          icluster->make_delayed();
        }

        clustering_.add_solution(h_tc_sol);
      }
    }
  }

  const bool merge_prompt_delayed_solutions = true;
  if (merge_prompt_delayed_solutions) {
    snedm::tracker_clustering_data::solution_col_type &the_solutions = clustering_.get_solutions();
    for (auto isol = the_solutions.begin(); isol != the_solutions.end(); ++isol) {
      snedm::tracker_clustering_solution &sol_prompt = *(*isol);
      datatools::properties &aux_prompt = sol_prompt.get_auxiliaries();

      if (!aux_prompt.has_flag(snedm::tracker_clustering_data::prompt_key())) {
        continue;
      }

      for (auto jsol = std::next(isol); jsol != the_solutions.end(); ++jsol) {
        snedm::tracker_clustering_solution &sol_delayed = *(*jsol);
        datatools::properties &aux_delayed = sol_delayed.get_auxiliaries();

        if (!aux_delayed.has_flag(snedm::tracker_clustering_data::delayed_key())) {
          continue;
        }

        aux_prompt.unset_flag(snedm::tracker_clustering_data::prompt_key());
        snedm::tracker_clustering_solution::copy_one_solution_in_one(sol_delayed, sol_prompt);
      }
    }
    // Delete all delayed solutions (use erase(remove_if)?)
    for (auto isol = the_solutions.begin(); isol != the_solutions.end(); /*++isol*/) {
      if ((*isol)->get_auxiliaries().has_flag(snedm::tracker_clustering_data::delayed_key())) {
        isol = the_solutions.erase(isol);
      } else {
        ++isol;
      }
    }
  }

  _post_process(gg_hits_, calo_hits_, clustering_);
  return status;
}

void base_tracker_clusterizer::tree_dump(std::ostream &out_, const std::string &title_,
                                         const std::string &indent, bool inherit_) const {
  if (!title_.empty()) {
    out_ << indent << title_ << std::endl;
  }

  out_ << indent << datatools::i_tree_dumpable::tag << "Logging          : '"
       << datatools::logger::get_priority_label(_logging_priority) << "'" << std::endl;
  out_ << indent << datatools::i_tree_dumpable::tag << "Initialized      : " << is_initialized()
       << std::endl;
  out_ << indent << datatools::i_tree_dumpable::tag << "Geometry manager : " << geoManager_
       << std::endl;
  if (geoManager_ != nullptr) {
    out_ << indent << datatools::i_tree_dumpable::tag << "Geometry setup label   : '"
         << geoManager_->get_setup_label() << "'" << std::endl;
    out_ << indent << datatools::i_tree_dumpable::tag << "Geometry setup version : '"
         << geoManager_->get_setup_version() << "'" << std::endl;
  }
  out_ << indent << datatools::i_tree_dumpable::inherit_tag(inherit_) << "End." << std::endl;
}

// static
void base_tracker_clusterizer::ocd_support(datatools::object_configuration_description &ocd_,
                                           const std::string &prefix_) {
  datatools::logger::declare_ocd_logging_configuration(ocd_, "fatal", prefix_ + "BTC.");

  {
    // Description of the 'locator_plugin_name' configuration property :
    datatools::configuration_property_description &cpd = ocd_.add_property_info();
    cpd.set_name_pattern("BTC.locator_plugin_name")
        .set_terse_description("The name of the geometry Geiger locator plugin to be used")
        .set_from("snemo::processing::base_tracker_clusterizer")
        .set_traits(datatools::TYPE_STRING)
        .set_long_description("Empty value means automatic search   \n")
        .add_example(
            "Set a specific value::                                  \n"
            "                                                        \n"
            "  TC.locator_plugin_name : string = \"locators_driver\" \n"
            "                                                        \n");
  }

  {
    // Description of the 'TPC.delayed_hit_cluster_time' configuration property :
    datatools::configuration_property_description &cpd = ocd_.add_property_info();
    cpd.set_name_pattern("TPC.delayed_hit_cluster_time")
        .set_terse_description("The minimum time to consider a Geiger hit as delayed")
        .set_from("snemo::processing::base_tracker_clusterizer")
        .set_traits(datatools::TYPE_REAL)
        .set_default_value_real(10 * CLHEP::microsecond, "us")
        .add_example(
            "Set the default value::                          \n"
            "                                                 \n"
            "  TPC.delayed_hit_cluster_time : real = 10 us    \n"
            "                                                 \n");
  }

  {
    // Description of the 'TPC.processing_prompt_hits' configuration property :
    datatools::configuration_property_description &cpd = ocd_.add_property_info();
    cpd.set_name_pattern("TPC.processing_prompt_hits")
        .set_terse_description("Flag to process prompt Geiger hits")
        .set_from("snemo::processing::base_tracker_clusterizer")
        .set_traits(datatools::TYPE_BOOLEAN)
        .set_default_value_boolean(true)
        .add_example(
            "Set the default value::                       \n"
            "                                              \n"
            "  TPC.processing_prompt_hits : boolean = 1    \n"
            "                                              \n");
  }

  {
    // Description of the 'TPC.processing_delayed_hits' configuration property :
    datatools::configuration_property_description &cpd = ocd_.add_property_info();
    cpd.set_name_pattern("TPC.processing_delayed_hits")
        .set_terse_description("Flag to process delayed Geiger hits")
        .set_from("snemo::processing::base_tracker_clusterizer")
        .set_traits(datatools::TYPE_BOOLEAN)
        .set_default_value_boolean(true)
        .add_example(
            "Set the default value::                       \n"
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
        .set_from("snemo::processing::base_tracker_clusterizer")
        .set_traits(datatools::TYPE_BOOLEAN)
        .set_default_value_boolean(false)
        .add_example(
            "Split chamber in 2 sides::          \n"
            "                                    \n"
            "  TPC.split_chamber : boolean = 1   \n"
            "                                    \n");
  }
}

}  // namespace processing

}  // end of namespace snemo
