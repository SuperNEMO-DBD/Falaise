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
#include <falaise/snemo/datamodels/tracker_clustering_data.h>
#include <falaise/snemo/geometry/gg_locator.h>
#include <falaise/snemo/geometry/locator_plugin.h>

namespace snemo {

namespace processing {

datatools::logger::priority base_tracker_clusterizer::get_logging_priority() const {
  return _logging_priority;
}

void base_tracker_clusterizer::set_logging_priority(datatools::logger::priority priority_) {
  _logging_priority = priority_;
  return;
}

const std::string &base_tracker_clusterizer::get_id() const { return _id_; }

bool base_tracker_clusterizer::is_initialized() const { return _initialized_; }

void base_tracker_clusterizer::_set_initialized(bool i_) {
  _initialized_ = i_;
  return;
}

const snemo::geometry::gg_locator &base_tracker_clusterizer::get_gg_locator() const {
  return *_gg_locator_;
}

void base_tracker_clusterizer::_initialize(const datatools::properties &setup_) {
  DT_THROW_IF(is_initialized(), std::logic_error, "Already initialized !");

  DT_THROW_IF(_geometry_manager_ == 0, std::logic_error, "Missing geometry manager !");
  DT_THROW_IF(!_geometry_manager_->is_initialized(), std::logic_error,
              "Geometry manager is not initialized !");

  // Extract the setup of the base tracker fitter :
  datatools::properties btc_setup;
  setup_.export_and_rename_starting_with(btc_setup, "BTC.", "");

  // Logging priority:
  datatools::logger::priority lp = datatools::logger::extract_logging_configuration(btc_setup);
  DT_THROW_IF(lp == datatools::logger::PRIO_UNDEFINED, std::logic_error,
              "Invalid logging priority level !");
  set_logging_priority(lp);

  // Initialization stuff:
  const std::string &geo_setup_label = _geometry_manager_->get_setup_label();
  DT_THROW_IF(
      geo_setup_label != "snemo::demonstrator" && geo_setup_label != "snemo::tracker_commissioning",
      std::logic_error, "Invalid geometry setup label '" << geo_setup_label << "' !");

  // Get the Geiger cell locator from geometry plugins :
  const geomtools::manager &geo_mgr = get_geometry_manager();

  // Locator plugin:
  std::string locator_plugin_name;
  if (btc_setup.has_key("locator_plugin_name")) {
    locator_plugin_name = btc_setup.fetch_string("locator_plugin_name");
  }

  // If no locator plugin name is set, then search for the first one
  if (locator_plugin_name.empty()) {
    const geomtools::manager::plugins_dict_type &plugins = geo_mgr.get_plugins();
    for (geomtools::manager::plugins_dict_type::const_iterator ip = plugins.begin();
         ip != plugins.end(); ip++) {
      const std::string &plugin_name = ip->first;
      if (geo_mgr.is_plugin_a<snemo::geometry::locator_plugin>(plugin_name)) {
        DT_LOG_DEBUG(get_logging_priority(), "Find locator plugin with name = " << plugin_name);
        locator_plugin_name = plugin_name;
        break;
      }
    }
  }
  // Access to a given plugin by name and type :
  if (geo_mgr.has_plugin(locator_plugin_name) &&
      geo_mgr.is_plugin_a<snemo::geometry::locator_plugin>(locator_plugin_name)) {
    DT_LOG_NOTICE(get_logging_priority(),
                  "Found locator plugin named '" << locator_plugin_name << "'");
    const snemo::geometry::locator_plugin &locplug =
        geo_mgr.get_plugin<snemo::geometry::locator_plugin>(locator_plugin_name);
    // Set the Geiger cell locator :
    _gg_locator_ = &(locplug.get_gg_locator());
  }
  if (get_logging_priority() >= datatools::logger::PRIO_DEBUG) {
    DT_LOG_DEBUG(get_logging_priority(), "Geiger locator :");
    _gg_locator_->tree_dump(std::clog, "", "[debug]: ");
  }

  // Cell geom_id mask
  if (btc_setup.has_key("cell_id_mask_rules")) {
    const std::string cell_id_mask_rules = btc_setup.fetch_string("cell_id_mask_rules");
    _cell_id_selector_.set_id_mgr(get_geometry_manager().get_id_mgr());
    _cell_id_selector_.initialize(cell_id_mask_rules);
  }

  // Default value for the TrackerPreClustering :
  _tpc_setup_data_.logging = get_logging_priority();
  _tpc_setup_data_.delayed_hit_cluster_time = 10.0 * CLHEP::microsecond;
  _tpc_setup_data_.cell_size = get_gg_locator().get_cell_diameter();
  _tpc_setup_data_.processing_prompt_hits = true;
  _tpc_setup_data_.processing_delayed_hits = true;
  _tpc_setup_data_.split_chamber = false;

  double default_time_unit = CLHEP::microsecond;

  // Delayed hit minimum time :
  if (setup_.has_key("TPC.delayed_hit_cluster_time")) {
    double delayed_hit_cluster_time = setup_.fetch_real("TPC.delayed_hit_cluster_time");
    if (!setup_.has_explicit_unit("TPC.delayed_hit_cluster_time")) {
      delayed_hit_cluster_time *= default_time_unit;
    }
    _tpc_setup_data_.delayed_hit_cluster_time = delayed_hit_cluster_time;
  }

  if (setup_.has_key("TPC.processing_prompt_hits")) {
    _tpc_setup_data_.processing_prompt_hits = setup_.fetch_boolean("TPC.processing_prompt_hits");
  }

  if (setup_.has_key("TPC.processing_delayed_hits")) {
    _tpc_setup_data_.processing_delayed_hits = setup_.fetch_boolean("TPC.processing_delayed_hits");
  }

  if (setup_.has_key("TPC.split_chamber")) {
    _tpc_setup_data_.split_chamber = setup_.fetch_boolean("TPC.split_chamber");
  }

  // Check :
  DT_THROW_IF(
      !_tpc_setup_data_.check(), std::logic_error,
      "Invalid TrackerPreClustering setup data : " << _tpc_setup_data_.get_last_error_message());

  // Configure the algorithm :
  _pc_.initialize(_tpc_setup_data_);

  return;
}

void base_tracker_clusterizer::_clear_working_arrays() {
  _ignored_hits_.clear();
  _prompt_time_clusters_.clear();
  _delayed_time_clusters_.clear();
  return;
}

void base_tracker_clusterizer::_reset() {
  _set_initialized(false);

  // Clear working arrays:
  _clear_working_arrays();
  _tpc_setup_data_.reset();
  _pc_.reset();
  _cell_id_selector_.reset();

  // Reset configuration params:
  _set_defaults();
  return;
}

void base_tracker_clusterizer::set_geometry_manager(const geomtools::manager &gmgr_) {
  DT_THROW_IF(is_initialized(), std::logic_error, "Already initialized !");
  _geometry_manager_ = &gmgr_;
  return;
}

const geomtools::manager &base_tracker_clusterizer::get_geometry_manager() const {
  DT_THROW_IF(!has_geometry_manager(), std::logic_error, "No geometry manager is setup !");
  return *_geometry_manager_;
}

bool base_tracker_clusterizer::has_geometry_manager() const { return _geometry_manager_ != 0; }

void base_tracker_clusterizer::_set_defaults() {
  _logging_priority = datatools::logger::PRIO_WARNING;
  _geometry_manager_ = 0;
  _gg_locator_ = 0;
  return;
}

// Constructor
base_tracker_clusterizer::base_tracker_clusterizer(const std::string &id_) {
  _id_ = id_;
  _set_initialized(false);
  _set_defaults();
  return;
}

base_tracker_clusterizer::~base_tracker_clusterizer() {
  if (_initialized_) {
    _reset();
  }
  return;
}

void base_tracker_clusterizer::tree_dump(std::ostream &out_, const std::string &title_,
                                         const std::string &indent_, bool inherit_) const {
  std::string indent;
  if (!indent_.empty()) {
    indent = indent_;
  }
  if (!title_.empty()) {
    out_ << indent_ << title_ << std::endl;
  }

  out_ << indent << datatools::i_tree_dumpable::tag << "Logging          : '"
       << datatools::logger::get_priority_label(_logging_priority) << "'" << std::endl;
  out_ << indent << datatools::i_tree_dumpable::tag << "Initialized      : " << is_initialized()
       << std::endl;
  out_ << indent << datatools::i_tree_dumpable::tag << "Geometry manager : " << _geometry_manager_
       << std::endl;
  if (_geometry_manager_) {
    out_ << indent << datatools::i_tree_dumpable::tag << "Geometry setup label   : '"
         << _geometry_manager_->get_setup_label() << "'" << std::endl;
    out_ << indent << datatools::i_tree_dumpable::tag << "Geometry setup version : '"
         << _geometry_manager_->get_setup_version() << "'" << std::endl;
  }
  out_ << indent << datatools::i_tree_dumpable::inherit_tag(inherit_) << "End." << std::endl;
  return;
}

int base_tracker_clusterizer::_prepare_process(
    const base_tracker_clusterizer::hit_collection_type &gg_hits_,
    const base_tracker_clusterizer::calo_hit_collection_type & /* calo_hits_ */,
    snemo::datamodel::tracker_clustering_data & /* clustering_ */) {
  /****************
   * Locate cells *
   ****************/
  const base_tracker_clusterizer::hit_collection_type &gg_hits = gg_hits_;
  DT_LOG_DEBUG(get_logging_priority(), "Number of Geiger hits = " << gg_hits.size());
  // Ensure the hits have registered X/Y position :
  BOOST_FOREACH (const snemo::datamodel::calibrated_data::tracker_hit_handle_type &gg_handle,
                 gg_hits) {
    if (!gg_handle.has_data()) continue;
    const snemo::datamodel::calibrated_tracker_hit &sncore_gg_hit = gg_handle.get();
    const geomtools::geom_id &gid = sncore_gg_hit.get_geom_id();
    // Check if X/Y position of the cell is recorded in the event :
    if (!sncore_gg_hit.has_xy()) {
      // if not, we do it :
      geomtools::vector_3d hit_pos;
      DT_LOG_DEBUG(get_logging_priority(), "Compute X/Y position of the hit cell with GID=" << gid);
      get_gg_locator().get_cell_position(gid, hit_pos);
      {
        // locally break the const-ness of the hit to store the X/Y doublet :
        snemo::datamodel::calibrated_tracker_hit &mutable_sncore_gg_hit =
            const_cast<snemo::datamodel::calibrated_tracker_hit &>(sncore_gg_hit);
        mutable_sncore_gg_hit.set_xy(hit_pos.x(), hit_pos.y());
      }
    }
  }

  /********************
   * Pre-clusterizing *
   ********************/

  // Input data
  TrackerPreClustering::input_data<hit_type> idata;
  idata.hits.reserve(gg_hits.size());
  std::map<const hit_type *, hit_handle_type> pre_cluster_mapping;

  // Fill the TrackerPreClustering input data model :
  BOOST_FOREACH (const snemo::datamodel::calibrated_data::tracker_hit_handle_type &gg_handle,
                 gg_hits) {
    if (!gg_handle.has_data()) continue;
    const snemo::datamodel::calibrated_tracker_hit &sncore_gg_hit = gg_handle.get();
    const geomtools::geom_id &gid = sncore_gg_hit.get_geom_id();
    // Drift cell selector
    if (_cell_id_selector_.is_initialized() && !_cell_id_selector_.match(gid)) {
      DT_LOG_TRACE(get_logging_priority(), "Drift cell with geom id '" << gid << "' excluded!");
      continue;
    }
    idata.hits.push_back(&sncore_gg_hit);
    // Mapping between both data models :
    pre_cluster_mapping[&sncore_gg_hit] = gg_handle;
  }

  // TrackerPreClustering output data :
  TrackerPreClustering::output_data<hit_type> odata;

  // Invoke pre-clusterizing algo :
  int status = _pc_.process<hit_type>(idata, odata);
  if (status != TrackerPreClustering::pre_clusterizer::OK) {
    DT_LOG_ERROR(get_logging_priority(), "Pre-clusterization has failed !");
    return 1;
  }

  // Repopulate the collections of pre-clusters :

  // Ignored hits :
  _ignored_hits_.reserve(odata.ignored_hits.size());
  for (size_t ihit = 0; ihit < odata.ignored_hits.size(); ihit++) {
    _ignored_hits_.push_back(pre_cluster_mapping[odata.ignored_hits.at(ihit)]);
  }
  DT_LOG_DEBUG(get_logging_priority(), _ignored_hits_.size()
                                           << " clusters of hits have been ignored");

  // Prompt time clusters :
  _prompt_time_clusters_.reserve(odata.prompt_clusters.size());
  for (size_t icluster = 0; icluster < odata.prompt_clusters.size(); icluster++) {
    // Add a new hit collection as a new prompt time cluster :
    {
      hit_collection_type hc;
      _prompt_time_clusters_.push_back(hc);
    }
    hit_collection_type &hc = _prompt_time_clusters_.back();
    hc.reserve(odata.prompt_clusters[icluster].size());
    for (size_t ihit = 0; ihit < odata.prompt_clusters[icluster].size(); ihit++) {
      hc.push_back(pre_cluster_mapping[odata.prompt_clusters[icluster].at(ihit)]);
    }
  }
  DT_LOG_DEBUG(get_logging_priority(), _prompt_time_clusters_.size()
                                           << " cluster of hits are prompt");

  // Delayed time clusters :
  _delayed_time_clusters_.reserve(odata.delayed_clusters.size());
  for (size_t icluster = 0; icluster < odata.delayed_clusters.size(); icluster++) {
    // Add a new hit collection as a new delayed time cluster :
    {
      hit_collection_type hc;
      _delayed_time_clusters_.push_back(hc);
    }
    hit_collection_type &hc = _delayed_time_clusters_.back();
    hc.reserve(odata.delayed_clusters[icluster].size());
    for (size_t ihit = 0; ihit < odata.delayed_clusters[icluster].size(); ihit++) {
      hc.push_back(pre_cluster_mapping[odata.delayed_clusters[icluster].at(ihit)]);
    }
  }
  DT_LOG_DEBUG(get_logging_priority(), _delayed_time_clusters_.size()
                                           << " cluster of hits are delayed");

  return 0;
}  // end of base_tracker_clusterizer::_prepare_process

// void
// base_tracker_clusterizer::_post_process_ignored_hits(snemo::datamodel::tracker_clustering_data &
// clustering_)
// {
//   // Process ignored hits :
//   for (size_t i = 0; i < _ignored_hits_.size(); i++) {
//     const hit_handle_type & the_hit = _ignored_hits_[i];
//     // All ignored hits are pushed as unclustered hits in
//     // all clustering solutions :
//     for (size_t j = 0; j < clustering_.get_solutions().size(); j++) {
//       snemo::datamodel::tracker_clustering_solution & the_solution =
//         clustering_.grab_solutions()[j].grab();
//       the_solution.grab_unclustered_hits().push_back(the_hit);
//     }
//   }
//   return;
// }

void base_tracker_clusterizer::_post_process_collect_unclustered_hits(
    const base_tracker_clusterizer::hit_collection_type &gg_hits_,
    snemo::datamodel::tracker_clustering_data &clustering_) {
  namespace sdm = snemo::datamodel;

  for (size_t isol = 0; isol < clustering_.get_solutions().size(); isol++) {
    sdm::tracker_clustering_solution &the_solution = clustering_.grab_solutions()[isol].grab();
    std::set<int> clustered_hits_ids;
    for (size_t icluster = 0; icluster < the_solution.get_clusters().size(); icluster++) {
      const sdm::tracker_cluster &the_cluster = the_solution.get_clusters()[icluster].get();
      for (size_t ihit = 0; ihit < the_cluster.get_number_of_hits(); ihit++) {
        const sdm::calibrated_tracker_hit &the_hit = the_cluster.get_hit(ihit);
        int hit_id = the_hit.get_hit_id();
        clustered_hits_ids.insert(hit_id);
        // std::cerr << "  -> clustered hit ID : " << hit_id << std::endl;
      }  // hit loop
    }    // cluster loop
    for (size_t ihit = 0; ihit < gg_hits_.size(); ihit++) {
      const sdm::calibrated_tracker_hit::handle_type &hhit = gg_hits_.at(ihit);
      int hit_id = hhit.get().get_hit_id();
      if (clustered_hits_ids.count(hit_id) == 0) {
        // This hit is not clustered in any of the clusters:
        the_solution.grab_unclustered_hits().push_back(hhit);
        // std::cerr << "  -> unclustered hit ID : " << hit_id << std::endl;
      }
    }  // hit loop
  }
  return;
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
  namespace sdm = snemo::datamodel;
  int status = 0;
  DT_THROW_IF(!is_initialized(), std::logic_error,
              "Clusterizer '" << _id_ << "' is not initialized !");
  _clear_working_arrays();

  clustering_.invalidate_solutions();

  // Run pre-processing based on time-coincidence to determine what are prompt hits,
  // what are candidate clusters of delayed hits :
  status = _prepare_process(gg_hits_, calo_hits_, clustering_);
  if (status != 0) {
    DT_LOG_ERROR(get_logging_priority(), "Pre-processing based on time-coincidence has failed !");
    return status;
  }

  std::vector<sdm::tracker_clustering_data> prompt_work_clusterings;
  prompt_work_clusterings.reserve(2);

  // Process prompt time-clusters :
  if (_tpc_setup_data_.processing_prompt_hits) {
    // Invoke the clustering algorithms on each prompt clusters :
    for (size_t i = 0; i < _prompt_time_clusters_.size(); i++) {
      const hit_collection_type &prompt_clusters = _prompt_time_clusters_[i];
      {
        sdm::tracker_clustering_data dummy;
        prompt_work_clusterings.push_back(dummy);
      }
      status = _process_algo(prompt_clusters, calo_hits_, prompt_work_clusterings.back());
      if (status != 0) {
        DT_LOG_ERROR(get_logging_priority(),
                     "Processing of prompt hits by '" << _id_ << "' algorithm has failed !");
        return status;
      }
    }

    if (_prompt_time_clusters_.size() == 0) {
      DT_LOG_DEBUG(get_logging_priority(), "No cluster of prompt hits to be processed !");
    } else if (_prompt_time_clusters_.size() == 1) {
      // In this case, only one clustering algorithm has been performed on
      // only one side of the tracking chamber or on both sides in a single shot:
      sdm::tracker_clustering_data &prompt_cd = prompt_work_clusterings[0];
      clustering_.grab_solutions().reserve(prompt_cd.get_number_of_solutions());
      for (size_t isol = 0; isol < prompt_cd.get_number_of_solutions(); isol++) {
        sdm::tracker_clustering_solution::handle_type h_tc_sol(
            new sdm::tracker_clustering_solution);
        sdm::tracker_clustering_solution &tc_sol = h_tc_sol.grab();
        tc_sol.set_solution_id(isol);
        tc_sol.grab_auxiliaries().store_flag(sdm::tracker_clustering_data::prompt_key());
        const sdm::tracker_clustering_solution &prompt_sol = prompt_cd.get_solution(isol);
        sdm::tracker_clustering_solution::copy_one_solution_in_one(prompt_sol, tc_sol);
        if (prompt_sol.get_auxiliaries().has_key(
                sdm::tracker_clustering_data::clusterizer_id_key())) {
          tc_sol.grab_auxiliaries().store_string(
              sdm::tracker_clustering_data::clusterizer_id_key(),
              prompt_sol.get_auxiliaries().fetch_string(
                  sdm::tracker_clustering_data::clusterizer_id_key()));
        }
        clustering_.add_solution(h_tc_sol);
      }
    } else if (_prompt_time_clusters_.size() == 2) {
      // We merge the two clusterings in as many as solutions are needed to take into
      // account the combinatory with both sides of the source:
      sdm::tracker_clustering_data &prompt_cd0 = prompt_work_clusterings[0];
      sdm::tracker_clustering_data &prompt_cd1 = prompt_work_clusterings[1];
      unsigned int nb_prompt_sol0 = prompt_cd0.get_number_of_solutions();
      unsigned int nb_prompt_sol1 = prompt_cd1.get_number_of_solutions();
      unsigned int nb_sols = nb_prompt_sol0 * nb_prompt_sol1;

      // Build all combinaisons of solutions from solutions found from both sides
      // of the source:
      for (size_t isol = 0; isol < nb_sols; isol++) {
        sdm::tracker_clustering_solution::handle_type h_tc_sol(
            new sdm::tracker_clustering_solution);
        sdm::tracker_clustering_solution &tc_sol = h_tc_sol.grab();
        tc_sol.set_solution_id(isol);
        tc_sol.grab_auxiliaries().store_flag(sdm::tracker_clustering_data::prompt_key());
        int isol0 = isol % nb_prompt_sol0;
        int isol1 = isol / nb_prompt_sol0;
        const sdm::tracker_clustering_solution &prompt_sol0 = prompt_cd0.get_solution(isol0);
        const sdm::tracker_clustering_solution &prompt_sol1 = prompt_cd1.get_solution(isol1);
        sdm::tracker_clustering_solution::merge_two_solutions_in_ones(prompt_sol0, prompt_sol1,
                                                                      tc_sol);
        if (prompt_sol0.get_auxiliaries().has_key(
                sdm::tracker_clustering_data::clusterizer_id_key())) {
          tc_sol.grab_auxiliaries().store_string(
              sdm::tracker_clustering_data::clusterizer_id_key(),
              prompt_sol0.get_auxiliaries().fetch_string(
                  sdm::tracker_clustering_data::clusterizer_id_key()));
        }
        clustering_.add_solution(h_tc_sol);
      }

    } else {
      DT_THROW_IF(true, std::logic_error,
                  "Cannot handle case with more than 2 prompt input clusters!");
    }
  }

  // Process delayed time-clusters :
  if (_tpc_setup_data_.processing_delayed_hits) {
    std::vector<sdm::tracker_clustering_data> delayed_work_clusterings;
    // Process delayed time-clusters :
    {
      for (size_t i = 0; i < _delayed_time_clusters_.size(); i++) {
        const hit_collection_type &delayed_cluster = _delayed_time_clusters_[i];
        {
          sdm::tracker_clustering_data dummy;
          delayed_work_clusterings.push_back(dummy);
        }
        status = _process_algo(delayed_cluster, calo_hits_, delayed_work_clusterings.back());
        if (status != 0) {
          DT_LOG_ERROR(get_logging_priority(),
                       "Processing of delayed hits by '" << _id_ << "' algorithm has failed !");
          return status;
        }
      }
    }

    for (size_t idelayed_clustering = 0; idelayed_clustering < delayed_work_clusterings.size();
         idelayed_clustering++) {
      sdm::tracker_clustering_data &delayed_cd = delayed_work_clusterings[idelayed_clustering];
      for (size_t idelayed_sol = 0; idelayed_sol < delayed_cd.get_number_of_solutions();
           idelayed_sol++) {
        // Extract the solution from the clustering result:
        const sdm::tracker_clustering_solution &delayed_sol = delayed_cd.get_solution(idelayed_sol);
        // Create a new clustering solution
        sdm::tracker_clustering_solution::handle_type h_tc_sol(
            new sdm::tracker_clustering_solution);
        sdm::tracker_clustering_solution &tc_sol = h_tc_sol.grab();
        // Give it an unique solution id:
        tc_sol.set_solution_id(clustering_.grab_solutions().size() + idelayed_sol);
        // Record the delayed time-cluster unique Idd solution:
        tc_sol.grab_auxiliaries().store_integer(sdm::tracker_clustering_data::delayed_id_key(),
                                                idelayed_clustering);
        sdm::tracker_clustering_solution::copy_one_solution_in_one(delayed_sol, tc_sol);
        if (delayed_sol.get_auxiliaries().has_key(
                sdm::tracker_clustering_data::clusterizer_id_key())) {
          tc_sol.grab_auxiliaries().store_string(
              sdm::tracker_clustering_data::clusterizer_id_key(),
              delayed_sol.get_auxiliaries().fetch_string(
                  sdm::tracker_clustering_data::clusterizer_id_key()));
        }
        // Flag it as a delayed clustering solution:
        tc_sol.grab_auxiliaries().store_flag(sdm::tracker_clustering_data::delayed_key());
        for (sdm::tracker_clustering_solution::cluster_col_type::iterator icluster =
                 tc_sol.grab_clusters().begin();
             icluster != tc_sol.grab_clusters().end(); ++icluster) {
          icluster->grab().make_delayed();
        }
        clustering_.add_solution(h_tc_sol);
      }
    }
  }

  const bool merge_prompt_delayed_solutions = true;
  if (merge_prompt_delayed_solutions) {
    sdm::tracker_clustering_data::solution_col_type &the_solutions = clustering_.grab_solutions();
    for (sdm::tracker_clustering_data::solution_col_type::iterator isol = the_solutions.begin();
         isol != the_solutions.end(); ++isol) {
      sdm::tracker_clustering_solution &sol_prompt = isol->grab();
      datatools::properties &aux_prompt = sol_prompt.grab_auxiliaries();
      if (!aux_prompt.has_flag(sdm::tracker_clustering_data::prompt_key())) continue;
      for (sdm::tracker_clustering_data::solution_col_type::iterator jsol = boost::next(isol);
           jsol != the_solutions.end(); ++jsol) {
        sdm::tracker_clustering_solution &sol_delayed = jsol->grab();
        datatools::properties &aux_delayed = sol_delayed.grab_auxiliaries();
        if (!aux_delayed.has_flag(sdm::tracker_clustering_data::delayed_key())) continue;
        aux_prompt.unset_flag(sdm::tracker_clustering_data::prompt_key());
        // aux_delayed.unset_flag(sdm::tracker_clustering_data::delayed_key());
        sdm::tracker_clustering_solution::copy_one_solution_in_one(sol_delayed, sol_prompt);
      }
    }
    // Delete all delayed solutions
    for (sdm::tracker_clustering_data::solution_col_type::iterator isol = the_solutions.begin();
         isol != the_solutions.end();
         /*++isol*/) {
      if (isol->get().get_auxiliaries().has_flag(sdm::tracker_clustering_data::delayed_key())) {
        isol = the_solutions.erase(isol);
      } else {
        ++isol;
      }
    }
  }

  _post_process(gg_hits_, calo_hits_, clustering_);
  return status;
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

  return;
}

}  // end of namespace processing

}  // end of namespace snemo
