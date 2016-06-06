/// \file falaise/snemo/reconstruction/cat_tracker_clustering_module.cc

// Ourselves:
#include <snemo/reconstruction/cat_tracker_clustering_module.h>

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
#include <falaise/snemo/processing/services.h>

// CAT:
#include <snemo/reconstruction/cat_driver.h>

namespace snemo {

  namespace reconstruction {

    // Registration instantiation macro :
    DPP_MODULE_REGISTRATION_IMPLEMENT(cat_tracker_clustering_module,
                                      "snemo::reconstruction::cat_tracker_clustering_module")

    void cat_tracker_clustering_module::set_cd_label(const std::string & cdl_)
    {
      DT_THROW_IF (is_initialized(), std::logic_error,
                   "Module '" << get_name() << "' is already initialized ! ");
      _CD_label_ = cdl_;
      return;
    }

    const std::string & cat_tracker_clustering_module::get_cd_label() const
    {
      return _CD_label_;
    }

    void cat_tracker_clustering_module::set_tcd_label(const std::string & tcdl_)
    {
      DT_THROW_IF (is_initialized(), std::logic_error,
                   "Module '" << get_name() << "' is already initialized ! ");
      _TCD_label_ = tcdl_;
      return;
    }

    const std::string & cat_tracker_clustering_module::get_tcd_label() const
    {
      return _TCD_label_;
    }

    const geomtools::manager & cat_tracker_clustering_module::get_geometry_manager() const
    {
      return *_geometry_manager_;
    }

    void cat_tracker_clustering_module::set_geometry_manager(const geomtools::manager & gmgr_)
    {
      DT_THROW_IF (is_initialized(), std::logic_error,
                   "Module '" << get_name() << "' is already initialized ! ");
      _geometry_manager_ = &gmgr_;

      // Check setup label:
      const std::string & setup_label = _geometry_manager_->get_setup_label();
      DT_THROW_IF (setup_label != "snemo::demonstrator" && setup_label != "snemo::tracker_commissioning",
                   std::logic_error,
                   "Setup label '" << setup_label << "' is not supported !");
      return;
    }

    void cat_tracker_clustering_module::_set_defaults()
    {
      _geometry_manager_ = 0;
      _CD_label_.clear();
      _TCD_label_.clear();
      _driver_.reset(0);
      return;
    }

    void cat_tracker_clustering_module::initialize(const datatools::properties  & setup_,
                                                   datatools::service_manager   & service_manager_,
                                                   dpp::module_handle_dict_type & /* module_dict_ */)
    {
      DT_THROW_IF (is_initialized(),
                   std::logic_error,
                   "Module '" << get_name() << "' is already initialized ! ");

      dpp::base_module::_common_initialize(setup_);

      if (_CD_label_.empty()) {
        if (setup_.has_key("CD_label")) {
          _CD_label_ = setup_.fetch_string("CD_label");
        }
      }
      // Default label:
      if (_CD_label_.empty()) {
        _CD_label_  = snemo::datamodel::data_info::default_calibrated_data_label();
      }

      if (_TCD_label_.empty()) {
        if (setup_.has_key("TCD_label")) {
          _TCD_label_ = setup_.fetch_string("TCD_label");
        }
      }
      // Default label:
      if (_TCD_label_.empty()) {
        _TCD_label_ = snemo::datamodel::data_info::default_tracker_clustering_data_label();
      }

      // Geometry manager :
      if (_geometry_manager_ == 0) {
        std::string geo_label = snemo::processing::service_info::default_geometry_service_label();
        if (setup_.has_key("Geo_label")) {
          geo_label = setup_.fetch_string("Geo_label");
        }
        DT_THROW_IF (geo_label.empty(), std::logic_error,
                     "Module '" << get_name() << "' has no valid '" << "Geo_label" << "' property !");
        DT_THROW_IF (! service_manager_.has(geo_label) ||
                     ! service_manager_.is_a<geomtools::geometry_service>(geo_label),
                     std::logic_error,
                     "Module '" << get_name() << "' has no '" << geo_label << "' service !");
        const geomtools::geometry_service & Geo
          = service_manager_.get<geomtools::geometry_service>(geo_label);
        set_geometry_manager(Geo.get_geom_manager());
      }

      // Clustering algorithm :
      std::string algorithm_id = cat_driver::CAT_ID;
      _driver_.reset(new cat_driver);
      DT_THROW_IF (! _driver_, std::logic_error,
                   "Module '" << get_name() << "' could not instantiate the '"
                   << algorithm_id << "' tracker clusterizer algorithm !");

      // Plug the geometry manager :
      _driver_.get()->set_geometry_manager(get_geometry_manager());

      // Initialize the clustering driver :
      _driver_.get()->initialize(setup_);

      _set_initialized(true);
      return;
    }

