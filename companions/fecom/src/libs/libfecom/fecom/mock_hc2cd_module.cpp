// -*- mode: c++ ; -*-
/** \file fecom/src/libs/libfecom/fecom/mock_hc2cd_module.cpp
 */

// Ourselves:
#include <fecom/mock_hc2cd_module.hpp>

// Standard library:
#include <stdexcept>
#include <sstream>

// Third party:
// - Bayeux/datatools:
#include <datatools/service_manager.h>
// - Bayeux/geomtools:
#include <geomtools/manager.h>
#include <geomtools/geometry_service.h>
// - Bayeux/mctools:
#include <mctools/utils.h>

// - Falaise:
#include <falaise/snemo/datamodels/data_model.h>
#include <falaise/snemo/processing/services.h>

// This project :
#include <fecom/calo_hit_calibration.hpp>

namespace fecom {

  const std::string mock_hc2cd_module::data_info::HALF_COM_RAW_DATA_LABEL = "HCRD";

  const std::string & mock_hc2cd_module::data_info::default_half_com_raw_data_label()
  {
    static std::string lbl("HCRD");
    return lbl;
  }

  // Constructor :
  mock_hc2cd_module::mock_hc2cd_module(datatools::logger::priority logging_priority_)
    : dpp::base_module(logging_priority_)
  {
    _geom_manager_    = 0;
    _external_random_ = 0;
    _set_defaults();
    return;
  }

  // Destructor :
  mock_hc2cd_module::~mock_hc2cd_module()
  {
    if (is_initialized()) mock_hc2cd_module::reset();
    return;
  }

  void mock_hc2cd_module::set_channel_mapping(const fecom::channel_mapping & chmap_)
  {
    DT_THROW_IF(is_initialized(),
		std::logic_error,
		"Module '" << get_name() << "' is already initialized ! ");
    _channel_mapping_ = &chmap_;

    return;
  }

  const fecom::channel_mapping & mock_hc2cd_module::get_channel_mapping() const
  {
    DT_THROW_IF(! is_initialized(),
		std::logic_error,
		"Module '" << get_name() << "' is not initialized ! ");
    return *_channel_mapping_;
  }

  void mock_hc2cd_module::set_geom_manager(const geomtools::manager & gmgr_)
  {
    DT_THROW_IF(is_initialized(),
		std::logic_error,
		"Module '" << get_name() << "' is already initialized ! ");
    _geom_manager_ = & gmgr_;

    return;
  }

  const geomtools::manager & mock_hc2cd_module::get_geom_manager() const
  {
    DT_THROW_IF(! is_initialized(),
		std::logic_error,
		"Module '" << get_name() << "' is not initialized ! ");
    return *_geom_manager_;
  }

  void mock_hc2cd_module::set_external_random(mygsl::rng & rng_)
  {
    DT_THROW_IF(is_initialized(),
		std::logic_error,
		"Module '" << get_name() << "' is already initialized ! ");
    _external_random_ = &rng_;
    return;
  }

  void mock_hc2cd_module::reset_external_random()
  {
    DT_THROW_IF(is_initialized(),
		std::logic_error,
		"Module '" << get_name() << "' is already initialized ! ");
    _external_random_ = 0;
    return;
  }

  bool mock_hc2cd_module::has_external_random() const
  {
    return _external_random_ != 0;
  }

  void mock_hc2cd_module::convert_gid_to_falaise_gid(const geomtools::geom_id geometric_id_,
						     geomtools::geom_id & falaise_geom_id_)
  {
    int module = 0;
    int side = 1;
    if (geometric_id_.get_type() == fecom::calo_constants::GEOMETRIC_CALO_TYPE)
      {
	int column = geometric_id_.get(fecom::calo_constants::COLUMN_INDEX);
	int row = geometric_id_.get(fecom::calo_constants::COLUMN_INDEX);
	falaise_geom_id_.set_type(1302); // To change with constants
	falaise_geom_id_.set(0, module);
	falaise_geom_id_.set(1, side);
	falaise_geom_id_.set(2, column);
	falaise_geom_id_.set(3, row);
	falaise_geom_id_.set_any(4);
      }
    else if (geometric_id_.get_type() == fecom::tracker_constants::GEOMETRIC_CELL_TYPE)
      {
	int layer = geometric_id_.get(fecom::tracker_constants::LAYER_INDEX);
	int row = geometric_id_.get(fecom::tracker_constants::ROW_INDEX);
	falaise_geom_id_.set_type(1204);
	falaise_geom_id_.set(0, module);
	falaise_geom_id_.set(1, side);
	falaise_geom_id_.set(2, layer);
	falaise_geom_id_.set(3, row);
      }

    return;
  }

