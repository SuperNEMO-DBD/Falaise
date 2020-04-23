/* sensitive_detector.cc
 *
 * How to access informations from a G4Step:
 * http://geant4.web.cern.ch/geant4/UserDocumentation/UsersGuides/FAQ/html/ch01s04.html
 *
 */

// Standard library:
#include <cstdlib>
#include <stdexcept>
#include <iostream>
#include <sstream>

// Third party:
// - Boost:
#include <boost/algorithm/string.hpp>
// - Bayeux/datatools:
#include <datatools/exception.h>
// - Bayeux/geomtools:
#include <geomtools/sensitive.h>
#include <geomtools/material.h>

// This project:
#include <mctools/g4/sensitive_detector.h>
#include <mctools/g4/manager.h>
#include <mctools/utils.h>

// G4 stuff:
#include <G4Step.hh>
#include <G4ThreeVector.hh>
#include <G4HCofThisEvent.hh>
#include <G4SDManager.hh>
#include <G4Gamma.hh>
#include <G4Neutron.hh>
#include <G4VProcess.hh>

namespace mctools {

  namespace g4 {

    // static
    const size_t sensitive_detector::DEFAULT_HIT_BUFFER_CAPACITY;
    const double sensitive_detector::DEFAULT_MAJOR_TRACK_MINIMUM_ENERGY = 10. * CLHEP::keV;

    bool sensitive_detector::is_drop_zero_energy_deposit_steps() const
    {
      return _drop_zero_energy_deposit_steps_;
    }

    void sensitive_detector::set_drop_zero_energy_deposit_steps(bool drop_)
    {
      _drop_zero_energy_deposit_steps_ = drop_;
      return;
    }

    void sensitive_detector::set_track_gamma(bool track_gamma_)
    {
      _track_gamma_ = track_gamma_;
      return;
    }

    void sensitive_detector::set_track_optical_photon(bool track_op_)
    {
      _track_optical_photon_ = track_op_;
      return;
    }

    void sensitive_detector::set_track_neutron(bool track_neutron_)
    {
      _track_neutron_ = track_neutron_;
      return;
    }

    bool sensitive_detector::has_manager() const
    {
      return _manager_ != nullptr;
    }

    void sensitive_detector::set_manager(manager & manager_)
    {
      _manager_ = &manager_;
      return;
    }

    const datatools::properties &
    sensitive_detector::get_auxiliaries() const
    {
      return _aux_;
    }

    datatools::properties &
    sensitive_detector::grab_auxiliaries()
    {
      return _aux_;
    }

    void sensitive_detector::set_record_delta_ray_from_alpha(bool flag_)
    {
      _record_delta_ray_from_alpha_ = flag_;
      return;
    }

    void sensitive_detector::set_record_track_id(bool record_)
    {
      _record_track_id_ = record_;
      return;
    }

    void sensitive_detector::set_record_step_length(bool record_)
    {
      _record_step_length_ = record_;
      return;
    }

    void sensitive_detector::set_record_primary_particle(bool record_)
    {
      _record_primary_particle_ = record_;
      return;
    }

    void sensitive_detector::set_record_major_track(bool record_)
    {
      _record_major_track_ = record_;
      return;
    }

    void sensitive_detector::set_major_track_minimum_energy(double major_)
    {
      _major_track_minimum_energy_ = major_;
      return;
    }

    void sensitive_detector::set_record_creator_process(bool record_)
    {
      _record_creator_process_ = record_;
      return;
    }

    void sensitive_detector::set_record_creator_category(bool record_)
    {
      _record_creator_category_ = record_;
      return;
    }

    void sensitive_detector::set_record_material(bool record_)
    {
      _record_material_ = record_;
      return;
    }

    void sensitive_detector::set_record_sensitive_category(bool record_)
    {
      _record_sensitive_category_ = record_;
      return;
    }

    void sensitive_detector::set_record_momentum(bool record_)
    {
      _record_momentum_ = record_;
      return;
    }

    void sensitive_detector::set_record_kinetic_energy(bool record_)
    {
      _record_kinetic_energy_ = record_;
      return;
    }

    void sensitive_detector::set_record_boundaries(bool record_)
    {
      _record_boundaries_ = record_;
      return;
    }

    int sensitive_detector::get_HCID() const
    {
      return _HCID_;
    }

    void sensitive_detector::set_HCID(int new_value_)
    {
      _HCID_ = new_value_;
      return;
    }

    sensitive_detector::hit_processor_dict_type &
    sensitive_detector::grab_hit_processors()
    {
      return _hit_processors_;
    }

    const sensitive_detector::hit_processor_dict_type &
    sensitive_detector::get_hit_processors() const
    {
      return _hit_processors_;
    }

    mctools::base_step_hit_processor &
    sensitive_detector::grab_hit_processor(const std::string & name_)
    {
      hit_processor_dict_type::iterator found = _hit_processors_.find(name_);
      DT_THROW_IF (found == _hit_processors_.end(),
                   std::logic_error, "No '" << name_ << "' hit processor !");
      return *found->second;
    }

    void sensitive_detector::add_hit_processor(const std::string & name_,
                                                mctools::base_step_hit_processor & shp_)
    {
      DT_THROW_IF (_hit_processors_.find(name_) != _hit_processors_.end(),
                   std::logic_error,
                   "This sensitive detector '" << _sensitive_category_
                   << "' already has a '" << name_ << "' hit processor !");
      _hit_processors_[name_] = &shp_;
      return;
    }

    bool sensitive_detector::has_hit_processor(const std::string & name_) const
    {
      return (_hit_processors_.find(name_) != _hit_processors_.end());
    }

    void sensitive_detector::attach_logical_volume(const std::string & log_volume_name_)
    {
      if (find(_attached_logical_volumes_.begin(),
                _attached_logical_volumes_.end(),
                log_volume_name_) != _attached_logical_volumes_.end()) {
        DT_LOG_WARNING(_logprio(), "Logical volume '" << log_volume_name_
                       << "' is already attached to sensitive detector '"
                       << _sensitive_category_ << "' !");
        return;
      }
      _attached_logical_volumes_.push_back(log_volume_name_);
      return;
    }

    std::list<std::string> &
    sensitive_detector::grab_attached_logical_volumes()
    {
      return _attached_logical_volumes_;
    }

    std::string sensitive_detector::make_hit_collection_name(const std::string & name_)
    {
      std::ostringstream oss;
      oss << name_ << ".hit_collection";
      return oss.str();
    }

    void sensitive_detector::set_record_g4_volume_properties(bool flag_)
    {
      _record_g4_volume_properties_ = flag_;
      return;
    }

    bool sensitive_detector::is_record_g4_volume_properties() const
    {
      return _record_g4_volume_properties_;
    }

    const std::string & sensitive_detector::get_sensitive_category() const
    {
      return _sensitive_category_;
    }

    unsigned int sensitive_detector::get_hits_buffer_capacity() const
    {
      return _hits_buffer_capacity_;
    }

    void sensitive_detector::set_hits_buffer_capacity(unsigned int capacity_)
    {
      if (capacity_ > 0) {
        _hits_buffer_capacity_ = capacity_;
      }
      return;
    }

