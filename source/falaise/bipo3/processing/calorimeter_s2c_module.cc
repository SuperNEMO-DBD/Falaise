// falaise/bipo3/processing/calorimeter_s2c_module.cc

// Ourselves:
#include <falaise/bipo3/processing/calorimeter_s2c_module.h>

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

// This project:
#include <falaise/snemo/datamodels/data_model.h>

namespace bipo3 {

  namespace processing {

    // Registration instantiation macro :
    DPP_MODULE_REGISTRATION_IMPLEMENT(calorimeter_s2c_module,
                                      "bipo3::processing::calorimeter_s2c_module");

    void calorimeter_s2c_module::compute_classification(const std::string & particle_name_,
                                                        std::string       & classification_)
    {
      size_t n_eminus = 0;
      size_t n_eplus  = 0;
      size_t n_gamma  = 0;
      size_t n_alpha  = 0;
      size_t n_others = 0;

      if (!classification_.empty()) {
        std::string dummy(classification_);
        replace(dummy.begin(), dummy.end(), 'e', ' ');
        replace(dummy.begin(), dummy.end(), 'p', ' ');
        replace(dummy.begin(), dummy.end(), 'g', ' ');
        replace(dummy.begin(), dummy.end(), 'a', ' ');
        replace(dummy.begin(), dummy.end(), 'X', ' ');

        std::istringstream iss(dummy);
        iss >> n_eminus >> n_eplus >> n_gamma >> n_alpha >> n_others;
      }

      if (particle_name_ == "e-")         n_eminus++;
      else if (particle_name_ == "p")     n_eplus++;
      else if (particle_name_ == "gamma") n_gamma++;
      else if (particle_name_ == "alpha") n_alpha++;
      else n_others++;

      std::ostringstream oss;
      oss << n_eminus << 'e'
          << n_eplus << 'p'
          << n_gamma << 'g'
          << n_alpha << 'a'
          << n_others << 'X';

      classification_ = oss.str();
      return;
    }

    void calorimeter_s2c_module::set_geom_manager(const geomtools::manager & gmgr_)
    {
      DT_THROW_IF(is_initialized(),
                  std::logic_error,
                  "Module '" << get_name() << "' is already initialized ! ");
      _geom_manager_ = &gmgr_;

      // Check setup label:
      const std::string & setup_label = _geom_manager_->get_setup_label();
      DT_THROW_IF(!(setup_label == "bipo3" || setup_label == "bipo1" ||
                    setup_label == "test_bench"),
                  std::logic_error,
                  "Setup label '" << setup_label << "' is not supported !");
      return;
    }

    const geomtools::manager & calorimeter_s2c_module::get_geom_manager() const
    {
      DT_THROW_IF(! is_initialized(),
                  std::logic_error,
                  "Module '" << get_name() << "' is not initialized ! ");
      return *_geom_manager_;
    }

