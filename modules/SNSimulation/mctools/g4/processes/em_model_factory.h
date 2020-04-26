/// \file mctools/g4/processes/em_model_factory.h
/* Author(s) :    Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2015-09-03
 * Last modified: 2015-09-04
 *
 * License:
 *
 * Description:
 *
 *   A factory of G4 electromagnetic model objects
 *
 * History:
 *
 */

#ifndef SNSIM_PROCESSES_EM_MODEL_FACTORY_H
#define SNSIM_PROCESSES_EM_MODEL_FACTORY_H

// Standard Library:
#include <string>

// Third party:
// Geant4:
#include <G4VEmModel.hh>
// Bayeux/datatools:
#include <datatools/properties.h>

namespace snsim {

namespace processes {

struct em_model_registration {
  std::string name;
  std::string description;
  // boost::factory f;
};

/// brief Factory of electromagnetic models
class em_model_factory {
 public:
  typedef datatools::factory_register<G4VEmModel> fact_reg_type;

  /// Default constructor
  em_model_factory();

  /// Destructor
  virtual ~em_model_factory();

  /// Check initialization
  bool is_initialized() const;

  /// Initialize
  void initialize();

  /// Reset
  void reset();

  /// Check if a model type is registered
  bool has_model_type(const std::string& model_type_id_) const;

  /// Factory method
  G4VEmModel* create_model(const std::string& model_type_id_, const std::string& model_name_ = "");

  /// Factory method
  G4VEmModel* create_model(const std::string& model_type_id_, const std::string& model_name_,
                           const datatools::properties& model_config_);

  /// Return a mutable reference to the default global instance
  static em_model_factory& grab_instance();

  /// Return a const reference to the default global instance
  static const em_model_factory& get_instance();

 protected:
  /// Hidden factory method
  virtual G4VEmModel* _create_model(const std::string& model_type_id_,
                                    const std::string& model_name_,
                                    const datatools::properties& model_config_);

  virtual void _registration();

  virtual void _registration_base();

  virtual void _registration_standard();

 private:
  static void _change_model_name_(G4VEmModel& model_, const std::string& name_);

  void _register_model(const std::string& name_, fact_reg_type::factory_type factory_, bool msc_);

  static em_model_factory& _instance_();

 private:
  bool _initialized_;   ///< Initialization flag
  fact_reg_type _reg_;  ///< Internal factory registration table
};

}  // end of namespace processes

}  // namespace snsim

#endif  // SNSIM_PROCESSES_EM_MODEL_FACTORY_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