    void sensitive_detector::configure(const datatools::properties & config_)
    {
      _initialize_logging_support(config_);

      // Association with some logical volumes in the geometry:
      std::vector<std::string> logical_volumes;
      if (config_.has_key("sensitive.volumes")) {
        config_.fetch("sensitive.volumes", logical_volumes);
        for (int i = 0; i < (int) logical_volumes.size(); i++) {
          const std::string & lvname = logical_volumes[i];
          attach_logical_volume(lvname);
        }
      }

      {
        // Record track id
        const std::string trkid_key
          = geomtools::sensitive::make_key(sensitive_utils::SENSITIVE_RECORD_TRACK_ID);
        if (config_.has_flag(trkid_key)) {
          // this method enables the sensitive detector to record the track ID
          // as an auxiliary properties of the step hit
          set_record_track_id(true);
        }
      }

      {
        // Record boundaries
        const std::string trkid_key
          = geomtools::sensitive::make_key(sensitive_utils::SENSITIVE_RECORD_BOUNDARIES);
        if (config_.has_flag(trkid_key)) {
          // this method enables the sensitive detector to record the boundary flags
          // as an auxiliary properties of the step hit
          set_record_boundaries(true);
        }
      }

      {
        // Record primary particle flag
        const std::string pp_key
          = geomtools::sensitive::make_key(sensitive_utils::SENSITIVE_RECORD_PRIMARY_PARTICLE);
        if (config_.has_flag(pp_key)) {
          // this method enables the sensitive detector to record a dedicated flag
          // as an auxiliary properties of the step hit to flag a primary track
          set_record_primary_particle(true);
        }
      }

      {
        // Record alpha quenching flag
        const std::string aq_key
          = geomtools::sensitive::make_key(sensitive_utils::SENSITIVE_RECORD_ALPHA_QUENCHING);
        if (config_.has_flag(aq_key)) {
          // this method enables to add a special flag to the step hit
          // for delta rays produced along the track of alpha particles;
          // this should make possible to take into account quenching effects
          // along alpha tracks by aggregation of the total energy deposit
          // by secondary delta rays along the alpha track.
          set_record_delta_ray_from_alpha(true);
        }
      }

      // XG (28/05/2011): the way to set 'record' properties is only
      // working when class members are set to false by default (which
      // is the case for the following parameters). For tracking
      // gammas and neutrons, boolean flag is set to true by default
      // and then such construction is not working anymore. If default
      // behavior change, have a look to 'tracking gamma/neutron' to
      // be sure to set properties properly
      {
        // Record major track
        const std::string rmt_key
          = geomtools::sensitive::make_key(sensitive_utils::SENSITIVE_RECORD_MAJOR_TRACK);
        if (config_.has_flag(rmt_key)) {
          set_record_major_track(true);
        }
      }

      {
        const std::string mtme_key
          = geomtools::sensitive::make_key(sensitive_utils::SENSITIVE_MAJOR_TRACK_MINIMUM_ENERGY);
        if (config_.has_key(mtme_key)) {
          const double e_min = config_.fetch_real(mtme_key) * CLHEP::keV;
          DT_THROW_IF (e_min < 0, std::logic_error,
                       "Invalid minimum energy for major track mode "
                       << "for sensitive detector '"
                       << _sensitive_category_
                       << "' !");
          set_major_track_minimum_energy(e_min);
        }
      }

      {
        // Record creator process
        const std::string record_creator_process_key
          = geomtools::sensitive::make_key(sensitive_utils::SENSITIVE_RECORD_CREATOR_PROCESS);
        if (config_.has_flag(record_creator_process_key)) {
          set_record_creator_process(true);
        }
      }

      {
        // Record sensitive category where particle is created
        const std::string record_creator_category_key
          = geomtools::sensitive::make_key(sensitive_utils::SENSITIVE_RECORD_CREATOR_CATEGORY);
        if (config_.has_flag(record_creator_category_key)) {
          set_record_creator_category(true);
        }
      }

      {
        // 2011-08-26 FM: new option : 'record material'
        const std::string record_material_key
          = geomtools::sensitive::make_key(sensitive_utils::SENSITIVE_RECORD_MATERIAL);
        if (config_.has_flag(record_material_key)) {
          set_record_material(true);
        }
      }

      {
        // 2011-08-26 FM: new option : 'record sensitive category'
        const std::string record_sensitive_category_key
          = geomtools::sensitive::make_key(sensitive_utils::SENSITIVE_RECORD_SENSITIVE_CATEGORY);
        if (config_.has_flag(record_sensitive_category_key)) {
          set_record_sensitive_category(true);
        }
      }

      {
        // Record momentum
        const std::string record_momentum_key
          = geomtools::sensitive::make_key(sensitive_utils::SENSITIVE_RECORD_MOMENTUM);
        if (config_.has_flag(record_momentum_key)) {
          set_record_momentum(true);
        }
      }

      {
        // Record kinetic energy
        const std::string record_kinetic_energy_key = geomtools::sensitive::make_key(sensitive_utils::SENSITIVE_RECORD_KINETIC_ENERGY);
        if (config_.has_flag(record_kinetic_energy_key)) {
          set_record_kinetic_energy(true);
        }
      }

      {
        // Record true step length
        const std::string record_step_length_key
          = geomtools::sensitive::make_key(sensitive_utils::SENSITIVE_RECORD_STEP_LENGTH);
        if (config_.has_flag(record_step_length_key)) {
          set_record_step_length(true);
        }
      }

      {
        const std::string key
          = geomtools::sensitive::make_key(sensitive_utils::SENSITIVE_HITS_BUFFER_CAPACITY);
        if (config_.has_key(key)) {
          const int cap = config_.fetch_integer(key);
          DT_THROW_IF (cap < 0, std::logic_error,
                       "Invalid buffer size for sensitive detector '"
                       << _sensitive_category_
                       << "' !");
          const unsigned int capacity = (unsigned int) cap;
          if (capacity > 0) {
            set_hits_buffer_capacity(capacity);
          }
        }
      }

      // Drop steps with no energy deposit
      if (geomtools::sensitive::has_key(config_, sensitive_utils::SENSITIVE_DROP_ZERO_ENERGY_DEPOSIT)) {
        const bool flag = geomtools::sensitive::has_flag(config_,
                                                         sensitive_utils::SENSITIVE_DROP_ZERO_ENERGY_DEPOSIT);
        set_drop_zero_energy_deposit_steps(flag);
      }

      // Storing Geant4 volume properties
      if (geomtools::sensitive::has_key(config_, sensitive_utils::SENSITIVE_RECORD_G4_VOLUME_INFOS)) {
        const bool flag = geomtools::sensitive::has_flag(config_,
                                                         sensitive_utils::SENSITIVE_RECORD_G4_VOLUME_INFOS);
        set_record_g4_volume_properties(flag);
      }

      // Tracking gamma
      if (geomtools::sensitive::has_key(config_, sensitive_utils::SENSITIVE_TRACK_GAMMA)) {
        const bool flag = geomtools::sensitive::has_flag(config_,
                                                         sensitive_utils::SENSITIVE_TRACK_GAMMA);
        set_track_gamma(flag);
      }

      // Tracking optical photon
      if (geomtools::sensitive::has_key(config_, sensitive_utils::SENSITIVE_TRACK_OPTICAL_PHOTON)) {
        const bool flag = geomtools::sensitive::has_flag(config_,
                                                         sensitive_utils::SENSITIVE_TRACK_OPTICAL_PHOTON);
        set_track_optical_photon(flag);
      }

      // Tracking neutron:
      if (geomtools::sensitive::has_key(config_, sensitive_utils::SENSITIVE_TRACK_NEUTRON)) {
        const bool flag = geomtools::sensitive::has_flag(config_,
                                                         sensitive_utils::SENSITIVE_TRACK_NEUTRON);
        set_track_neutron(flag);
      }

      return;
    }

