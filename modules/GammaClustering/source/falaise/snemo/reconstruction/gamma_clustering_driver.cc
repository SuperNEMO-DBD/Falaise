/// \file falaise/snemo/reconstruction/gamma_clustering_driver.cc

// Ourselves:
#include <snemo/reconstruction/gamma_clustering_driver.h>

// Standard library:
#include <stdexcept>
#include <sstream>

// Third party:
//- GSL:
#include <gsl/gsl_cdf.h>
// - Bayeux/datatools:
#include <bayeux/datatools/service_manager.h>
// - Bayeux/geomtools:
#include <bayeux/geomtools/geometry_service.h>
#include <bayeux/geomtools/manager.h>

// This project:
#include <falaise/snemo/datamodels/data_model.h>
#include <falaise/snemo/datamodels/particle_track_data.h>
#include <falaise/snemo/processing/services.h>
#include <falaise/snemo/geometry/locator_plugin.h>
#include <falaise/snemo/geometry/calo_locator.h>
#include <falaise/snemo/geometry/xcalo_locator.h>
#include <falaise/snemo/geometry/gveto_locator.h>

namespace snemo {

  namespace reconstruction {

    const std::string & gamma_clustering_driver::gamma_clustering_id()
    {
      static const std::string _id("gamma_clustering");
      return _id;
    }

    // Constructor
    gamma_clustering_driver::gamma_clustering_driver() :
      snemo::processing::base_gamma_builder(gamma_clustering_driver::gamma_clustering_id())
    {
      _set_defaults();
      return;
    }

    // Destructor
    gamma_clustering_driver::~gamma_clustering_driver()
    {
      if (is_initialized()) {
        reset();
      }
      return;
    }

    void gamma_clustering_driver::_set_defaults()
    {
      base_gamma_builder::_set_defaults();

      _cluster_time_range_ = 6 * CLHEP::ns;
      _cluster_grid_mask_ = "first";
      _min_prob_ = 1e-3 * CLHEP::perCent;
      _sigma_time_good_calo_ = 2.5 * CLHEP::ns;
      return;
    }

    // Initialization :
    void gamma_clustering_driver::initialize(const datatools::properties  & setup_)
    {
      this->snemo::processing::base_gamma_builder::_initialize(setup_);

      // Extract the setup of the base gamma builder :
      datatools::properties gc_setup;
      setup_.export_and_rename_starting_with(gc_setup, "GC.", "");

      std::string key;
      if (gc_setup.has_key(key = "cluster_time_range")) {
        _cluster_time_range_ = gc_setup.fetch_real(key);
        if (! gc_setup.has_explicit_unit(key)) {
          _cluster_time_range_ *= CLHEP::ns;
        }
      }

      if (gc_setup.has_key(key = "cluster_grid_mask")) {
        _cluster_grid_mask_ = gc_setup.fetch_string(key);
      }

      if (gc_setup.has_key(key = "minimal_internal_probability")) {
        _min_prob_ = gc_setup.fetch_real(key);
        if (! gc_setup.has_explicit_unit(key)) {
          _min_prob_ *= CLHEP::perCent;
        }
      }

      if (gc_setup.has_key(key = "sigma_time_good_calo")) {
        _sigma_time_good_calo_ = gc_setup.fetch_real(key);
        if (! gc_setup.has_explicit_unit(key)) {
          _sigma_time_good_calo_ *= CLHEP::ns;
        }
      }

      _set_initialized(true);
      return;
    }

    void gamma_clustering_driver::reset()
    {
      this->snemo::processing::base_gamma_builder::_reset();
      _set_defaults();
      return;
    }

