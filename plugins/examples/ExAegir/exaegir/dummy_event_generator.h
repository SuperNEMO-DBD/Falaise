// -*- mode: c++ ; -*-
/** \file exaegir/dummy_event_generator.h
 * Author(s) : Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2023-09-21
 * Last modified: 2023-09-21
 */

#ifndef EXAEGIR_DUMMY_EVENT_GENERATOR_H
#define EXAEGIR_DUMMY_EVENT_GENERATOR_H 1

// - Bayeux:
#include <genbb_help/i_genbb.h>
#include <datatools/clhep_units.h>

namespace exaegir {

  class dummy_event_generator
    : public ::genbb::i_genbb
  {

  public:

    /// Constructor
    dummy_event_generator();

    /// Destructor
    virtual ~dummy_event_generator();

    /// Check if an external random engine can be plugged in
    bool can_external_random() const override;

    /// Check initialization status
    bool is_initialized() const override;

    /// Initialize the generator from configuration properties
    void initialize(const datatools::properties & config,
		    datatools::service_manager & srv_mgr_,
		    genbb::detail::pg_dict_type & pg_dict_) override;

    /// Reset the object
    void reset() override;

    /// Check is a next event is available
    bool has_next() override;
 
  protected:

    /// Load/generate a new event
    void _load_next(::genbb::primary_event & event_,
		    bool compute_classification_ = true) override;

  private:
  
    void _at_init_();

    void _at_reset_();

  private:
      
    bool _initialized_ = false; //!< Initialization flag

    double _kinetic_energy_ = 1.0 * CLHEP::MeV;
    bool _isotropic_ = false;
    
    GENBB_PG_REGISTRATION_INTERFACE(dummy_event_generator)
      
  };
      
} // end of namespace exaegir

#endif // EXAEGIR_DUMMY_EVENT_GENERATOR_H