    void sensitive_detector::_set_defaults()
    {
      _track_gamma_                    = true;
      _track_optical_photon_           = true;
      _track_neutron_                  = true;
      _drop_zero_energy_deposit_steps_ = false;
      _record_g4_volume_properties_    = false;
      _record_delta_ray_from_alpha_    = false;
      _record_primary_particle_        = false;
      _record_track_id_                = false;
      _record_major_track_             = false;
      _major_track_minimum_energy_     = DEFAULT_MAJOR_TRACK_MINIMUM_ENERGY;
      _record_momentum_                = false;
      _record_kinetic_energy_          = false;
      _record_creator_process_         = false;
      _record_creator_category_        = false;
      _record_material_                = false;
      _record_sensitive_category_      = false;
      _record_step_length_             = false;
      _record_boundaries_              = false;
      _hits_buffer_capacity_           = DEFAULT_HIT_BUFFER_CAPACITY;

      // G4 Stuff:
      _HCID_ = -1; // Initialized with an invalid value

      return;
    }

    // ctor:
    sensitive_detector::sensitive_detector(const std::string & sensitive_category_)
      : G4VSensitiveDetector(sensitive_category_)
    {
      _manager_ = nullptr;
      _sensitive_category_ = sensitive_category_;

      _set_defaults();

      _track_info_ptr_ = nullptr;
      _parent_track_info_ptr_ = nullptr;

      _hits_collection_ = nullptr;
      _number_of_sensitive_steps_ = 0;

      _used_hits_count_ = 0;

      // G4 Stuff:
      G4String HCname;
      HCname = sensitive_detector::make_hit_collection_name(_sensitive_category_).c_str();
      collectionName.insert(HCname);
      return;
    }

    sensitive_detector::~sensitive_detector()
    {
      _hit_processors_.clear();
      _track_info_ptr_ = nullptr;
      _parent_track_info_ptr_ = nullptr;
      if (_hits_collection_ != nullptr) {
        delete _hits_collection_;
        _hits_collection_ = nullptr;
      }
      return;
    }

    void sensitive_detector::clear()
    {
      // Reset the internal ID if needed :
      for (int i = 0; i < _used_hits_count_; i++) {
        _hits_buffer_[i].grab_hit_data().reset();
      }
      _used_hits_count_ = 0;
      if (_hits_collection_ != nullptr) {
        _hits_collection_->grab_hits().clear();
      }
      return;
    }

    void sensitive_detector::Initialize(G4HCofThisEvent * /*some_hit_collections_*/)
    {
      // First pass, initialize the buffer :
      if (_hits_buffer_.size() == 0) {
        _hits_buffer_.reserve(_hits_buffer_capacity_);
        // pre-allocate some hits :
        for (int i = 0; i <(int) _hits_buffer_capacity_; i++) {
          sensitive_hit a_hit;
          _hits_buffer_.push_back(a_hit);
          // make sure they have no valid ID :
          //_hits_buffer.back().get_hit_data().invalidate_hit_id();
        }
        // No hit has been validated from this buffer yet :
        _used_hits_count_ = 0;
        //_hits_buffer.back().get_hit_data().tree_dump(cerr, "" , "DEVEL: ");
        DT_LOG_NOTICE(_logprio(), "Setup the buffer of hits for detector '" << _sensitive_category_ << "' "
                      << "with size = " << _hits_buffer_.size());
      }

      // Reset the hits in the buffer if needed :
      for (int i = 0; i < _used_hits_count_; i++) {
        _hits_buffer_[i].grab_hit_data().reset();
      }
      if (_hits_collection_ != nullptr) {
        _hits_collection_->grab_hits().clear();
      }

      // Reset the hit counter :
      _used_hits_count_ = 0;

      // Activates the track info mechanism if needed:
      bool track_history_request = false;
      if (_record_delta_ray_from_alpha_
          || _record_track_id_
          || _record_primary_particle_
          || _record_major_track_
          || _record_creator_process_
          || _record_creator_category_) {
        track_history_request = true;
      }

      if (track_history_request && ! _manager_->has_track_history()) {
        // Ask the manager...
        _manager_->set_use_track_history(true);
      }
      return;
    }

    void sensitive_detector::EndOfEvent(G4HCofThisEvent * some_hit_collections_)
    {
      if ( _HCID_ < 0 ) {
        _HCID_ = G4SDManager::GetSDMpointer()->GetCollectionID(collectionName[0]);
      }

      // Only if we have some hits :
      if (_used_hits_count_ > 0) {

        // Set the hits collection pointer :
        if (_hits_collection_ == nullptr) {
          _hits_collection_ = new sensitive_hit_collection(SensitiveDetectorName,
                                                            collectionName[0]);
          // Assign proper capacity to the 'hits collection' :
          _hits_collection_->grab_hits().reserve(_hits_buffer_.size());
        }

        sensitive_hit_collection::hit_collection_type & the_hits
          = _hits_collection_->grab_hits();

        // fill it with the hits' addresses(pointer) :
        for (int i = 0; i < _used_hits_count_; i++) {
          the_hits.push_back(&_hits_buffer_[i]);
        }

        // Push the "hits collection" in the event's list of hits collections :
        some_hit_collections_->AddHitsCollection(_HCID_, _hits_collection_);
      }

      // clear tracks infos for this event:
      _track_info_ptr_ = nullptr;
      _parent_track_info_ptr_ = nullptr;

      // BUGFIX? FM+AC 2014/09/05: should not be done here but in the event_action(to be validated)
      //_manager_->grab_track_history().reset();

      /*
        if ( _verbose > 1 )
        {
        sensitive_hits_collection * HC_tmp = nullptr;
        HC_tmp =(sensitive_hits_collection * )( some_hit_collections->GetHC(_HCID));
        for (unsigned int i = 0; i < HC_tmp->GetSize(); i++ )
        {
        clog << "DEBUG: sensitive_detector::EndOfEvent: "
        << "hit collection(" << i
        << ") is " << HC_tmp->GetVector()->at(i)
        << "." << endl;
        }
        }
      */
      return;
    }