  void mock_hc2cd_module::initialize(const datatools::properties  &  setup_,
				     datatools::service_manager   & service_manager_,
				     dpp::module_handle_dict_type & /* module_dict_ */)
  {
    DT_THROW_IF(is_initialized(),
		std::logic_error,
		"Module '" << get_name() << "' is already initialized ! ");

    this->base_module::_common_initialize(setup_);

    if (_HCRD_label_.empty()) {
      if (setup_.has_key("HCRD_label")) {
	_HCRD_label_ = setup_.fetch_string("HCRD_label");
      }
    }
    if (_HCRD_label_.empty()) {
      _HCRD_label_ = fecom::mock_hc2cd_module::data_info::default_half_com_raw_data_label();
    }

    if (_CD_label_.empty()) {
      if (setup_.has_key("CD_label")) {
	_CD_label_ = setup_.fetch_string("CD_label");
      }
    }
    if (_CD_label_.empty()) {
      _CD_label_ = snemo::datamodel::data_info::default_calibrated_data_label();
    }

    if (_Geo_label_.empty()) {
      if (setup_.has_key("Geo_label")){
	_Geo_label_ = setup_.fetch_string("Geo_label");
      }
    }
    if (_Geo_label_.empty()) {
      _Geo_label_ = snemo::processing::service_info::default_geometry_service_label();
    }
    if (_geom_manager_ == 0) {
      DT_THROW_IF(_Geo_label_.empty(), std::logic_error,
		  "Module '" << get_name() << "' has no valid '" << _Geo_label_ << "' property !");
      DT_THROW_IF(! service_manager_.has(_Geo_label_) ||
		  ! service_manager_.is_a<geomtools::geometry_service>(_Geo_label_),
		  std::logic_error,
		  "Module '" << get_name() << "' has no '" << _Geo_label_ << "' service !");
      const geomtools::geometry_service & Geo = service_manager_.get<geomtools::geometry_service>(_Geo_label_);
      set_geom_manager(Geo.get_geom_manager());
    }
    DT_THROW_IF(_geom_manager_ == 0, std::logic_error, "Missing geometry manager !");

    // Module geometry category:
    if (_module_category_.empty()) {
      if (setup_.has_key("module_category")) {
	_module_category_ = setup_.fetch_string("module_category");
      }
    }
    // Default value:
    if (_module_category_.empty()) {
      _module_category_ = "module";
    }

    if (! has_external_random()) {
      int random_seed = 12345;
      if (setup_.has_key("random.seed")) {
	random_seed = setup_.fetch_integer("random.seed");
      }
      std::string random_id = "mt19937";
      if (setup_.has_key("random.id")) {
	random_id = setup_.fetch_string("random.id");
      }
      // Initialize the embedded random number generator:
      _random_.init(random_id, random_seed);
    }

    // Initialize the Geiger regime utility:
    _geiger_.initialize(setup_);

    const double time_unit = CLHEP::microsecond;

    // Set minimum drift time for peripheral hits:
    if (setup_.has_key("peripheral_drift_time_threshold")) {
      _peripheral_drift_time_threshold_ = setup_.fetch_real("peripheral_drift_time_threshold");
      if (! setup_.has_explicit_unit("peripheral_drift_time_threshold")) {
	_peripheral_drift_time_threshold_ *= time_unit;
      }
    }
    // Default value:
    if (! datatools::is_valid(_peripheral_drift_time_threshold_)) {
      _peripheral_drift_time_threshold_ = _geiger_.get_t0();
    }
    DT_LOG_DEBUG(get_logging_priority(), "peripheral_drift_time_threshold = "
		 << _peripheral_drift_time_threshold_ / CLHEP::microsecond << " us");

    // Set minium drift time for delayed hits:
    if (setup_.has_key("delayed_drift_time_threshold")) {
      _delayed_drift_time_threshold_ = setup_.fetch_real("delayed_drift_time_threshold");
      if (! setup_.has_explicit_unit("delayed_drift_time_threshold")) {
	_delayed_drift_time_threshold_ *= time_unit;
      }
    }
    // Default value:
    if (! datatools::is_valid(_delayed_drift_time_threshold_)) {
      _delayed_drift_time_threshold_ = _geiger_.get_tcut();
    }
    DT_LOG_DEBUG(get_logging_priority(), "delayed_drift_time_threshold = "
		 << _delayed_drift_time_threshold_ / CLHEP::microsecond << " us");

    this->base_module::_set_initialized(true);
    return;
  }

