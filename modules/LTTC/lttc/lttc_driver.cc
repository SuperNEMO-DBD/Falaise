/// \file lttc/lttc_driver.cc

// Ourselves:
#include <lttc/lttc_driver.hh>

// Standard library:
#include <sstream>
#include <stdexcept>

// Third party:
// - Boost :
#include <boost/foreach.hpp>
// - Bayeux/datatools :
#include <datatools/exception.h>
#include <datatools/logger.h>
#include <datatools/properties.h>
#include <datatools/utils.h>
// - Bayeux/geomtools :
#include <geomtools/manager.h>

// This project :
#include <falaise/snemo/datamodels/tracker_clustering_data.h>
#include <falaise/snemo/geometry/calo_locator.h>
#include <falaise/snemo/geometry/gg_locator.h>
#include <falaise/snemo/geometry/gveto_locator.h>
#include <falaise/snemo/geometry/locator_plugin.h>
#include <falaise/snemo/geometry/xcalo_locator.h>

namespace lttc {

  const std::string lttc_driver::LTTC_ID = "LTTC";

  lttc_driver::lttc_driver()
    : ::snemo::processing::base_tracker_clusterizer(lttc_driver::LTTC_ID)
  {
    _set_defaults();
  }

  lttc_driver::~lttc_driver() = default;

  void lttc_driver::set_detector_description(const snemo::processing::detector_description & dd_)
  {
    DT_THROW_IF(is_initialized(), std::logic_error,
                "LTTC tracker clusterizer driver is already initialized ! ");
    _detector_desc_ = dd_;
    set_geometry_manager(_detector_desc_.get_geometry_manager());
  }

  const snemo::processing::detector_description &
  lttc_driver::get_detector_description() const
  {
    return _detector_desc_;
  }

  void lttc_driver::_set_defaults()
  {
    {
      lttc_algo::config dummyConfig;
      _lttc_config_ = dummyConfig;
    }
    this->base_tracker_clusterizer::_reset();
  }

  // Initialize the clusterizer through configuration properties
  void lttc_driver::initialize(const datatools::properties & setup_)
  {
    // Invoke initialization at parent level :
    this->snemo::processing::base_tracker_clusterizer::_initialize(setup_);

    datatools::properties lttcConfig;
    setup_.export_and_rename_starting_with(lttcConfig, "LTTC.", "");

    if (lttcConfig.has_key("mode")) {
      std::string modeLabel = lttcConfig.fetch_string("mode");
      if (modeLabel == "line") {
        _lttc_config_.mode = lttc_algo::MODE_LINE;
      } else if (modeLabel == "circle") {
        _lttc_config_.mode = lttc_algo::MODE_CIRCLE;
      } else {
        DT_THROW(std::logic_error, "Unsupported LTTC mode '" << modeLabel << "'");
      }
    }
    
    if (lttcConfig.has_flag("debug")) {
      _lttc_config_.logging = datatools::logger::PRIO_DEBUG;
    }

    if (lttcConfig.has_flag("draw")) {
      _lttc_config_.draw = true;
    }

    if (lttcConfig.has_key("draw_prefix")) {
      _lttc_config_.draw = true;
      _lttc_config_.draw_prefix = lttcConfig.fetch_string("draw_prefix");
    }

    if (lttcConfig.has_key("step1.ntbins")) {
      _lttc_config_.step1_ntbins = lttcConfig.fetch_positive_integer("step1.ntbins");
    }

    if (lttcConfig.has_key("step1.nrbins")) {
      _lttc_config_.step1_nrbins = lttcConfig.fetch_positive_integer("step1.nrbins");
    }

    _lttc_config_.step1_track_threshold = 3.0;
    if (lttcConfig.has_key("step1.track_threshold")) {
      _lttc_config_.step1_track_threshold = lttcConfig.fetch_real("step1.track_threshold");
    }

    _lttc_config_.step2_max_nlines = 20;
    if (lttcConfig.has_key("step2.max_nlines")) {
      _lttc_config_.step2_max_nlines = lttcConfig.fetch_positive_integer("step2.max_nlines");
    }
      
    _lttc_config_.step2_delta_theta = 1.5e-3;
    _lttc_config_.step2_delta_r = 0.2;
    _lttc_config_.step2_gauss_threshold = 0.05;
    _lttc_config_.step2_track_threshold = 3.0;
    
    _lttc_config_.step3_nsigma = 4.0;
    _lttc_config_.step3_nsigma_outliers = 2.0;
  
    // More parsing for more configuration parameters

    // Instantiate the algorithm
    _clusterizer_ = std::make_unique<lttc_algo>(_detector_desc_, _lttc_config_);
    _set_initialized(true);
  }

