/// \file mctools/g4/em_physics_constructor.h
/* Author(s) :    Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2013-07-03
 * Last modified: 2015-09-02
 *
 * License:
 *
 * Description:
 *
 *   G4 electro-magnetic physics constructor
 *
 *   http://geant4.in2p3.fr/IMG/pdf_Lecture-LowEnergyEMPhysics.pdf
 *
 * History:
 *
 */

#ifndef SNSIM_EM_PHYSICS_CONSTRUCTOR_H
#define SNSIM_EM_PHYSICS_CONSTRUCTOR_H 1

// This project:
#include <mctools/g4/base_physics_constructor.h>

// Third party:
// - Boost:
#include <boost/scoped_ptr.hpp>

namespace snsim {

/// \brief Geant4 physics constructor for electromagnetic processes
class em_physics_constructor : public base_physics_constructor {
 public:
  // http://geant4.in2p3.fr/IMG/pdf_Lecture-LowEnergyEMPhysics.pdf
  static const std::string EM_MODEL_STANDARD;
  static const std::string EM_MODEL_LOW_ENERGY_LIVERMORE;
  static const std::string EM_MODEL_LOW_ENERGY_PENELOPE;
  static const std::string EM_PIXE_MODEL_EMPIRICAL;
  static const std::string EM_PIXE_MODEL_ECPSSR_FORMFACTOR;
  static const std::string EM_PIXE_MODEL_ECPSSR_ANALYTICAL;
  static const std::string EM_PIXE_ELECTRON_MODEL_LIVERMORE;
  static const std::string EM_PIXE_ELECTRON_MODEL_PROTON_ANALYTICAL;
  static const std::string EM_PIXE_ELECTRON_MODEL_PROTON_EMPIRICAL;
  static const std::string EM_PIXE_ELECTRON_MODEL_PENELOPE;

  /// \brief Atomic deexcitation configuration for a given region
  struct region_deexcitation_type {
    /// Default constructor
    region_deexcitation_type();
    /// Constructor
    region_deexcitation_type(bool, bool, bool);
    /// Check if deexcitation is activated
    bool is_activated() const;
    /// Check if fluorescence is activated
    bool is_fluorescence() const;
    /// Check if Auger effect is activated
    bool is_auger() const;
    /// Check if PIXE is activated
    bool is_pixe() const;

    bool fluorescence;  ///< Activation flag for fluorescence
    bool auger;         ///< Activation flag for Auger effect
    bool pixe;          ///< Activation flag for PIXE model
  };

  /// Check the standard EM processes
  bool is_em_standard() const;

  /// Check the low energy Livermore EM processes
  bool is_em_low_energy_livermore() const;

  /// Check the low energy Penelope EM processes
  bool is_em_low_energy_penelope() const;

  /// Return the name of the EM processes
  const std::string& get_em_model() const;

  /// Default constructor
  em_physics_constructor();

  /// Destructor
  virtual ~em_physics_constructor();

  /// Initialization
  virtual void initialize(const datatools::properties& config_,
                          physics_constructor_dict_type& dict_);

  /// Reset
  virtual void reset();

  /// Smart print
  virtual void tree_dump(std::ostream& out_ = std::clog, const std::string& title_ = "",
                         const std::string& indent_ = "", bool inherit_ = false) const;

  /// Particles construction Geant4 interface :
  virtual void ConstructParticle();

  /// Processes construction Geant4 interface :
  virtual void ConstructProcess();

 protected:
  // These methods construct processes:

  /// Main construction of EM processes
  void _ConstructEMProcess();

  /// Construction of EM deexcitation
  void _ConstructEMDeexcitation();

  /// Construction of extra models associated to particles/processes/regions
  void _ConstructExtraModels();

  /// Setup the electromagnetic extra models configuration
  void _setup_em_extra_models_configurator(const datatools::properties& config_);

  /// Set attributes default values
  void _set_defaults();

 private:
  // EM processes:
  std::string _em_model_;  ///< Electromagnetic model ("standard"/"livermore"/"penelope")

  // For gammas:
  bool _em_gamma_rayleigh_scattering_;  ///< Activation flag for gamma Rayleigh scattering
                                        ///< (default=false)
  bool _em_gamma_photo_electric_;       ///< Activation flag for gamma photo electric effect
                                        ///< (default=true)
  bool _em_gamma_compton_scattering_;   ///< Activation flag for gamma Compton scattering
                                        ///< (default=true)
  bool _em_gamma_conversion_;  ///< Activation flag for gamma e+/e- pair conversion (default=true)
  bool _em_gamma_conversion_to_muons_;  ///< Activation flag for gamma muon pair conversion
                                        ///< (default=false)
  bool _em_gamma_step_limiter_;         ///< Activation flag for gamma step limiter (default=false)
  bool _em_gamma_user_special_cuts_;    ///< Activation flag for gamma user special cuts
                                        ///< (default=false)