  void mock_hc2cd_module::reset()
  {
    DT_THROW_IF(! is_initialized(),
		std::logic_error,
		"Module '" << get_name() << "' is not initialized !");

    this->base_module::_set_initialized(false);

    if (! has_external_random()) {
      // Reset the random number generator:
      _random_.reset();
    }
    _external_random_ = 0;
    // Reset the Geiger regime utility:
    _geiger_.reset();
    _set_defaults();
    _module_category_.clear();
    return;
  }

  void mock_hc2cd_module::_set_defaults()
  {
    _module_category_.clear();
    _external_random_ = 0;
    _geom_manager_ = 0;
    _HCRD_label_.clear();
    _CD_label_.clear();
    _Geo_label_.clear();
    datatools::invalidate(_peripheral_drift_time_threshold_);
    datatools::invalidate(_delayed_drift_time_threshold_);
    return;
  }

  // Processing :
  dpp::base_module::process_status mock_hc2cd_module::process(datatools::things & event_record_)
  {
    DT_THROW_IF(! is_initialized(),
		std::logic_error,
		"Module '" << get_name() << "' is not initialized !");

    // Check simulated data *
    const bool abort_at_missing_input = true;
    // check if some 'simulated_data' are available in the data model:
    if (! event_record_.has(_HCRD_label_)) {
      DT_THROW_IF(abort_at_missing_input, std::logic_error, "Missing half commissioning raw data to be processed !");
      // leave the data unchanged.
      return dpp::base_module::PROCESS_ERROR;
    }
    // grab the 'hc_raw_data' entry from the data model :
    const fecom::commissioning_event & the_hc_raw_data
      = event_record_.get<fecom::commissioning_event>(_HCRD_label_);

    // Check calibrated data *
    const bool abort_at_former_output = false;
    const bool preserve_former_output = false;
    // check if some 'calibrated_data' are available in the data model:
    snemo::datamodel::calibrated_data * ptr_calib_data = 0;
    if (! event_record_.has(_CD_label_)) {
      ptr_calib_data = &(event_record_.add<snemo::datamodel::calibrated_data>(_CD_label_));
    } else {
      ptr_calib_data = &(event_record_.grab<snemo::datamodel::calibrated_data>(_CD_label_));
    }

    snemo::datamodel::calibrated_data & the_calibrated_data = *ptr_calib_data;
    if (the_calibrated_data.calibrated_calorimeter_hits().size() > 0) {
      DT_THROW_IF(abort_at_former_output, std::logic_error, "Already has processed calorimeter calibrated data !");
      if (! preserve_former_output) {
	the_calibrated_data.calibrated_calorimeter_hits().clear();
      }
    }

    if (the_calibrated_data.calibrated_tracker_hits().size() > 0) {
      DT_THROW_IF(abort_at_former_output, std::logic_error,
		  "Already has processed tracker calibrated data !");
      if (! preserve_former_output) {
	the_calibrated_data.calibrated_tracker_hits().clear();
      }
    }

    /********************
     * Process the data *
     ********************/

    // Main processing method :
    _process(the_hc_raw_data, the_calibrated_data);

    return dpp::base_module::PROCESS_SUCCESS;
  }

  mygsl::rng & mock_hc2cd_module::_get_random()
  {
    if (has_external_random()) return *_external_random_;
    return _random_;
  }

