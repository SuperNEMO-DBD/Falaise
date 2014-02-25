// -*- mode: c++ ; -*-
/** \file falaise/snemo/processing/mock_tracker_s2c_module.cc
 */

// Ourselves:
#include <falaise/snemo/processing/mock_tracker_s2c_module.h>

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
#include <mctools/simulated_data.h>
#include <mctools/utils.h>

// This project :
#include <falaise/snemo/datamodels/data_model.h>

namespace snemo {

  namespace processing {

    // Registration instantiation macro :
    DPP_MODULE_REGISTRATION_IMPLEMENT(mock_tracker_s2c_module,
                                      "snemo::processing::mock_tracker_s2c_module");

    double mock_tracker_s2c_module::get_peripheral_drift_time_threshold() const
    {
      return _peripheral_drift_time_threshold_;
    }

    double mock_tracker_s2c_module::get_delayed_drift_time_threshold() const
    {
      return _delayed_drift_time_threshold_;
    }

    bool mock_tracker_s2c_module::has_external_random() const
    {
      return _external_random_ != 0;
    }

    void mock_tracker_s2c_module::reset_external_random()
    {
      DT_THROW_IF(is_initialized(),
                  std::logic_error,
                  "Module '" << get_name() << "' is already initialized ! ");
      _external_random_ = 0;
      return;
    }

    void mock_tracker_s2c_module::set_external_random(mygsl::rng & rng_)
    {
      DT_THROW_IF(is_initialized(),
                  std::logic_error,
                  "Module '" << get_name() << "' is already initialized ! ");
      _external_random_ = &rng_;
      return;
    }

    void mock_tracker_s2c_module::set_geom_manager(const geomtools::manager & gmgr_)
    {
      DT_THROW_IF(is_initialized(),
                  std::logic_error,
                  "Module '" << get_name() << "' is already initialized ! ");
      _geom_manager_ = &gmgr_;

      // Check setup label:
      const std::string & setup_label = _geom_manager_->get_setup_label();
      DT_THROW_IF(!(setup_label == "snemo::demonstrator" || setup_label == "snemo::tracker_commissioning"),
                  std::logic_error,
                  "Setup label '" << setup_label << "' is not supported !");
      return;
    }

    void mock_tracker_s2c_module::initialize(const datatools::properties  &  setup_,
                                             datatools::service_manager   & service_manager_,
                                             dpp::module_handle_dict_type & /* module_dict_ */)
    {
      DT_THROW_IF(is_initialized(),
                  std::logic_error,
                  "Module '" << get_name() << "' is already initialized ! ");

      this->base_module::_common_initialize(setup_);

      if (_SD_label_.empty()) {
        if (setup_.has_key("SD_label")) {
          _SD_label_ = setup_.fetch_string("SD_label");
        }
      }
      if (_SD_label_.empty()) {
        _SD_label_ = snemo::datamodel::data_info::SIMULATED_DATA_LABEL;
      }

      if (_CD_label_.empty()) {
        if (setup_.has_key("CD_label")) {
          _CD_label_ = setup_.fetch_string("CD_label");
        }
      }
      if (_CD_label_.empty()) {
        _CD_label_ = snemo::datamodel::data_info::CALIBRATED_DATA_LABEL;
      }

      if (setup_.has_key("module_category"))
        {
          _module_category_ = setup_.fetch_string("module_category");
        }

      if (setup_.has_key("hit_category"))
        {
          _hit_category_ = setup_.fetch_string("hit_category");
        }

      const double time_unit = CLHEP::microsecond;
      if (setup_.has_key("peripheral_drift_time_threshold"))
        {
          _peripheral_drift_time_threshold_ = setup_.fetch_real("peripheral_drift_time_threshold");
          if (! setup_.has_explicit_unit("peripheral_drift_time_threshold"))
            _peripheral_drift_time_threshold_ *= time_unit;
        }

      if (setup_.has_key("delayed_drift_time_threshold"))
        {
          _delayed_drift_time_threshold_ = setup_.fetch_real("delayed_drift_time_threshold");
          if (! setup_.has_explicit_unit("delayed_drift_time_threshold"))
            _delayed_drift_time_threshold_ *= time_unit;
        }

      std::string geo_label = "Geo";
      if (setup_.has_key("Geo_label"))
        {
          geo_label = setup_.fetch_string("Geo_label");
        }
      if (_geom_manager_ == 0)
        {
          DT_THROW_IF(geo_label.empty(), std::logic_error,
                      "Module '" << get_name() << "' has no valid '" << "Geo_label" << "' property !");
          DT_THROW_IF(! service_manager_.has(geo_label) ||
                      ! service_manager_.is_a<geomtools::geometry_service>(geo_label),
                      std::logic_error,
                      "Module '" << get_name() << "' has no '" << geo_label << "' service !");
          geomtools::geometry_service & Geo = service_manager_.get<geomtools::geometry_service>(geo_label);
          set_geom_manager(Geo.get_geom_manager());
        }
      DT_THROW_IF(_geom_manager_ == 0, std::logic_error, "Missing geometry manager !");

      if (! has_external_random())
        {
          int random_seed = 12345;
          if (setup_.has_key("random.seed"))
            {
              random_seed = setup_.fetch_integer("random.seed");
            }
          std::string random_id = "mt19937";
          if (setup_.has_key("random.id"))
            {
              random_id = setup_.fetch_string("random.id");
            }

          // Initialize the embedded random number generator:
          _random_.init(random_id, random_seed);
        }

      // Initialize the Geiger regime utility:
      _geiger_.initialize(setup_);

      if (! datatools::is_valid(_peripheral_drift_time_threshold_))
        {
          _peripheral_drift_time_threshold_ = _geiger_.get_t0();
        }

      if (! datatools::is_valid(_delayed_drift_time_threshold_))
        {
          _delayed_drift_time_threshold_ = _geiger_.get_tcut();
        }

      DT_LOG_DEBUG(get_logging_priority(), "peripheral_drift_time_threshold = "
                   << _peripheral_drift_time_threshold_ / CLHEP::microsecond << " us");
      DT_LOG_DEBUG(get_logging_priority(), "delayed_drift_time_threshold = "
                   << _delayed_drift_time_threshold_ / CLHEP::microsecond << " us");

      // 2012-07-26 FM : support reference to the MC true hit ID
      if (setup_.has_flag("store_mc_hit_id"))
        {
          _store_mc_hit_id_ = true;
        }

      this->base_module::_set_initialized(true);
      return;
    }