    void cat_tracker_clustering_module::reset()
    {
      DT_THROW_IF (! is_initialized(),
                   std::logic_error,
                   "Module '" << get_name() << "' is not initialized !");
      _set_initialized(false);
      // Reset the clusterizer driver :
      if (_driver_) {
        if (_driver_->is_initialized()) {
          _driver_->reset();
        }
        _driver_.reset();
      }
      _set_defaults();
      return;
    }

    // Constructor :
    cat_tracker_clustering_module::cat_tracker_clustering_module(datatools::logger::priority logging_priority_)
      : dpp::base_module(logging_priority_)
    {
      _set_defaults();
      return;
    }

    // Destructor :
    cat_tracker_clustering_module::~cat_tracker_clustering_module()
    {
      if (is_initialized()) cat_tracker_clustering_module::reset();
      return;
    }

    // Processing :
    dpp::base_module::process_status cat_tracker_clustering_module::process(datatools::things & data_record_)
    {
      DT_THROW_IF (! is_initialized(), std::logic_error,
                   "Module '" << get_name() << "' is not initialized !");

      ///////////////////////////
      // Check calibrated data //
      ///////////////////////////

      bool abort_at_missing_input = true;

      // Check if some 'calibrated_data' are available in the data model:
      if (! data_record_.has(_CD_label_)) {
        DT_THROW_IF (abort_at_missing_input, std::logic_error,
                     "Missing calibrated data to be processed !");
        // leave the data unchanged.
        return dpp::base_module::PROCESS_ERROR;
      }
      // grab the 'calibrated_data' entry from the data model :
      snemo::datamodel::calibrated_data & the_calibrated_data
        = data_record_.grab<snemo::datamodel::calibrated_data>(_CD_label_);

      ///////////////////////////////////
      // Check tracker clustering data //
      ///////////////////////////////////

      bool abort_at_former_output = false;
      bool preserve_former_output = false;

      // check if some 'tracker_clustering_data' are available in the data model:
      snemo::datamodel::tracker_clustering_data * ptr_cluster_data = 0;
      if (! data_record_.has(_TCD_label_)) {
        ptr_cluster_data = &(data_record_.add<snemo::datamodel::tracker_clustering_data>(_TCD_label_));
      } else {
        ptr_cluster_data = &(data_record_.grab<snemo::datamodel::tracker_clustering_data>(_TCD_label_));
      }
      snemo::datamodel::tracker_clustering_data & the_clustering_data = *ptr_cluster_data;
      if (the_clustering_data.has_solutions())  {
        DT_THROW_IF (abort_at_former_output, std::logic_error,
                     "Already has processed tracker clustering data !");
        if (! preserve_former_output) {
          the_clustering_data.reset();
        }
      }

      /********************
       * Process the data *
       ********************/

      // Main processing method :
      _process(the_calibrated_data, the_clustering_data);

      return dpp::base_module::PROCESS_SUCCESS;
    }

    void cat_tracker_clustering_module::_process(const snemo::datamodel::calibrated_data & calib_data_,
                                                 snemo::datamodel::tracker_clustering_data & clustering_data_)
    {
      DT_LOG_TRACE(get_logging_priority(), "Entering...");

      // Process the clusterizer driver :
      _driver_.get()->process(calib_data_.calibrated_tracker_hits(),
                              calib_data_.calibrated_calorimeter_hits(),
                              clustering_data_);

      DT_LOG_TRACE(get_logging_priority(), "Exiting.");
      return;
    }

  } // end of namespace reconstruction

} // end of namespace snemo

