// -*- mode: c++ ; -*-
/** \file falaise/snemo/processing/mock_calorimeter_s2c_module.cc
 */

// Ourselves:
#include <falaise/snemo/processing/mock_calorimeter_s2c_module.h>

// Standard library:
#include <stdexcept>
#include <sstream>

// Third party:
// - Bayeux/datatools:
#include <datatools/service_manager.h>
#include <datatools/utils.h>
// - Bayeux/geomtools:
#include <geomtools/manager.h>
#include <geomtools/geometry_service.h>
// - Bayeux/mctools:
#include <mctools/simulated_data.h>
#include <mctools/utils.h>

// This project :
#include <falaise/snemo/datamodels/data_model.h>
#include <falaise/snemo/processing/services.h>

namespace snemo {

  namespace processing {

    // Registration instantiation macro :
    DPP_MODULE_REGISTRATION_IMPLEMENT(mock_calorimeter_s2c_module,
                                      "snemo::processing::mock_calorimeter_s2c_module");

    void mock_calorimeter_s2c_module::set_geom_manager(const geomtools::manager & gmgr_)
    {
      DT_THROW_IF(is_initialized(),
                  std::logic_error,
                  "Module '" << get_name() << "' is already initialized ! ");
      _geom_manager_ = &gmgr_;

      // Check setup label:
      const std::string & setup_label = _geom_manager_->get_setup_label();
      DT_THROW_IF(setup_label != "snemo::demonstrator" && setup_label != "snemo::tracker_commissioning",
                  std::logic_error,
                  "Setup label '" << setup_label << "' is not supported !");
      return;
    }

    const geomtools::manager & mock_calorimeter_s2c_module::get_geom_manager() const
    {
      DT_THROW_IF(! is_initialized(),
                  std::logic_error,
                  "Module '" << get_name() << "' is not initialized ! ");
      return *_geom_manager_;
    }

    bool mock_calorimeter_s2c_module::has_external_random() const
    {
      return _external_random_ != 0;
    }

    void mock_calorimeter_s2c_module::reset_external_random()
    {
      DT_THROW_IF(is_initialized(),
                  std::logic_error,
                  "Module '" << get_name() << "' is already initialized ! ");
      _external_random_ = 0;
      return;
    }

    void mock_calorimeter_s2c_module::set_external_random(mygsl::rng & rng_)
    {
      DT_THROW_IF(is_initialized(),
                  std::logic_error,
                  "Module '" << get_name() << "' is already initialized ! ");
      _external_random_ = &rng_;
      return;
    }

    void mock_calorimeter_s2c_module::initialize(const datatools::properties  & setup_,
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
        geomtools::geometry_service & Geo = service_manager_.get<geomtools::geometry_service>(_Geo_label_);
        set_geom_manager(Geo.get_geom_manager());
      }
      DT_THROW_IF(_geom_manager_ == 0, std::logic_error, "Missing geometry manager !");

      if (! has_external_random()) {
        int random_seed  = 12345;
        if (setup_.has_key("random.seed"))  {
          random_seed = setup_.fetch_integer("random.seed");
        }
        std::string random_id = "mt19937";
        if (setup_.has_key("random.id")) {
          random_id = setup_.fetch_string("random.id");
        }

        // Initialize the embedded random number generator:
        _random_.init(random_id, random_seed);
      }

      // Get the calorimeter categories:
      if (setup_.has_key("hit_categories")) {
        setup_.fetch("hit_categories", _hit_categories_);
      }

      // Initialize the calorimeter regime utility:
      for (std::vector<std::string>::const_iterator icategory = _hit_categories_.begin();
           icategory != _hit_categories_.end();
           ++icategory) {
        const std::string & the_category = *icategory;
        {
          calorimeter_regime tmp_regime;
          _calorimeter_regimes_[the_category] = tmp_regime;
        }
        calorimeter_regime & a_regime = _calorimeter_regimes_[the_category];
        a_regime.set_category(the_category);
        datatools::properties per_category_setup;
        setup_.export_and_rename_starting_with(per_category_setup, the_category + ".", "");
        a_regime.initialize(per_category_setup);
        if (get_logging_priority() >= datatools::logger::PRIO_DEBUG) {
          DT_LOG_DEBUG(get_logging_priority(), "Calorimeter '" << the_category << "' parameters:");
          a_regime.tree_dump(std::clog, "", "[debug]: ");
        }
      }

