/** \file falaise/snemo/processing/base_tracker_clusterizer.cc */

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
#include <falaise/snemo/geometry/locator_plugin.h>
#include <falaise/snemo/geometry/gg_locator.h>
#include <falaise/snemo/datamodels/tracker_clustering_data.h>

namespace snemo {

  namespace processing {

    datatools::logger::priority base_tracker_clusterizer::get_logging_priority () const
    {
      return _logging_priority;
    } // end of base_tracker_clusterizer::get_logging_priority

    void base_tracker_clusterizer::set_logging_priority (datatools::logger::priority priority_)
    {
      _logging_priority = priority_;
      return;
    } // end of base_tracker_clusterizer::set_logging_priority

    const std::string & base_tracker_clusterizer::get_id () const
    {
      return _id_;
    } // end of base_tracker_clusterizer::get_id

    bool base_tracker_clusterizer::is_initialized () const
    {
      return _initialized_;
    } // end of base_tracker_clusterizer::is_initialized

    void base_tracker_clusterizer::_set_initialized (bool i_)
    {
      _initialized_ = i_;
      return;
    } // end of base_tracker_clusterizer::_set_initialized

    const snemo::geometry::gg_locator & base_tracker_clusterizer::get_gg_locator () const
    {
      return *_gg_locator_;
    } // end of base_tracker_clusterizer::get_gg_locator

    void base_tracker_clusterizer::_initialize (const datatools::properties & setup_)
    {
      DT_THROW_IF (is_initialized (), std::logic_error, "Already initialized !");

      DT_THROW_IF (_geometry_manager_ == 0, std::logic_error, "Missing geometry manager !");
      DT_THROW_IF (!_geometry_manager_->is_initialized (), std::logic_error, "Geometry manager is not initialized !");

      /*** Parse configuration parameters ***/

      // Logging priority:
      datatools::logger::priority lp = datatools::logger::extract_logging_configuration (setup_);
      DT_THROW_IF (lp == datatools::logger::PRIO_UNDEFINED,
                   std::logic_error,
                   "Invalid logging priority level for geometry manager !");
      set_logging_priority(lp);

      /*** initialization stuff ***/

      const std::string & geo_setup_label = _geometry_manager_->get_setup_label ();
      DT_THROW_IF (geo_setup_label != "snemo::demonstrator" &&
                   geo_setup_label != "snemo::tracker_commissioning",
                   std::logic_error,
                   "Invalid geometry setup label '" << geo_setup_label << "' !");

      // Get the Geiger cell locator from geometry plugins :
      const geomtools::manager & geo_mgr = get_geometry_manager ();
      std::string locator_plugin_name;
      if (setup_.has_key ("locator_plugin_name")) {
        locator_plugin_name = setup_.fetch_string ("locator_plugin_name");
      }
      // If no locator plugin name is set, then search for the first one
      if (locator_plugin_name.empty ()) {
        const geomtools::manager::plugins_dict_type & plugins = geo_mgr.get_plugins ();
        for (geomtools::manager::plugins_dict_type::const_iterator ip = plugins.begin ();
             ip != plugins.end ();
             ip++) {
          const std::string & plugin_name = ip->first;
          if (geo_mgr.is_plugin_a<snemo::geometry::locator_plugin> (plugin_name)) {
            DT_LOG_DEBUG (get_logging_priority (), "Find locator plugin with name = " << plugin_name);
            locator_plugin_name = plugin_name;
            break;
          }
        }
      }
      // Access to a given plugin by name and type :
      if (geo_mgr.has_plugin (locator_plugin_name)
          && geo_mgr.is_plugin_a<snemo::geometry::locator_plugin> (locator_plugin_name)) {
        DT_LOG_NOTICE (get_logging_priority (), "Found locator plugin named '" << locator_plugin_name << "'");
        const snemo::geometry::locator_plugin & lp
          = geo_mgr.get_plugin<snemo::geometry::locator_plugin> (locator_plugin_name);
        // Set the Geiger cell locator :
        _gg_locator_ = dynamic_cast<const snemo::geometry::gg_locator*>(&(lp.get_gg_locator ()));
      }
      if (get_logging_priority () >= datatools::logger::PRIO_DEBUG) {
        DT_LOG_DEBUG (get_logging_priority (), "Geiger locator :");
        _gg_locator_->dump (std::clog);
      }

      /***********************************************
       * TrackerPreClustering                        *
       ***********************************************/

      TrackerPreClustering::setup_data tpc_setup_data;
      tpc_setup_data.debug = get_logging_priority () == datatools::logger::PRIO_DEBUG;
      tpc_setup_data.cell_size = get_gg_locator ().get_cell_diameter ();

      double default_time_unit = CLHEP::microsecond;

      // Delayed hit minimum time :
      if (setup_.has_key ("TPC.delayed_hit_cluster_time")) {
        double delayed_hit_cluster_time = setup_.fetch_real ("TPC.delayed_hit_cluster_time");
        if (!setup_.has_explicit_unit ("TPC.delayed_hit_cluster_time")) {
          delayed_hit_cluster_time *= default_time_unit;
        }
        tpc_setup_data.delayed_hit_cluster_time = delayed_hit_cluster_time;
      }

      if (setup_.has_key ("TPC.processing_prompt_hits")) {
        tpc_setup_data.processing_prompt_hits = setup_.fetch_boolean ("TPC.processing_prompt_hits");
      }

      if (setup_.has_key ("TPC.processing_delayed_hits")) {
        tpc_setup_data.processing_delayed_hits = setup_.fetch_boolean ("TPC.processing_delayed_hits");
      }

      if (setup_.has_key ("TPC.split_chamber")) {
        tpc_setup_data.split_chamber = setup_.fetch_boolean ("TPC.split_chamber");
      }

      // Check :
      DT_THROW_IF (! tpc_setup_data.check (), std::logic_error,
                   "Invalid TrackerPreClustering setup data : " << tpc_setup_data.get_last_error_message ());

      // Configure the algorithm :
      _pc_.initialize(tpc_setup_data);

      return;
    } // end of base_tracker_clusterizer::_initialize

