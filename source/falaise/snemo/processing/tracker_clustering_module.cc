/// \file falaise/snemo/processing/tracker_clustering_module.cc

// Ourselves:
#include <falaise/snemo/processing/tracker_clustering_module.h>

// Standard library:
#include <stdexcept>
#include <sstream>

// Third party:
// - Bayeux/datatools:
#include <datatools/service_manager.h>
// - Bayeux/geomtools:
#include <geomtools/geometry_service.h>
#include <geomtools/manager.h>

// This project:
#include <falaise/snemo/datamodels/data_model.h>
#include <falaise/snemo/datamodels/tracker_clustering_data.h>
#include <falaise/snemo/processing/base_tracker_clusterizer.h>

// CAT:
// #include <snemo/processing/cat_driver.h>

// Sultan:
// #include <snemo/processing/sultan_driver.h>

// TrackerClusterPath
// #include <snemo/processing/tcp_driver.h>

// Yaca
// #include <snemo/processing/tc_yaca_driver.h>

namespace snemo {

  namespace processing {

    // Registration instantiation macro :
    DPP_MODULE_REGISTRATION_IMPLEMENT(tracker_clustering_module,
                                      "snemo::processing::tracker_clustering_module");

    const geomtools::manager & tracker_clustering_module::get_geometry_manager () const
    {
      return *_geometry_manager_;
    }

    void tracker_clustering_module::set_geometry_manager (const geomtools::manager & gmgr_)
    {
      DT_THROW_IF (is_initialized (), std::logic_error,
                   "Module '" << get_name () << "' is already initialized ! ");
      _geometry_manager_ = &gmgr_;

      // Check setup label:
      const std::string & setup_label = _geometry_manager_->get_setup_label ();
      DT_THROW_IF (setup_label != "snemo" && setup_label != "snemo::tracker_commissioning",
                   std::logic_error,
                   "Setup label '" << setup_label << "' is not supported !");
      return;
    }

    void tracker_clustering_module::_set_defaults ()
    {
      _geometry_manager_ = 0;
      _CD_label_  = snemo::datamodel::data_info::CALIBRATED_DATA_LABEL;
      _TCD_label_ = snemo::datamodel::data_info::TRACKER_CLUSTERING_DATA_LABEL;
      _driver_.reset (0);
      return;
    }

    void tracker_clustering_module::initialize(const datatools::properties  & setup_,
                                               datatools::service_manager   & service_manager_,
                                               dpp::module_handle_dict_type & /* module_dict_ */)
    {
      DT_THROW_IF (is_initialized (),
                   std::logic_error,
                   "Module '" << get_name () << "' is already initialized ! ");

      dpp::base_module::_common_initialize (setup_);

      if (setup_.has_key ("CD_label")) {
        _CD_label_ = setup_.fetch_string ("CD_label");
      }

      if (setup_.has_key ("TCD_label")) {
        _TCD_label_ = setup_.fetch_string ("TCD_label");
      }

      std::string geo_label = "Geo";
      if (setup_.has_key("Geo_label")) {
        geo_label = setup_.fetch_string("Geo_label");
      }
      // Geometry manager :
      if (_geometry_manager_ == 0) {
        DT_THROW_IF (geo_label.empty (), std::logic_error,
                     "Module '" << get_name () << "' has no valid '" << "Geo_label" << "' property !");
        DT_THROW_IF (! service_manager_.has (geo_label) ||
                     ! service_manager_.is_a<geomtools::geometry_service> (geo_label),
                     std::logic_error,
                     "Module '" << get_name () << "' has no '" << geo_label << "' service !");
        geomtools::geometry_service & Geo
          = service_manager_.get<geomtools::geometry_service> (geo_label);
        set_geometry_manager (Geo.get_geom_manager ());
      }

      // Clustering algorithm :
      std::string algorithm_id;
      if (setup_.has_key ("algorithm")) {
        algorithm_id = setup_.fetch_string ("algorithm");
      }
      // Initialize the clusterizer algo:
      if (algorithm_id == "Foo")  {
        // _driver_.reset (new tc_foo_driver);
      }
      // if (algorithm_id == "Foo" /* cat_driver::CAT_ID*/)  {
      //        _driver_.reset (new cat_driver);
      // }
      //         else if (_algorithm_id_ == tc_yaca_driver::TRACKER_CLUSTER_YACA_ID)
      //           {
      //             _driver_.reset (new tc_yaca_driver);
      //           }
      //         else if (_algorithm_id_ == tcp_driver::TRACKER_CLUSTER_PATH_ID)
      //           {
      //             //_driver_.reset (new tcp_driver);
      //           }
      else
        {
          DT_THROW_IF (true, std::logic_error,
                       "Module '" << get_name () << "' does not know any '"
                       << algorithm_id << "' clusterizer algorithm !");
        }

      // Plug the geometry manager :
      _driver_.get()->set_geometry_manager (get_geometry_manager ());

      // Initialize the clustering driver :
      _driver_.get()->initialize (setup_);

      _set_initialized (true);
      return;
    }

