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

  void mock_hc2cd_module::convert_gid_to_falaise_gid(const geomtools::geom_id geometric_id_,
						     geomtools::geom_id & falaise_geom_id_)
  {
    int module = 0;
    int side = 1;
    if (geometric_id_.get_type() == fecom::calo_constants::GEOMETRIC_CALO_TYPE)
      {
	int column = geometric_id_.get(fecom::calo_constants::COLUMN_INDEX);
	int row = geometric_id_.get(fecom::calo_constants::ROW_INDEX);
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

    // Initialize the Geiger regime utility:
    datatools::properties gg_setup;
    // void 	store_real_with_explicit_unit (const std::string &prop_key, double a_value, const std::string &desc="", bool a_lock=false)
    // void 	set_unit_symbol (const std::string &prop_key, const std::string &unit_symbol="")
    gg_setup.store_real_with_explicit_unit("cell_diameter", 44 * CLHEP::mm);
    gg_setup.set_unit_symbol("cell_diameter", "mm");

    gg_setup.store_real_with_explicit_unit("cell_length", 2900 * CLHEP::mm);
    gg_setup.set_unit_symbol("cell_length", "mm");

    gg_setup.store_real_with_explicit_unit("tcut", 10 * CLHEP::microsecond);
    gg_setup.set_unit_symbol("tcut", "us");

    gg_setup.store_real_with_explicit_unit("sigma_anode_time", 25 * CLHEP::nanosecond);
    gg_setup.set_unit_symbol("sigma_anode_time", "ns");

    gg_setup.store_real_with_explicit_unit("sigma_cathode_time", 1 * CLHEP::microsecond);
    gg_setup.set_unit_symbol("sigma_cathode_time", "us");

    gg_setup.store_real("base_anode_efficiency", 1);

    gg_setup.store_real("base_cathode_efficiency", 1);

    gg_setup.store_real_with_explicit_unit("plasma_longitudinal_speed", 20 * CLHEP::cm / CLHEP::microsecond);
    gg_setup.set_unit_symbol("plasma_longitudinal_speed", "cm/us");

    gg_setup.store_real_with_explicit_unit("sigma_plasma_longitudinal_speed", 5 * CLHEP::cm / CLHEP::microsecond);
    gg_setup.set_unit_symbol("sigma_plasma_longitudinal_speed", "cm/us");

    gg_setup.store_real_with_explicit_unit("sigma_z", 5 * CLHEP::cm);
    gg_setup.set_unit_symbol("sigma_z", "cm");

    gg_setup.store_real_with_explicit_unit("sigma_z_missing_cathode", 20 * CLHEP::cm);
    gg_setup.set_unit_symbol("sigma_z_missing_cathode", "cm");

    _geiger_.initialize(gg_setup);

    _fout_.reset(new std::ofstream("__mock_hc2cd.data"));
    _fout_->precision(15);

    this->base_module::_set_initialized(true);
    return;
  }

  void mock_hc2cd_module::reset()
  {
    DT_THROW_IF(! is_initialized(),
		std::logic_error,
		"Module '" << get_name() << "' is not initialized !");

    this->base_module::_set_initialized(false);

    // Reset the Geiger regime utility:
    _geiger_.reset();
    _set_defaults();
    _module_category_.clear();
    return;
  }

  void mock_hc2cd_module::_set_defaults()
  {
    _module_category_.clear();
    _geom_manager_ = 0;
    _HCRD_label_.clear();
    _CD_label_.clear();
    _Geo_label_.clear();
    _tracker_clock_tick_ = tracker_constants::tracker_clock_tick();
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

  void mock_hc2cd_module::_process(const fecom::commissioning_event & hc_raw_com_event_,
				   snemo::datamodel::calibrated_data & the_calibrated_data_)
  {
    DT_LOG_DEBUG(get_logging_priority(), "Entering...");
    DT_THROW_IF(!is_initialized(), std::logic_error, "Not initialized !");

    _process_calo_calibration(hc_raw_com_event_,
			      the_calibrated_data_.calibrated_calorimeter_hits());

    _process_tracker_calibration(hc_raw_com_event_,
				 the_calibrated_data_.calibrated_tracker_hits());

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
	if (!icalo->low_threshold && !icalo->high_threshold) {
	  continue;
	}

	// Convert electronic ID into falaise geometric ID
	geomtools::geom_id fecom_geometric_id;

	// Check if the calo electronic mapping is in the bimap
	if (_channel_mapping_->is_calo_channel_in_map(icalo->electronic_id))
	  {
	    _channel_mapping_->get_geometric_id_from_electronic_id(icalo->electronic_id, fecom_geometric_id);
	  }
	// If no, skip the hit
	else break;
	geomtools::geom_id falaise_geometric_id;
	convert_gid_to_falaise_gid(fecom_geometric_id,
				   falaise_geometric_id);

	snemo::datamodel::calibrated_data::calorimeter_hit_handle_type new_handle(new snemo::datamodel::calibrated_calorimeter_hit);
	snemo::datamodel::calibrated_calorimeter_hit & new_calibrated_calorimeter_hit = new_handle.grab();

	new_calibrated_calorimeter_hit.set_hit_id(calibrated_calorimeter_hit_id++);
	new_calibrated_calorimeter_hit.set_geom_id(falaise_geometric_id);

	// sigma time and sigma energy are computed later
	double time = (icalo->tdc_ns + icalo->falling_time_ns - hc_raw_com_event_.get_time_start_ns() ) * CLHEP::nanosecond;
	new_calibrated_calorimeter_hit.set_time(time);

	double energy = 0;
	double sigma_energy = 0;
	chb.calibrate_hit(falaise_geometric_id,
			  icalo->peak_volt * CLHEP::volt,
			  energy,
			  sigma_energy);

	// std::clog << "GID : " << falaise_geometric_id
	// 	  << " Time = " << time
	// 	  << " Energy = " << energy
	// 	  << " Sigma energy = " << sigma_energy << std::endl;
	new_calibrated_calorimeter_hit.set_energy(energy);

	// Append it to the collection :
	calibrated_calorimeter_hits_.push_back(new_handle);

      } // loop over hit calo

    return;
  }

  /** Calibrate tracker hits from raw data informations:
   */
  void mock_hc2cd_module::_process_tracker_calibration(const fecom::commissioning_event & hc_raw_com_event_,
						       snemo::datamodel::calibrated_data::tracker_hit_collection_type & calibrated_tracker_hits_)
  {
    DT_LOG_DEBUG(get_logging_priority(), "Entering...");

    // pickup the ID mapping from the geometry manager:
    const geomtools::mapping & the_mapping = _geom_manager_->get_mapping();
    const geomtools::id_mgr & the_id_mgr   = _geom_manager_->get_id_mgr();

    // current module geometry ID and information:
    int module_number = geomtools::geom_id::INVALID_ADDRESS;
    const geomtools::geom_info * module_ginfo = 0;
    const geomtools::placement * module_placement = 0;

    uint32_t calibrated_tracker_hit_id = 0;

    //// std::vector<double> anode_times;

    for (auto itracker = hc_raw_com_event_.get_tracker_hit_collection().begin();
	 itracker != hc_raw_com_event_.get_tracker_hit_collection().end();
	 itracker++)
      {
	if (itracker->is_valid()) {
	// Convert electronic ID into falaise geometric ID
	geomtools::geom_id fecom_geometric_id = itracker->cell_geometric_id;

	geomtools::geom_id falaise_geometric_id;
	convert_gid_to_falaise_gid(fecom_geometric_id,
				   falaise_geometric_id);

	const int this_cell_module_number = falaise_geometric_id.get(0);
        if (this_cell_module_number != module_number) {
          // build the module FALAISE_GEOMETRIC_ID by extraction from the cell FALAISE_GEOMETRIC_ID:
          geomtools::geom_id module_falaise_geometric_id;
          the_id_mgr.make_id(_module_category_, module_falaise_geometric_id);
          the_id_mgr.extract(falaise_geometric_id, module_falaise_geometric_id);
          module_number    = this_cell_module_number;
          module_ginfo     = &the_mapping.get_geom_info(module_falaise_geometric_id);
          module_placement = &(module_ginfo->get_world_placement());
        }

        // extract the geom info of the corresponding cell:
        const geomtools::geom_info & ginfo = the_mapping.get_geom_info(falaise_geometric_id);

	snemo::datamodel::calibrated_data::tracker_hit_handle_type new_handle(new snemo::datamodel::calibrated_tracker_hit);
	snemo::datamodel::calibrated_tracker_hit & new_calibrated_tracker_hit = new_handle.grab();

	new_calibrated_tracker_hit.set_hit_id(calibrated_tracker_hit_id++);
	new_calibrated_tracker_hit.set_geom_id(falaise_geometric_id);

	double anode_time;
	double radius;
	double sigma_radius;
	datatools::invalidate(radius);
	datatools::invalidate(sigma_radius);
	datatools::invalidate(anode_time);

	if (itracker->has_anodic_t0()) {
	  // std::clog.precision(15);
	  // std::clog << "Event anodic t0    = " << itracker->get_anodic_t0() << std::endl
	  // 	    << "Tracker clock tick = " << _tracker_clock_tick_ << std::endl
	  // 	    << "Mult               = " << itracker->get_anodic_t0() * _tracker_clock_tick_ << std::endl
	  // 	    << "Event Time start   = " << hc_raw_com_event_.get_time_start_ns() << std::endl;

	  anode_time = (itracker->anodic_t0_ns - hc_raw_com_event_.get_time_start_ns()) * CLHEP::nanosecond;
	  std::clog << "Anode time         = " << anode_time / CLHEP::nanosecond << " ns" << std::endl;

	  if (anode_time < 0) anode_time = 1.0 * CLHEP::nanosecond;
	  if (_fout_.get() != nullptr) {
	    *_fout_ << anode_time << '\n';
	  }
	  if (datatools::is_valid(anode_time)) {
	    _geiger_.calibrate_drift_radius_from_drift_time(anode_time,
							    radius,
							    sigma_radius);

	    std::clog << "Anode time = " << anode_time / CLHEP::nanosecond << " ns" << std::endl
	    	      << "Radius     = " << radius / CLHEP::cm << " cm" << std::endl
	    	      << "Sigma rad  = " << sigma_radius / CLHEP::cm << " cm" << std::endl << std::endl;

	    if (!datatools::is_valid(radius))
	      {
		radius = 20 * CLHEP::mm;
		sigma_radius = 2 * CLHEP::mm;
	      }

	    new_calibrated_tracker_hit.set_anode_time(anode_time);
	  }
	} else {
	  new_calibrated_tracker_hit.set_noisy(true);
	  DT_LOG_DEBUG(get_logging_priority(), "Geiger cell is noisy");
	}

	double z;
	double sigma_z;
	datatools::invalidate(z);
	datatools::invalidate(sigma_z);

	if (datatools::is_valid(radius)) {
	  new_calibrated_tracker_hit.set_r(radius);
	  z = 0 * CLHEP::mm;
	  sigma_z = 1 * CLHEP::meter;
	}
	if (datatools::is_valid(sigma_radius)) new_calibrated_tracker_hit.set_sigma_r(sigma_radius);
	if (datatools::is_valid(z)) new_calibrated_tracker_hit.set_z(z);
	if (datatools::is_valid(sigma_z)) new_calibrated_tracker_hit.set_sigma_z(sigma_z);

	new_calibrated_tracker_hit.set_peripheral(false);

	new_calibrated_tracker_hit.set_top_cathode_missing(true);
	new_calibrated_tracker_hit.set_bottom_cathode_missing(true);

	// store the X-Y position of the cell within the module coordinate system:
	const double cell_x = 0.0;
	const double cell_y = 0.0;
	const double cell_z = 0.0;
	geomtools::vector_3d cell_self_pos(cell_x, cell_y, cell_z);
	geomtools::vector_3d cell_world_pos;
	ginfo.get_world_placement().child_to_mother(cell_self_pos, cell_world_pos);
	geomtools::vector_3d cell_module_pos;
	module_placement->mother_to_child(cell_world_pos, cell_module_pos);

	new_calibrated_tracker_hit.set_xy(cell_module_pos.getX(),
					  cell_module_pos.getY());

	// // Add the geiger cell only if anode time is valid
	// if (datatools::is_valid(anode_time))
	//   {

	// Append it to the collection :
	calibrated_tracker_hits_.push_back(new_handle);
	//// anode_times.push_back(anode_time);

	} // end of itrack is valid

      } // end of itracker++

    // DT_LOG_DEBUG(get_logging_priority(), "Exiting.");
    return;
  }

} // end of namespace fecom

// end of fecom/src/libs/libfecom/fecom/mock_hc2cd_module.cpp
