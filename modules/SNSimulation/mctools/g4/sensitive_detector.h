/// \file mctools/g4/sensitive_detector.h
/* Author(s) :     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-05-26
 * Last modified: 2013-03-09
 *
 * License:
 *
 * Description:
 *
 *   GEANT4 sensitive detector implementation
 *
 * History:
 *
 */

#ifndef MCTOOLS_G4_SENSITIVE_DETECTOR_H
#define MCTOOLS_G4_SENSITIVE_DETECTOR_H 1

// Standard library:
#include <list>
#include <map>
#include <string>
#include <vector>

// Third party:
// - Boost:
#include <boost/cstdint.hpp>
// - Bayeux/datatools :
#include <datatools/properties.h>

// For C++11 support, remove once Bayeux is c++11/Geant4 10.2 only
#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-register"
#endif
#include <G4VSensitiveDetector.hh>
#ifdef __clang__
#pragma clang diagnostic push
#endif

// This project:
#include <mctools/base_step_hit_processor.h>
#include <mctools/g4/loggable_support.h>
#include <mctools/g4/sensitive_hit.h>
#include <mctools/g4/sensitive_hit_collection.h>
#include <mctools/g4/track_history.h>

class G4Step;

namespace mctools {

namespace g4 {

// Forward declaration:
class manager;

/// \brief The generic sensitive detector (Geant4 interface)
class sensitive_detector : public G4VSensitiveDetector, public loggable_support {
 public:
  /// Dictionary of step hit processors' addresses
  typedef std::map<std::string, mctools::base_step_hit_processor *> hit_processor_dict_type;

  /// Default capacity for buffer of hits
  static const size_t DEFAULT_HIT_BUFFER_CAPACITY = 1000;

  /// Default minimum ebergy threshodl for "major" track classification
  static const double DEFAULT_MAJOR_TRACK_MINIMUM_ENERGY;

 public:
  /// Check if a Geant4 manager is defined
  bool has_manager() const;

  /// Set the Geant4 manage
  void set_manager(manager &manager_);

  /// Set the flag to track the gammas
  void set_track_gamma(bool);

  /// Set the flag to track the optical photons
  void set_track_optical_photon(bool);

  /// Set the flag to track the neutrons
  void set_track_neutron(bool);

  void set_drop_zero_energy_deposit_steps(bool);
  void set_record_delta_ray_from_alpha(bool);
  void set_record_primary_particle(bool);
  void set_record_track_id(bool);
  void set_record_creator_process(bool);
  /// \deprecated This method craps
  void set_record_creator_category(bool);
  void set_record_momentum(bool);
  void set_record_kinetic_energy(bool);
  void set_record_material(bool);
  void set_record_sensitive_category(bool);
  void set_record_major_track(bool);
  void set_major_track_minimum_energy(double);
  void set_record_step_length(bool);
  void set_record_boundaries(bool);
  void set_record_g4_volume_properties(bool);

  /// Return the capacity of the buffer of hits
  unsigned int get_hits_buffer_capacity() const;

  /// Set the capacity of the buffer of hits
  void set_hits_buffer_capacity(unsigned int);

  /// Return the collection of auxiliaries
  const datatools::properties &get_auxiliaries() const;

  /// Return the mutable collection of auxiliaries
  datatools::properties &grab_auxiliaries();

  /// Return the ID of the Geant4 collection of hits
  int get_HCID() const;

  /// Set the ID of the Geant4 collection of hits
  void set_HCID(int);

  /// Check if step with no energy deposit are taken into account
  bool is_drop_zero_energy_deposit_steps() const;

  /// Check if informations about the Geant4 volumes are recorded
  bool is_record_g4_volume_properties() const;

  /// Attache a logical volume to the sensitive detector
  void attach_logical_volume(const std::string &log_volume_name_);

  /// Return the list of names of the logical volumes attached to the sensitive detector
  const std::list<std::string> &get_attached_logical_volumes() const;

  /// Return the mutable list of names of the logical volumes attached to the sensitive detector
  std::list<std::string> &grab_attached_logical_volumes();

  /// Return the sensitive category
  const std::string &get_sensitive_category() const;

  /// Return the mutable dictionary of step hit processors attached to the sensitive detector
  hit_processor_dict_type &grab_hit_processors();

  /// Return the dictionary of step hit processors attached to the sensitive detector
  const hit_processor_dict_type &get_hit_processors() const;

  /// Check if some step hit processor is attached to the sensitive detector
  bool has_hit_processor(const std::string &name_) const;

  /// Attach a step hit processor is attached to the sensitive detector
  void add_hit_processor(const std::string &name_, mctools::base_step_hit_processor &shp_);

