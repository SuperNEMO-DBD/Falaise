// stepping_action.cc

// Ourselves:
#include <snsim/stepping_action.h>

// Standard library:
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <stdexcept>

// Third party:
// - Bayeux/datatools:
#include <bayeux/datatools/ioutils.h>
#include <bayeux/datatools/properties.h>
// - Geant4:
#include <globals.hh>
// For C++11 compatibility, remove once Bayeux is C++11 Geant4 10.2 only
#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-register"
#endif
#include "G4SteppingManager.hh"
#ifdef __clang__
#pragma clang diagnostic pop
#endif

#include "G4NavigationHistory.hh"
#include "G4ReferenceCountedHandle.hh"
#include "G4UnitsTable.hh"
#include "G4VTouchable.hh"

namespace snsim {

stepping_action::stepping_action() {
  _dumped_ = false;
  _number_of_steps_ = 0;
}

stepping_action::~stepping_action() {
  // clog << "NOTICE: " << "snemo::g4::stepping_action::~stepping_action: "
  //      << "Number of steps = "
  //      << _number_of_steps_
  //      << endl;
  reset();
}

void stepping_action::set_dumped(bool d_) { _dumped_ = d_; }

auto stepping_action::is_dumped() const -> bool { return _dumped_; }

void stepping_action::initialize(const datatools::properties& config_) {
  // Parsing configuration properties...
  loggable_support::_initialize_logging_support(config_);

  if (config_.has_key("dumped")) {
    set_dumped(config_.fetch_boolean("dumped"));
  }
}

void stepping_action::reset() { _dumped_ = false; }

void stepping_action::UserSteppingAction(const G4Step* g4_step_) {
  DT_LOG_TRACE_ENTERING(_logprio());
  this->_stepping_action_base(g4_step_);
  DT_LOG_TRACE_EXITING(_logprio());
}

auto stepping_action::g4_track_status_to_label(G4TrackStatus track_status_) -> std::string {
  switch (track_status_) {
    case fAlive:
      return "alive";  // Continue the tracking
    case fStopButAlive:
      return "stop_but_alive";  // Invoke active rest physics processes and
      // and kill the current track afterward
    case fStopAndKill:
      return "stop_and_kill";  // Kill the current track
    case fKillTrackAndSecondaries:
      return "kill_track_and_secondaries";
      // Kill the current track and also associated
      // secondaries.
    case fSuspend:
      return "suspend";  // Suspend the current track
    case fPostponeToNextEvent:
      return "postpone_to_next_event";
      // Postpones the tracking of thecurrent track
      // to the next event.
    default:
      return "";
  }
}

auto stepping_action::g4_step_status_to_label(G4StepStatus step_status_) -> std::string {
  switch (step_status_) {
    case fWorldBoundary:
      return "world_boundary";
      // Step reached the world boundary
    case fGeomBoundary:
      return "geom_boundary";
      // Step defined by a geometry boundary
    case fAtRestDoItProc:
      return "at_rest_do_it_proc";
      // Step defined by a PreStepDoItVector
    case fAlongStepDoItProc:
      return "along_step_do_it_proc";
      // Step defined by a AlongStepDoItVector
    case fPostStepDoItProc:
      return "post_step_do_it_proc";
      // Step defined by a PostStepDoItVector
    case fUserDefinedLimit:
      return "user_defined_limit";
      // Step defined by the user Step limit in the logical volume
    case fExclusivelyForcedProc:
      return "exclusively_forced_proc";
      // Step defined by an exclusively forced PostStepDoIt process
    default:
    case fUndefined:
      return "undefined";
  }
}

auto stepping_action::g4_stepping_control_to_label(G4SteppingControl stepping_control_)
    -> std::string {
  switch (stepping_control_) {
    case NormalCondition:
      return "normal_condition";
    case AvoidHitInvocation:
      return "avoid_hit_invocation_alive";
    case Debug:
      return "debug";
    default:
      return "";
  }
}

void stepping_action::_stepping_action_base(const G4Step* g4_step_) {
  DT_LOG_TRACE(_logprio(), "Entering...");
  _number_of_steps_++;

  bool process_step = false;

  if (_dumped_) {
    process_step = true;
  }

  if (process_step) {
    // Fetch track informations:
    G4Track* track = g4_step_->GetTrack();  // fpSteppingManager->GetTrack();
    G4int current_step_number = track->GetCurrentStepNumber();
    G4int track_id = track->GetTrackID();
    G4int track_parent_id = track->GetParentID();

    const G4DynamicParticle* dynamic_particle = track->GetDynamicParticle();
    G4double dynamic_particle_charge = dynamic_particle->GetCharge();

    const G4ParticleDefinition* particle = track->GetParticleDefinition();
    const G4String& particle_name = particle->GetParticleName();
    const G4ThreeVector& position = track->GetPosition();
    // G4double track_global_time = track->GetGlobalTime();
    // G4double track_local_time  = track->GetLocalTime();
    // G4double track_proper_time = track->GetProperTime();
    // G4VPhysicalVolume * volume = track->GetVolume();
    // G4String volume_name;
    // if (volume) {
    //   volume_name = volume->GetName();
    // }
    // G4VPhysicalVolume * next_volume = track->GetNextVolume();
    // G4String next_volume_name;
    // if (next_volume) {
    //   next_volume_name = next_volume->GetName();
    // }
    // G4ThreeVector momentum     = track->GetMomentum();
    // G4Material * material      = track->GetMaterial();
    // G4Material * next_material = track->GetNextMaterial();
    // G4double kinetic_energy    = track->GetKineticEnergy();
    // G4double total_energy      = track->GetTotalEnergy();
    // G4double velocity          = track->GetVelocity();
    // G4TrackStatus track_status = track->GetTrackStatus();
    // G4bool below_threshold     = track->IsBelowThreshold();
    // G4bool good_for_traking    = track->IsGoodForTracking();
    // G4double track_length      = track->GetTrackLength();
    // G4double track_step_length = track->GetStepLength();
    // G4double track_weight      = track->GetWeight();

    // Fetch step informations:

    // Pre step:
    // const G4ThreeVector & pre_position = pre_step_point->GetPosition();
    // const G4StepPoint * pre_step_point = g4_step_->GetPreStepPoint();
    // const G4VTouchable * pre_step_touchable  = pre_step_point->GetTouchable();
    // const G4ReferenceCountedHandle<G4VTouchable> post_step_touchable_handle
    //   = g4_step_->GetPostStepPoint()->GetTouchableHandle();
    // G4ThreeVector pre_position_t = pre_step_touchable->GetTranslation();

    // Post step:
    // const G4ThreeVector & post_position = post_step_point->GetPosition();
    // const G4StepPoint * post_step_point = g4_step_->GetPostStepPoint();
    // const G4VTouchable * post_step_touchable = post_step_point->GetTouchable();
    // G4ThreeVector post_position_t = post_step_touchable->GetTranslation();
    // G4String process_name         = post_step_point->GetProcessDefinedStep()->GetProcessName();

    // G4double step_length          = g4_step_->GetStepLength();
    // G4double total_energy_deposit = g4_step_->GetTotalEnergyDeposit();
    // G4double non_ionizing_energy_deposit = g4_step_->GetNonIonizingEnergyDeposit();
    // G4SteppingControl stepping_control   = g4_step_->GetControlFlag();
    // G4bool first_step_in_volume   = g4_step_->IsFirstStepInVolume();
    // G4bool last_step_in_volume    = g4_step_->IsLastStepInVolume();
    // G4ThreeVector delta_position  = g4_step_->GetDeltaPosition();
    // G4double delta_time           = g4_step_->GetDeltaTime();
    // G4ThreeVector delta_momentum  = g4_step_->GetDeltaMomentum(); // obsolete
    // G4double delta_energy         = g4_step_->GetDeltaEnergy();   // obsolete

    if (_dumped_) {
      std::cerr << "\n**************** G4 STEP DUMP ******************** " << std::endl;
      std::cerr.precision(15);
      std::cerr << "Current step number : " << current_step_number << std::endl;
      std::cerr << "Track Id            : " << track_id << std::endl;
      std::cerr << "Track parent Id     : " << track_parent_id << std::endl;
      std::cerr << "Dyn. part. charge   : " << dynamic_particle_charge / CLHEP::eplus << " [e]"
                << std::endl;
      std::cerr << "Particle name       : " << particle_name << std::endl;
      std::cerr << "Particle position   : " << position << std::endl;

      std::cerr << "\n************* END OF G4 STEP DUMP **************** " << std::endl;
    }
  }

  DT_LOG_TRACE(_logprio(), "Exiting.");
}

}  // namespace snsim