    void base_tracker_clusterizer::_clear_working_arrays ()
    {
      _ignored_hits_.clear ();
      _prompt_time_clusters_.clear ();
      _delayed_time_clusters_.clear ();
      return;
    } // end of base_tracker_clusterizer::_clear_working_arrays

    void base_tracker_clusterizer::_reset ()
    {
      _set_initialized (false);

      /*** Clear working arrays ***/
      _clear_working_arrays ();

      /*** Reset configuration params ***/
      _set_defaults ();
      return;
    } // end of base_tracker_clusterizer::_reset

    void base_tracker_clusterizer::set_geometry_manager (const geomtools::manager & gmgr_)
    {
      DT_THROW_IF (is_initialized (), std::logic_error, "Already initialized !");
      _geometry_manager_ = &gmgr_;
      return;
    } // end of base_tracker_clusterizer::set_geometry_manager

    const geomtools::manager & base_tracker_clusterizer::get_geometry_manager () const
    {
      DT_THROW_IF (! has_geometry_manager (), std::logic_error, "No geometry manager is setup !");
      return *_geometry_manager_;
    } // end of base_tracker_clusterizer::get_geometry_manager


    bool base_tracker_clusterizer::has_geometry_manager () const
    {
      return _geometry_manager_ != 0;
    } // end of base_tracker_clusterizer::has_geometry_manager

    void base_tracker_clusterizer::_set_defaults ()
    {
      _logging_priority = datatools::logger::PRIO_WARNING;
      _geometry_manager_ = 0;
      _gg_locator_ = 0;
      return;
    } // end of base_tracker_clusterizer::_set_defaults

      // Constructor
    base_tracker_clusterizer::base_tracker_clusterizer (const std::string & id_)
    {
      _id_ = id_;
      _set_initialized (false);
      _set_defaults ();
      return;
    } // end of base_tracker_clusterizer::base_tracker_clusterizer

    base_tracker_clusterizer::~base_tracker_clusterizer ()
    {
      if (_initialized_)
        {
          _reset ();
        }
      return;
    } // end of base_tracker_clusterizer::~base_tracker_clusterizer

    void base_tracker_clusterizer::tree_dump (std::ostream & out_,
                                              const std::string & title_,
                                              const std::string & indent_,
                                              bool inherit_) const
    {
      std::string indent;
      if (! indent_.empty ()) {
        indent = indent_;
      }
      if ( ! title_.empty () ) {
        out_ << indent << title_ << std::endl;
      }

      out_ << indent << datatools::i_tree_dumpable::tag
           << "Logging          : '"
           << datatools::logger::get_priority_label (_logging_priority) << "'"
           << std::endl;
      out_ << indent << datatools::i_tree_dumpable::tag
           << "Initialized      : " << is_initialized () << std::endl;
      out_ << indent << datatools::i_tree_dumpable::tag
           << "Geometry manager : " << _geometry_manager_ << std::endl;
      if (_geometry_manager_) {
        out_ << indent << datatools::i_tree_dumpable::tag
             << "Geometry setup label   : '" << _geometry_manager_->get_setup_label () << "'" << std::endl;
        out_ << indent << datatools::i_tree_dumpable::tag
             << "Geometry setup version : '" << _geometry_manager_->get_setup_version () << "'" << std::endl;
      }
      out_ << indent << datatools::i_tree_dumpable::inherit_tag (inherit_)
           << "End." << std::endl;
      return;
    } // end of base_tracker_clusterizer::tree_dump