    int gamma_clustering_driver::_process_algo(const base_gamma_builder::hit_collection_type & calo_hits_,
                                               snemo::datamodel::particle_track_data & ptd_)
    {
      DT_LOG_TRACE(get_logging_priority(), "Entering...");

      // Registered calorimeter hits (to be skipped)
      gid_list_type registered_calos;

      // Getting gamma clusters
      cluster_collection_type the_reconstructed_clusters;
      for (snemo::datamodel::calibrated_calorimeter_hit::collection_type::const_iterator
             icalo = calo_hits_.begin(); icalo != calo_hits_.end(); ++icalo) {
        const snemo::datamodel::calibrated_calorimeter_hit & a_calo_hit = icalo->get();

        const geomtools::geom_id & a_gid = a_calo_hit.get_geom_id();
        // If already clustered then skip it
        if (std::find(registered_calos.begin(), registered_calos.end(), a_gid)
            != registered_calos.end())
          continue;

        {
          // Insert empty cluster
          DT_LOG_TRACE(get_logging_priority(), "Insert new gamma cluster (#"
                       << the_reconstructed_clusters.size() << ")");
          cluster_type dummy;
          the_reconstructed_clusters.push_back(dummy);
        }

        cluster_type & a_cluster = the_reconstructed_clusters.back();
        a_cluster.insert(std::make_pair(a_calo_hit.get_time(), *icalo));

        // Get geometrical neighbours given the current geom id
        _get_geometrical_neighbours(a_calo_hit, calo_hits_, a_cluster, registered_calos);

        // Ensure all calorimeter hits within a cluster are in time
        _get_time_neighbours(a_cluster, the_reconstructed_clusters);
      }

      if (get_logging_priority() >= datatools::logger::PRIO_TRACE) {
        for (size_t i = 0; i < the_reconstructed_clusters.size(); ++i) {
          const cluster_type & a_cluster = the_reconstructed_clusters.at(i);
          DT_LOG_TRACE(get_logging_priority(), "New gamma cluster #" << i
                       << " (" << a_cluster.size() << " associated calorimeters)");
          for (cluster_type::const_iterator j = a_cluster.begin(); j != a_cluster.end(); ++j) {
            const snemo::datamodel::calibrated_calorimeter_hit & a_calo_hit = j->second.get();
            a_calo_hit.tree_dump();
          }
        }
      }

      /*A : Carry on with tracking*/
      cluster_collection_type the_reconstructed_gammas;
      _get_tof_association(the_reconstructed_clusters, the_reconstructed_gammas);

      /*B : Stop here, with simple clustering*/
      // cluster_collection_type the_reconstructed_gammas = the_reconstructed_clusters;

      // Set new particles within 'particle track data' container
      for (size_t i = 0; i < the_reconstructed_gammas.size(); ++i) {
        DT_LOG_TRACE(get_logging_priority(), "Adding a new clustered gamma");
        snemo::datamodel::particle_track::handle_type hPT(new snemo::datamodel::particle_track);
        ptd_.add_particle(hPT);
        hPT.grab().set_track_id(ptd_.get_number_of_particles());
        hPT.grab().set_charge(snemo::datamodel::particle_track::neutral);

        const cluster_type & a_cluster = the_reconstructed_gammas.at(i);
        for (cluster_type::const_iterator j = a_cluster.begin(); j != a_cluster.end(); ++j) {
          const snemo::datamodel::calibrated_calorimeter_hit & a_calo_hit = j->second.get();
          hPT.grab().grab_associated_calorimeter_hits().push_back(j->second);

          const geomtools::geom_id & a_gid = a_calo_hit.get_geom_id();

          // Build calorimeter vertices
          snemo::datamodel::particle_track::handle_spot hBS(new geomtools::blur_spot);
          hPT.grab().grab_vertices().push_back(hBS);
          geomtools::blur_spot & spot = hBS.grab();
          spot.set_hit_id(a_calo_hit.get_hit_id());
          spot.set_geom_id(a_gid);

          const snemo::geometry::calo_locator & calo_locator   = base_gamma_builder::get_calo_locator();
          const snemo::geometry::xcalo_locator & xcalo_locator = base_gamma_builder::get_xcalo_locator();
          const snemo::geometry::gveto_locator & gveto_locator = base_gamma_builder::get_gveto_locator();

          geomtools::vector_3d position;
          std::string label;
          if (calo_locator.is_calo_block_in_current_module(a_gid)) {
            calo_locator.get_block_position(a_gid, position);
            const double offset = 45.5 * CLHEP::mm;
            if (calo_locator.extract_side(a_gid) == snemo::geometry::utils::SIDE_BACK) {
              position.setX(position.x()-offset);
            } else {
              position.setX(position.x()+offset);
            }
            label = snemo::datamodel::particle_track::vertex_on_main_calorimeter_label();
          } else if (xcalo_locator.is_calo_block_in_current_module(a_gid)) {
            xcalo_locator.get_block_position(a_gid, position);
            label = snemo::datamodel::particle_track::vertex_on_x_calorimeter_label();
          } else if (gveto_locator.is_calo_block_in_current_module(a_gid)) {
            gveto_locator.get_block_position(a_gid, position);
            label = snemo::datamodel::particle_track::vertex_on_gamma_veto_label();
          } else {
            DT_THROW_IF(true, std::logic_error,
                        "Current geom id '" << a_gid << "' does not match any scintillator block !");
          }
          spot.grab_auxiliaries().store(snemo::datamodel::particle_track::vertex_type_key(),
                                        label);
          spot.set_blur_dimension(geomtools::blur_spot::dimension_three);
          spot.set_position(position);
        }
      }

      DT_LOG_DEBUG(get_logging_priority(), "Number of clusters : " << the_reconstructed_clusters.size());
      DT_LOG_DEBUG(get_logging_priority(), "Number of gammas : " << the_reconstructed_gammas.size());
      DT_LOG_TRACE(get_logging_priority(), "Exiting.");
      return 0;
    }