      // Setup trigger time
      if (!datatools::is_valid(_cluster_time_width_)) {
        if (setup_.has_key("cluster_time_width")) {
          _cluster_time_width_ = setup_.fetch_real("cluster_time_width");
          if (!setup_.has_explicit_unit("cluster_time_width")) {
            _cluster_time_width_ *= CLHEP::ns;
          }
        }
      }
      if (!datatools::is_valid(_cluster_time_width_)) {
        _cluster_time_width_ = 100 * CLHEP::ns;
       }

      // 2012-09-17 FM : support reference to the MC true hit ID
      if (setup_.has_flag("store_mc_hit_id")) {
        _store_mc_hit_id_ = true;
      }

      // Get the alpha quenching:
      if (setup_.has_key("alpha_quenching")) {
        _alpha_quenching_ = setup_.fetch_boolean("alpha_quenching");
      }

      this->base_module::_set_initialized(true);
      return;
    }

    void mock_calorimeter_s2c_module::reset()
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

      // Reset the calorimeter regime utility:
      for (calorimeter_regime_col_type::iterator icalo = _calorimeter_regimes_.begin();
           icalo != _calorimeter_regimes_.end(); ++icalo) {
        icalo->second.reset();
      }
      _hit_categories_.clear();
      _set_defaults();
      return;
    }

    void mock_calorimeter_s2c_module::_set_defaults()
    {
      _external_random_ = 0;
      _geom_manager_ = 0;
      _SD_label_.clear();
      _CD_label_.clear();
      _Geo_label_.clear();
      datatools::invalidate(_cluster_time_width_);
      _alpha_quenching_ = true;
      _store_mc_hit_id_ = false;
      return;
    }


    // Constructor :
    mock_calorimeter_s2c_module::mock_calorimeter_s2c_module(datatools::logger::priority logging_priority_)
      : dpp::base_module(logging_priority_)
    {
      _geom_manager_ = 0;
      _external_random_ = 0;
      _set_defaults();
      return;
    }

    // Destructor :
    mock_calorimeter_s2c_module::~mock_calorimeter_s2c_module()
    {
      if (is_initialized()) mock_calorimeter_s2c_module::reset();
      return;
    }

    // Processing :
    dpp::base_module::process_status mock_calorimeter_s2c_module::process(datatools::things & event_record_)
    {
      DT_THROW_IF(! is_initialized(),
                  std::logic_error,
                  "Module '" << get_name() << "' is not initialized !");

      // Check simulated data
      const bool abort_at_missing_input = true;
      // check if some 'simulated_data' are available in the data model:
      if (! event_record_.has(_SD_label_)) {
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

      // Main processing method :
      _process(the_simulated_data, the_calibrated_data.calibrated_calorimeter_hits());

      return dpp::base_module::PROCESS_SUCCESS;
    }

    mygsl::rng & mock_calorimeter_s2c_module::_get_random()
    {
      if (has_external_random()) return *_external_random_;
      return _random_;
    }

    //
    // Here collect the 'calorimeter' raw hits from the simulation data source
    // and build the final list of calibrated 'calorimeter' hits.
    //
    //

    void mock_calorimeter_s2c_module::_process_calorimeter_digitization
    (const mctools::simulated_data & simulated_data_,
     snemo::datamodel::calibrated_data::calorimeter_hit_collection_type & calibrated_calorimeter_hits_)
    {
      DT_LOG_DEBUG(get_logging_priority(), "Entering...");

      uint32_t calibrated_calorimeter_hit_id = 0;

      // Loop over all 'calorimeter hit' categories:
      for (std::vector<std::string>::const_iterator icategory = _hit_categories_.begin();
           icategory != _hit_categories_.end(); ++icategory) {
        const std::string & category = *icategory;
        if (! simulated_data_.has_step_hits(category)) {
          // Nothing to do.
          continue;
        }

        const size_t nb_hits = simulated_data_.get_number_of_step_hits(category);
        for (size_t ihit = 0; ihit < nb_hits; ++ihit) {
          // Get a reference to the step hit through the handle :
          //if(! i->has_data()) continue;
          //const base_step_hit & a_calo_mc_hit = i->get();
          const mctools::base_step_hit & a_calo_mc_hit = simulated_data_.get_step_hit(category, ihit);

          // The hit ID of the true hit :
          int true_calo_hit_id = geomtools::base_hit::INVALID_HIT_ID;
          if (a_calo_mc_hit.has_hit_id()) {
            true_calo_hit_id = a_calo_mc_hit.get_hit_id();
          }

          // Extract the corresponding geom ID:
          const geomtools::geom_id & gid = a_calo_mc_hit.get_geom_id();

          // Get the step hit time start:
          const double step_hit_time_start = a_calo_mc_hit.get_time_start();

          // Get the step hit energy deposit:
          double step_hit_energy_deposit = a_calo_mc_hit.get_energy_deposit();

          // Quench the alpha particle energy:
          if (a_calo_mc_hit.get_particle_name() == "alpha" && _alpha_quenching_) {
            const calorimeter_regime & the_calo_regime = _calorimeter_regimes_.at(*icategory);
            const double quenched_energy
              = the_calo_regime.quench_alpha_energy(step_hit_energy_deposit);

            step_hit_energy_deposit = quenched_energy;
          }

          // Find if some calorimeter hit already uses this geom ID :
          geomtools::base_hit::has_geom_id_predicate pred_has_gid(gid);
          // Wrapper predicates :
          datatools::mother_to_daughter_predicate<geomtools::base_hit, snemo::datamodel::calibrated_calorimeter_hit> pred_M2D(pred_has_gid);
          datatools::handle_predicate<snemo::datamodel::calibrated_calorimeter_hit> pred_via_handle(pred_M2D);

          snemo::datamodel::calibrated_data::calorimeter_hit_collection_type::reverse_iterator found
            = std::find_if(calibrated_calorimeter_hits_.rbegin(),
                           calibrated_calorimeter_hits_.rend(),
                           pred_via_handle);

          if (found == calibrated_calorimeter_hits_.rend()) {
            // This geom_id is not used by any previous calorimeter hit:
            // we create a new calorimeter hit !
            snemo::datamodel::calibrated_data::calorimeter_hit_handle_type new_handle(new snemo::datamodel::calibrated_calorimeter_hit);
            snemo::datamodel::calibrated_calorimeter_hit & new_calibrated_calorimeter_hit = new_handle.grab();

            new_calibrated_calorimeter_hit.set_hit_id(calibrated_calorimeter_hit_id++);
            new_calibrated_calorimeter_hit.set_geom_id(a_calo_mc_hit.get_geom_id());

            // sigma time and sigma energy are computed later
            new_calibrated_calorimeter_hit.set_time(step_hit_time_start);
            new_calibrated_calorimeter_hit.set_energy(step_hit_energy_deposit);

            // Add a properties to ease the final calibration
            new_calibrated_calorimeter_hit.grab_auxiliaries().store("category", *icategory);

            // 2012-09-17 FM : support reference to the MC true hit ID
            if (_store_mc_hit_id_ && true_calo_hit_id > geomtools::base_hit::INVALID_HIT_ID) {
              new_calibrated_calorimeter_hit.grab_auxiliaries().store(mctools::hit_utils::HIT_MC_HIT_ID_KEY, true_calo_hit_id);
            }

            // Append it to the collection :
            calibrated_calorimeter_hits_.push_back(new_handle);
          } else {
            // This geom_id is already used by some previous calorimeter hit:
            // we update this hit !
            snemo::datamodel::calibrated_calorimeter_hit & some_calibrated_calorimeter_hit = found->grab();

            // Grab auxiliaries :
            datatools::properties & cc_prop = some_calibrated_calorimeter_hit.grab_auxiliaries();

            // 2012-07-26 FM : support reference to the MC true hit ID
            if (_store_mc_hit_id_ && true_calo_hit_id > geomtools::base_hit::INVALID_HIT_ID) {
              cc_prop.update(mctools::hit_utils::HIT_MC_HIT_ID_KEY, true_calo_hit_id);
            }

            // Check time between clusters
            const double delta_time = step_hit_time_start - some_calibrated_calorimeter_hit.get_time();
            // cluster arriving too late : do not care of it
            if (delta_time > _cluster_time_width_) {
              cc_prop.update_flag("pile_up");
              continue;
            }

            // Cluster coming before : it becomes the new one
            if (delta_time < -_cluster_time_width_) {
              cc_prop.update_flag("pile_up");
              some_calibrated_calorimeter_hit.set_time(step_hit_time_start);
              some_calibrated_calorimeter_hit.set_energy(step_hit_energy_deposit);
              continue;
            }

            // Find the first hit in time
            const double calo_time = std::min(step_hit_time_start,
                                              some_calibrated_calorimeter_hit.get_time());
            some_calibrated_calorimeter_hit.set_time(calo_time);

            // Sum energies
            const double calo_energy = step_hit_energy_deposit
              + some_calibrated_calorimeter_hit.get_energy();
            some_calibrated_calorimeter_hit.set_energy(calo_energy);
          }
        } // loop over hits
      } // loop over hit category

      DT_LOG_DEBUG(get_logging_priority(), "Exiting.");
      return;
    }

    // Calibrate calorimeter hits from digitization informations:

    void mock_calorimeter_s2c_module::_process_calorimeter_calibration
    (snemo::datamodel::calibrated_data::calorimeter_hit_collection_type & calibrated_calorimeter_hits_)
    {
      DT_LOG_DEBUG(get_logging_priority(), "Entering..." );

      for (snemo::datamodel::calibrated_data::calorimeter_hit_collection_type::iterator
             icalo = calibrated_calorimeter_hits_.begin();
           icalo != calibrated_calorimeter_hits_.end(); ++icalo) {
        snemo::datamodel::calibrated_calorimeter_hit & the_calo_cluster = icalo->grab();

        // Setting category in order to get the correct energy resolution:
        // first recover the calorimeter category
        const std::string & category_name
          = the_calo_cluster.get_auxiliaries().fetch_string("category");
        const calorimeter_regime & the_calo_regime = _calorimeter_regimes_.at(category_name);

        // Compute a random 'experimental' energy taking into account
        // the expected energy resolution of the calorimeter hit:
        const double energy           = the_calo_cluster.get_energy();
        const double exp_energy       = the_calo_regime.randomize_energy(_get_random(), energy);
        const double exp_sigma_energy = the_calo_regime.get_sigma_energy(exp_energy);
        the_calo_cluster.set_energy(exp_energy);
        the_calo_cluster.set_sigma_energy(exp_sigma_energy);

        // Compute a random 'experimental' time taking into account
        // the expected time resolution of the calorimeter hit:
        const double time           = the_calo_cluster.get_time();
        const double exp_time       = the_calo_regime.randomize_time(_get_random(), time, exp_energy);
        const double exp_sigma_time = the_calo_regime.get_sigma_time(exp_energy);
        the_calo_cluster.set_time(exp_time);
        the_calo_cluster.set_sigma_time(exp_sigma_time);
      }

      DT_LOG_DEBUG(get_logging_priority(), "Exiting.");
      return;
    }

    // Select calorimeter hit following trigger conditions

    void mock_calorimeter_s2c_module::_process_calorimeter_trigger
    (snemo::datamodel::calibrated_data::calorimeter_hit_collection_type & calibrated_calorimeter_hits_)
    {
      DT_LOG_DEBUG(get_logging_priority(), "Entering..." );

      bool high_threshold = false;
      for (snemo::datamodel::calibrated_data::calorimeter_hit_collection_type::iterator
             icalo = calibrated_calorimeter_hits_.begin();
           icalo != calibrated_calorimeter_hits_.end(); ++icalo)  {
        snemo::datamodel::calibrated_calorimeter_hit & the_calo_cluster = icalo->grab();

        // Setting category in order to get the correct trigger parameters:
        // first recover the calorimeter category
        const std::string & category_name = the_calo_cluster.get_auxiliaries().fetch_string("category");
        const calorimeter_regime & the_calo_regime = _calorimeter_regimes_.at(category_name);
        const double energy = the_calo_cluster.get_energy();
        if (the_calo_regime.is_high_threshold(energy)) {
          high_threshold = true;
          // Find one is enough
          break;
        }
      }

      if (high_threshold) {
        // Search for low threshold hits:
        for (snemo::datamodel::calibrated_data::calorimeter_hit_collection_type::iterator
               icalo = calibrated_calorimeter_hits_.begin();
             icalo != calibrated_calorimeter_hits_.end();/*++icalo*/) {
          snemo::datamodel::calibrated_calorimeter_hit & the_calo_cluster = icalo->grab();

          const std::string & category_name = the_calo_cluster.get_auxiliaries().fetch_string("category");
          const calorimeter_regime & the_calo_regime = _calorimeter_regimes_.at(category_name);
          const double energy = the_calo_cluster.get_energy();
          // If energy hit is too low then remove calorimeter hit
          if (! the_calo_regime.is_low_threshold(energy)) {
            icalo = calibrated_calorimeter_hits_.erase(icalo);
          } else {
            ++icalo;
          }
        }
      } else {
        DT_LOG_DEBUG(get_logging_priority(), "No 'high threshold' calorimeter hit was found !");
        calibrated_calorimeter_hits_.clear();
      }

      DT_LOG_DEBUG(get_logging_priority(), "Exiting.");
      return;
    }


    void mock_calorimeter_s2c_module::_process
    (const mctools::simulated_data & simulated_data_,
     snemo::datamodel::calibrated_data::calorimeter_hit_collection_type & calibrated_calorimeter_hits_)
    {
      DT_LOG_DEBUG(get_logging_priority(), "Entering...");
      DT_THROW_IF(! is_initialized(), std::logic_error, "Module not intialized !");

      // process "calo"-like raw(simulated) hits to build the list of digitized calorimeter hits:
      _process_calorimeter_digitization(simulated_data_, calibrated_calorimeter_hits_);

      // process digitized calorimeter(simulated) hits and calibrate geometry informations:
      _process_calorimeter_calibration(calibrated_calorimeter_hits_);

      // process digitized calorimeter(simulated) hits and select triggered ones:
      _process_calorimeter_trigger(calibrated_calorimeter_hits_);

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
 *  @arg snemo::processing::mock_calorimeter_s2c_module the full class name
 *  @arg ocd_ is the identifier of the 'datatools::object_configuration_description'
 *            to be initialized (passed by mutable reference).
 */
DOCD_CLASS_IMPLEMENT_LOAD_BEGIN(snemo::processing::mock_calorimeter_s2c_module,ocd_)
{
  ocd_.set_class_name("snemo::processing::mock_calorimeter_s2c_module");
  ocd_.set_class_description("A module that performs a mock digitization/calibration of the calorimeter simulated data");
  ocd_.set_class_library("falaise");
  //ocd_.set_class_documentation("");

  dpp::base_module::common_ocd(ocd_);

  {
    // Description of the 'SD_label' configuration property :
    datatools::configuration_property_description & cpd
      = ocd_.add_property_info();
    cpd.set_name_pattern("SD_label")
      .set_terse_description("The label/name of the 'simulated data' bank")
      .set_traits(datatools::TYPE_STRING)
      .set_mandatory(false)
      .set_long_description("This is the name of the bank to be used   \n"
                            "as the input simulated calorimeter hits.  \n"
                            )
      .set_default_value_string(snemo::datamodel::data_info::default_simulated_data_label())
      .add_example("Use an alternative name for the 'simulated data' bank:: \n"
                   "                                \n"
                   "  SD_label : string = \"SD2\"   \n"
                   "                                \n"
                   )
      ;
  }


  {
    // Description of the 'CD_label' configuration property :
    datatools::configuration_property_description & cpd
      = ocd_.add_property_info();
    cpd.set_name_pattern("CD_label")
      .set_terse_description("The label/name of the 'calibrated data' bank")
      .set_traits(datatools::TYPE_STRING)
      .set_mandatory(false)
      .set_long_description("This is the name of the bank to be used    \n"
                            "as the output calibrated calorimeter hits. \n"
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
    std::ostringstream ldesc;
    ldesc << "This is the name of the service to be used as the \n"
          << "geometry service.                                 \n"
          << "This property is only used if no geometry manager \n"
          << "as been provided to the module.                   \n";
    // Description of the 'Geo_label' configuration property :
    datatools::configuration_property_description & cpd
      = ocd_.add_property_info();
    cpd.set_name_pattern("Geo_label")
      .set_terse_description("The label/name of the geometry service")
      .set_traits(datatools::TYPE_STRING)
      .set_mandatory(false)
      .set_long_description(ldesc.str())
      .set_default_value_string(snemo::processing::service_info::default_geometry_service_label())
      .add_example("Use an alternative name for the geometry service:: \n"
                   "                                     \n"
                   "  Geo_label : string = \"geometry2\" \n"
                   "                                     \n"
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
      .set_long_description("Only used if no external PRNG is provided.")
      .set_default_value_integer(12345)
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
      .set_terse_description("The Id for the embedded PRNG")
      .set_traits(datatools::TYPE_STRING)
      .set_mandatory(false)
      .set_long_description("Only used if no external PRNG is provided.")
      .set_default_value_string("mt19937")
      .add_example("Use an alternative Id for the PRNG::   \n"
                   "                                       \n"
                   "  random.id : string = \"taus2\"       \n"
                   "                                       \n"
                   )
      ;
  }

  {
    // Description of the 'cluster_time_width' configuration property :
    datatools::configuration_property_description & cpd
      = ocd_.add_property_info();
    cpd.set_name_pattern("cluster_time_width")
      .set_terse_description("The width of the clustering time window")
      .set_traits(datatools::TYPE_REAL)
      .set_mandatory(false)
      .set_long_description("This is the time spread in which one consider a set of \n"
                            "calorimeter hits to be part of the same signal.        \n")
      .set_explicit_unit(true)
      .set_unit_label("energy")
      .set_unit_symbol("ns")
      .set_default_value_real(100 * CLHEP::ns)
      .add_example("Use the default value::                     \n"
                   "                                             \n"
                   "  cluster_time_width : real as time = 100 ns \n"
                   "                                             \n"
                   )
      ;
  }

  {
    // Description of the 'alpha_quenching' configuration property :
    datatools::configuration_property_description & cpd
      = ocd_.add_property_info();
    cpd.set_name_pattern("alpha_quenching")
      .set_terse_description("Flag to activate the energy quenching effect for alpha particles")
      .set_traits(datatools::TYPE_BOOLEAN)
      .set_mandatory(false)
      .set_long_description("When activated, this flag computes the quenching factor for alpha \n"
                            "particles that deposit some energy in the calorimeter volumes,    \n"
                            "taking into account delta-rays.                                   \n"
                            )
      .set_default_value_boolean(true)
      .add_example("Use the default value::          \n"
                   "                                 \n"
                   "  alpha_quenching : boolean = 1  \n"
                   "                                 \n"
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
      // .set_long_description("Default value: ``0`` ")
      .set_default_value_boolean(false)
      .add_example("Use the default value::          \n"
                   "                                 \n"
                   "  store_mc_hit_id : boolean = 0  \n"
                   "                                 \n"
                   )
      ;
  }

  {
    // Description of the 'hit_categories' configuration property :
    datatools::configuration_property_description & cpd
      = ocd_.add_property_info();
    cpd.set_name_pattern("hit_categories")
      .set_terse_description("The list of hit categories to be processed")
      .set_traits(datatools::TYPE_STRING,
                  configuration_property_description::ARRAY)
      .set_mandatory(true)
      .set_long_description("For each hit category, we must declare some            \n"
                            "configuration parameters for the associated            \n"
                            "calorimeter regime. These properties must be prefixed  \n"
                            "by the name of the hit category (see OCD support for   \n"
                            "the ``snemo::processing::calorimeter_regime`` class).  \n"
                            )
      .add_example("Use 2 hit categories to be processed by this module:: \n"
                   "                                                      \n"
                   "                                                      \n"
                   "  hit_categories : string[2] = \"calo\" \"xcalo\"               \n"
                   "                                                                \n"
                   "  # Parameters for the calorimeter regime for 'calo' hits:      \n"
                   "  calo.energy.resolution     : real as fraction = 8 %           \n"
                   "  calo.energy.low_threshold  : real as energy = 50 keV          \n"
                   "  calo.energy.high_threshold : real as energy = 150 keV         \n"
                   "  calo.scintillator_relaxation_time : real as time = 6.0 ns     \n"
                   "  calo.alpha_quenching_parameters   : real[3] = 77.4 0.639 2.34 \n"
                   "  ...                                                           \n"
                   "  # Parameters for the calorimeter regime for 'xcalo' hits:     \n"
                   "  xcalo.energy.resolution     : real as fraction = 12 %         \n"
                   "  xcalo.energy.low_threshold  : real as energy = 50 keV         \n"
                   "  xcalo.energy.high_threshold : real as energy = 150 keV        \n"
                   "  xcalo.scintillator_relaxation_time : real as time = 6.0 ns    \n"
                   "  xcalo.alpha_quenching_parameters   : real[3] = 77.4 0.639 2.34\n"
                   "  ...                                                           \n"
                   "                                                                \n"
                   )
      ;
  }

  // Additionnal configuration hints :
  ocd_.set_configuration_hints("Here is a full configuration example in the                    \n"
                               "``datatools::properties`` ASCII format::                       \n"
                               "                                                               \n"
                               "  SD_label    : string = \"SD\"                                \n"
                               "  CD_label    : string = \"CD\"                                \n"
                               "  Geo_label   : string = \"geometry\"                          \n"
                               "  random.seed : integer = 314159                               \n"
                               "  random.id   : string = \"taus2\"                             \n"
                               "  cluster_time_width : real as time = 100 ns                   \n"
                               "  alpha_quenching    : boolean = 1                             \n"
                               "  store_mc_hit_id    : boolean = 0                             \n"
                               "                                                               \n"
                               "  hit_categories     : string[2] = \"calo\" \"xcalo\"          \n"
                               "                                                               \n"
                               "  calo.energy.resolution     : real as fraction = 8 %          \n"
                               "  calo.energy.low_threshold  : real as energy = 50 keV         \n"
                               "  calo.energy.high_threshold : real as energy = 150 keV        \n"
                               "  calo.scintillator_relaxation_time : real as time = 6.0 ns    \n"
                               "  calo.alpha_quenching_parameters : real[3] = 77.4 0.639 2.34  \n"
                               "                                                               \n"
                               "  xcalo.energy.resolution     : real as fraction = 12 %        \n"
                               "  xcalo.energy.low_threshold  : real as energy = 50 keV        \n"
                               "  xcalo.energy.high_threshold : real as energy = 150 keV       \n"
                               "  xcalo.scintillator_relaxation_time : real as time = 6.0 ns   \n"
                               "  xcalo.alpha_quenching_parameters : real[3] = 77.4 0.639 2.34 \n"
                                "                                                              \n"
                               );

  ocd_.set_validation_support(true);
  ocd_.lock();
  return;
}
DOCD_CLASS_IMPLEMENT_LOAD_END() // Closing macro for implementation

// Registration macro for class 'snemo::processing::mock_calorimeter_s2c_module' :
DOCD_CLASS_SYSTEM_REGISTRATION(snemo::processing::mock_calorimeter_s2c_module,
                               "snemo::processing::mock_calorimeter_s2c_module")

// end of falaise/snemo/processing/mock_calorimeter_s2c_module.cc