/** Opening macro for implementation
 *  This macro must be used outside of any namespace.
 */
DOCD_CLASS_IMPLEMENT_LOAD_BEGIN(snsim::stepping_action, ocd_) {
  // The class name :
  ocd_.set_class_name("snsim::stepping_action");

  // The class terse description :
  ocd_.set_class_description("The Geant4 simulation optional stepping action");

  // The library the class belongs to :
  ocd_.set_class_library("mctools_g4");

  // The class detailed documentation :
  ocd_.set_class_documentation("This is Geant4 simulation engine embedded stepping action.\n");

  {
    // Description of the 'logging.priority' configuration property :
    datatools::configuration_property_description& cpd = ocd_.add_property_info();
    cpd.set_name_pattern("logging.priority")
        .set_terse_description("Logging priority threshold")
        .set_traits(datatools::TYPE_STRING)
        .set_mandatory(false)
        .set_long_description(
            "Allowed values are:                                    \n"
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
            "                                                       \n");
  }

  // Additionnal configuration hints :
  ocd_.set_configuration_hints(
      "Typical configuration is::                          \n"
      "                                                    \n"
      " #@description Stacking action logging priority     \n"
      " logging.priority : string = \"warning\"            \n"
      "                                                    \n");

  ocd_.set_validation_support(true);

  // Lock the description:
  ocd_.lock();

  // ... and we are done.
  return;
}
DOCD_CLASS_IMPLEMENT_LOAD_END()  // Closing macro for implementation