    void mock_tracker_s2c_module::reset()
    {
      DT_THROW_IF(! is_initialized(),
                  std::logic_error,
                  "Module '" << get_name() << "' is not initialized !");

      if (! has_external_random())
        {
          // Reset the random number generator:
          _random_.reset();
        }

      // Reset the Geiger regime utility:
      _geiger_.reset();

      this->base_module::_set_initialized(false);
      return;
    }

    // Constructor :
    mock_tracker_s2c_module::mock_tracker_s2c_module(datatools::logger::priority logging_priority_)
      : dpp::base_module(logging_priority_)
    {
      _geom_manager_    = 0;

      _module_category_ = "module";
      _hit_category_    = "gg";
      _external_random_ = 0;

      datatools::invalidate(_peripheral_drift_time_threshold_);
      datatools::invalidate(_delayed_drift_time_threshold_);

      _SD_label_ = snemo::datamodel::data_info::SIMULATED_DATA_LABEL;
      _CD_label_ = snemo::datamodel::data_info::CALIBRATED_DATA_LABEL;

      // 2012-07-26 FM : support reference to the MC true hit ID
      _store_mc_hit_id_ = false;

      return;
    }

    // Destructor :
    mock_tracker_s2c_module::~mock_tracker_s2c_module()
    {
      if (is_initialized()) mock_tracker_s2c_module::reset();
      return;
    }

    // Processing :
    dpp::base_module::process_status mock_tracker_s2c_module::process(datatools::things & event_record_)
    {
      DT_THROW_IF(! is_initialized(),
                  std::logic_error,
                  "Module '" << get_name() << "' is not initialized !");

      // Check simulated data *
      const bool abort_at_missing_input = true;
      // check if some 'simulated_data' are available in the data model:
      if (! event_record_.has(_SD_label_))
        {
          DT_THROW_IF(abort_at_missing_input, std::logic_error, "Missing simulated data to be processed !");
          // leave the data unchanged.
          return dpp::base_module::PROCESS_ERROR;
        }
      // grab the 'simulated_data' entry from the data model :
      const mctools::simulated_data & the_simulated_data
        = event_record_.get<mctools::simulated_data>(_SD_label_);

      // Check calibrated data *
      const bool abort_at_former_output = false;
      const bool preserve_former_output = false;
      // check if some 'calibrated_data' are available in the data model:
      snemo::datamodel::calibrated_data * ptr_calib_data = 0;
      if (! event_record_.has(_CD_label_))
        {
          ptr_calib_data = &(event_record_.add<snemo::datamodel::calibrated_data>(_CD_label_));
        }
      else
        {
          ptr_calib_data = &(event_record_.grab<snemo::datamodel::calibrated_data>(_CD_label_));
        }
      snemo::datamodel::calibrated_data & the_calibrated_data = *ptr_calib_data;
      if (the_calibrated_data.calibrated_tracker_hits().size() > 0)
        {
          DT_THROW_IF(abort_at_former_output, std::logic_error,
                      "Already has processed tracker calibrated data !");
          if (! preserve_former_output)
            {
              the_calibrated_data.calibrated_tracker_hits().clear();
            }
        }

      /********************
       * Process the data *
       ********************/

      // Main processing method :
      _process(the_simulated_data, the_calibrated_data.calibrated_tracker_hits());

      return dpp::base_module::PROCESS_SUCCESS;
    }