/* OCD support */
#include <datatools/object_configuration_description.h>
DOCD_CLASS_IMPLEMENT_LOAD_BEGIN(snemo::reconstruction::cat_tracker_clustering_module, ocd_)
{
  ocd_.set_class_name("snemo::reconstruction::cat_tracker_clustering_module");
  ocd_.set_class_description("A module that performs the CAT clusterization of tracker hits");
  ocd_.set_class_library("Falaise_CAT");
  ocd_.set_class_documentation("This module uses the CAT clustering algorithm.");

  // Invoke OCD support from parent class :
  dpp::base_module::common_ocd(ocd_);

  // Invoke specific OCD support from the driver class:
  ::snemo::reconstruction::cat_driver::init_ocd(ocd_);

  {
    // Description of the 'CD_label' configuration property :
    datatools::configuration_property_description & cpd
      = ocd_.add_property_info();
    cpd.set_name_pattern("CD_label")
      .set_terse_description("The label/name of the 'calibrated data' bank")
      .set_traits(datatools::TYPE_STRING)
      .set_mandatory(false)
      .set_long_description("This is the name of the bank to be used \n"
                            "as the source of input tracker hits.    \n"
                            )
      .set_default_value_string(snemo::datamodel::data_info::default_calibrated_data_label())
      .add_example("Use an alternative name for the 'calibrated data' bank:: \n"
                   "                                \n"
                   "  CD_label : string = \"CD2\"   \n"
                   "                                \n"
                   )
      ;
  }

  {
    // Description of the 'TCD_label' configuration property :
    datatools::configuration_property_description & cpd
      = ocd_.add_property_info();
    cpd.set_name_pattern("TCD_label")
      .set_terse_description("The label/name of the 'tracker clustering data' bank")
      .set_traits(datatools::TYPE_STRING)
      .set_mandatory(false)
      .set_long_description("This is the name of the bank to be used as   \n"
                            "the sink of output clusters of tracker hits. \n"
                            )
      .set_default_value_string(snemo::datamodel::data_info::default_tracker_clustering_data_label())
      .add_example("Use an alternative name for the 'tracker clustering data' bank:: \n"
                   "                                  \n"
                   "  TCD_label : string = \"TCD2\"   \n"
                   "                                  \n"
                   )
      ;
  }

  {
    // Description of the 'Geo_label' configuration property :
    datatools::configuration_property_description & cpd
      = ocd_.add_property_info();
    cpd.set_name_pattern("Geo_label")
      .set_terse_description("The label/name of the geometry service")
      .set_traits(datatools::TYPE_STRING)
      .set_mandatory(false)
      .set_long_description("This is the name of the service to be used as the \n"
                            "geometry service.                                 \n"
                            "This property is only used if no geometry manager \n"
                            "as been provided to the module.                   \n"
                            )
      .set_default_value_string(snemo::processing::service_info::default_geometry_service_label())
      .add_example("Use an alternative name for the geometry service:: \n"
                   "                                                   \n"
                   "  Geo_label : string = \"geometry2\"               \n"
                   "                                                   \n"
                   )
      ;
  }

  // Additionnal configuration hints :
  ocd_.set_configuration_hints("Here is a full configuration example in the                          \n"
                               "``datatools::properties`` ASCII format::                             \n"
                               "                                                                     \n"
                               "  TC.logging.priority          : string = \"fatal\"                  \n"
                               "  TC.locator_plugin_name       : string = \"locators_driver\"        \n"
                               "  TPC.delayed_hit_cluster_time : real as time = 10 us                \n"
                               "  TPC.processing_prompt_hits   : boolean = 1                         \n"
                               "  TPC.processing_delayed_hits  : boolean = 1                         \n"
                               "  TPC.split_chamber            : boolean = 0                         \n"
                               "  CD_label                     : string = \"CD\"                     \n"
                               "  TCD_label                    : string = \"TCD\"                    \n"
                               "  Geo_label                    : string = \"geometry\"               \n"
                               "  CAT.magnetic_field           : real   = 25 gauss                   \n"
                               "  CAT.max_time                 : real   = 5000.0 ms                  \n"
                               "  CAT.nofflayers               : integer = 1                         \n"
                               "                                                                     \n"
                               "Additional specific parameters can be used to configure              \n"
                               "the embedded ``CAT`` driver itself; see the OCD support              \n"
                               "of the ``snemo::reconstruction::cat_driver`` class.                  \n"
                               );

  ocd_.set_validation_support(true);
  ocd_.lock();
  return;
}
DOCD_CLASS_IMPLEMENT_LOAD_END() // Closing macro for implementation
DOCD_CLASS_SYSTEM_REGISTRATION(snemo::reconstruction::cat_tracker_clustering_module,
                               "snemo::reconstruction::cat_tracker_clustering_module")