  /// Return a mutable step hit processor is attached to the sensitive detector
  mctools::base_step_hit_processor &grab_hit_processor(const std::string &name_);

  /// Build the name of a collection of hits from the name of the sensitive category
  static std::string make_hit_collection_name(const std::string &name_);

  /// Constructor
  sensitive_detector(const std::string &sensitive_category_);

  /// Desctructor
  virtual ~sensitive_detector();

  /// Configure the sensitive detector from a container of parameters
  void configure(const datatools::properties &config_);

  /// Terminate
  virtual void clear();

  /// Smart print
  virtual void tree_dump(std::ostream &out_ = std::clog, const std::string &title_ = "",
                         const std::string &indent_ = "", bool inherit_ = false) const;

  /// Initialize the Geant4 collection of hits (Geant4 interface)
  virtual void Initialize(G4HCofThisEvent *);

  /// Terminate the Geant4 collection of hits (Geant4 interface)
  virtual void EndOfEvent(G4HCofThisEvent *);

  /// Process the current Geant4 step (Geant4 interface)
  virtual G4bool ProcessHits(G4Step *, G4TouchableHistory *);

 protected:
  /// Set default values
  void _set_defaults();

 private:
  std::string _sensitive_category_;                   //!< The name of the sensitive hit catagory
  std::list<std::string> _attached_logical_volumes_;  //!< The list of geometry logical volumes
                                                      //!< attached to the sensitive category
  bool _drop_zero_energy_deposit_steps_;  //!< Do not record steps with no energy deposit
  bool _track_gamma_;                     //!< Take into account gammas
  bool _track_optical_photon_;            //!< Take into account optical photons
  bool _track_neutron_;                   //!< Take into account neutrons
  bool
      _record_g4_volume_properties_;  //!< Record Geant4 identifiers of the current volume traversed
                                      //!< by the track (string property "sensitive.g4_volume.name"
                                      //!< and integer property "sensitive.g4_volume.copy_number")
  bool _record_momentum_;             //!< record the start and stop momentum of the particle
  bool _record_kinetic_energy_;       //!< record the start and stop kinetic energy of the particle
  bool _record_primary_particle_;     //!< Record the boolean property "track.primary"
  bool _record_track_id_;     //!< Record the integer properties "track.id" and "track.parent_id"
  bool _record_major_track_;  //!< Record the boolean property "track.major" for tracks of some
                              //!< importance (primary and energy above an user threshold)
  double _major_track_minimum_energy_;  //!< Energy threshold to flag "major" tracks
  bool _record_creator_process_;   //!< Record the informations about the creator process (string
                                   //!< properties "track.creator_process" and
                                   //!< "track.creator_category")
  bool _record_creator_category_;  //!< \deprecated Record the category of the creator process
  bool _record_material_;  //!< Record the current material traversed by the track (string property
                           //!< "material.ref")
  bool _record_sensitive_category_;  //!< Record the sensitive category that generated the current
                                     //!< hit (string property "sensitive.category")
  bool _record_boundaries_;          //!< Record boolean properties "track.entering_volume" and
                             //!< "track.leaving_volume" respectively when particle enters and
                             //!< leaves the volume
  bool _record_delta_ray_from_alpha_;  //!< Record boolean property "track.delta_ray_from_alpha" for
                                       //!< secondary electrons generated as delta-rays along an
                                       //!< alpha particle track
  bool _record_step_length_;           //!< Record the real property "track.step_length"
  unsigned int _hits_buffer_capacity_;  //!< The capacity of the pre-allocated buffer of hits

  const track_history::track_info *_track_info_ptr_ =
      nullptr;  //!< Handle to the tracking information of the current track
  const track_history::track_info *_parent_track_info_ptr_ =
      nullptr;  //!< Handle to the tracking information of the parent track of the current track

  manager *_manager_ = nullptr;  //!< Handle to the Geant4 manager

  datatools::properties _aux_;  //!< Auxiliary properties

  int _HCID_;                                //!< Hit collection Id
  std::vector<sensitive_hit> _hits_buffer_;  //!< Buffer of hits
  sensitive_hit_collection *_hits_collection_ =
      nullptr;            //!< Handle to the collection of sensitive hits
  int _used_hits_count_;  //!< Counter for buffered hits that are already in use

  // Dictionary to attach some processors for step hits:
  hit_processor_dict_type _hit_processors_;

  // Tools :
  uint32_t _number_of_sensitive_steps_;  //!< Counter for processed hits
};

}  // end of namespace g4

}  // end of namespace mctools

/// OCD support : interface
#include <datatools/ocd_macros.h>
DOCD_CLASS_DECLARATION(mctools::g4::sensitive_detector)

#endif  // MCTOOLS_G4_SENSITIVE_DETECTOR_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