    mygsl::rng & mock_tracker_s2c_module::_get_random()
    {
      if (has_external_random()) return *_external_random_;
      return _random_;
    }

    /**
     * Here collect the Geiger raw hits from the simulation data source
     * and build the final list of digitized 'tracker' hits.
     *
     */
    void mock_tracker_s2c_module::_process_tracker_digitization(const mctools::simulated_data & simulated_data_,
                                                                mock_tracker_s2c_module::raw_tracker_hit_col_type & raw_tracker_hits_)
    {
      DT_LOG_DEBUG(get_logging_priority(), "Entering...");

      if (! simulated_data_.has_step_hits(_hit_category_))
        {
          // Nothing to do.
          return;
        }

      // reset the output raw tracker hits collection:
      raw_tracker_hits_.clear();

      // pickup the ID mapping from the geometry manager:
      const geomtools::mapping & the_mapping = _geom_manager_->get_mapping();

      // reset the raw hit numbering:
      uint32_t raw_tracker_hit_id = 0;

      // Loop on Geiger step hits:
      const size_t nb_hits = simulated_data_.get_number_of_step_hits(_hit_category_);
      for (size_t ihit = 0; ihit < nb_hits; ++ihit)
        {
          // get a reference to the step hit
          // through the handle :
          // if (! i->has_data()) continue;
          // const base_step_hit & a_step_hit = i->get();

          // XXX : Maybe inefficient :
          const mctools::base_step_hit & a_tracker_hit = simulated_data_.get_step_hit(_hit_category_, ihit);

          // The hit ID of the true hit :
          int true_tracker_hit_id = geomtools::base_hit::INVALID_HIT_ID;
          if (a_tracker_hit.has_hit_id())
            {
              true_tracker_hit_id = a_tracker_hit.get_hit_id();
            }

          // extract the corresponding geom ID:
          const geomtools::geom_id & gid = a_tracker_hit.get_geom_id();

          // extract the geom info of the corresponding cell:
          const geomtools::geom_info & ginfo = the_mapping.get_geom_info(gid);

          // the position of the ion/electron pair creation within the cell volume:
          const geomtools::vector_3d & ionization_world_pos = a_tracker_hit.get_position_start();

          // the position of the Geiger avalanche impact on the anode wire:
          const geomtools::vector_3d & avalanche_impact_world_pos
            = a_tracker_hit.get_position_stop();

          // compute the position of the anode impact in the drift cell coordinates reference frame:
          geomtools::vector_3d avalanche_impact_cell_pos;
          ginfo.get_world_placement().mother_to_child(avalanche_impact_world_pos,
                                                      avalanche_impact_cell_pos);
          // longitudinal position:
          const double longitudinal_position = avalanche_impact_cell_pos.z();

          // true drift distance:
          const double drift_distance =(avalanche_impact_world_pos - ionization_world_pos).mag();
          const double anode_efficiency = _geiger_.get_anode_efficiency(drift_distance);
          const double r = _get_random().uniform();
          if (r > anode_efficiency)
            {
              // This hit is lost due to anode signal inefficiency:
              DT_LOG_DEBUG(get_logging_priority(), "Geiger cell efficiency below anode efficiency !");
              continue;
            }

          // the time of the ion/electron pair creation:
          const double ionization_time = a_tracker_hit.get_time_start();

          /*** Anode TDC ***/
          // randomize the expected Geiger drift time:
          const double expected_drift_time = _geiger_.randomize_drift_time_from_drift_distance(_get_random(),
                                                                                               drift_distance);
          const double anode_time          = ionization_time + expected_drift_time;
          const double sigma_anode_time    = _geiger_.get_sigma_anode_time(anode_time);

          /*** Cathodes TDCs ***/
          const double cathode_efficiency = _geiger_.get_cathode_efficiency();
          double bottom_cathode_time;
          double top_cathode_time;
          datatools::invalidate(bottom_cathode_time);
          datatools::invalidate(top_cathode_time);
          const double sigma_cathode_time = _geiger_.get_sigma_cathode_time();
          size_t missing_cathodes = 2;
          const double r1 = _get_random().uniform();
          if (r1 < cathode_efficiency)
            {
              const double l_bottom = longitudinal_position + 0.5 * _geiger_.get_cell_length();
              const double mean_bottom_cathode_time  = l_bottom / _geiger_.get_plasma_longitudinal_speed();
              const double sigma_bottom_cathode_time = 0.0;
              bottom_cathode_time = _get_random().gaussian(mean_bottom_cathode_time, sigma_bottom_cathode_time);
              if (bottom_cathode_time < 0.0) bottom_cathode_time = 0.0;
              missing_cathodes--;
            }
          const double r2 = _get_random().uniform();
          if (r2 < cathode_efficiency)
            {
              const double l_top =  0.5 * _geiger_.get_cell_length() - longitudinal_position;
              const double mean_top_cathode_time  = l_top / _geiger_.get_plasma_longitudinal_speed();
              const double sigma_top_cathode_time = 0.0;
              top_cathode_time = _get_random().gaussian(mean_top_cathode_time, sigma_top_cathode_time);
              if (top_cathode_time < 0.0) top_cathode_time = 0.0;
              missing_cathodes--;
            }

          // find if some tracker hit already uses this geom ID:
          geomtools::base_hit::has_geom_id_predicate pred_has_gid(gid);
          raw_tracker_hit_col_type::iterator found =
            std::find_if(raw_tracker_hits_.begin(),
                         raw_tracker_hits_.end(),
                         pred_has_gid);
          if (found == raw_tracker_hits_.end())
            {
              // This geom_id is not used by any previous tracker hit: we create a new tracker hit !
              {
                snemo::datamodel::mock_raw_tracker_hit dummy;
                // add the new tracker hit in the list:
                raw_tracker_hits_.push_back(dummy);
              }
              snemo::datamodel::mock_raw_tracker_hit & new_raw_tracker_hit = raw_tracker_hits_.back();

              // assign a hit ID and the geometry ID to the hit:
              new_raw_tracker_hit.set_hit_id(raw_tracker_hit_id);
              new_raw_tracker_hit.set_geom_id(a_tracker_hit.get_geom_id());
              // 2012-07-26 FM : support reference to the MC true hit ID
              if (_store_mc_hit_id_ && true_tracker_hit_id > geomtools::base_hit::INVALID_HIT_ID)
                {
                  new_raw_tracker_hit.grab_auxiliaries().store(mctools::hit_utils::HIT_MC_HIT_ID_KEY,
                                                               true_tracker_hit_id); // XXX
                }

              if (datatools::is_valid(anode_time))
                {
                  new_raw_tracker_hit.set_drift_time(anode_time);
                  new_raw_tracker_hit.set_sigma_drift_time(sigma_anode_time);
                  if (datatools::is_valid(top_cathode_time))
                    {
                      new_raw_tracker_hit.set_top_time(top_cathode_time);
                      new_raw_tracker_hit.set_sigma_top_time(sigma_cathode_time);
                    }
                  if (datatools::is_valid(bottom_cathode_time))
                    {
                      new_raw_tracker_hit.set_bottom_time(bottom_cathode_time);
                      new_raw_tracker_hit.set_sigma_bottom_time(sigma_cathode_time);
                    }
                }
              raw_tracker_hit_id++;
            }
          else
            {
              // This geom_id is already used by some previous tracker hit: we update this hit !
              snemo::datamodel::mock_raw_tracker_hit & some_raw_tracker_hit = *found;

              if (datatools::is_valid(anode_time))
                {
                  if (anode_time < some_raw_tracker_hit.get_drift_time())
                    {
                      // reset TDC infos for the current hit:
                      some_raw_tracker_hit.invalidate_times();
                      // update TDC infos:
                      some_raw_tracker_hit.set_drift_time(anode_time);
                      some_raw_tracker_hit.set_sigma_drift_time(sigma_anode_time);
                      if (datatools::is_valid(top_cathode_time))
                        {
                          some_raw_tracker_hit.set_top_time(top_cathode_time);
                          some_raw_tracker_hit.set_sigma_top_time(sigma_cathode_time);
                        }
                      if (datatools::is_valid(bottom_cathode_time))
                        {
                          some_raw_tracker_hit.set_bottom_time(bottom_cathode_time);
                          some_raw_tracker_hit.set_sigma_bottom_time(sigma_cathode_time);
                        }
                      // 2012-07-26 FM : support reference to the MC true hit ID
                      if (_store_mc_hit_id_ && true_tracker_hit_id > geomtools::base_hit::INVALID_HIT_ID)
                        {
                          some_raw_tracker_hit.grab_auxiliaries().update(mctools::hit_utils::HIT_MC_HIT_ID_KEY,
                                                                         true_tracker_hit_id); // XXX
                        }
                    }
                }
            }
        }

      DT_LOG_DEBUG(get_logging_priority(), "Exiting.");
      return;
    }