    int base_tracker_clusterizer::_prepare_process (const base_tracker_clusterizer::hit_collection_type & hits_,
                                                    snemo::datamodel::tracker_clustering_data & /* clustering_ */)
    {
      /****************
       * Locate cells *
       ****************/

      DT_LOG_DEBUG (get_logging_priority (), "Number of Geiger hits = " << hits_.size ());
      // Ensure the hits have registered X/Y position :
      BOOST_FOREACH (const snemo::datamodel::calibrated_data::tracker_hit_handle_type & gg_handle, hits_)
        {
          if (! gg_handle.has_data ()) continue;
          const snemo::datamodel::calibrated_tracker_hit & sncore_gg_hit = gg_handle.get ();
          const geomtools::geom_id & gid = sncore_gg_hit.get_geom_id ();
          // Check if X/Y position of the cell is recorded in the event :
          if (! sncore_gg_hit.has_xy ())
            {
              // if not, we do it :
              geomtools::vector_3d hit_pos;
              DT_LOG_DEBUG (get_logging_priority (), "Compute X/Y position of the hit cell with GID=" << gid);
              get_gg_locator ().get_cell_position (gid, hit_pos);
              {
                // locally break the const-ness of the hit to store the X/Y doublet :
                snemo::datamodel::calibrated_tracker_hit & mutable_sncore_gg_hit
                  = const_cast <snemo::datamodel::calibrated_tracker_hit &> (sncore_gg_hit);
                mutable_sncore_gg_hit.set_xy (hit_pos.x (), hit_pos.y ());
              }
            }
        }

      /********************
       * Pre-clusterizing *
       ********************/

      // Input data
      TrackerPreClustering::input_data<hit_type> idata;
      idata.hits.reserve (hits_.size());
      std::map<const hit_type *, hit_handle_type > pre_cluster_mapping;

      // Fill the TrackerPreClustering input data model :
      BOOST_FOREACH (const snemo::datamodel::calibrated_data::tracker_hit_handle_type & gg_handle,
                     hits_)
        {
          if (! gg_handle.has_data ()) continue;
          const snemo::datamodel::calibrated_tracker_hit & sncore_gg_hit = gg_handle.get ();
          idata.hits.push_back (&sncore_gg_hit);
          // Mapping between both data models :
          pre_cluster_mapping[&sncore_gg_hit] = gg_handle;
        }

      // TrackerPreClustering output data :
      TrackerPreClustering::output_data<hit_type> odata;

      // Invoke pre-clusterizing algo :
      int status = _pc_.process<hit_type> (idata, odata);
      if (status != TrackerPreClustering::pre_clusterizer::OK)
        {
          DT_LOG_ERROR (get_logging_priority (), "Pre-clusterization has failed !");
          return 1;
        }

      // Repopulate the collections of pre-clusters :

      // Ignored hits :
      _ignored_hits_.reserve (odata.ignored_hits.size ());
      for (size_t ihit = 0; ihit < odata.ignored_hits.size (); ihit++)
        {
          _ignored_hits_.push_back (pre_cluster_mapping[odata.ignored_hits.at (ihit)]);
        }
      DT_LOG_DEBUG (get_logging_priority (), _ignored_hits_.size () << " clusters of hits have been ignored");

      // Prompt time clusters :
      _prompt_time_clusters_.reserve (odata.prompt_clusters.size ());
      for (size_t icluster = 0; icluster < odata.prompt_clusters.size (); icluster++)
        {
          // Add a new hit collection as a new prompt time cluster :
          {
            hit_collection_type hc;
            _prompt_time_clusters_.push_back (hc);
          }
          hit_collection_type & hc = _prompt_time_clusters_.back ();
          hc.reserve (odata.prompt_clusters[icluster].size ());
          for (size_t ihit = 0; ihit < odata.prompt_clusters[icluster].size (); ihit++)
            {
              hc.push_back (pre_cluster_mapping[odata.prompt_clusters[icluster].at (ihit)]);
            }
        }
      DT_LOG_DEBUG (get_logging_priority (), _prompt_time_clusters_.size () << " cluster of hits are prompt");

      // Delayed time clusters :
      _delayed_time_clusters_.reserve (odata.delayed_clusters.size ());
      for (size_t icluster = 0; icluster < odata.delayed_clusters.size (); icluster++)
        {
          // Add a new hit collection as a new delayed time cluster :
          {
            hit_collection_type hc;
            _delayed_time_clusters_.push_back (hc);
          }
          hit_collection_type & hc = _delayed_time_clusters_.back ();
          hc.reserve (odata.delayed_clusters[icluster].size ());
          for (size_t ihit = 0; ihit < odata.delayed_clusters[icluster].size (); ihit++)
            {
              hc.push_back (pre_cluster_mapping[odata.delayed_clusters[icluster].at (ihit)]);
            }
        }
      DT_LOG_DEBUG (get_logging_priority (), _delayed_time_clusters_.size () << " cluster of hits are delayed");

      return 0;
    } // end of base_tracker_clusterizer::_prepare_process