    void tracker_clustering_module::reset()
    {
      DT_THROW_IF(! is_initialized (),
                  std::logic_error,
                  "Module '" << get_name () << "' is not initialized !");
      _set_initialized (false);

      // Reset the clusterizer driver :
      if (_driver_) {
        if (_driver_->is_initialized()) {
          _driver_->reset();
        }
        _driver_.reset();
      }

      _set_defaults ();
      return;
    }

    // Constructor :
    tracker_clustering_module::tracker_clustering_module(datatools::logger::priority logging_priority_)
      : dpp::base_module(logging_priority_)
    {
      _set_defaults ();
      return;
    }

    // Destructor :
    tracker_clustering_module::~tracker_clustering_module()
    {
      if (is_initialized()) tracker_clustering_module::reset();
      return;
    }

    // Processing :
    dpp::base_module::process_status tracker_clustering_module::process(datatools::things & data_record_)
    {
      DT_THROW_IF (! is_initialized (), std::logic_error,
                   "Module '" << get_name () << "' is not initialized !");

      ///////////////////////////
      // Check calibrated data //
      ///////////////////////////

      bool abort_at_missing_input = true;

      // Check if some 'calibrated_data' are available in the data model:
      if (! data_record_.has (_CD_label_))
        {
          DT_THROW_IF (abort_at_missing_input, std::logic_error,
                       "Missing calibrated data to be processed !");
          // leave the data unchanged.
          return dpp::base_module::PROCESS_ERROR;
        }
      // grab the 'calibrated_data' entry from the data model :
      snemo::datamodel::calibrated_data & the_calibrated_data
        = data_record_.grab<snemo::datamodel::calibrated_data> (_CD_label_);

      ///////////////////////////////////
      // Check tracker clustering data //
      ///////////////////////////////////

      bool abort_at_former_output = false;
      bool preserve_former_output = false;

      // check if some 'tracker_clustering_data' are available in the data model:
      snemo::datamodel::tracker_clustering_data * ptr_cluster_data = 0;
      if (! data_record_.has (_TCD_label_)) {
        ptr_cluster_data = &(data_record_.add<snemo::datamodel::tracker_clustering_data> (_TCD_label_));
      } else {
        ptr_cluster_data = &(data_record_.grab<snemo::datamodel::tracker_clustering_data> (_TCD_label_));
      }
      snemo::datamodel::tracker_clustering_data & the_clustering_data = *ptr_cluster_data;
      if (the_clustering_data.has_solutions ())  {
        DT_THROW_IF (abort_at_former_output, std::logic_error,
                     "Already has processed tracker clustering data !");
        if (! preserve_former_output) {
          the_clustering_data.reset ();
        }
      }

      /********************
       * Process the data *
       ********************/

      // Main processing method :
      _process (the_calibrated_data.calibrated_tracker_hits (),
                the_clustering_data);

      return dpp::base_module::PROCESS_SUCCESS;
    }

    void tracker_clustering_module::_process (const snemo::datamodel::calibrated_data::tracker_hit_collection_type & calibrated_tracker_hits_,
                                              snemo::datamodel::tracker_clustering_data & clustering_data_)
    {
      DT_LOG_TRACE (get_logging_priority (), "Entering...");

      // process the clusterizer driver :
      _driver_.get ()->process (calibrated_tracker_hits_, clustering_data_);

      DT_LOG_TRACE (get_logging_priority (), "Exiting.");
      return;
    }

  } // end of namespace processing

} // end of namespace snemo

// end of tracker_clustering_module.cc