    G4bool sensitive_detector::ProcessHits(G4Step * step_,
                                           G4TouchableHistory * /*touchable_history_*/)
    {
      DT_LOG_TRACE(_logprio(),"Entering...");
      const bool do_process_hits = true;
      if (! do_process_hits) {
        DT_LOG_TRACE(_logprio(),"Exiting.");
        return false;
      }

      DT_LOG_TRACE(_logprio(),"Buffer size = " << _hits_buffer_.size());
      DT_LOG_TRACE(_logprio(),"Hit count = " << _used_hits_count_);

      const double energy_deposit = step_->GetTotalEnergyDeposit();
      if (energy_deposit <= 1.e-10 * CLHEP::keV) {
        bool quit = true;
        if (step_->GetTrack()->GetDefinition() == G4Gamma::GammaDefinition()) {
          if (_track_gamma_) quit = false;
        } else if (step_->GetTrack()->GetDefinition() == G4Neutron::NeutronDefinition()) {
          if (_track_neutron_) quit = false;
          //} else if (step_->GetTrack()->GetDefinition() == G4Neutron::OpticalPhotonDefinition()) {
          //if (_track_optical_photon_) quit = false;
        } else if (!_drop_zero_energy_deposit_steps_) {
          quit = false;
        }
        if (quit) {
          // We decide not to process this hit further because it is not interesting
          DT_LOG_TRACE(_logprio(),"Exiting.");
          return false;
        }
      }

      if (_manager_->using_time_stat()) {
        _manager_->grab_CT_map()["EA"].pause();
        _manager_->grab_CT_map()["SD"].start();
      }

      const std::string track_particle_name = step_->GetTrack()->GetDefinition()->GetParticleName();
      const int track_id        = step_->GetTrack()->GetTrackID();
      const int parent_track_id = step_->GetTrack()->GetParentID();
      /*
      const double global_time  = step_->GetTrack()->GetGlobalTime();
      const double local_time   = step_->GetTrack()->GetLocalTime();
      const double proper_time  = step_->GetTrack()->GetProperTime();
      step_->GetTrack()->GetVertexPosition();
      step_->GetTrack()->GetVertexMomentumDirection();
      step_->GetTrack()->GetLogicalVolumeAtVertex();
      step_->GetTrack()->GetWeight();
      */

      bool primary_track           = false;
      bool delta_ray_from_an_alpha = false;
      bool major_track             = false;

      // Grabbing the track history to fill G4 track info
      track_history & the_track_history = _manager_->grab_track_history();
      if (_manager_->has_track_history()) {
        if ((_track_info_ptr_ == nullptr) || (_track_info_ptr_->get_id() != track_id)) {
          // Here we don't have current track info at hand:
          if (! the_track_history.has_track_info(track_id)) {
            // infos about this track are not registered yet,
            // we add a new record and link it :
            track_history::track_info dummy;
            the_track_history.add_track_info(track_id, dummy);
            track_history::track_info & ti = the_track_history.grab_track_info(track_id);
            ti.set_id(track_id);
            ti.set_parent_id(parent_track_id);
            ti.set_particle_name(track_particle_name);
            if (step_->GetTrack()->GetCreatorProcess()) {
              const std::string & process_name
                = step_->GetTrack()->GetCreatorProcess()->GetProcessName();
              ti.set_creator_process_name(process_name);
            }
            // const std::string & category = get_sensitive_category();
            // ti.set_creator_sensitive_category(category);
            _track_info_ptr_ = &ti;
            _parent_track_info_ptr_ = nullptr;
          } else {
            // infos about this track have already been registered, we link it:
            _track_info_ptr_ = &the_track_history.grab_track_info(track_id);
            _parent_track_info_ptr_ = nullptr;
         }
        }
        primary_track = _track_info_ptr_->is_primary();

        // Set the 'major_track' flag :
        // MOD: FM+AC 2014-09-05: accept neutral particles as "major" tracks(gamma, neutron...):
        // const bool has_charge =(step_->GetTrack()->GetDynamicParticle()->GetCharge() != 0.0);
        // if (has_charge) {
        if (primary_track) {
          major_track = true;
        }
        const double kinetic_energy= step_->GetTrack()->GetKineticEnergy();
        if (kinetic_energy >= _major_track_minimum_energy_) {
          major_track = true;
        }
        // }

        if (_record_delta_ray_from_alpha_) {
          /* Identify a delta-ray generated along
           * the track of an alpha particle:
           */
          if (track_particle_name == "e-" && ! primary_track) {
            // this is a secondary electron from a parent track:
            if ((_parent_track_info_ptr_ == nullptr) ||
               (_parent_track_info_ptr_->get_id() != parent_track_id)) {
              // we don't have current parent track info at hand
              // so we try to find it in the track info dictionnary:
              if (the_track_history.has_track_info(parent_track_id)) {
                // if found, make it the current link:
                _parent_track_info_ptr_ = &the_track_history.grab_track_info(parent_track_id);
              }
            }

            // if the parent track has been identified and it is an alpha particle:
            if ((_parent_track_info_ptr_ != nullptr) &&
               (_parent_track_info_ptr_->get_particle_name() == "alpha")) {
              // flag the delta_ray:
              delta_ray_from_an_alpha = true;
            }
          }
        } // _record_delta_ray_from_alpha_

      } // if (_using_track_infos)

      if (_used_hits_count_ == (int) _hits_buffer_.size()) {
        //unsigned int osize = _hits_buffer.size();
        sensitive_hit a_hit;
        _hits_buffer_.push_back(a_hit);
        //unsigned int nsize = _hits_buffer.size();
        // cerr << datatools::io::devel
        //      << "snemo::g4::sensitive_detector::ProcessHits: "
        //      << "Increase the size of the buffer of sensitive hits for detector '"
        //      << _sensitive_category_ << "' from " << osize
        //      << " to " << nsize << "(capacity=" << _hits_buffer.capacity() << ")!"
        //      << endl;
      }
      // Increment the hit counter :
      _used_hits_count_++;

      sensitive_hit * new_hit = &_hits_buffer_[_used_hits_count_ - 1];

      DT_LOG_TRACE(_logprio(), "Buffer size = " << _hits_buffer_.size());
      DT_LOG_TRACE(_logprio(), "Hit count = " << _used_hits_count_);
      DT_LOG_TRACE(_logprio(), "New hit @ " << new_hit << " : ");
      if (_logprio() == datatools::logger::PRIO_TRACE){
        new_hit->get_hit_data().tree_dump(std::cerr);
      }

      _number_of_sensitive_steps_++;
      // 2011-05-26 FM : was using "step_->GetTrack()->GetGlobalTime()";
      const double time_start = step_->GetPreStepPoint()->GetGlobalTime();
      const double time_stop = step_->GetPostStepPoint()->GetGlobalTime();
      new_hit->grab_hit_data().set_time_start(time_start);
      new_hit->grab_hit_data().set_time_stop(time_stop);
      new_hit->grab_hit_data().set_position_start(step_->GetPreStepPoint()->GetPosition());
      new_hit->grab_hit_data().set_position_stop(step_->GetPostStepPoint()->GetPosition());
      new_hit->grab_hit_data().set_energy_deposit(energy_deposit);
      new_hit->grab_hit_data().set_particle_name(track_particle_name);

      // Add optional data :
      if (_record_momentum_) {
        new_hit->grab_hit_data().set_momentum_start(step_->GetPreStepPoint()->GetMomentum());
        new_hit->grab_hit_data().set_momentum_stop(step_->GetPostStepPoint()->GetMomentum());
      }

      // 2018-08-08, FM:
      //  New version of the base_step_hit class does not make
      //  use of the auxiliary properties.
      //// Grab base hit auxiliaries
      //// datatools::properties & hit_aux = new_hit->grab_hit_data().grab_auxiliaries();

      // Add auxiliary properties :
      if (_record_kinetic_energy_) {
        new_hit->grab_hit_data().set_kinetic_energy_start(step_->GetPreStepPoint()->GetKineticEnergy());
        new_hit->grab_hit_data().set_kinetic_energy_stop(step_->GetPostStepPoint()->GetKineticEnergy());
        //   hit_aux.store_real(mctools::track_utils::START_KINETIC_ENERGY_KEY,
        //                       step_->GetPreStepPoint()->GetKineticEnergy());
        //   hit_aux.store_real(mctools::track_utils::STOP_KINETIC_ENERGY_KEY,
        //                       step_->GetPostStepPoint()->GetKineticEnergy());
      }

      if (_record_step_length_) {
        new_hit->grab_hit_data().set_step_length(step_->GetStepLength());
        // hit_aux.store_real(mctools::track_utils::STEP_LENGTH, step_->GetStepLength());
      }

      if (_record_boundaries_) {
        if (step_->GetPreStepPoint()->GetStepStatus() == fGeomBoundary) {
          new_hit->grab_hit_data().set_entering_volume(true);
          // hit_aux.store_flag(mctools::track_utils::ENTERING_VOLUME_FLAG);
        }
        if (step_->GetPostStepPoint()->GetStepStatus() == fGeomBoundary) {
          new_hit->grab_hit_data().set_leaving_volume(true);
          // hit_aux.store_flag(mctools::track_utils::LEAVING_VOLUME_FLAG);
        }
      }

      const bool use_track_info = _manager_->has_track_history();
      if (use_track_info) {
        // special features:
        if (_record_creator_process_ && ! _track_info_ptr_->get_creator_process_name().empty()) {
          new_hit->grab_hit_data().set_creator_process_name(_track_info_ptr_->get_creator_process_name());
          // hit_aux.store_string(mctools::track_utils::CREATOR_PROCESS_KEY,
          //                   _track_info_ptr_->get_creator_process_name());
        }

        // Unused:
        // if (_record_creator_category_ && ! _track_info_ptr_->get_creator_sensitive_category().empty()) {
        //   hit_aux.store_string(mctools::track_utils::CREATOR_CATEGORY_KEY,
        //                         _track_info_ptr_->get_creator_sensitive_category());
        // }

        if (_record_primary_particle_ && primary_track) {
          new_hit->grab_hit_data().set_primary_particle(true);
          // hit_aux.store_flag(mctools::track_utils::PRIMARY_PARTICLE_FLAG);
        }

        if (_record_major_track_ && major_track) {
          new_hit->grab_hit_data().set_major_track(true);
          // hit_aux.store_flag(mctools::track_utils::MAJOR_TRACK_FLAG);
        }

        if (_record_delta_ray_from_alpha_ && delta_ray_from_an_alpha) {
          new_hit->grab_hit_data().set_delta_ray_from_alpha(true);
          // hit_aux.store_flag(mctools::track_utils::DELTA_RAY_FROM_ALPHA_FLAG);
        }

        if (_record_track_id_) {
          new_hit->grab_hit_data().set_track_id(track_id);
          new_hit->grab_hit_data().set_parent_track_id(parent_track_id);
          // hit_aux.store_integer(mctools::track_utils::TRACK_ID_KEY, track_id);
          // hit_aux.store_integer(mctools::track_utils::PARENT_TRACK_ID_KEY, parent_track_id);
        }
      }

      if (_record_material_) {
        static std::string material_ref_key =
          geomtools::material::make_key(geomtools::material::material_ref_property());
        const G4Material * the_g4_material = step_->GetTrack()->GetMaterial();
        std::string material_ref = the_g4_material->GetName().data();
        boost::replace_all(material_ref, "__" , "::");
        new_hit->grab_hit_data().set_material_name(material_ref);
        // hit_aux.store_string(material_ref_key, material_ref);
      }

      if (_record_sensitive_category_) {
        static std::string sensitive_category_key =
          geomtools::sensitive::make_key(geomtools::sensitive::constants::instance().SENSITIVE_CATEGORY_PROPERTY);
        new_hit->grab_hit_data().set_sensitive_category(get_sensitive_category());
        // hit_aux.store_string(sensitive_category_key, get_sensitive_category());
      }

      if (_record_g4_volume_properties_) {
        G4VPhysicalVolume * volume = step_->GetTrack()->GetVolume();
        new_hit->grab_hit_data().set_g4_volume_name(volume->GetName());
        new_hit->grab_hit_data().set_g4_volume_copy_number(volume->GetCopyNo());
        // hit_aux.store_string(sensitive_utils::SENSITIVE_G4_VOLUME_NAME_KEY, volume->GetName());
        // hit_aux.store_integer(sensitive_utils::SENSITIVE_G4_VOLUME_COPY_NUMBER_KEY, volume->GetCopyNo());
      }

      if (_manager_->using_time_stat()) {
        _manager_->grab_CT_map()["SD"].stop();
        _manager_->grab_CT_map()["EA"].resume();
      }

      DT_LOG_TRACE(_logprio(),"Exiting.");
      return true;
    }