    void gamma_clustering_driver::_get_geometrical_neighbours(const snemo::datamodel::calibrated_calorimeter_hit & hit_,
                                                              const snemo::datamodel::calibrated_data::calorimeter_hit_collection_type & hits_,
                                                              cluster_type & cluster_,
                                                              gid_list_type & registered_calos_) const
    {
      const geomtools::geom_id & a_gid = hit_.get_geom_id();

      // If already clustered then skip it
      if (std::find(registered_calos_.begin(), registered_calos_.end(), a_gid)
          != registered_calos_.end())
        return;

      // Store the current calorimeter as registered one
      registered_calos_.push_back(a_gid);
      if (get_logging_priority() >= datatools::logger::PRIO_TRACE) {
        for (gid_list_type::const_iterator i = registered_calos_.begin();
             i != registered_calos_.end(); ++i) {
          DT_LOG_TRACE(get_logging_priority(), "Registered geom id = " << *i);
        }
      }

      gid_list_type the_neighbours;
      const snemo::geometry::calo_locator & calo_locator   = base_gamma_builder::get_calo_locator();
      const snemo::geometry::xcalo_locator & xcalo_locator = base_gamma_builder::get_xcalo_locator();
      const snemo::geometry::gveto_locator & gveto_locator = base_gamma_builder::get_gveto_locator();

      uint8_t mask = snemo::geometry::utils::NEIGHBOUR_NONE;
      if (_cluster_grid_mask_ == "first") {
        mask = snemo::geometry::utils::NEIGHBOUR_FIRST;
      } else if (_cluster_grid_mask_ == "second") {
        mask = snemo::geometry::utils::NEIGHBOUR_FIRST |
          snemo::geometry::utils::NEIGHBOUR_SECOND;
      } else if (_cluster_grid_mask_ == "diagonal") {
        mask = snemo::geometry::utils::NEIGHBOUR_DIAG;
      } else if (_cluster_grid_mask_ == "side") {
        mask = snemo::geometry::utils::NEIGHBOUR_SIDE;
      } else if (_cluster_grid_mask_ == "none") {
        mask = snemo::geometry::utils::NEIGHBOUR_NONE;
      } else {
        DT_THROW_IF(true, std::logic_error, "Unknown neighbour mask '" << _cluster_grid_mask_ << "' !")
          }
      if (calo_locator.is_calo_block_in_current_module(a_gid)) {
        calo_locator.get_neighbours_ids(a_gid, the_neighbours, mask);
      } else if (xcalo_locator.is_calo_block_in_current_module(a_gid)) {
        xcalo_locator.get_neighbours_ids(a_gid, the_neighbours, mask);
      } else if (gveto_locator.is_calo_block_in_current_module(a_gid)) {
        gveto_locator.get_neighbours_ids(a_gid, the_neighbours, mask);
      } else {
        DT_THROW_IF(true, std::logic_error,
                    "Current geom id '" << a_gid << "' does not match any scintillator block !");
      }

      for (gid_list_type::const_iterator ineighbour = the_neighbours.begin();
           ineighbour != the_neighbours.end(); ++ineighbour) {
        const geomtools::geom_id & aa_gid = *ineighbour;
        DT_LOG_TRACE(get_logging_priority(), "Neighbour geom_id " << aa_gid);
        if (std::find(registered_calos_.begin(), registered_calos_.end(), aa_gid)
            != registered_calos_.end()) {
          continue;
        }

        // 2015/02/06 SC : using lambda function in C++14 (really easier)
        // if (std::find_if(cch.begin(), cch.end(), [ineighbour] (auto icalo)
        //                  {return ineighbour == icalo.get().get_geom_id();}) != cch.end())
        // Find if the eight neighbours belong to calibrated calo. hits
        geomtools::base_hit::has_geom_id_predicate hit_pred(a_gid);
        datatools::mother_to_daughter_predicate<geomtools::base_hit,
                                                snemo::datamodel::calibrated_calorimeter_hit> pred_M2D(hit_pred);
        datatools::handle_predicate<snemo::datamodel::calibrated_calorimeter_hit> pred_via_handle(pred_M2D);
        snemo::datamodel::calibrated_calorimeter_hit::collection_type::const_iterator
          found = std::find_if(hits_.begin(), hits_.end(), pred_via_handle);
        if (found == hits_.end()) {
          continue;
        }

        registered_calos_.push_back(a_gid);
        cluster_.insert(std::make_pair(found->get().get_time(), *found));
        _get_geometrical_neighbours(found->get(), hits_, cluster_, registered_calos_);
      }
      return;
    }