  // Reset the clusterizer
  void lttc_driver::reset()
  {
    _set_initialized(false);

    if (_clusterizer_) {
      _clusterizer_.reset();
    }
      
    {
      snemo::processing::detector_description defaultDummy;
      _detector_desc_ = defaultDummy;
    }

    _set_defaults();
    this->base_tracker_clusterizer::_reset();
  }

  /// Main clustering method
  int lttc_driver::_process_algo(const base_tracker_clusterizer::hit_collection_type & gg_hits_,
                                 const base_tracker_clusterizer::calo_hit_collection_type & /* calo_hits_ */,
                                 snemo::datamodel::tracker_clustering_data & clustering_) {
    namespace sdm = snemo::datamodel;

    unsigned int nb_gg_hits = gg_hits_.size();

    // Prepare input data model :

    // - Support event timestamp for RC
    snemo::time::invalidate(_lttc_input_.timestamp );
    _lttc_input_.hits.clear();
    if (has_event_timestamp()) {
      _lttc_input_.timestamp = get_event_timestamp();
    }

    // - Prepare buffer of hits:
    if (_lttc_input_.hits.capacity() < nb_gg_hits) {
      _lttc_input_.hits.reserve(nb_gg_hits);
    }
    
    size_t ihit = 0; // Indexing of hits

    // Hit accounting :
    std::map<int, sdm::TrackerHitHdl> hits_mapping;
    std::map<int, int> hits_status;

    // GG hit loop :
    for (const sdm::TrackerHitHdl& gg_handle : gg_hits_) {
      // Skip NULL handle :
      if (!gg_handle) {
        continue;
      }

      // Get a const reference on the calibrated Geiger hit :
      const sdm::calibrated_tracker_hit & snemo_gg_hit = gg_handle.get();

      // Check the geometry ID as a Geiger cell :
      const snemo::geometry::gg_locator & gg_locator = get_gg_locator();
      const geomtools::geom_id & gg_hit_gid = snemo_gg_hit.get_geom_id();
      DT_THROW_IF(! gg_locator.isGeigerCell(gg_hit_gid), std::logic_error,
                  "Calibrated tracker hit ID is not valid !");

      if (! gg_locator.isGeigerCellInThisModule(gg_hit_gid)) {
        continue;
      }

      // Extract the numbering scheme of the cell from its geom ID :
      const int side_id = gg_locator.getSideAddress(gg_hit_gid);
      const int layer_id = gg_locator.getLayerAddress(gg_hit_gid);
      const int row_id = gg_locator.getRowAddress(gg_hit_gid);
 
      // X-Y position of the anode wire of the hit cell :
      double x = snemo_gg_hit.get_x();
      double y = snemo_gg_hit.get_y();

      // Z pos of the Geiger hit
      double z = snemo_gg_hit.get_z();
      double z_err = snemo_gg_hit.get_sigma_z();

      // Prompt/delayed trait of the hit :
      const bool prompt = snemo_gg_hit.is_prompt();

      // Transverse Geiger drift distance :
      const double rdrift = snemo_gg_hit.get_r();
      const double rdrift_err = snemo_gg_hit.get_sigma_r();

      // Build the Geiger hit position :
      // geomtools::vector_3d gg_hit_position(x, y, z);

      // Add a new hit cell in the LTTC input data model :
      tracker_hit h;
      h.id = ihit++;
      h.side_id = side_id;
      h.layer_id = layer_id;
      h.row_id = row_id;
      h.drift_radius = rdrift;
      h.drift_radius_err = rdrift_err;
      h.x = x;
      h.y = y;
      h.z = z;
      h.z_err = z_err;
      h.delayed = !prompt;
      _lttc_input_.hits.add_hit(h);
 
      // Store mapping info between both data models :
      hits_mapping[h.id] = gg_handle;
      hits_status[h.id] = 0;
    }  

    // No support for calo hits
      
    // Validate the input data :
    // if (!_lttc_input_.hits.size()) {
    //   DT_LOG_ERROR(get_logging_priority(), "Empty LTTC input data !");
    //   return 1;
    // }

    _clusterizer_->process(_lttc_input_, _lttc_output_);
    
    if (datatools::logger::is_debug(get_logging_priority())) {
      clustering_.tree_dump(std::cerr, "Output clustering data : ", "[debug] ");
    }
    return 0;
  }