  void mock_hc2cd_module::_process(const fecom::commissioning_event & hc_raw_com_event_,
				   snemo::datamodel::calibrated_data & the_calibrated_data_)
  {
    DT_LOG_DEBUG(get_logging_priority(), "Entering...");

    DT_THROW_IF(!is_initialized(), std::logic_error, "Not initialized !");

    _process_calo_calibration(hc_raw_com_event_,
			      the_calibrated_data_.calibrated_calorimeter_hits());

    // _process_tracker_calibration(the_raw_tracker_hits,
    // the_calibrated_data_.calibrated_tracker_hits_());

    DT_LOG_DEBUG(get_logging_priority(), "Exiting.");
    return;
  }

  /** Calibrate calo hits from raw data informations:
   */
  void mock_hc2cd_module::_process_calo_calibration(const fecom::commissioning_event & hc_raw_com_event_,
						    snemo::datamodel::calibrated_data::calorimeter_hit_collection_type & calibrated_calorimeter_hits_)
  {
    DT_LOG_DEBUG(get_logging_priority(), "Entering...");

    uint32_t calibrated_calorimeter_hit_id = 0;

    calo_hit_calibration chb;

    for (auto icalo = hc_raw_com_event_.get_calo_hit_collection().begin();
	 icalo != hc_raw_com_event_.get_calo_hit_collection().end();
	 icalo++)
      {
	// Convert electronic ID into falaise geometric ID
	geomtools::geom_id fecom_geometric_id;
	// icalo.electronic_id
	_channel_mapping_->get_geometric_id_from_electronic_id(icalo->electronic_id, fecom_geometric_id);
	geomtools::geom_id falaise_geometric_id;
	convert_gid_to_falaise_gid(fecom_geometric_id,
				   falaise_geometric_id);

	// This geom_id is not used by any previous calorimeter hit:
	// we create a new calorimeter hit !
	snemo::datamodel::calibrated_data::calorimeter_hit_handle_type new_handle(new snemo::datamodel::calibrated_calorimeter_hit);
	snemo::datamodel::calibrated_calorimeter_hit & new_calibrated_calorimeter_hit = new_handle.grab();

	new_calibrated_calorimeter_hit.set_hit_id(calibrated_calorimeter_hit_id++);
	new_calibrated_calorimeter_hit.set_geom_id(falaise_geometric_id);

	// sigma time and sigma energy are computed later
	double time = (icalo->tdc_ns + icalo->falling_time_ns) * CLHEP::nanosecond;
	new_calibrated_calorimeter_hit.set_time(time);

	double energy = 0;
	double sigma_energy = 0;
	chb.calibrate_hit(falaise_geometric_id,
			  icalo->peak_volt * CLHEP::volt,
			  energy,
			  sigma_energy);

	std::clog << "GID : " << falaise_geometric_id
		  << " Time = " << time
		  << " Energy = " << energy
		  << " Sigma energy = " << sigma_energy << std::endl;
	new_calibrated_calorimeter_hit.set_energy(energy);

	// Append it to the collection :
	calibrated_calorimeter_hits_.push_back(new_handle);

      } // loop over hit calo

    return;
  }