  // For electron/positron:
  bool _em_electron_ionization_;           ///< Activation flag for electron/positron ionization
                                           ///< (default=true)
  bool _em_electron_bremsstrahlung_;       ///< Activation flag for electron/positron bremsstrahlung
                                           ///< (default=true)
  bool _em_electron_multiple_scattering_;  ///< Activation flag for electron/positron multiple
                                           ///< scattering (default=true)
  bool _em_electron_ms_use_distance_to_boundary_;  ///< Activation flag for electron/positron
                                                   ///< multiple scattering special algorithm near
                                                   ///< volume boundary (default=true)
  double _em_electron_ms_range_factor_;  ///< Special factor for electron/positron multiple
                                         ///< scattering (default=0.005)
  bool _em_positron_annihilation_;  ///< Activation flag for positron annihilation (default=true)
  bool _em_electron_step_limiter_;  ///< Activation flag for electron/positron step limiter
                                    ///< (default=true)
  bool _em_electron_user_special_cuts_;  ///< Activation flag for electron/positron user special
                                         ///< cuts (default=false)

  // Atomic deexcitation:
  bool _em_fluorescence_;  ///< Activation flag for Fluorescence (default=false)
  bool _em_auger_;         ///< Activation flag for Auger process  (default=false)
  bool _em_pixe_;          ///< Activation flag for PIXE model (default=false)
  std::string _em_pixe_cross_section_model_;  ///< PIXE cross section mode (default="Empirical")
  std::string _em_pixe_electron_cross_section_model_;  ///< PIXE electron cross section model
                                                       ///< (default="Livermore")
  std::map<std::string, region_deexcitation_type>
      _em_regions_deexcitation_;  ///< Dictionary of atomic deexcitation configuration for regions
                                  ///< (default=empty)

  // For ions:
  bool
      _em_ion_multiple_scattering_;  ///< Activation flag for ion multiple scattering (default=true)
  bool _em_ion_ionization_;          ///< Activation flag for ion ionization (default=true)
  bool _em_ion_step_limiter_;        ///< Activation flag for ion step limiter (default=true)
  bool _em_ion_user_special_cuts_;   ///< Activation flag for ion user special cuts (default=false)

  // For muons:
  bool _em_muon_multiple_scattering_;  ///< Activation flag for muon multiple scattering
                                       ///< (default=true)
  bool _em_muon_ionization_;           ///< Activation flag for muon ionization (default=true)
  bool _em_muon_bremsstrahlung_;       ///< Activation flag for muon bremsstrahlung (default=true)
  bool _em_muon_pair_production_;      ///< Activation flag for muon pair production (default=true)
  bool _em_muon_step_limiter_;         ///< Activation flag for muon step limiter (default=false)
  bool _em_muon_user_special_cuts_;  ///< Activation flag for muon user special cuts (default=false)

  // Others:
  bool _em_other_multiple_scattering_;  ///< Activation flag for other particle multiple scattering
                                        ///< (default=true)
  bool _em_other_ionization_;  ///< Activation flag for other particle ionization (default=true)
  bool
      _em_other_step_limiter_;  ///< Activation flag for other particle step limiter (default=false)
  bool _em_other_user_special_cuts_;  ///< Activation flag for other particle user special cuts
                                      ///< (default=false)

  bool _em_using_extra_models_;  ///< Activation flag for EM extra models associated to specific
                                 ///< particles/processes/regions combination

  /// \brief PIMPL-ized private working data structure
  struct _work_type_;

  /// Return the mutable reference to the internal private data, create it if not existing
  _work_type_& _grab_work_();

  boost::scoped_ptr<_work_type_> _work_;  ///< Private working data

  DATATOOLS_FACTORY_SYSTEM_AUTO_REGISTRATION_INTERFACE(base_physics_constructor,
                                                       em_physics_constructor)
};

}  // namespace snsim

/// OCD support : interface
#include <datatools/ocd_macros.h>
DOCD_CLASS_DECLARATION(snsim::em_physics_constructor)

#endif  // SNSIM_EM_PHYSICS_CONSTRUCTOR_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