    void sensitive_detector::tree_dump(std::ostream & out_,
                                       const std::string & title_,
                                       const std::string & indent_,
                                       bool /*inherit_*/) const
    {
      std::string indent;
      if (! indent_.empty()) indent = indent_;
      if (! title_.empty()) {
        out_ << indent << title_ << std::endl;
      }
      out_ << indent << datatools::i_tree_dumpable::tag
           << "Logging priority : '" << datatools::logger::get_priority_label(_logging_priority) << "'" <<  std::endl;
      out_ << indent << datatools::i_tree_dumpable::tag
           << "Category         : '" << _sensitive_category_ << "'" << std::endl;
      // Logical volumes
      {
        out_ << indent << datatools::i_tree_dumpable::tag
             << "Attached logical volumes    : " << _attached_logical_volumes_.size() << std::endl;
        for (std::list<std::string>::const_iterator
               ilog = _attached_logical_volumes_.begin();
             ilog != _attached_logical_volumes_.end(); ++ilog) {
          out_ << indent << datatools::i_tree_dumpable::skip_tag;
          std::list<std::string>::const_iterator jlog = ilog;
          if (++jlog == _attached_logical_volumes_.end()) {
            out_ << datatools::i_tree_dumpable::last_tag;
          } else {
            out_ << datatools::i_tree_dumpable::tag;
          }
          out_ << "Logical["
               << std::distance(_attached_logical_volumes_.begin(), ilog)
               << "]: '" << *ilog << "'" << std::endl;
        }
      }

      // Flags
      {
        out_ << indent << datatools::i_tree_dumpable::tag
             << "Drop zero energy deposit    : "
             << (_drop_zero_energy_deposit_steps_ ? "Yes" : "No") << std::endl;

        out_ << indent << datatools::i_tree_dumpable::tag
             << "Track gamma                 : "
             << (_track_gamma_ ? "Yes" : "No") << std::endl;

        out_ << indent << datatools::i_tree_dumpable::tag
             << "Track optical photon        : "
             << (_track_optical_photon_ ? "Yes" : "No") << std::endl;

        out_ << indent << datatools::i_tree_dumpable::tag
             << "Track neutron               : "
             << (_track_neutron_ ? "Yes" : "No") << std::endl;

        out_ << indent << datatools::i_tree_dumpable::tag
             << "Record volume properties    : "
             << (_record_g4_volume_properties_ ? "Yes" : "No") << std::endl;

        out_ << indent << datatools::i_tree_dumpable::tag
             << "Record momentum             : "
             << (_record_momentum_ ? "Yes" : "No") << std::endl;

        out_ << indent << datatools::i_tree_dumpable::tag
             << "Record kinetic energy       : "
             << (_record_kinetic_energy_ ? "Yes" : "No") << std::endl;

        out_ << indent << datatools::i_tree_dumpable::tag
             << "Record primary particle     : "
             << (_record_primary_particle_ ? "Yes" : "No") << std::endl;

        out_ << indent << datatools::i_tree_dumpable::tag
             << "Record track id             : "
             << (_record_track_id_ ? "Yes" : "No") << std::endl;

        out_ << indent << datatools::i_tree_dumpable::tag
             << "Record creator process      : "
             << (_record_creator_process_ ? "Yes" : "No") << std::endl;

        out_ << indent << datatools::i_tree_dumpable::tag
             << "Record major track          : "
             << (_record_major_track_ ? "Yes" : "No");

        if (_record_major_track_) {
          out_ << "(Emin > "
               << _major_track_minimum_energy_ / CLHEP::keV << " keV)"
               << std::endl;
        } else {
          out_ << std::endl;
        }

        out_ << indent << datatools::i_tree_dumpable::tag
             << "Record material name        : "
             << (_record_material_ ? "Yes" : "No") << std::endl;

        out_ << indent << datatools::i_tree_dumpable::tag
             << "Record delta ray from alpha : "
             << (_record_delta_ray_from_alpha_ ? "Yes" : "No") << std::endl;

        out_ << indent << datatools::i_tree_dumpable::tag
             << "Track info pointer          : ";
        if (_track_info_ptr_ != nullptr) out_ << _track_info_ptr_;
        else                       out_ << "Not allocated";
        out_ << std::endl;

        out_ << indent << datatools::i_tree_dumpable::tag
             << "Parent track info pointer   : ";
        if (_parent_track_info_ptr_ != nullptr) out_ << _parent_track_info_ptr_;
        else                              out_ << "Not allocated";
        out_ << std::endl;

        out_ << indent << datatools::i_tree_dumpable::tag
             << "Hits buffer capacity        : "
             << _hits_buffer_capacity_ << std::endl;
      }

      // {
      //   out_ << indent << datatools::i_tree_dumpable::tag
      //        << "Properties : "
      //        << std::endl;
      //   {
      //     ostringstream indent_oss;
      //     indent_oss << indent;
      //     indent_oss << datatools::i_tree_dumpable::skip_tag;
      //     _aux.tree_dump(out_, "", indent_oss.str());
      //   }
      // }

      // Associated step hit processor
      {
        out_ << indent << datatools::i_tree_dumpable::last_tag
             << "Step hit processors         : ";

        if (_hit_processors_.size() == 0) {
          out_ << "No" << std::endl;
        } else {
          out_ << _hit_processors_.size() << std::endl;
          for (hit_processor_dict_type::const_iterator ihp
                 = _hit_processors_.begin();
               ihp != _hit_processors_.end();
               ++ihp) {
            const std::string & hp_name = ihp->first;
            mctools::base_step_hit_processor * hp = ihp->second;

            out_ << indent << datatools::i_tree_dumpable::last_skip_tag;
            hit_processor_dict_type::const_iterator jhp = ihp;
            if (++jhp == _hit_processors_.end()) {
              out_ << datatools::i_tree_dumpable::last_tag;
            } else {
              out_ << datatools::i_tree_dumpable::tag;
            }
            out_ << "Processor[" << std::distance(_hit_processors_.begin(), ihp)
                 << "]: '" << hp_name
                 << "' @ " << hp << " with hit category '"
                 << hp->get_hit_category() << "'" << std::endl;
          }
        }
      }
      return;
    }

  } // end of namespace g4

} // end of namespace mctools