    /** Calibrate tracker hits from digitization informations:
     */
    void mock_tracker_s2c_module::_process_tracker_calibration(const mock_tracker_s2c_module::raw_tracker_hit_col_type & raw_tracker_hits_,
                                                               snemo::datamodel::calibrated_data::tracker_hit_collection_type & calibrated_tracker_hits_)
    {
      DT_LOG_DEBUG(get_logging_priority(), "Entering...");

      // pickup the ID mapping from the geometry manager:
      const geomtools::mapping & the_mapping = _geom_manager_->get_mapping();
      const geomtools::id_mgr & the_id_mgr   = _geom_manager_->get_id_mgr();

      // current module geometry ID and information:
      int module_number = geomtools::geom_id::INVALID_ADDRESS;
      geomtools::geom_id module_gid;
      const geomtools::geom_info * module_ginfo = 0;
      const geomtools::placement * module_placement = 0;

      int32_t calibrated_tracker_hit_id = 0;
      // Loop on raw tracker hits:
      for (raw_tracker_hit_col_type::const_iterator i = raw_tracker_hits_.begin();
           i != raw_tracker_hits_.end();
           i++)
        {
          // get a reference to the tracker hit:
          const snemo::datamodel::mock_raw_tracker_hit & the_raw_tracker_hit = *i;

          // create the calibrated tracker hit to build:
          snemo::datamodel::calibrated_data::tracker_hit_handle_type the_hit_handle(new snemo::datamodel::calibrated_tracker_hit);
          snemo::datamodel::calibrated_tracker_hit & the_calibrated_tracker_hit = the_hit_handle.grab();

          // extract the corresponding geom ID:
          const geomtools::geom_id & gid = the_raw_tracker_hit.get_geom_id();
          //int this_cell_module_number = geom_manager_->get_id_mgr().get(gid, "module");
          const int this_cell_module_number = gid.get(0);
          if (this_cell_module_number != module_number)
            {
              // build the module GID by extraction from the cell GID:
              geomtools::geom_id module_gid;
              the_id_mgr.make_id(_module_category_, module_gid);
              the_id_mgr.extract(gid, module_gid);
              module_number    = this_cell_module_number;
              module_ginfo     = &the_mapping.get_geom_info(module_gid);
              module_placement = &(module_ginfo->get_world_placement());
            }

          // extract the geom info of the corresponding cell:
          const geomtools::geom_info & ginfo = the_mapping.get_geom_info(gid);

          // assign a hit ID and the geometry ID to the hit:
          the_calibrated_tracker_hit.set_hit_id(calibrated_tracker_hit_id);
          //the_raw_tracker_hit.get_hit_id());
          the_calibrated_tracker_hit.set_geom_id(gid);

          // Use the anode time :
          const double anode_time = the_raw_tracker_hit.get_drift_time();

          // Calibrate the transverse drift distance:
          double radius;
          double sigma_radius;
          datatools::invalidate(radius);
          datatools::invalidate(sigma_radius);

          if (datatools::is_valid(anode_time))
            {
              if (anode_time <= _delayed_drift_time_threshold_)
                {
                  // Case of a normal/prompt hit :
                  _geiger_.calibrate_drift_radius_from_drift_time(anode_time, radius, sigma_radius);
                  the_calibrated_tracker_hit.set_anode_time(anode_time);
                  if (anode_time > _peripheral_drift_time_threshold_)
                    {
                      the_calibrated_tracker_hit.set_peripheral(true);
                    }
                }
              else
                {
                  // 2012-03-29 FM : store the anode_time as the reference delayed time
                  the_calibrated_tracker_hit.set_delayed_time(anode_time,
                                                              _geiger_.get_sigma_anode_time(anode_time));
                  // Case of a delayed Geiger hit :
                  // 2012-03-29 FM : do no push anymore specific values, let the radius be invalid

                  // radius       = _geiger_.get_r0();
                  // sigma_radius = _geiger_.get_sigma_r(radius);
                }
            }
          else
            {
              the_calibrated_tracker_hit.set_noisy(true);
              DT_LOG_DEBUG(get_logging_priority(), "Geiger cell is noisy");
            }
          if (datatools::is_valid(radius)) the_calibrated_tracker_hit.set_r(radius);
          if (datatools::is_valid(sigma_radius)) the_calibrated_tracker_hit.set_sigma_r(sigma_radius);

          // Calibrate the longitudinal drift distance:
          const double t1 = the_raw_tracker_hit.get_bottom_time();
          const double t2 = the_raw_tracker_hit.get_top_time();
          double z;
          double sigma_z;
          datatools::invalidate(z);
          datatools::invalidate(sigma_z);

          const double plasma_propagation_speed = _geiger_.get_plasma_longitudinal_speed();
          size_t missing_cathodes = 0;
          if (! datatools::is_valid(t1) && ! datatools::is_valid(t2))
            {
              // missing top/bottom cathode signals:
              missing_cathodes = 2;
              sigma_z = _geiger_.get_sigma_z(z, missing_cathodes);
              z       = 0.0;
              the_calibrated_tracker_hit.set_top_cathode_missing(true);
              the_calibrated_tracker_hit.set_bottom_cathode_missing(true);
            }
          else if (! datatools::is_valid(t1) && datatools::is_valid(t2))
            {
              // missing bottom cathode signal:
              missing_cathodes = 1;
              const double mean_z = 0.5 *_geiger_.get_cell_length() - t2 * plasma_propagation_speed;
              sigma_z = _geiger_.get_sigma_z(mean_z, missing_cathodes);
              z       = _geiger_.randomize_z(_get_random(), mean_z, sigma_z);
              the_calibrated_tracker_hit.set_bottom_cathode_missing(true);
            }
          else if (datatools::is_valid(t1) && ! datatools::is_valid(t2))
            {
              // missing top cathode signal:
              missing_cathodes = 1;
              const double mean_z = t1 * plasma_propagation_speed - 0.5 *_geiger_.get_cell_length();
              sigma_z = _geiger_.get_sigma_z(mean_z, missing_cathodes);
              z       = _geiger_.randomize_z(_get_random(), mean_z, sigma_z);
              the_calibrated_tracker_hit.set_top_cathode_missing(true);
            }
          else
            {
              missing_cathodes = 0;
              const double plasma_propagation_speed = _geiger_.get_cell_length() /(t1 + t2);
              const double mean_z = 0.5 *_geiger_.get_cell_length() - t2 * plasma_propagation_speed;
              sigma_z = _geiger_.get_sigma_z(mean_z, missing_cathodes);
              z       = _geiger_.randomize_z(_get_random(), mean_z, sigma_z);
            }

          // set values in the calibrated tracker hit:
          if (datatools::is_valid(z)) the_calibrated_tracker_hit.set_z(z);
          if (datatools::is_valid(sigma_z)) the_calibrated_tracker_hit.set_sigma_z(sigma_z);

          // store the X-Y position of the cell within the module coordinate system:
          const double cell_x = 0.0;
          const double cell_y = 0.0;
          const double cell_z = 0.0;
          geomtools::vector_3d cell_self_pos(cell_x, cell_y, cell_z);
          geomtools::vector_3d cell_world_pos;
          ginfo.get_world_placement().child_to_mother(cell_self_pos, cell_world_pos);
          geomtools::vector_3d cell_module_pos;
          module_placement->mother_to_child(cell_world_pos, cell_module_pos);
          the_calibrated_tracker_hit.set_xy(cell_module_pos.getX(),
                                            cell_module_pos.getY());

          // 2012-07-26 FM : suspend this for now :
          // if (the_raw_tracker_hit.has_hit_id())
          //   {
          //     the_calibrated_tracker_hit.get_auxiliaries()
          //       .store("raw_hit_id", the_raw_tracker_hit.get_hit_id());
          //   }

          // 2012-07-26 FM : add a reference to the MC true hit ID
          if (_store_mc_hit_id_)
            {
              if (the_raw_tracker_hit.get_auxiliaries().has_key(mctools::hit_utils::HIT_MC_HIT_ID_KEY))
                {
                  const int true_tracker_hit_id = the_raw_tracker_hit.get_auxiliaries().fetch_integer(mctools::hit_utils::HIT_MC_HIT_ID_KEY);
                  the_calibrated_tracker_hit.grab_auxiliaries().update(mctools::hit_utils::HIT_MC_HIT_ID_KEY,
                                                                       true_tracker_hit_id);
                }
            }

          // save the calibrate tracker hit:
          calibrated_tracker_hits_.push_back(the_hit_handle);

          calibrated_tracker_hit_id++;
        } // loop over raw tracker hits

      DT_LOG_DEBUG(get_logging_priority(), "Exiting.");
      return;
    }


