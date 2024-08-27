/// \file falaise/snemo/simulation/tracker_drift_time_generator.cpp
///
/// Author(s) :    François Mauger <mauger@lpccaen.in2p3.fr>
/// Creation date: 2024-07-02
/// Last modified: 2024-07-02
///

// Ourselves:
#include <falaise/snemo/simulation/tracker_drift_time_generator.hpp>

// Bayeux:
#include <bayeux/datatools/exception.h>
#include <bayeux/datatools/properties.h>

// This project:
#include <falaise/math/double_gauss_pdf.hpp>

namespace snemo {

  namespace simulation {
    
    tracker_drift_time_generator::tracker_drift_time_generator()
    {
      return;
    }
     
    tracker_drift_time_generator::tracker_drift_time_generator(const datatools::properties & config_,
							       const datatools::service_dict_type & services_)
    {
      initialize(config_, services_);
      return;
    }
    
    void tracker_drift_time_generator::initialize(const datatools::properties & config_,
						  const datatools::service_dict_type & services_)
    {
      // Geometry services:
      std::string geometryLabel = service_info::geometryServiceName();
      DT_THROW_IF(not datatools::has<geomtools::geometry_service>(services_, geometryLabel),
                  std::logic_error, "No geometry service");
      _geomgr_ = &datatools::get<geomtools::geometry_service>(services_, geometryLabel).get_geom_manager();
      const snemo::geometry::locator_plugin & locators
        = _geomgr_->get_plugin<snemo::geometry::locator_plugin>("locators_driver");
      _gg_locator_ = &locators.geigerLocator();

      std::string drift_model_name;
      if (config_.has_key("drift_model_name")) {
	drift_model_name = config_.fetch_string("drift_model_name");
      }
      if (drift_model_name == "betsy") {
	// betsy's drift model:
	datatools::properties tdmConfig;
	config_.export_and_rename_starting_with(tdmConfig, "drift_model.", "");
	if (tdmConfig.size() == 0) {
	  // default configuration:
	  tdmConfig.store("model", "betsy");
	  tdmConfig.store("version", 0);
	  tdmConfig.store_with_explicit_unit("default_gas_pressure", 880.e-3 * CLHEP::bar);
	  tdmConfig.set_unit_symbol("default_gas_pressure", "millibar");
	}
	auto verbosity = datatools::logger::PRIO_FATAL;
	_drift_model_ptr_ = std::make_unique<physics_model::tracker_drift_model>(verbosity);
	_drift_model_ptr_->initialize(tdmConfig, services_);
      } else {
	DT_THROW(std::logic_error, "Unsupported drift model " << std::quoted(drift_model_name)); 	
      }

      if (config_.has_key("mean_ionization_energy")) {
	double mie = config_.fetch_real_with_explicit_dimension("mean_ionization_energy", "energy");
	_mean_ionization_energy_ = mie;
      }
      
      return;
    }
    
    void tracker_drift_time_generator::reset()
    {
      _drift_model_ptr_->reset();
      return;
    }
     
    double tracker_drift_time_generator::generate_drift_time(const geomtools::geom_id & cell_gid_,
							     const time::time_point & event_reference_timepoint_,
							     const geomtools::vector_3d & position_start_,
							     const geomtools::vector_3d & position_stop_,
							     const double time_start_,
							     const double time_stop_,
							     const double energy_deposit_,
							     mygsl::rng & random_) const
    {
      auto verbosity = datatools::logger::PRIO_FATAL;
      // verbosity = datatools::logger::PRIO_DEBUG;
      const double mean_number_of_ionisations = energy_deposit_ / _mean_ionization_energy_;
      const size_t number_of_ionizations = random_.poisson(mean_number_of_ionisations);
      geomtools::vector_3d step_dir = (position_stop_ - position_start_);
      double driftTime = std::numeric_limits<double>::max();
      // int bestIon = -1;
      geomtools::vector_3d bestIonizationPos;
      auto cellPos = _drift_model_ptr_->gg_locator().getCellPosition(cell_gid_);
      for (size_t ion = 0; ion < number_of_ionizations; ++ion) {
	const double r = random_.uniform();
	const double ionization_time = time_start_ + r * (time_stop_ - time_start_);
	const geomtools::vector_3d ionization_pos = position_start_ + r * step_dir;
	const double ionization_x = ionization_pos.x();
	const double ionization_y = ionization_pos.y();
	// const double ionization_z = ionization_pos.z();
	int cellQuarterIndex = _drift_model_ptr_->locate_cell_quarter(ionization_pos);
	const double radius = std::hypot(ionization_x, ionization_y);
	// const double angle = std::atan2(ionization_y, ionization_x);
	if (_drift_model_ptr_) {
	  auto cellInfo = _drift_model_ptr_->fetch_cell_info(cell_gid_, event_reference_timepoint_);
	  auto gasInfo = _drift_model_ptr_->fetch_gas_info(event_reference_timepoint_);
	  double gasPressure = gasInfo.pressure;
	  const auto /* cell_quarter_info */ & cqi = cellInfo.efield_info->qinfos[cellQuarterIndex];
	  auto result = _drift_model_ptr_->compute_drift_time(cqi.category, gasPressure, radius);
	  double tdown, tmed, tup;
	  std::tie(tdown, tmed, tup) = result;
	  DT_LOG_DEBUG(verbosity, "tmed=" << tmed / CLHEP::microsecond);
	  DT_LOG_DEBUG(verbosity, "tdown=" << tdown / CLHEP::microsecond);
	  DT_LOG_DEBUG(verbosity, "tup=" << tup / CLHEP::microsecond);
	  double sigmaLeft = tmed - tdown;
	  double sigmaRight = tup - tmed;
	  DT_LOG_DEBUG(verbosity, "sigmaLeft=" << sigmaLeft);
	  DT_LOG_DEBUG(verbosity, "sigmaRight=" << sigmaRight);
	  falaise::math::double_gauss_pdf dgPdf(tmed, sigmaLeft, sigmaRight);
	  falaise::math::double_gauss_generator dgGen(dgPdf);
	  double randomizedDriftTime = dgGen.shoot(random_);
	  if (randomizedDriftTime < 5.0 * CLHEP::ns) randomizedDriftTime = 5.0 * CLHEP::ns;
	  if (randomizedDriftTime < driftTime) {
	    // bestIon = ion;
	    bestIonizationPos = ionization_pos;
	    driftTime = ionization_time + randomizedDriftTime;
	  }
	}
      }
      auto & last = const_cast<tracker_drift_time_generator*>(this)->_last_;
      last.position = bestIonizationPos;
      last.cell_center = cellPos;
      last.drift_time = driftTime;
      last.nb_ionizations = number_of_ionizations;
      return driftTime;
    }
				 
    const tracker_drift_time_generator::last_ionization_info & tracker_drift_time_generator::last() const
    {
      return _last_;
    }

  } // end of namespace simulation

} // end of namespace snemo