// Registration macro for class 'snsim::manager' :
DOCD_CLASS_SYSTEM_REGISTRATION(snsim::stepping_action, "snsim::stepping_action")

/*
// Code sample that may be useful but care must be taken with the
// navigation through the depth of the G4TouchableHistory:

G4TouchableHandle pre_step_touchable_handle = g4_step_->GetPreStepPoint ()->GetTouchableHandle ();
G4VPhysicalVolume * mother_volume = 0;
int depth = pre_step_touchable_handle->GetHistoryDepth();
clog << datatools::io::devel << "stepping_action::_stepping_action_base: "
<< "depth==" << depth << " volume=" << volume_name << endl;
if (depth < 1)
{
clog << datatools::io::devel << "stepping_action::_stepping_action_base: "
<< "depth==0 for " << pre_step_touchable_handle->GetVolume (0)->GetName () << endl;

}
else
{
mother_volume = pre_step_touchable_handle->GetVolume (1);
if (! mother_volume)
{
if (devel)
{
clog << datatools::io::devel << "stepping_action::_stepping_action_base: "
<< "mother_volume==NULL" << endl;
}
}
else
{
G4VPhysicalVolume * grand_mother_volume = 0;
grand_mother_volume = pre_step_touchable_handle->GetVolume (2);
if (! grand_mother_volume)
{
if (devel)
{
clog << datatools::io::devel << "stepping_action::_stepping_action_base: "
<< "grand_mother_volume==NULL" << endl;
}
}
}
G4int some_copy_number = pre_step_touchable_handle->GetCopyNumber (1);
}
*/