  /** Calibrate tracker hits from raw data informations:
   */
  void mock_hc2cd_module::_process_tracker_calibration(const fecom::commissioning_event & /* hc_raw_com_event_*/,
						       snemo::datamodel::calibrated_data::tracker_hit_collection_type & /*calibrated_tracker_hits_*/)
  {
    // DT_LOG_DEBUG(get_logging_priority(), "Entering...");

    // // pickup the ID mapping from the geometry manager:
    // const geomtools::mapping & the_mapping = _geom_manager_->get_mapping();
    // const geomtools::id_mgr & the_id_mgr   = _geom_manager_->get_id_mgr();

    // // current module geometry ID and information:
    // int module_number = geomtools::geom_id::INVALID_ADDRESS;
    // const geomtools::geom_info * module_ginfo = 0;
    // const geomtools::placement * module_placement = 0;

    // int32_t calibrated_tracker_hit_id = 0;
    // // Loop on raw tracker hits:
    // for (raw_tracker_hit_col_type::const_iterator i = raw_tracker_hits_.begin();
    // 	 i != raw_tracker_hits_.end();
    // 	 i++) {
    //   // get a reference to the tracker hit:
    //   const snemo::datamodel::mock_raw_tracker_hit & the_raw_tracker_hit = *i;

    //   // create the calibrated tracker hit to build:
    //   snemo::datamodel::calibrated_data::tracker_hit_handle_type the_hit_handle(new snemo::datamodel::calibrated_tracker_hit);
    //   snemo::datamodel::calibrated_tracker_hit & the_calibrated_tracker_hit = the_hit_handle.grab();

    //   // extract the corresponding geom ID:
    //   const geomtools::geom_id & gid = the_raw_tracker_hit.get_geom_id();
    //   //int this_cell_module_number = geom_manager_->get_id_mgr().get(gid, "module");
    //   const int this_cell_module_number = gid.get(0);
    //   if (this_cell_module_number != module_number) {
    // 	// build the module GID by extraction from the cell GID:
    // 	geomtools::geom_id module_gid;
    // 	the_id_mgr.make_id(_module_category_, module_gid);
    // 	the_id_mgr.extract(gid, module_gid);
    // 	module_number    = this_cell_module_number;
    // 	module_ginfo     = &the_mapping.get_geom_info(module_gid);
    // 	module_placement = &(module_ginfo->get_world_placement());
    //   }

    //   // extract the geom info of the corresponding cell:
    //   const geomtools::geom_info & ginfo = the_mapping.get_geom_info(gid);

    //   // assign a hit ID and the geometry ID to the hit:
    //   the_calibrated_tracker_hit.set_hit_id(calibrated_tracker_hit_id);
    //   //the_raw_tracker_hit.get_hit_id());
    //   the_calibrated_tracker_hit.set_geom_id(gid);

    //   // Use the anode time :
    //   const double anode_time = the_raw_tracker_hit.get_drift_time();

    //   // Calibrate the transverse drift distance:
    //   double radius;
    //   double sigma_radius;
    //   datatools::invalidate(radius);
    //   datatools::invalidate(sigma_radius);

    //   if (datatools::is_valid(anode_time)) {
    // 	if (anode_time <= _delayed_drift_time_threshold_) {
    // 	  // Case of a normal/prompt hit :
    // 	  _geiger_.calibrate_drift_radius_from_drift_time(anode_time, radius, sigma_radius);
    // 	  the_calibrated_tracker_hit.set_anode_time(anode_time);
    // 	  if (anode_time > _peripheral_drift_time_threshold_) {
    // 	    DT_LOG_TRACE(get_logging_priority(),
    // 			 "Peripheral Geiger hit with anode time = " << anode_time / CLHEP::microsecond << " us");
    // 	    the_calibrated_tracker_hit.set_peripheral(true);
    // 	  }
    // 	} else {
    // 	  DT_LOG_TRACE(get_logging_priority(),
    // 		       "Delayed Geiger hit with anode time = " << anode_time / CLHEP::microsecond << " us");
    // 	  // 2012-03-29 FM : store the anode_time as the reference delayed time
    // 	  the_calibrated_tracker_hit.set_delayed_time(anode_time,
    // 						      _geiger_.get_sigma_anode_time(anode_time));
    // 	  // Case of a delayed Geiger hit :
    // 	  // 2012-03-29 FM : do no push anymore specific values, let the radius be invalid

    // 	  // radius       = _geiger_.get_r0();
    // 	  // sigma_radius = _geiger_.get_sigma_r(radius);
    // 	}
    //   } else {
    // 	the_calibrated_tracker_hit.set_noisy(true);
    // 	DT_LOG_DEBUG(get_logging_priority(), "Geiger cell is noisy");
    //   }
    //   if (datatools::is_valid(radius)) the_calibrated_tracker_hit.set_r(radius);
    //   if (datatools::is_valid(sigma_radius)) the_calibrated_tracker_hit.set_sigma_r(sigma_radius);

    //   // Calibrate the longitudinal drift distance:
    //   const double t1 = the_raw_tracker_hit.get_bottom_time();
    //   const double t2 = the_raw_tracker_hit.get_top_time();
    //   double z;
    //   double sigma_z;
    //   datatools::invalidate(z);
    //   datatools::invalidate(sigma_z);

    //   const double plasma_propagation_speed = _geiger_.get_plasma_longitudinal_speed();
    //   size_t missing_cathodes = 0;
    //   if (! datatools::is_valid(t1) && ! datatools::is_valid(t2)) {
    // 	// missing top/bottom cathode signals:
    // 	missing_cathodes = 2;
    // 	sigma_z = _geiger_.get_sigma_z(z, missing_cathodes);
    // 	z       = 0.0;
    // 	the_calibrated_tracker_hit.set_top_cathode_missing(true);
    // 	the_calibrated_tracker_hit.set_bottom_cathode_missing(true);
    //   } else if (! datatools::is_valid(t1) && datatools::is_valid(t2)) {
    // 	// missing bottom cathode signal:
    // 	missing_cathodes = 1;
    // 	const double mean_z = 0.5 *_geiger_.get_cell_length() - t2 * plasma_propagation_speed;
    // 	sigma_z = _geiger_.get_sigma_z(mean_z, missing_cathodes);
    // 	z       = _geiger_.randomize_z(_get_random(), mean_z, sigma_z);
    // 	the_calibrated_tracker_hit.set_bottom_cathode_missing(true);
    //   } else if (datatools::is_valid(t1) && ! datatools::is_valid(t2)) {
    // 	// missing top cathode signal:
    // 	missing_cathodes = 1;
    // 	const double mean_z = t1 * plasma_propagation_speed - 0.5 *_geiger_.get_cell_length();
    // 	sigma_z = _geiger_.get_sigma_z(mean_z, missing_cathodes);
    // 	z       = _geiger_.randomize_z(_get_random(), mean_z, sigma_z);
    // 	the_calibrated_tracker_hit.set_top_cathode_missing(true);
    //   } else {
    // 	missing_cathodes = 0;
    // 	const double plasma_propagation_speed_2 = _geiger_.get_cell_length() /(t1 + t2);
    // 	const double mean_z = 0.5 *_geiger_.get_cell_length() - t2 * plasma_propagation_speed_2;
    // 	sigma_z = _geiger_.get_sigma_z(mean_z, missing_cathodes);
    // 	z       = _geiger_.randomize_z(_get_random(), mean_z, sigma_z);
    //   }

    //   // set values in the calibrated tracker hit:
    //   if (datatools::is_valid(z)) the_calibrated_tracker_hit.set_z(z);
    //   if (datatools::is_valid(sigma_z)) the_calibrated_tracker_hit.set_sigma_z(sigma_z);

    //   // store the X-Y position of the cell within the module coordinate system:
    //   const double cell_x = 0.0;
    //   const double cell_y = 0.0;
    //   const double cell_z = 0.0;
    //   geomtools::vector_3d cell_self_pos(cell_x, cell_y, cell_z);
    //   geomtools::vector_3d cell_world_pos;
    //   ginfo.get_world_placement().child_to_mother(cell_self_pos, cell_world_pos);
    //   geomtools::vector_3d cell_module_pos;
    //   module_placement->mother_to_child(cell_world_pos, cell_module_pos);
    //   the_calibrated_tracker_hit.set_xy(cell_module_pos.getX(),
    // 					cell_module_pos.getY());

    //   // 2012-07-26 FM : suspend this for now :
    //   // if (the_raw_tracker_hit.has_hit_id())
    //   //   {
    //   //     the_calibrated_tracker_hit.get_auxiliaries()
    //   //       .store("raw_hit_id", the_raw_tracker_hit.get_hit_id());
    //   //   }

    //   // 2012-07-26 FM : add a reference to the MC true hit ID
    //   if (_store_mc_hit_id_) {
    // 	if (the_raw_tracker_hit.get_auxiliaries().has_key(mctools::hit_utils::HIT_MC_HIT_ID_KEY)) {
    // 	  const int true_tracker_hit_id = the_raw_tracker_hit.get_auxiliaries().fetch_integer(mctools::hit_utils::HIT_MC_HIT_ID_KEY);
    // 	  the_calibrated_tracker_hit.grab_auxiliaries().update(mctools::hit_utils::HIT_MC_HIT_ID_KEY,
    // 							       true_tracker_hit_id);
    // 	}
    //   }

    //   // save the calibrate tracker hit:
    //   calibrated_tracker_hits_.push_back(the_hit_handle);

    //   calibrated_tracker_hit_id++;
    // } // loop over raw tracker hits

    // DT_LOG_DEBUG(get_logging_priority(), "Exiting.");
    // return;
  }

} // end of namespace fecom

// end of falaise/snemo/processing/mock_hc2cd_module.cpp