    void gamma_clustering_driver::_get_time_neighbours(cluster_type & cluster_,
                                                       cluster_collection_type & clusters_) const
    {
      if (cluster_.size() < 2) return;

      cluster_type::iterator it = cluster_.begin();
      for (;it != cluster_.end(); ++it) {
        const double current_time = it->first;
        const double next_time = boost::next(it)->first;
        const double delta_time = next_time - current_time;
        if (delta_time > _cluster_time_range_) {
          DT_LOG_TRACE(get_logging_priority(),
                       "Delta time > " << _cluster_time_range_/CLHEP::ns << " ns !!");
          break;
        }
      }

      if (it == cluster_.end())
        return;

      // Create a new cluster to be checked later. To be done in this way
      // i.e. create first a new empty cluster and then add it to the collection
      // of clusters otherwise clusters_ stays in a frozen state if the cluster
      // is added first and then filled.
      cluster_type a_cluster;
      a_cluster.insert(boost::next(it), cluster_.end());
      cluster_.erase(boost::next(it), cluster_.end());
      clusters_.push_back(a_cluster);

      _get_time_neighbours(a_cluster, clusters_);
      return;
    }

    void gamma_clustering_driver::_get_tof_association(const cluster_collection_type & the_reconstructed_clusters,
                                                       cluster_collection_type & the_reconstructed_gammas) const
    {
      /*****  Associate clusters from TOF callculations  *****/
      // Store the indices of the two clusters to be later concatenated
      std::map<size_t,size_t> merge_indices;
      for (size_t i = 0; i < the_reconstructed_clusters.size(); ++i) {
        // The candidate for association to next_cluster
        const cluster_type & a_cluster = the_reconstructed_clusters.at(i);

        // Retrieve the last calorimeter for check in quality
        cluster_type::const_reverse_iterator it_head = a_cluster.rbegin();

        // The algo tries to find a tail to a head
        for (; it_head != a_cluster.rend(); ++it_head) {
          const snemo::datamodel::calibrated_calorimeter_hit & a_calo = it_head->second.get();
          if (a_calo.get_sigma_time() < _sigma_time_good_calo_) break;
        }
        if (it_head == a_cluster.rend()) it_head = a_cluster.rbegin();

        // Holds the probability from head->tail and the index of the tail
        std::map<double,size_t> possible_clusters_association;

        for (size_t j = i+1; j < the_reconstructed_clusters.size(); ++j) {
          const cluster_type & next_cluster = the_reconstructed_clusters.at(j);

          cluster_type::const_iterator it_tail = next_cluster.begin();
          if (_are_on_same_wall(it_head->second.get(), it_tail->second.get()))
            continue;

          for (; it_tail != next_cluster.end(); ++it_tail) {
            const snemo::datamodel::calibrated_calorimeter_hit & a_calo = it_tail->second.get();
            if (a_calo.get_sigma_time() < _sigma_time_good_calo_) break;
          }
          if (it_tail == next_cluster.end()) it_tail = next_cluster.begin();

          const snemo::datamodel::calibrated_calorimeter_hit & head_end_calo_hit = it_head->second.get();
          const snemo::datamodel::calibrated_calorimeter_hit & tail_begin_calo_hit = it_tail->second.get();
          const double tof_prob = _get_tof_probability(head_end_calo_hit, tail_begin_calo_hit);
          if (tof_prob > _min_prob_) {
            // Keep all possible solutions
            possible_clusters_association.insert(std::make_pair(tof_prob, j));
          }
        } // end of second loop on cluster

        if (possible_clusters_association.empty()) continue;

        // The probability distribution is flat so above P=50%, there are as much
        // chances for a 51% pair and a 99% to be the correct pair. Here, it
        // arbitrarily chooses the first pair built
        std::map<double,size_t>::const_reverse_iterator it_best_proba
          = possible_clusters_association.rbegin();
        bool tail_already_associated = false;
        for (std::map<size_t,size_t>::const_iterator it_idx = merge_indices.begin();
             it_idx != merge_indices.end(); ++it_idx) {
          if (it_idx->second == it_best_proba->second) {
            tail_already_associated = true;
            break;
          }
        }

        if (!tail_already_associated) {
          merge_indices.insert(std::make_pair(i, it_best_proba->second));
        }
      }// end of first loop on cluster

      // Initialize with all the clusters in the event
      std::vector<size_t> cluster_to_be_considered;
      for (size_t i = 0; i < the_reconstructed_clusters.size(); ++i)
        cluster_to_be_considered.push_back(i);

      for (std::map<size_t,size_t>::const_iterator i_pair = merge_indices.begin();
           i_pair != merge_indices.end(); ++i_pair) {
        size_t i_cluster = i_pair->first;

        // Skip the cluster if it has already been involved in an association before
        if (std::find(cluster_to_be_considered.begin(),
                      cluster_to_be_considered.end(), i_cluster) == cluster_to_be_considered.end())
          continue;

        {
          cluster_type dummy;
          the_reconstructed_gammas.push_back(dummy);
        }
        cluster_type & new_cluster = the_reconstructed_gammas.back();

        // Fill a new cluster made of the concatenation of successive clusters
        while (merge_indices.count(i_cluster)) {
          const size_t i_next_cluster = merge_indices.at(i_cluster);

          const std::vector<size_t>::iterator it1
            = std::find(cluster_to_be_considered.begin(),cluster_to_be_considered.end(),i_cluster);
          if (it1 != cluster_to_be_considered.end()) {
            cluster_to_be_considered.erase(it1);
          }
          const std::vector<size_t>::iterator it2
            = std::find(cluster_to_be_considered.begin(),cluster_to_be_considered.end(),i_next_cluster);
          if (it2 != cluster_to_be_considered.end()) {
            cluster_to_be_considered.erase(it2);
          }

          if (new_cluster.empty()) {
            new_cluster.insert(the_reconstructed_clusters.at(i_cluster).begin(),the_reconstructed_clusters.at(i_cluster).end());
          }
          new_cluster.insert(the_reconstructed_clusters.at(i_next_cluster).begin(),the_reconstructed_clusters.at(i_next_cluster).end());

          i_cluster = i_next_cluster;
        }
      }

      // Add the remaining isolated clusters
      for(std::vector<size_t>::const_iterator i_solo = cluster_to_be_considered.begin();
          i_solo != cluster_to_be_considered.end(); ++i_solo) {
        const cluster_type & new_cluster = the_reconstructed_clusters.at(*i_solo);
        the_reconstructed_gammas.push_back(new_cluster);
      }

      if (get_logging_priority() >= datatools::logger::PRIO_TRACE) {
        for (size_t i = 0; i < the_reconstructed_clusters.size(); ++i) {
          const cluster_type & a_cluster = the_reconstructed_clusters.at(i);
          DT_LOG_TRACE(get_logging_priority(), "Cluster #" << i
                       << " (" << a_cluster.size() << " associated calorimeters)");
          for (cluster_type::const_iterator j = a_cluster.begin(); j != a_cluster.end(); ++j) {
            const snemo::datamodel::calibrated_calorimeter_hit & a_calo_hit = j->second.get();
            a_calo_hit.tree_dump();
          }
        }
      }

      if (get_logging_priority() >= datatools::logger::PRIO_TRACE) {
        for (size_t i = 0; i < the_reconstructed_gammas.size(); ++i) {
          const cluster_type & a_gamma = the_reconstructed_gammas.at(i);
          DT_LOG_TRACE(get_logging_priority(), "Gamma #" << i
                       << " (" << a_gamma.size() << " associated calorimeters)");
          for (cluster_type::const_iterator j = a_gamma.begin(); j != a_gamma.end(); ++j) {
            const snemo::datamodel::calibrated_calorimeter_hit & a_calo_hit = j->second.get();
            a_calo_hit.tree_dump();
          }
        }
      }
    }

