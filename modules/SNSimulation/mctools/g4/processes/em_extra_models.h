/// \file mctools/g4/processes/em_extra_models.h
/* Author(s) :    Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2015-09-03
 * Last modified: 2015-09-04
 *
 * License:
 *
 * Description:
 *
 *   G4 electromagnetic extra models
 *
 * History:
 *
 */

#ifndef SNSIM_PROCESSES_EM_EXTRA_MODELS_H
#define SNSIM_PROCESSES_EM_EXTRA_MODELS_H

// Third party:
// Bayeux/datatools:
#include <datatools/enriched_base.h>
#include <datatools/properties.h>

class G4EmConfigurator;

namespace snsim {

namespace processes {

class em_model_factory;

/// \brief Parameters for an electromagnetic extra model
class em_extra_model : public datatools::enriched_base {
 public:
  /// Default constructor
  em_extra_model();

  /// Destructor
  virtual ~em_extra_model();

  /// Check initialization
  bool is_initialized() const;

  /// Initialization
  void initialize();

  /// Initialization
  ///
  /// Example of configuration that associates a given model to a particle
  /// through a given process:
  /// @code
  /// particle_name : string = "GenericIon"
  /// process_name  : string = "ionIoni"
  /// model_name    : string = "MyModel"
  /// region_name   : string = "TPC"
  /// energy_min    : real as energy = 0.0 MeV
  /// energy_max    : real as energy = 1.0 MeV
  /// @endcode
  void initialize(const datatools::properties &);

  /// Reset
  void reset();

  /// Set the particle name
  ///
  /// Supported particle names are:
  ///  * the empty string, "*", "all", "ALL" : represent all particles supported by Geant4
  ///  * "charged" : represents all charged particles supported by Geant4
  ///  * the name of a specific particle supported by Geant4 (example: "e-")
  void set_particle_name(const std::string &, bool check_ = false);

  /// Return the particle name
  const std::string &get_particle_name() const;

  /// Set the process name
  void set_process_name(const std::string &);

  /// Return the process name
  const std::string &get_process_name() const;

  /// Set the model type identifier
  void set_model_type_id(const std::string &);

  /// Return the model type identifier
  const std::string &get_model_type_id() const;

  /// Set the model name
  void set_model_name(const std::string &);

  /// Return the model name
  const std::string &get_model_name() const;

  /// Set the region name
  ///
  /// Supported region names are:
  /// * the empty string, "world", "World", "DefaultRegionForTheWorld": represent the default region
  /// associated
  ///   to the top-level "world" volume.
  /// * the name of a specific region defined in the geometry setup (example: "Calorimeter")
  void set_region_name(const std::string &, bool check_ = false);

  /// Return the region name
  const std::string &get_region_name() const;

  /// Set the minimum energy for model activation
  ///
  /// Default value is 0.
  void set_energy_min(double);

  /// Return the minimum energy for model activation
  double get_energy_min() const;

  /// Set the maximum energy for model activation
  ///
  /// Default value is DBL_MAX.
  void set_energy_max(double);

  /// Return the maximum energy for model activation
  double get_energy_max() const;

  /// Configure the extra model from a EM configurator with help from a EM model factory
  void apply_to_g4(G4EmConfigurator &, em_model_factory &) const;

  /// Validate a particle name with respect to the Geant4 particle table
  static bool validate_g4_particle_name(std::string &particle_name_);

  /// Validate a region name with respect to the Geant4 region store
  static bool validate_g4_region_name(std::string &region_name_);

  /// Validate a model type identifier with respect to the Geant4 library
  static bool validate_g4_model_type_id(std::string &model_type_id_);

  /// Smart print
  virtual void tree_dump(std::ostream &out_ = std::clog, const std::string &title_ = "",
                         const std::string &indent_ = "", bool inherit_ = false) const;

 public:
  /// Set attributes values to default
  void _set_defaults();

 private:
  bool _initialized_;           ///< Initialization flag
  std::string _particle_name_;  ///< The particle name
  std::string _process_name_;   ///< The process name
  std::string
      _model_type_id_;  ///< The model type/class identifier as registered in some EM model factory
  std::string _model_name_;   ///< The model name
  std::string _region_name_;  ///< The region name
  double _energy_min_;        ///< The minimum energy for model activation
  double _energy_max_;        ///< The maximum energy for model activation
};

}  // end of namespace processes

}  // namespace snsim

// /// OCD support : interface
// #include <datatools/ocd_macros.h>
// DOCD_CLASS_DECLARATION(mctools::g4::processes::em_extra_models)

#endif  // SNSIM_PROCESSES_EM_EXTRA_MODELS_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