    void base_tracker_clusterizer::_post_process_ignored_hits (snemo::datamodel::tracker_clustering_data & clustering_)
    {
      // Process ignored hits :
      for (size_t i = 0; i < _ignored_hits_.size (); i++) {
        const hit_handle_type & the_hit = _ignored_hits_[i];
        // All ignored hits are pushed as unclustered hits in
        // all clustering solutions :
        for (size_t j = 0; j < clustering_.get_solutions ().size (); j++) {
          snemo::datamodel::tracker_clustering_solution & the_solution =
            clustering_.grab_solutions ()[j].grab ();
          the_solution.grab_unclustered_hits ().push_back (the_hit);
        }
      }
      return;
    }

    void base_tracker_clusterizer::_post_process_collect_unclustered_hits (snemo::datamodel::tracker_clustering_data & /* clustering_ */)
    {
      // Process unclustered hits :
      /*
        hit_collection_type clustered_hits;
        for (int j = 0; j < clustering_.get_solutions ().size (); j++)
        {
        snemo::datamodel::tracker_clustering_solution & the_solution =
        clustering_.grab_solutions ()[j].grab ();


        the_solution.grab_unclustered_hits ().push_back (the_hit);
        }
      */
      return;
    }

    int base_tracker_clusterizer::_post_process (const base_tracker_clusterizer::hit_collection_type & /* hits_ */,
                                                 snemo::datamodel::tracker_clustering_data & clustering_)
    {
      // 2012-10-03 FM : Hits ignored by the pre-clusterizer are classified
      // as 'unclustered' hits; this is questionnable : should we use a
      // dedicated collection of hits for 'ignored' hits ?
      _post_process_ignored_hits (clustering_);

      return 0;
    } // end of base_tracker_clusterizer::_post_process


    int base_tracker_clusterizer::process (const base_tracker_clusterizer::hit_collection_type & hits_,
                                           snemo::datamodel::tracker_clustering_data & clustering_)
    {
      int status = 0;
      DT_THROW_IF (! is_initialized (), std::logic_error,
                   "Clusterizer '" << _id_ << "' is not initialized !");
      _clear_working_arrays ();

      clustering_.invalidate_solutions ();

      // Run pre-processing based on time-coincidence to determine what are prompt hits,
      // what are candidate clusters of delayed hits :
      status = _prepare_process (hits_, clustering_);
      if (status != 0)
        {
          DT_LOG_ERROR (get_logging_priority (), "Pre-processing based on time-coincidence has failed !");
          return status;
        }

      // Process prompt time-clusters :
      for (size_t i = 0; i < _prompt_time_clusters_.size (); i++)
        {
          const hit_collection_type & prompt_clusters = _prompt_time_clusters_[i];
          status = _process_algo (prompt_clusters, clustering_);
          if (status != 0)
            {
              DT_LOG_ERROR (get_logging_priority (), "Processing of prompt hits by '" << _id_ << "' algorithm has failed !");
              return status;
            }
        }

      // Process delayed time-clusters :
      for (size_t i = 0; i < _delayed_time_clusters_.size (); i++)
        {
          const hit_collection_type & delayed_clusters = _delayed_time_clusters_[i];
          status = _process_algo (delayed_clusters, clustering_);
          if (status != 0)
            {
              DT_LOG_ERROR (get_logging_priority (), "Processing of delayed hits by '" << _id_ << "' algorithm has failed !");
              return status;
            }
        }

      _post_process (hits_, clustering_);
      return status;
    } // end of base_tracker_clusterizer::process

  }  // end of namespace processing

}  // end of namespace snemo
