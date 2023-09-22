// exaegir/dummy_event_generator2.cc

// Ourselves
#include <exaegir/dummy_event_generator2.h>

// Standard library:
#include <cmath>

// Third party:
// - Bayeux:
#include <datatools/exception.h>
#include <datatools/logger.h>
#include <datatools/properties.h>
#include <datatools/units.h>
#include <genbb_help/primary_event.h>
#include <genbb_help/pdg_particle_tools.h>
#include <genbb_help/kinematics.h>
#include <mygsl/rng.h>

namespace exaegir {

  GENBB_PG_REGISTRATION_IMPLEMENT(dummy_event_generator2,
				  "exaegir::dummy_event_generator2")
    
  dummy_event_generator2::dummy_event_generator2()
  {
    _initialized_ = false;
    _at_reset_();
    return;
  }

  dummy_event_generator2::~dummy_event_generator2()
  {
    if (_initialized_) {
      reset();
    }
    return;
  }

  bool dummy_event_generator2::can_external_random() const
  {
    return true;
  }

  bool dummy_event_generator2::is_initialized() const
  {
    return _initialized_;
  }

  void dummy_event_generator2::initialize(const datatools::properties & config_,
					  datatools::service_manager & srv_mgr_,
					  genbb::detail::pg_dict_type & pg_dict_)
  {
    DT_THROW_IF(_initialized_, std::logic_error,
		"Operation prohibited! Object is already initialized!");
    _initialize_base(config_);

    DT_THROW_IF(not config_.has_key("tabulated_kinetic_energy_spectrum"),
		std::logic_error,
		"Missing 'tabulated_kinetic_energy_spectrum' property!");
    _tabulated_kinetic_energy_spectrum_path_
      = config_.fetch_string("tabulated_kinetic_energy_spectrum");
    DT_LOG_DEBUG(get_logging_priority(),
		 "Tabulated kinetic energy spectrum file path: '" << _tabulated_kinetic_energy_spectrum_path_ << "'");

    if (config_.has_key("isotropic")) {
      _isotropic_ = config_.fetch_boolean("isotropic");
    }
    DT_LOG_DEBUG(get_logging_priority(),
		 "Isotropic emission : " << std::boolalpha << _isotropic_);
   
    _at_init_();
    _initialized_ = true;
    return;
  }

  void dummy_event_generator2::reset()
  {
    if (!_initialized_) {
      return;
    }
    _initialized_ = false;
    _at_reset_();
    return;
  }

  bool dummy_event_generator2::has_next()
  {
    return true;
  }

  void dummy_event_generator2::_load_next(::genbb::primary_event & event_,
					 bool compute_classification_)
  {
    DT_LOG_TRACE_ENTERING(get_logging_priority());

    event_.reset();
    event_.set_time(0.0 * CLHEP::second);
    event_.set_label("dummy");
    ::genbb::primary_particle & newPart = event_.add_particle();
    newPart.set_type(::genbb::primary_particle::particle_type::ELECTRON);
    newPart.set_pdg_code(::genbb::pdg::particle::ELECTRON);
    newPart.set_time(0.0 * CLHEP::second);
    // Convert kinetic energy to momentum:
    genbb::kinematics eKinematics;
    double kineticEnergy = _rejection_method_.shoot(grab_external_random());
    eKinematics.initialize_from_m_k(newPart.get_mass(), kineticEnergy);
    double px = 0.0 * CLHEP::MeV;
    double py = 0.0 * CLHEP::MeV;
    double pz = eKinematics.p();
    if (_isotropic_) {
      double cosTheta = -1.0 + 2.0 * grab_external_random().uniform();
      double sinTheta = std::sqrt(1.0 - cosTheta * cosTheta);
      double phi = 2 * M_PI * grab_external_random().uniform();
      px = pz * sinTheta * std::cos(phi);
      py = pz * sinTheta * std::sin(phi);
      pz = pz * cosTheta;
    }
    newPart.grab_momentum().setX(px);
    newPart.grab_momentum().setY(py);
    newPart.grab_momentum().setZ(pz);
    if (compute_classification_) {
      event_.compute_classification();
    }
    
    DT_LOG_TRACE_EXITING(get_logging_priority());
    return;
  }
 
  void dummy_event_generator2::_at_init_()
  {
    // Resolve the path:
    std::string kineticEnergySpectrumTabulatedFile = _tabulated_kinetic_energy_spectrum_path_;
    datatools::fetch_path_with_env(kineticEnergySpectrumTabulatedFile);

    // Load the tabulated spectrum:
    _tabulated_kinetic_energy_spectrum_.load_from_file(kineticEnergySpectrumTabulatedFile);

    // Kinetic energy range from the tabulated spectrum:
    double eMin = _tabulated_kinetic_energy_spectrum_.x_min();
    double eMax = _tabulated_kinetic_energy_spectrum_.x_max();
    DT_LOG_DEBUG(get_logging_priority(), "Min energy=" << eMin / CLHEP::MeV << " MeV");
    DT_LOG_DEBUG(get_logging_priority(), "Max energy=" << eMax / CLHEP::MeV << " MeV");

    // Maxi probability value from the  tabulated spectrum:
    double fMax = 0.0;
    for (const auto & point : _tabulated_kinetic_energy_spectrum_.points()) {
      double f = point.second;
      DT_THROW_IF(f < 0.0, std::domain_error, "Probability negative value is not allowed for the energy spectrum!");
      if (f > fMax) fMax = f;
    }
    fMax *= 1.1; // Arbitrary safe factor in case of non linear interpolation scheme
    DT_LOG_DEBUG(get_logging_priority(), "Max spectrum value=" << fMax);
    
    // Initialize the Von Neumann rejection method:
    _rejection_method_.initialize(eMin, eMax, _tabulated_kinetic_energy_spectrum_, fMax, 1000, 30);
    
    return;
  }
  
  void dummy_event_generator2::_at_reset_()
  {
    _rejection_method_.reset();
    _tabulated_kinetic_energy_spectrum_.reset();
    _tabulated_kinetic_energy_spectrum_path_.clear();
    _isotropic_ = false;
    return;
  }

} // end of namespace exaegir