    double gamma_clustering_driver::_get_tof_probability(const snemo::datamodel::calibrated_calorimeter_hit & head_end_calo_hit_,
                                                         const snemo::datamodel::calibrated_calorimeter_hit & tail_begin_calo_hit_) const
    {
      const snemo::geometry::calo_locator & calo_locator   = base_gamma_builder::get_calo_locator();
      const snemo::geometry::xcalo_locator & xcalo_locator = base_gamma_builder::get_xcalo_locator();
      const snemo::geometry::gveto_locator & gveto_locator = base_gamma_builder::get_gveto_locator();

      geomtools::vector_3d head_position;
      const geomtools::geom_id & head_gid = head_end_calo_hit_.get_geom_id();
      if (calo_locator.is_calo_block_in_current_module(head_gid))
        calo_locator.get_block_position(head_gid, head_position);
      else if (xcalo_locator.is_calo_block_in_current_module(head_gid))
        xcalo_locator.get_block_position(head_gid, head_position);
      else if (gveto_locator.is_calo_block_in_current_module(head_gid))
        gveto_locator.get_block_position(head_gid, head_position);
      else
        DT_THROW_IF(true, std::logic_error,
                    "Current geom id '" << head_gid << "' does not match any scintillator block !");

      geomtools::vector_3d tail_position;
      const geomtools::geom_id & tail_gid = tail_begin_calo_hit_.get_geom_id();
      if (calo_locator.is_calo_block_in_current_module(tail_gid))
        calo_locator.get_block_position(tail_gid, tail_position);
      else if (xcalo_locator.is_calo_block_in_current_module(tail_gid))
        xcalo_locator.get_block_position(tail_gid, tail_position);
      else if (gveto_locator.is_calo_block_in_current_module(tail_gid))
        gveto_locator.get_block_position(tail_gid, tail_position);
      else
        DT_THROW_IF(true, std::logic_error,
                    "Current geom id '" << tail_gid << "' does not match any scintillator block !");

      const double t1 = head_end_calo_hit_.get_time();
      const double t2 = tail_begin_calo_hit_.get_time();
      const double track_length = (head_position-tail_position).mag();
      const double t_th = track_length / CLHEP::c_light;
      const double sigma_l = 0.6 * CLHEP::ns;
      const double sigma_exp = pow(head_end_calo_hit_.get_sigma_time(),2)
        + pow(tail_begin_calo_hit_.get_sigma_time(),2) + pow(sigma_l,2);
      const double chi2 = pow(std::abs(t1 - t2) - t_th,2)/sigma_exp;
      return gsl_cdf_chisq_Q(chi2, 1)*100*CLHEP::perCent;
    }