  // static
  void lttc_driver::init_ocd(datatools::object_configuration_description & ocd_)
  {
    // Invoke OCD support from parent class :
    ::snemo::processing::base_tracker_clusterizer::ocd_support(ocd_);

    /*
      {
      // Description of the 'CAT.magnetic_field' configuration property :
      datatools::configuration_property_description& cpd = ocd_.add_property_info();
      cpd.set_name_pattern("CAT.magnetic_field")
      .set_from("snemo::reconstruction::lttc_driver")
      .set_terse_description("Force the magnetic field value (vertical)")
      .set_traits(datatools::TYPE_REAL)
      .set_mandatory(false)
      // .set_long_description("Default value: 25 gauss")
      .set_default_value_real(25 * CLHEP::gauss, "gauss")
      .add_example(
      "Use no magnetic field::               \n"
      "                                      \n"
      "  CAT.magnetic_field : real = 0 gauss \n"
      "                                      \n");
      }

      {
      // Description of the 'CAT.level' configuration property :
      datatools::configuration_property_description& cpd = ocd_.add_property_info();
      cpd.set_name_pattern("CAT.level")
      .set_from("snemo::reconstruction::lttc_driver")
      .set_terse_description("Verbosity level")
      .set_traits(datatools::TYPE_STRING)
      .set_mandatory(false)
      // .set_long_description("Default value: \"normal\"")
      .set_default_value_string("normal")
      .add_example(
      "Use normal verbosity:: \n"
      "                                  \n"
      "  CAT.level : string = \"normal\" \n"
      "                                  \n");
      }

      {
      // Description of the 'CAT.max_time' configuration property :
      datatools::configuration_property_description& cpd = ocd_.add_property_info();
      cpd.set_name_pattern("CAT.max_time")
      .set_from("snemo::reconstruction::lttc_driver")
      .set_terse_description("Maximum processing time")
      .set_traits(datatools::TYPE_REAL)
      .set_mandatory(false)
      // .set_long_description("Default value: 5000 ms")
      .set_default_value_real(5000 * CLHEP::ms, "ms")
      .add_example(
      "Use default value::               \n"
      "                                  \n"
      "  CAT.max_time : real = 5000 ms   \n"
      "                                  \n");
      }

      {
      // Description of the 'CAT.small_radius' configuration property :
      datatools::configuration_property_description& cpd = ocd_.add_property_info();
      cpd.set_name_pattern("CAT.small_radius")
      .set_from("snemo::reconstruction::lttc_driver")
      .set_terse_description("Max radius of cells to be not treated as points in distance unit")
      .set_traits(datatools::TYPE_REAL)
      .set_mandatory(false)
      .set_long_description("Default value: 2.0 mm")
      .add_example(
      "Use default value::                \n"
      "                                   \n"
      "  CAT.small_radius : real = 2.0 mm \n"
      "                                   \n");
      }

      {
      // Description of the 'CAT.probmin' configuration property :
      datatools::configuration_property_description& cpd = ocd_.add_property_info();
      cpd.set_name_pattern("CAT.probmin")
      .set_from("snemo::reconstruction::lttc_driver")
      .set_terse_description("Minimal probability away from the straight line")
      .set_traits(datatools::TYPE_REAL)
      .set_mandatory(false)
      .set_long_description("Default value: 0.0")
      .add_example(
      "Use default value::               \n"
      "                                  \n"
      "  CAT.probmin : real = 0.0        \n"
      "                                  \n");
      }

      {
      // Description of the 'CAT.nofflayers' configuration property :
      datatools::configuration_property_description& cpd = ocd_.add_property_info();
      cpd.set_name_pattern("CAT.nofflayers")
      .set_from("snemo::reconstruction::lttc_driver")
      .set_terse_description(
      "Number of cells which can be skipped (because the cell did not work) and still the "
      "cluster is continuous")
      .set_traits(datatools::TYPE_INTEGER)
      .set_mandatory(false)
      .set_long_description("Default value: 1")
      .add_example(
      "Use default value::               \n"
      "                                  \n"
      "  CAT.nofflayers : integer = 1    \n"
      "                                  \n");
      }

      {
      // Description of the 'CAT.first_event' configuration property :
      datatools::configuration_property_description& cpd = ocd_.add_property_info();
      cpd.set_name_pattern("CAT.first_event")
      .set_from("snemo::reconstruction::lttc_driver")
      .set_terse_description("First event to be processed")
      .set_traits(datatools::TYPE_INTEGER)
      .set_mandatory(false)
      .set_long_description("Default value: -1")
      .add_example(
      "Do not specify any first event::  \n"
      "                                  \n"
      "  CAT.first_event : integer = -1  \n"
      "                                  \n");
      }

      {
      // Description of the 'CAT.ratio' configuration property :
      datatools::configuration_property_description& cpd = ocd_.add_property_info();
      cpd.set_name_pattern("CAT.ratio")
      .set_from("snemo::reconstruction::lttc_driver")
      .set_terse_description("Ratio of 2nd best to best chi2 which is acceptable as 2nd solution")
      .set_traits(datatools::TYPE_REAL)
      .set_mandatory(false)
      .set_long_description("Default value: 10000.0")
      .add_example(
      "Use the default value::          \n"
      "                                 \n"
      "  CAT.ratio : real = 10000.0     \n"
      "                                 \n");
      }

      {
      // Description of the 'CAT.driver.sigma_z_factor' configuration property :
      datatools::configuration_property_description& cpd = ocd_.add_property_info();
      cpd.set_name_pattern("CAT.sigma_z_factor")
      .set_from("snemo::reconstruction::lttc_driver")
      .set_terse_description("Sigma Z factor")
      .set_traits(datatools::TYPE_REAL)
      .set_mandatory(false)
      .set_long_description("Default value: 1.0")
      .add_example(
      "Use the default value::                  \n"
      "                                         \n"
      "  CAT.sigma_z_factor : real = 1.0        \n"
      "                                         \n");
      }
    */
    return;
  }
  
}  // end of namespace lttc

/* OCD support */
#include <datatools/object_configuration_description.h>
DOCD_CLASS_IMPLEMENT_LOAD_BEGIN(lttc::lttc_driver, ocd_) {
  ocd_.set_class_name("lttc::lttc_driver");
  ocd_.set_class_description("A driver class for the LTTC algorithm");
  ocd_.set_class_library("Falaise_Lttc");
  ocd_.set_class_documentation("This driver manager for the LTTC clustering algorithm.");

  // Invoke specific OCD support :
  ::lttc::lttc_driver::init_ocd(ocd_);

  ocd_.set_validation_support(true);
  ocd_.lock();
  return;
}
DOCD_CLASS_IMPLEMENT_LOAD_END()  // Closing macro for implementation
DOCD_CLASS_SYSTEM_REGISTRATION(lttc::lttc_driver, "lttc::lttc_driver")