    void calorimeter_s2c_module::initialize(const datatools::properties  & setup_,
                                            datatools::service_manager   & service_manager_,
                                            dpp::module_handle_dict_type & /* module_dict_ */)
    {
      DT_THROW_IF(is_initialized(),
                  std::logic_error,
                  "Module '" << get_name() << "' is already initialized ! ");

      this->base_module::_common_initialize(setup_);

      if (setup_.has_key("SD_label")) {
        _SD_label_ = setup_.fetch_string("SD_label");
      }

      if (setup_.has_key("CD_label")) {
        _CD_label_ = setup_.fetch_string("CD_label");
      }

      std::string geo_label = "Geo";
      if (setup_.has_key("Geo_label")) {
        geo_label = setup_.fetch_string("Geo_label");
      }

      if (_geom_manager_ == 0) {
        DT_THROW_IF(geo_label.empty(), std::logic_error,
                    "Module '" << get_name() << "' has no valid '" << "Geo_label" << "' property !");
        DT_THROW_IF(! service_manager_.has(geo_label) ||
                    ! service_manager_.is_a<geomtools::geometry_service>(geo_label),
                    std::logic_error,
                    "Module '" << get_name() << "' has no '" << geo_label << "' service !");
        const geomtools::geometry_service & Geo = service_manager_.get<geomtools::geometry_service>(geo_label);
        set_geom_manager(Geo.get_geom_manager());
      }
      DT_THROW_IF(_geom_manager_ == 0, std::logic_error, "Missing geometry manager !");

      std::string random_id = "mt19937";
      int random_seed  = 12345;
      if (setup_.has_key("random.seed")) {
        random_seed = setup_.fetch_integer("random.seed");
      }
      if (setup_.has_key("random.id")) {
        random_id = setup_.fetch_string("random.id");
      }

      // Initialize the embedded random number generator:
      _random_.init(random_id, random_seed);

      // Get the calorimeter categories:
      if (setup_.has_key("hit_categories")) {
        setup_.fetch("hit_categories", _hit_categories_);
      }

      // Initialize the calorimeter regime utility:
      for (category_col_type::const_iterator icategory = _hit_categories_.begin();
           icategory != _hit_categories_.end(); ++icategory) {
        const std::string & the_category = *icategory;

        snemo::processing::calorimeter_regime new_regime;
        new_regime.set_category(the_category);
        new_regime.initialize(setup_);

        _calorimeter_regimes_[the_category] = new_regime;
      }

      // Setup trigger time
      if (setup_.has_key("cluster_time_width")) {
        _cluster_time_width_ = setup_.fetch_real("cluster_time_width") * CLHEP::ns;
      }

      // Get the alpha quenching:
      if (setup_.has_key("alpha_quenching")) {
        setup_.fetch("alpha_quenching", _alpha_quenching_);
      }

      this->base_module::_set_initialized(true);
      return;
    }

    void calorimeter_s2c_module::reset()
    {
      DT_THROW_IF(! is_initialized(),
                  std::logic_error,
                  "Module '" << get_name() << "' is not initialized !");

      // Reset the random number generator:
      _random_.reset();

      // Reset the calorimeter regime utility:
      for (calorimeter_regime_col_type::iterator icalo = _calorimeter_regimes_.begin();
           icalo != _calorimeter_regimes_.end(); ++icalo) {
        icalo->second.reset();
      }

      _hit_categories_.clear();

      this->base_module::_set_initialized(false);
      return;
    }

    // Constructor :
    calorimeter_s2c_module::calorimeter_s2c_module(datatools::logger::priority logging_priority_)
      : dpp::base_module(logging_priority_)
    {
      _geom_manager_ = 0;

      _SD_label_ = snemo::datamodel::data_info::SIMULATED_DATA_LABEL;
      _CD_label_ = snemo::datamodel::data_info::CALIBRATED_DATA_LABEL;

      _cluster_time_width_ = 100 * CLHEP::ns;
      _alpha_quenching_    = true;
      return;
    }

    // Destructor :
    calorimeter_s2c_module::~calorimeter_s2c_module()
    {
      if (is_initialized()) calorimeter_s2c_module::reset();
      return;
    }

    // Processing :
    dpp::base_module::process_status calorimeter_s2c_module::process(datatools::things & event_record_)
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

      // Check calibrated data
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
      _process(the_simulated_data,
               the_calibrated_data.calibrated_calorimeter_hits());

      return dpp::base_module::PROCESS_SUCCESS;
    }

    mygsl::rng & calorimeter_s2c_module::_get_random()
    {
      return _random_;
    }

