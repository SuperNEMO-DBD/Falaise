/// \file snsim/primary_generator.h
/* Author(s) :    Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-04-10
 * Last modified: 2013-03-09
 *
 * License: GPL3
 *
 * Description:
 *
 *   G4 primary event generator action
 *
 * History:
 *
 */

#ifndef SNSIM_PRIMARY_GENERATOR_H
#define SNSIM_PRIMARY_GENERATOR_H

// Standard library:
#include <map>
#include <string>

// Third party:
// - Boost
#include <boost/scoped_ptr.hpp>
// - Geant4
#include <G4VUserPrimaryGeneratorAction.hh>
// - Bayeux/geomtools:
#include <bayeux/geomtools/utils.h>

// This project:
#include <snsim/loggable_support.h>

class G4ParticleGun;
class G4Event;

namespace genbb {
class primary_particle;
class i_genbb;
}  // namespace genbb

namespace datatools {
class properties;
}

namespace genvtx {
class i_vertex_generator;
}

namespace mctools {
namespace biasing {
class primary_event_bias;
}
}  // namespace mctools

namespace snsim {

// Forward declarations:
class run_action;
class event_action;

/// \brief Generator of primary particles
class primary_generator : public G4VUserPrimaryGeneratorAction, public loggable_support {
 public:
  /// Check initialization flag
  bool is_initialized() const;

  /// Set the run action object
  void set_run_action(snsim::run_action &run_action_);

  /// Set the event action object
  void set_event_action(snsim::event_action &event_action_);

  /// Check if a vertex generator is defined
  bool has_vertex_generator() const;

  /// Set the vertex generator
  void set_vertex_generator(genvtx::i_vertex_generator &vertex_generator_);

  /// Set the event generator
  void set_event_generator(genbb::i_genbb &event_generator_);

  /// Return the event counter
  size_t get_event_counter() const;

  /// Reset the event counter
  void reset_event_counter();

  /// Default constructor
  primary_generator();

  /// Destructor
  virtual ~primary_generator();

  /// Initialization
  void initialize(const datatools::properties &config_);

  /// Reset
  void reset();

  /// Return the Geant4 particle name corresponding to a Genbb particle
  std::string get_g4_particle_name_from_genbb_particle(const ::genbb::primary_particle &p_) const;

  ///  Geant4 interface for primary event generation
  void GeneratePrimaries(G4Event *);

 protected:
  /// Set default attributes' values
  void _set_defaults();

  /// Perform some internal tests
  void _check();

  /// Generate a vertex
  void _generate_vertex();

  /// Generate an event
  void _generate_event(G4Event *g4_event_);

 private:
  bool _initialized_;                                       //!< Initialization flag
  run_action *_run_action_;                                 //!< The Geant4 run action
  event_action *_event_action_;                             //!< The Geant4 event action
  ::genvtx::i_vertex_generator *_vertex_generator_;         //!< The external vertex generator
  ::genbb::i_genbb *_event_generator_;                      //!< The external event generator
  G4ParticleGun *_particle_gun_;                            //!< The Geant4 particle gun
  geomtools::vector_3d _current_vertex_;                    //!< The current generated vertex
  double _current_time_;                                    //!< The current generated time (if any)
  size_t _event_counter_;                                   //!< The event counter
  std::map<std::string, std::string> _particle_names_map_;  //!< A dictionary of particle names
  boost::scoped_ptr<mctools::biasing::primary_event_bias>
      _bias_;  //!< Handle to a primary event bias algorithm
};

}  // namespace snsim

/// OCD support : interface
#include <bayeux/datatools/ocd_macros.h>
DOCD_CLASS_DECLARATION(snsim::primary_generator)

#endif  // SNSIM_PRIMARY_GENERATOR_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