    bool gamma_clustering_driver::_are_on_same_wall(const snemo::datamodel::calibrated_calorimeter_hit & head_end_calo_hit,
                                                    const snemo::datamodel::calibrated_calorimeter_hit & tail_begin_calo_hit) const
    {
      const geomtools::geom_id & head_gid = head_end_calo_hit.get_geom_id();
      const geomtools::geom_id & tail_gid = tail_begin_calo_hit.get_geom_id();

      const snemo::geometry::calo_locator & calo_locator   = base_gamma_builder::get_calo_locator();
      const snemo::geometry::xcalo_locator & xcalo_locator = base_gamma_builder::get_xcalo_locator();
      const snemo::geometry::gveto_locator & gveto_locator = base_gamma_builder::get_gveto_locator();

      if (calo_locator.is_calo_block_in_current_module(head_gid) &&
          calo_locator.is_calo_block_in_current_module(tail_gid)) {
        const size_t head_side = calo_locator.extract_side(head_gid);
        const size_t tail_side = calo_locator.extract_side(tail_gid);
        return head_side == tail_side;
      }

      if (xcalo_locator.is_calo_block_in_current_module(head_gid) &&
          xcalo_locator.is_calo_block_in_current_module(tail_gid)) {
        const size_t head_wall = xcalo_locator.extract_wall(head_gid);
        const size_t tail_wall = xcalo_locator.extract_wall(tail_gid);
        return head_wall == tail_wall;
      }

      if (gveto_locator.is_calo_block_in_current_module(head_gid) &&
          gveto_locator.is_calo_block_in_current_module(tail_gid)) {
        const size_t head_wall = gveto_locator.extract_wall(head_gid);
        const size_t tail_wall = gveto_locator.extract_wall(tail_gid);
        return head_wall == tail_wall;
      }

      return false;
    }