    void mock_tracker_s2c_module::_process(const mctools::simulated_data & simulated_data_,
                                           snemo::datamodel::calibrated_data::tracker_hit_collection_type & calibrated_tracker_hits_)
    {
      DT_LOG_DEBUG(get_logging_priority(), "Entering...");

      DT_THROW_IF(! is_initialized(), std::logic_error, "Not initialized !");

      // temporary raw tracker hits to be constructed:
      raw_tracker_hit_col_type the_raw_tracker_hits;

      // process "Geiger" raw(simulated) hits to build the list of digitized tracker hits:
      _process_tracker_digitization(simulated_data_, the_raw_tracker_hits);

      // process digitized tracker(simulated) hits and calibrate geometry informations:
      _process_tracker_calibration(the_raw_tracker_hits, calibrated_tracker_hits_);

      DT_LOG_DEBUG(get_logging_priority(), "Exiting.");
      return;
    }

  } // end of namespace processing

} // end of namespace snemo

/********************************
 * OCD support : implementation *
 ********************************/

#include <datatools/object_configuration_description.h>

/** Opening macro for implementation
 *  @arg snemo::processing::mock_tracker_s2c_module the full class name
 *  @arg ocd_ is the identifier of the 'datatools::object_configuration_description'
 *            to be initialized (passed by mutable reference).
 */