    // Here collect the 'calos' raw hits from the simulation data source
    // and build the final list of 'calorimeter' hits.
    void calorimeter_s2c_module::_process_calorimeter_digitization
    (const mctools::simulated_data & simulated_data_,
     snemo::datamodel::calibrated_data::calorimeter_hit_collection_type & calibrated_calorimeter_hits_)
    {
      DT_LOG_DEBUG(get_logging_priority(), "Entering...");

      size_t calibrated_calorimeter_hit_id = 0;
      // Loop over all 'calos' category
      for (category_col_type::const_iterator icategory = _hit_categories_.begin();
           icategory != _hit_categories_.end(); ++icategory) {
        const std::string & category = *icategory;
        if (! simulated_data_.has_step_hits(category)) {
          // Nothing to do.
          continue;
        }

        // Loop on 'calos' step hits:
        const size_t nb_hits = simulated_data_.get_number_of_step_hits(category);
        for (size_t ihit = 0; ihit < nb_hits; ++ihit) {
          const mctools::base_step_hit & a_calo_hit = simulated_data_.get_step_hit(category, ihit);

          // extract the corresponding geom ID:
          const geomtools::geom_id & gid = a_calo_hit.get_geom_id();

          // get the step hit time start:
          const double step_hit_time_start = a_calo_hit.get_time_start();

          // get the step hit energy deposit:
          double step_hit_energy_deposit = a_calo_hit.get_energy_deposit();

          // quench the alpha particle energy if property is activated:
          if (a_calo_hit.get_particle_name() == "alpha" && _alpha_quenching_) {
            const snemo::processing::calorimeter_regime & the_calo_regime = _calorimeter_regimes_.at(category);
            const double quenched_energy = the_calo_regime.quench_alpha_energy(step_hit_energy_deposit);
            step_hit_energy_deposit = quenched_energy;
          }

          // find if some calorimeter hit already uses this geom ID :
          geomtools::base_hit::has_geom_id_predicate pred_has_gid(gid);
          // wrapper predicates :
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
            new_calibrated_calorimeter_hit.set_geom_id(a_calo_hit.get_geom_id());

            // sigma time and sigma energy are computed later
            new_calibrated_calorimeter_hit.set_time(step_hit_time_start);
            new_calibrated_calorimeter_hit.set_energy(step_hit_energy_deposit);

            // Add a properties to ease the final calibration
            new_calibrated_calorimeter_hit.grab_auxiliaries().store("category", category);

            // Add a property to store only primary particle id
            if (a_calo_hit.is_primary_particle()) {
              std::string classification;
              compute_classification(a_calo_hit.get_particle_name(), classification);
              new_calibrated_calorimeter_hit.grab_auxiliaries().store("classification", classification);
            }

            // Add the(x,y,z) start and stop position of the hit in properties:
            const geomtools::vector_3d & start_position = a_calo_hit.get_position_start();
            const geomtools::vector_3d & stop_position  = a_calo_hit.get_position_stop();

            new_calibrated_calorimeter_hit.grab_auxiliaries().store("x_start", start_position.x());
            new_calibrated_calorimeter_hit.grab_auxiliaries().store("y_start", start_position.y());
            new_calibrated_calorimeter_hit.grab_auxiliaries().store("z_start", start_position.z());

            new_calibrated_calorimeter_hit.grab_auxiliaries().store("x_stop", stop_position.x());
            new_calibrated_calorimeter_hit.grab_auxiliaries().store("y_stop", stop_position.y());
            new_calibrated_calorimeter_hit.grab_auxiliaries().store("z_stop", stop_position.z());

            // Append it to the collection :
            calibrated_calorimeter_hits_.push_back(new_handle);
          } else {
            // This geom_id is already used by some previous calorimeter hit:
            // we update this hit !
            snemo::datamodel::calibrated_calorimeter_hit & some_calibrated_calorimeter_hit = found->grab();

            // Check time between clusters
            const double delta_time = step_hit_time_start
              - some_calibrated_calorimeter_hit.get_time();

            // Calo hit arriving some time after a first calo hit in the same block:
            // we create a new calorimeter hit !
            if (std::fabs(delta_time) > _cluster_time_width_) {
              snemo::datamodel::calibrated_data::calorimeter_hit_handle_type new_handle(new snemo::datamodel::calibrated_calorimeter_hit);
              snemo::datamodel::calibrated_calorimeter_hit & new_calibrated_calorimeter_hit = new_handle.grab();

              new_calibrated_calorimeter_hit.set_hit_id(calibrated_calorimeter_hit_id++);
              new_calibrated_calorimeter_hit.set_geom_id(a_calo_hit.get_geom_id());

              // Sigma time and sigma energy are computed later
              new_calibrated_calorimeter_hit.set_time(step_hit_time_start);
              new_calibrated_calorimeter_hit.set_energy(step_hit_energy_deposit);

              // Add a properties to ease the final calibration
              new_calibrated_calorimeter_hit.grab_auxiliaries().store("category", category);

              // Add a property to store only primary particle id
              if (a_calo_hit.is_primary_particle()) {
                std::string classification;
                compute_classification(a_calo_hit.get_particle_name(), classification);
                new_calibrated_calorimeter_hit.grab_auxiliaries().store("classification", classification);
              }

              // Add the(x,y,z) start and stop position of the hit in properties:
              const geomtools::vector_3d & start_position = a_calo_hit.get_position_start();
              const geomtools::vector_3d & stop_position  = a_calo_hit.get_position_stop();

              new_calibrated_calorimeter_hit.grab_auxiliaries().store("x_start", start_position.x());
              new_calibrated_calorimeter_hit.grab_auxiliaries().store("y_start", start_position.y());
              new_calibrated_calorimeter_hit.grab_auxiliaries().store("z_start", start_position.z());

              new_calibrated_calorimeter_hit.grab_auxiliaries().store("x_stop", stop_position.x());
              new_calibrated_calorimeter_hit.grab_auxiliaries().store("y_stop", stop_position.y());
              new_calibrated_calorimeter_hit.grab_auxiliaries().store("z_stop", stop_position.z());

              // Append it to the collection :
              calibrated_calorimeter_hits_.push_back(new_handle);
            } else {
              // Clusterize the short delay calo hit with the previous one:
              // Find the first hit in time
              const double calo_time = std::min(step_hit_time_start,
                                                some_calibrated_calorimeter_hit.get_time());
              some_calibrated_calorimeter_hit.set_time(calo_time);

              // Sum energies
              const double calo_energy = step_hit_energy_deposit
                + some_calibrated_calorimeter_hit.get_energy();
              some_calibrated_calorimeter_hit.set_energy(calo_energy);

              // Change properties storing particle id information
              if (a_calo_hit.is_primary_particle()) {
                std::string classification;
                if (some_calibrated_calorimeter_hit.get_auxiliaries().has_key("classification")) {
                  classification = some_calibrated_calorimeter_hit.get_auxiliaries().fetch_string("classification");
                }
                compute_classification(a_calo_hit.get_particle_name(), classification);
                some_calibrated_calorimeter_hit.grab_auxiliaries().update("classification", classification);
              }

              // Add the(x,y,z) start and stop position of the hit in properties:
              const geomtools::vector_3d & start_position = a_calo_hit.get_position_start();
              const geomtools::vector_3d & stop_position  = a_calo_hit.get_position_stop();
              double x_start = 0.0, y_start = 0.0, z_start = 0.0;
              double x_stop = 0.0, y_stop = 0.0, z_stop = 0.0;

              if (some_calibrated_calorimeter_hit.get_auxiliaries().has_key("x_start")){
                x_start = some_calibrated_calorimeter_hit.get_auxiliaries().fetch_real("x_start");
              }
              x_start = std::min(x_start, start_position.x());
              if (some_calibrated_calorimeter_hit.get_auxiliaries().has_key("y_start")) {
                y_start = some_calibrated_calorimeter_hit.get_auxiliaries().fetch_real("y_start");
              }
              y_start = std::min(y_start, start_position.y());
              if (some_calibrated_calorimeter_hit.get_auxiliaries().has_key("z_start")) {
                z_start = some_calibrated_calorimeter_hit.get_auxiliaries().fetch_real("z_start");
              }
              z_start = std::min(z_start, start_position.z());

              some_calibrated_calorimeter_hit.grab_auxiliaries().change("x_start", x_start);
              some_calibrated_calorimeter_hit.grab_auxiliaries().change("y_start", y_start);
              some_calibrated_calorimeter_hit.grab_auxiliaries().change("z_start", z_start);

              if (some_calibrated_calorimeter_hit.get_auxiliaries().has_key("x_stop")) {
                x_stop = some_calibrated_calorimeter_hit.get_auxiliaries().fetch_real("x_stop");
              }
              x_stop = std::max(x_stop, stop_position.x());
              if (some_calibrated_calorimeter_hit.get_auxiliaries().has_key("y_stop")) {
                y_stop = some_calibrated_calorimeter_hit.get_auxiliaries().fetch_real("y_stop");
              }
              y_stop = std::max(y_stop, stop_position.y());
              if (some_calibrated_calorimeter_hit.get_auxiliaries().has_key("z_stop")) {
                z_stop = some_calibrated_calorimeter_hit.get_auxiliaries().fetch_real("z_stop");
              }
              z_stop = std::max(z_stop, stop_position.z());

              some_calibrated_calorimeter_hit.grab_auxiliaries().change("x_stop", x_stop);
              some_calibrated_calorimeter_hit.grab_auxiliaries().change("y_stop", y_stop);
              some_calibrated_calorimeter_hit.grab_auxiliaries().change("z_stop", z_stop);
            }
          }
        } // loop over hits
      } // loop over hit category