    // static
    void gamma_clustering_driver::init_ocd(datatools::object_configuration_description & ocd_)
    {
      // Invoke OCD support from parent class :
      ::snemo::processing::base_gamma_builder::ocd_support(ocd_);

      {
        // Description of the 'GC.cluster_time_range' configuration property :
        datatools::configuration_property_description & cpd
          = ocd_.add_property_info();
        cpd.set_name_pattern("GC.cluster_time_range")
          .set_from("snemo::reconstruction::gamma_clustering_driver")
          .set_terse_description("Set the maximal time difference allowed between two calorimeter hits of a same cluster")
          .set_traits(datatools::TYPE_REAL)
          .set_mandatory(false)
          .set_default_value_real(6 * CLHEP::ns, "ns")
          .add_example("Use the default value::                       \n"
                       "                                              \n"
                       "  GC.cluster_time_range : real as time = 6 ns \n"
                       "                                              \n"
                       )
          ;
      }
      {
        // Description of the 'GC.cluster_grid_mask' configuration property :
        datatools::configuration_property_description & cpd
          = ocd_.add_property_info();
        cpd.set_name_pattern("GC.cluster_grid_mask")
          .set_from("snemo::reconstruction::gamma_clustering_driver")
          .set_terse_description("Set the geometrical conditions where to look for neighbours")
          .set_traits(datatools::TYPE_STRING)
          .set_mandatory(false)
          .set_default_value_string("first")
          .add_example("Use the default value::                     \n"
                       "                                            \n"
                       "  GC.cluster_grid_mask : string = \"first\" \n"
                       "                                            \n"
                       )
          ;
      }
      {
        // Description of the 'GC.minimal_internal_probability' configuration property :
        datatools::configuration_property_description & cpd
          = ocd_.add_property_info();
        cpd.set_name_pattern("GC.minimal_internal_probability")
          .set_from("snemo::reconstruction::gamma_clustering_driver")
          .set_terse_description("Minimal TOF probability to link gamma clusters")
          .set_traits(datatools::TYPE_REAL)
          .set_mandatory(false)
          .set_default_value_real(1e-3 * CLHEP::perCent, "%")
          .add_example("Use the default value::                                      \n"
                       "                                                             \n"
                       "  GC.minimal_internal_probability : real as fraction = 1e-3% \n"
                       "                                                             \n"
                       )
          ;
      }
      {
        // Description of the 'GC.sigma_time_good_calo' configuration property :
        datatools::configuration_property_description & cpd
          = ocd_.add_property_info();
        cpd.set_name_pattern("GC.sigma_time_good_calo")
          .set_from("snemo::reconstruction::gamma_clustering_driver")
          .set_terse_description("Set the time uncertainty below which a hit is considered a good calorimeter hit")
          .set_traits(datatools::TYPE_REAL)
          .set_mandatory(false)
          .set_default_value_real(2.5 * CLHEP::ns, "ns")
          .add_example("Use the default value::                           \n"
                       "                                                  \n"
                       "  GC.sigma_time_good_calo : real as time = 2.5 ns \n"
                       "                                                  \n"
                       )
          ;
      }

      return;
    }

  } // end of namespace reconstruction

} // end of namespace snemo

/* OCD support */
#include <datatools/object_configuration_description.h>
DOCD_CLASS_IMPLEMENT_LOAD_BEGIN(snemo::reconstruction::gamma_clustering_driver, ocd_)
{
  ocd_.set_class_name("snemo::reconstruction::gamma_clustering_driver");
  ocd_.set_class_description("A driver class for the Gamma Clustering algorithm");
  ocd_.set_class_library("Falaise_GammaClustering");
  ocd_.set_class_documentation("The driver does the gammas clusterization and given gamma clusters, \n"
                               "join them if Time-Of-Flight measurements allow such association.    \n"
                               );

  // Invoke specific OCD support :
  ::snemo::reconstruction::gamma_clustering_driver::init_ocd(ocd_);

  ocd_.set_validation_support(true);
  ocd_.lock();
  return;
}
DOCD_CLASS_IMPLEMENT_LOAD_END() // Closing macro for implementation
DOCD_CLASS_SYSTEM_REGISTRATION(snemo::reconstruction::gamma_clustering_driver,
                               "snemo::reconstruction::gamma_clustering_driver")