DOCD_CLASS_IMPLEMENT_LOAD_BEGIN(snemo::processing::mock_tracker_s2c_module,ocd_)
{
  ocd_.set_class_name("snemo::processing::mock_tracker_s2c_module");
  ocd_.set_class_description("A module that performs a mock digitization/calibration of the tracker simulated data");
  ocd_.set_class_library("falaise");
  //ocd_.set_class_documentation("");

  dpp::base_module::common_ocd(ocd_);


  {
    std::ostringstream ldesc;
    ldesc << "This is the name of the bank to be used \n"
          << "as the input simulated calorimeter hits.    \n"
          << "Default value is: \"" << snemo::datamodel::data_info::SIMULATED_DATA_LABEL<< "\".  \n";
    // Description of the 'SD_label' configuration property :
    datatools::configuration_property_description & cpd
      = ocd_.add_property_info();
    cpd.set_name_pattern("SD_label")
      .set_terse_description("The label/name of the 'simulated data' bank")
      .set_traits(datatools::TYPE_STRING)
      .set_mandatory(false)
      .set_long_description(ldesc.str())
      .add_example("Use an alternative name for the 'simulated data' bank:: \n"
                   "                                \n"
                   "  SD_label : string = \"SD2\"   \n"
                   "                                \n"
                   )
      ;
  }


  {
    std::ostringstream ldesc;
    ldesc << "This is the name of the bank to be used \n"
          << "as the output calibrated calorimeter hits.    \n"
          << "Default value is: \"" << snemo::datamodel::data_info::CALIBRATED_DATA_LABEL<< "\".  \n";
    // Description of the 'CD_label' configuration property :
    datatools::configuration_property_description & cpd
      = ocd_.add_property_info();
    cpd.set_name_pattern("CD_label")
      .set_terse_description("The label/name of the 'calibrated data' bank")
      .set_traits(datatools::TYPE_STRING)
      .set_mandatory(false)
      .set_long_description(ldesc.str())
      .add_example("Use an alternative name for the 'calibrated data' bank:: \n"
                   "                                \n"
                   "  CD_label : string = \"CD2\"   \n"
                   "                                \n"
                   )
      ;
  }


  {
    std::ostringstream ldesc;
    ldesc << "This is the name of the service to be used as the \n"
          << "geometry service.                                 \n"
          << "Default value is: \"" << "Geo" << "\".            \n"
          << "This property is only used if no geoemtry manager \n"
          << "as been provided to the module.                   \n";
    // Description of the 'Geo_label' configuration property :
    datatools::configuration_property_description & cpd
      = ocd_.add_property_info();
    cpd.set_name_pattern("Geo_label")
      .set_terse_description("The label/name of the geometry service")
      .set_traits(datatools::TYPE_STRING)
      .set_mandatory(false)
      .set_long_description(ldesc.str())
      .add_example("Use an alternative name for the geometry service:: \n"
                   "                                    \n"
                   "  Geo_label : string = \"geometry\" \n"
                   "                                    \n"
                   )
      ;
  }

  {
     // Description of the 'random.seed' configuration property :
    datatools::configuration_property_description & cpd
      = ocd_.add_property_info();
    cpd.set_name_pattern("random.seed")
      .set_terse_description("The seed for the embedded PRNG")
      .set_traits(datatools::TYPE_INTEGER)
      .set_mandatory(false)
      .set_long_description("Default value: ``12345``")
      .set_complex_triggering_conditions(true)
      .add_example("Use an alternative seed for the PRNG:: \n"
                   "                                       \n"
                   "  random.seed : integer = 314159       \n"
                   "                                       \n"
                   )
      ;
  }

  {
     // Description of the 'random.id' configuration property :
    datatools::configuration_property_description & cpd
      = ocd_.add_property_info();
    cpd.set_name_pattern("random.id")
      .set_terse_description("The Id for the embedded PRNG (GSL)")
      .set_traits(datatools::TYPE_STRING)
      .set_mandatory(false)
      .set_long_description("Default value: ``mt19937``")
      .set_complex_triggering_conditions(true)
      .add_example("Use an alternative Id for the PRNG::   \n"
                   "                                       \n"
                   "  random.id : string = \"taus2\"       \n"
                   "                                       \n"
                   )
      ;
  }

  {
     // Description of the 'module_category' configuration property :
    datatools::configuration_property_description & cpd
      = ocd_.add_property_info();
    cpd.set_name_pattern("module_category")
      .set_terse_description("The geometry category of the SuperNEMO module")
      .set_traits(datatools::TYPE_STRING)
      .set_mandatory(false)
      .set_long_description("Default value: ``\"module\"``")
      .add_example("Use the default value::                 \n"
                   "                                        \n"
                   "  module_category : string = \"module\" \n"
                   "                                        \n"
                   )
      ;
  }

  {
     // Description of the 'peripheral_drift_time_threshold' configuration property :
    datatools::configuration_property_description & cpd
      = ocd_.add_property_info();
    cpd.set_name_pattern("peripheral_drift_time_threshold")
      .set_terse_description("The minimum drift time for peripheral Geiger hit")
      .set_traits(datatools::TYPE_REAL)
      .set_mandatory(false)
      .set_long_description("Default value: ``4.0 us``")
      .add_example("Use the default value::                         \n"
                   "                                       \n"
                   "  peripheral_drift_time_threshold : real = 4.0 us \n"
                   "                                       \n"
                   )
      ;
  }

  {
     // Description of the 'delayed_drift_time_threshold' configuration property :
    datatools::configuration_property_description & cpd
      = ocd_.add_property_info();
    cpd.set_name_pattern("delayed_drift_time_threshold")
      .set_terse_description("The minimum drift time for delayed Geiger hit")
      .set_traits(datatools::TYPE_REAL)
      .set_mandatory(false)
      .set_long_description("Default value: ``10.0 us``")
      .add_example("Use the default value::                         \n"
                   "                                                \n"
                   "  delayed_drift_time_threshold : real = 10.0 us \n"
                   "                                                \n"
                   )
      ;
  }

  {
     // Description of the 'store_mc_hit_id' configuration property :
    datatools::configuration_property_description & cpd
      = ocd_.add_property_info();
    cpd.set_name_pattern("store_mc_hit_id")
      .set_terse_description("Flag to activate the storage of the truth hit Ids in the calibrated hits")
      .set_traits(datatools::TYPE_BOOLEAN)
      .set_mandatory(false)
      .set_long_description("Default value: ``0`` ")
      .add_example("Use the default value::          \n"
                   "                                 \n"
                   "  store_mc_hit_id : boolean = 0  \n"
                   "                                 \n"
                   )
      ;
  }

  {
     // Description of the 'hit_category' configuration property :
    datatools::configuration_property_description & cpd
      = ocd_.add_property_info();
    cpd.set_name_pattern("hit_category")
      .set_terse_description("The category of the traker hits to be processed")
      .set_traits(datatools::TYPE_STRING)
      .set_mandatory(false)
      .set_long_description("Default value: ``\"gg\"``                   \n"
                            "For this hit category, we must declare some \n"
                            "configuration parameters for the associated \n"
                            "geiger regime (see OCD support for the      \n"
                            "``snemo::processing::geiger_regime`` class).\n"
                            )
      .add_example("Use the default value::          \n"
                   "                                 \n"
                   "  hit_category : string = \"gg\" \n"
                   "                                 \n"
                   )
      ;
  }

  // Additionnal configuration hints :
  ocd_.set_configuration_hints("Here is a full configuration example in the                  \n"
                               "``datatools::properties`` ASCII format::                     \n"
                                "                                                            \n"
                               "  SD_label        : string = \"SD\"                          \n"
                               "  CD_label        : string = \"CD\"                          \n"
                               "  Geo_label       : string = \"geometry\"                    \n"
                               "  random.seed     : integer = 314159                         \n"
                               "  random.id       : string = \"taus2\"                       \n"
                               "  module_category : string = \"module\"                      \n"
                               "  peripheral_drift_time_threshold : real = 4.0 us            \n"
                               "  delayed_drift_time_threshold    : real = 10.0 us           \n"
                               "  store_mc_hit_id  : boolean = 0                             \n"
                               "  hit_category     : string = \"gg\"                         \n"
                               "  cell_diameter    : real as length = 44 mm                  \n"
                               "  cell_length      : real as length = 2900 mm                \n"
                               "  sigma_anode_time : real as time = 12.5 ns                  \n"
                               "  tcut             : real as time = 10.0 us                  \n"
                               "  sigma_cathode_time      : real = 100 ns                    \n"
                               "  sigma_z                 : real = 1.0 cm                    \n"
                               "  sigma_z_missing_cathode : real as length = 5.0 cm          \n"
                               "  sigma_r_a  : real as length = 0.425 mm                     \n"
                               "  sigma_r_b  : real = 0.0083                                 \n"
                               "  sigma_r_r0 : real as length = 12.25 mm                     \n"
                               "  base_anode_efficiency     : real = 1.0                     \n"
                               "  base_cathode_efficiency   : real = 1.0                     \n"
                               "  plasma_longitudinal_speed : real = 5.0 cm/us               \n"
                               "  sigma_plasma_longitudinal_speed : real = 0.5 cm/us         \n"
                               "                                                             \n"
                               );

  ocd_.set_validation_support(true);
  ocd_.lock();
  return;
}
DOCD_CLASS_IMPLEMENT_LOAD_END() // Closing macro for implementation

// Registration macro for class 'snemo::processing::mock_tracker_s2c_module' :
DOCD_CLASS_SYSTEM_REGISTRATION(snemo::processing::mock_tracker_s2c_module,
                               "snemo::processing::mock_tracker_s2c_module")

// end of falaise/snemo/processing/mock_tracker_s2c_module.cc