/** Opening macro for implementation
 *  This macro must be used outside of any namespace.
 */
DOCD_CLASS_IMPLEMENT_LOAD_BEGIN(mctools::g4::sensitive_detector,ocd_)
{
  // The class name :
  ocd_.set_class_name("mctools::g4::sensitive_detector");

  // The class terse description :
  ocd_.set_class_description("The Geant4 virtual sensitive detector");

  // The library the class belongs to :
  ocd_.set_class_library("mctools_g4");

  // The class detailed documentation :
  ocd_.set_class_documentation("This is the Geant4 virtual sensitive detector.        \n"
                                "Sensitive detectors are associated to logical volumes \n"
                                "in order to apply some special processing on the particles\n"
                                "tracking steps that traverse these geometry volumes.  \n"
                                "The sensitive detector object is thus responsible of  \n"
                                "the recording of specific physics informations along  \n"
                                "each step. The ``mctools::base_step_hit`` class is used\n"
                                "as the data model for Monte-Carlo truth hits. It store \n"
                                "the following attributes :                            \n"
                                "                                                      \n"
                                " * hit ID                                             \n"
                                " * geometry ID of the traversed geometry volume       \n"
                                " * start and stop position of the truth step          \n"
                                " * start and stop time of the truth step              \n"
                                " * start and stop momentum of the truth step          \n"
                                " * particle name                                      \n"
                                " * energy deposited along the step                    \n"
                                "                                                      \n"
                                "Additionnal auxiliary properties may be stored in any \n"
                                "truth hit object:                                     \n"
                                "                                                      \n"
                                " * track ID and parent track ID                       \n"
                                " * Geant4 geometry volume informations (name and copy number)\n"
                                " * kinetic energy of the particle along the step      \n"
                                " * primary particle flag                              \n"
                                " * name of the creator process                        \n"
                                " * major track flag                                   \n"
                                " * name of the material                               \n"
                                " * name of the sensitive category                     \n"
                                " * step length                                        \n"
                                " * boundary flags                                     \n"
                                "                                                      \n"
                                );


  {
    // Description of the 'logging.priority' configuration property :
    datatools::configuration_property_description & cpd
      = ocd_.add_property_info();
    cpd.set_name_pattern("logging.priority")
      .set_terse_description("Logging priority threshold")
      .set_traits(datatools::TYPE_STRING)
      .set_mandatory(false)
      .set_long_description("Allowed values are:                                    \n"
                            "                                                       \n"
                            " * ``\"fatal\"``       : print fatal error messages    \n"
                            " * ``\"critical\"``    : print critical error messages \n"
                            " * ``\"error\"``       : print error messages          \n"
                            " * ``\"warning\"``     : print warnings                \n"
                            " * ``\"notice\"``      : print notice messages         \n"
                            " * ``\"information\"`` : print informational messages  \n"
                            " * ``\"debug\"``       : print debug messages          \n"
                            " * ``\"trace\"``       : print trace messages          \n"
                            "                                                       \n"
                            "Default value: ``\"warning\"``                         \n"
                            "                                                       \n"
                            "Example::                                              \n"
                            "                                                       \n"
                            "  logging.priority : string = \"warning\"              \n"
                            "                                                       \n"
                            )
      ;
  }


  {
    // Description of the 'track_gamma' configuration property :
    datatools::configuration_property_description & cpd
      = ocd_.add_property_info();
    cpd.set_name_pattern("sensitive.track_gamma")
      .set_terse_description("Record *zero energy deposit* hits from gamma particle")
      .set_traits(datatools::TYPE_BOOLEAN)
      .set_mandatory(false)
      .set_long_description("Default value: ``1``                                   \n"
                            "                                                       \n"
                            "Example::                                              \n"
                            "                                                       \n"
                            "  sensitive.track_gamma : boolean = 1                  \n"
                            "                                                       \n"
                            )
      ;
  }

  {
    // Description of the 'track_optical_photon' configuration property :
    datatools::configuration_property_description & cpd
      = ocd_.add_property_info();
    cpd.set_name_pattern("sensitive.track_optical_photon")
      .set_terse_description("Record *zero energy deposit* hits from optical photon")
      .set_traits(datatools::TYPE_BOOLEAN)
      .set_mandatory(false)
      .set_long_description("Default value: ``1``                                   \n"
                            "                                                       \n"
                            "Example::                                              \n"
                            "                                                       \n"
                            "  sensitive.track_optical_photon : boolean = 1         \n"
                            "                                                       \n"
                            )
      ;
  }

  {
    // Description of the 'track_neutron' configuration property :
    datatools::configuration_property_description & cpd
      = ocd_.add_property_info();
    cpd.set_name_pattern("sensitive.track_neutron")
      .set_terse_description("Record *zero energy deposit* hits from neutron")
      .set_traits(datatools::TYPE_BOOLEAN)
      .set_mandatory(false)
      .set_long_description("Default value: ``1``                                   \n"
                            "                                                       \n"
                            "Example::                                              \n"
                            "                                                       \n"
                            "  sensitive.track_neutron : boolean = 1                \n"
                            "                                                       \n"
                            )
      ;
  }

  {
    // Description of the 'drop_zero_energy_deposit_steps' configuration property :
    datatools::configuration_property_description & cpd
      = ocd_.add_property_info();
    cpd.set_name_pattern("sensitive.drop_zero_energy_deposit_steps")
      .set_terse_description("Do not record *zero energy deposit* hits from particles other than photons and neutrons")
      .set_traits(datatools::TYPE_BOOLEAN)
      .set_mandatory(false)
      .set_long_description("Default value: ``0``                                   \n"
                            "                                                       \n"
                            "Example::                                              \n"
                            "                                                       \n"
                            "  sensitive.drop_zero_energy_deposit_steps : boolean = 0 \n"
                            "                                                       \n"
                            )
      ;
  }


  {
    // Description of the 'record_primary_particle' configuration property :
    datatools::configuration_property_description & cpd
      = ocd_.add_property_info();
    cpd.set_name_pattern("sensitive.record_primary_particle")
      .set_terse_description("Record the 'track.primary' flag")
      .set_traits(datatools::TYPE_BOOLEAN)
      .set_mandatory(false)
      .set_long_description("                                                       \n"
                            "Example::                                              \n"
                            "                                                       \n"
                            "  sensitive.record_primary_particle : boolean = 1      \n"
                            "                                                       \n"
                            )
      ;
  }


  {
    // Description of the 'record_track_id' configuration property :
    datatools::configuration_property_description & cpd
      = ocd_.add_property_info();
    cpd.set_name_pattern("sensitive.record_track_id")
      .set_terse_description("Record the 'track.id' and 'track.parent_id' properties")
      .set_traits(datatools::TYPE_BOOLEAN)
      .set_mandatory(false)
      .set_long_description("Example::                                              \n"
                            "                                                       \n"
                            "  sensitive.record_track_id : boolean = 1              \n"
                            "                                                       \n"
                            )
      ;
  }

  {
    // Description of the 'record_alpha_quenching' configuration property :
    datatools::configuration_property_description & cpd
      = ocd_.add_property_info();
    cpd.set_name_pattern("sensitive.record_alpha_quenching")
      .set_terse_description("Record the 'track.delta_ray_from_alpha' flag for secondary ionization electrons from an alpha particle")
      .set_traits(datatools::TYPE_BOOLEAN)
      .set_mandatory(false)
      .set_long_description("Example::                                              \n"
                            "                                                       \n"
                            "  sensitive.record_alpha_quenching : boolean = 1       \n"
                            "                                                       \n"
                            )
      ;
  }

  {
    // Description of the 'record_major_track' configuration property :
    datatools::configuration_property_description & cpd
      = ocd_.add_property_info();
    cpd.set_name_pattern("sensitive.record_major_track")
      .set_terse_description("Record the 'track.major' flag for primary or high-energy particles")
      .set_traits(datatools::TYPE_BOOLEAN)
      .set_mandatory(false)
      .set_long_description("A *major* track is defined either :                    \n"
                            "                                                       \n"
                            " * as a primary particle track                         \n"
                            " * as a track of a charged particle with kinetic energy\n"
                            "   above a given threshold.                            \n"
                            "                                                       \n"
                            "Example::                                              \n"
                            "                                                       \n"
                            "  sensitive.record_major_track : boolean = 1           \n"
                            "                                                       \n"
                            )
      ;
  }

  {
    // Description of the 'record_major_track' configuration property :
    datatools::configuration_property_description & cpd
      = ocd_.add_property_info();
    cpd.set_name_pattern("sensitive.major_track_minimum_energy")
      .set_terse_description("Set the low energy threshold for major charged particles")
      .set_traits(datatools::TYPE_REAL)
      .set_explicit_unit(true)
      .set_unit_label("energy")
      .set_mandatory(false)
      .set_triggered_by_flag("sensitive.record_major_track")
      .set_long_description("Default value:: 10 keV (above the energy of most delta-rays)\n"
                            "                                                       \n"
                            "Example::                                              \n"
                            "                                                       \n"
                            "  sensitive.record_major_track : boolean = 1           \n"
                            "  sensitive.major_track_minimum_energy : real as energy = 25 keV \n"
                            "                                                       \n"
                            )
      ;
  }


  {
    // Description of the 'record_creator_process' configuration property :
    datatools::configuration_property_description & cpd
      = ocd_.add_property_info();
    cpd.set_name_pattern("sensitive.record_creator_process")
      .set_terse_description("Record the 'track.creator_process' string property")
      .set_traits(datatools::TYPE_BOOLEAN)
      .set_mandatory(false)
      .set_long_description("Example::                                              \n"
                            "                                                       \n"
                            "  sensitive.record_creator_process : boolean = 1       \n"
                            "                                                       \n"
                            )
      ;
  }


  {
    // Description of the 'record_material' configuration property :
    datatools::configuration_property_description & cpd
      = ocd_.add_property_info();
    cpd.set_name_pattern("sensitive.record_material")
      .set_terse_description("Record the 'material.ref' string property")
      .set_traits(datatools::TYPE_BOOLEAN)
      .set_mandatory(false)
      .set_long_description("Example::                                              \n"
                            "                                                       \n"
                            "  sensitive.record_material : boolean = 1              \n"
                            "                                                       \n"
                            )
      ;
  }

  {
    // Description of the 'record_kinetic_energy' configuration property :
    datatools::configuration_property_description & cpd
      = ocd_.add_property_info();
    cpd.set_name_pattern("sensitive.record_kinetic_energy")
      .set_terse_description("Record the 'track.start_kinetic_energy' and 'track.stop_kinetic_energy' real properties")
      .set_traits(datatools::TYPE_BOOLEAN)
      .set_mandatory(false)
      .set_long_description("Example::                                              \n"
                            "                                                       \n"
                            "  sensitive.record_kinetic_energy : boolean = 1        \n"
                            "                                                       \n"
                            )
      ;
  }

  {
    // Description of the 'record_momentum' configuration property :
    datatools::configuration_property_description & cpd
      = ocd_.add_property_info();
    cpd.set_name_pattern("sensitive.record_momentum")
      .set_terse_description("Record the start and stop momentum")
      .set_traits(datatools::TYPE_BOOLEAN)
      .set_mandatory(false)
      .set_long_description("Example::                                              \n"
                            "                                                       \n"
                            "  sensitive.record_momentum : boolean = 1              \n"
                            "                                                       \n"
                            )
      ;
  }

  {
    // Description of the 'record_sensitive_category' configuration property :
    datatools::configuration_property_description & cpd
      = ocd_.add_property_info();
    cpd.set_name_pattern("sensitive.record_sensitive_category")
     .set_terse_description("Record the 'sensitive.category' string property")
      .set_traits(datatools::TYPE_BOOLEAN)
      .set_mandatory(false)
      .set_long_description("Example::                                              \n"
                            "                                                       \n"
                            "  sensitive.record_sensitive_category : boolean = 1    \n"
                            "                                                       \n"
                            )
      ;
  }

  {
    // Description of the 'record_g4_volume_infos' configuration property :
    datatools::configuration_property_description & cpd
      = ocd_.add_property_info();
    cpd.set_name_pattern("sensitive.record_g4_volume_infos")
      .set_terse_description("Record the 'g4_volume.name' string and 'g4_volume.copy_number' integer properties")
      .set_traits(datatools::TYPE_BOOLEAN)
      .set_mandatory(false)
      .set_long_description("Example::                                              \n"
                            "                                                       \n"
                            "  sensitive.record_g4_volume_infos : boolean = 1       \n"
                            "                                                       \n"
                            )
      ;
  }

  {
    // Description of the 'hits_buffer_capacity' configuration property :
    datatools::configuration_property_description & cpd
      = ocd_.add_property_info();
    cpd.set_name_pattern("sensitive.hits_buffer_capacity")
      .set_terse_description("Setup the initial capacity of the sensitive hit buffer for memory optimization")
      .set_traits(datatools::TYPE_INTEGER)
      .set_mandatory(false)
      .set_long_description("This property pre-allocate a buffer of hits from which \n"
                            "truth/step hits associated to the sensitive detector   \n"
                            "will be instantiated. This mechanism is supposed to    \n"
                            "fasten the simulation, particularly when many hits are \n"
                            "generated along a track within a given sensitive volume.\n"
                            "The buffer is resused in each event, so memory allocation\n"
                            "is done at the very first simulated event.             \n"
                            "                                                       \n"
                            "Example::                                              \n"
                            "                                                       \n"
                            "  sensitive.hits_buffer_capacity : integer = 100       \n"
                            "                                                       \n"
                            )
      ;
  }


  {
    // Description of the 'record_boundaries' configuration property :
    datatools::configuration_property_description & cpd
      = ocd_.add_property_info();
    cpd.set_name_pattern("sensitive.record_boundaries")
      .set_terse_description("Record the 'track.entering_volume' or 'track.leaving_volume' flags")
      .set_traits(datatools::TYPE_BOOLEAN)
      .set_mandatory(false)
      .set_long_description("Example::                                              \n"
                            "                                                       \n"
                            "  sensitive.record_boundaries : boolean = 1            \n"
                            "                                                       \n"
                            )
      ;
  }


  {
    // Description of the 'record_step_length' configuration property :
    datatools::configuration_property_description & cpd
      = ocd_.add_property_info();
    cpd.set_name_pattern("sensitive.record_step_length")
      .set_terse_description("Record the 'track.step_length' real property")
      .set_traits(datatools::TYPE_BOOLEAN)
      .set_mandatory(false)
      .set_long_description("Example::                                              \n"
                            "                                                       \n"
                            "  sensitive.record_step_length : boolean = 1           \n"
                            "                                                       \n"
                            )
      ;
  }

  // Additionnal configuration hints :
  ocd_.set_configuration_hints("Typical configuration is::                                             \n"
                               "                                                                       \n"
                               "  logging.priority                     : string = \"warning\"          \n"
                               "  sensitive.hits_buffer_capacity       : integer = 100                 \n"
                               "  sensitive.record_primary_particle    : boolean = 1                   \n"
                               "  sensitive.record_track_id            : boolean = 1                   \n"
                               "  sensitive.record_alpha_quenching     : boolean = 1                   \n"
                               "  sensitive.record_major_track         : boolean = 1                   \n"
                               "  sensitive.major_track_minimum_energy : real as energy = 25 keV       \n"
                               "  sensitive.record_creator_process     : boolean = 1                   \n"
                               "  sensitive.record_material            : boolean = 1                   \n"
                               "  sensitive.record_kinetic_energy      : boolean = 1                   \n"
                               "  sensitive.record_momentum            : boolean = 1                   \n"
                               "  sensitive.record_sensitive_category  : boolean = 1                   \n"
                               "  sensitive.record_g4_volume_infos     : boolean = 1                   \n"
                               "  sensitive.record_step_length         : boolean = 1                   \n"
                               "  sensitive.record_boundaries          : boolean = 1                   \n"
                               "                                                                       \n"
                               );

  ocd_.set_validation_support(true);

  // Lock the description:
  ocd_.lock();

  // ... and we are done.
  return;
}
DOCD_CLASS_IMPLEMENT_LOAD_END() // Closing macro for implementation

// Registration macro for class 'mctools::g4::sensitive_detector' :
DOCD_CLASS_SYSTEM_REGISTRATION(mctools::g4::sensitive_detector,"mctools::g4::sensitive_detector")