      DT_LOG_DEBUG(get_logging_priority(), "Exiting.");
      return;
    }

    // Calibrate calorimeter hits from digitization informations:
    void calorimeter_s2c_module::_process_calorimeter_calibration
    (snemo::datamodel::calibrated_data::calorimeter_hit_collection_type & calibrated_calorimeter_hits_)
    {
      DT_LOG_DEBUG(get_logging_priority(), "Entering...");

      for (snemo::datamodel::calibrated_data::calorimeter_hit_collection_type::iterator
             icalo = calibrated_calorimeter_hits_.begin();
           icalo != calibrated_calorimeter_hits_.end(); ++icalo) {
        snemo::datamodel::calibrated_calorimeter_hit & the_calo_cluster = icalo->grab();

        // Setting category in order to get the correct energy resolution:
        // first recover the calorimeter category
        const std::string & category_name = the_calo_cluster.get_auxiliaries().fetch_string("category");
        const snemo::processing::calorimeter_regime & the_calo_regime = _calorimeter_regimes_.at(category_name);

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
    void calorimeter_s2c_module::_process_calorimeter_trigger
    (snemo::datamodel::calibrated_data::calorimeter_hit_collection_type & calibrated_calorimeter_hits_)
    {
      DT_LOG_DEBUG(get_logging_priority(), "Entering...");

      bool high_threshold = false;
      for (snemo::datamodel::calibrated_data::calorimeter_hit_collection_type::iterator
             icalo = calibrated_calorimeter_hits_.begin();
           icalo != calibrated_calorimeter_hits_.end(); ++icalo) {
        snemo::datamodel::calibrated_calorimeter_hit & the_calo_cluster = icalo->grab();

        // Setting category in order to get the correct trigger parameters:
        // first recover the calorimeter category
        const std::string & category_name = the_calo_cluster.get_auxiliaries().fetch_string("category");
        const snemo::processing::calorimeter_regime & the_calo_regime = _calorimeter_regimes_.at(category_name);

        const double energy = the_calo_cluster.get_energy();

        if (! the_calo_regime.is_high_threshold(energy)) continue;

        high_threshold = true;
      }

      if (high_threshold) {
        // search for low threshold hits:
        for (snemo::datamodel::calibrated_data::calorimeter_hit_collection_type::iterator
               icalo = calibrated_calorimeter_hits_.begin();
             icalo != calibrated_calorimeter_hits_.end();) {
          snemo::datamodel::calibrated_calorimeter_hit & the_calo_cluster = icalo->grab();

          const std::string & category_name = the_calo_cluster.get_auxiliaries().fetch_string("category");
          const snemo::processing::calorimeter_regime & the_calo_regime = _calorimeter_regimes_.at(category_name);

          const double energy = the_calo_cluster.get_energy();

          // if energy hit is too low then remove calorimeter hit
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

    void calorimeter_s2c_module::_process
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

} // end of namespace bipo3
