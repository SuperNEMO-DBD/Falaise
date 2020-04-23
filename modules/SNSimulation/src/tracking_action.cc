// tracking_action.cc

// Standard library:
#include <cstdlib>
#include <stdexcept>
#include <iostream>
#include <sstream>

// Third party:
// - Bayeux/datatools:
#include <datatools/properties.h>
// - Bayeux/mctools:
#include <mctools/g4/tracking_action.h>
// - Geant4:
#include <globals.hh>

// For C++11 compatibility, remove once Bayeux is C++11 Geant4 10.2 only
#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-register"
#endif
#include <G4VPhysicalVolume.hh>
#include <G4TrackingManager.hh>
#ifdef __clang__
#pragma clang diagnostic pop
#endif

#include <G4Track.hh>
#include <G4UnitsTable.hh>
#include <G4Step.hh>

namespace mctools {

  namespace g4 {

    tracking_action::tracking_action()
    {
      return;
    }

    tracking_action::~tracking_action()
    {
      return;
    }

    void tracking_action::initialize(const datatools::properties & config_)
    {
      // Parsing configuration properties:
      loggable_support::_initialize_logging_support(config_);
      return;
    }

    void tracking_action::PreUserTrackingAction(const G4Track * /*track_*/)
    {
      /*
        G4int track_id     = track_->GetTrackID();
        G4int mother_id    = track_->GetParentID();
        G4String part_name = track_->GetDefinition()->GetParticleName();
        std::string volume = track_->GetVolume()->GetName();
        CLHEP::Hep3Vector position = track_->GetPosition();
      */

      return;
    }

    void tracking_action::PostUserTrackingAction(const G4Track* /*track_*/)
    {
      //G4int             track_id = track_->GetTrackID();
      //CLHEP::Hep3Vector position = track_->GetPosition();

      return;
    }

  } // end of namespace g4

} // end of namespace mctools

/** Opening macro for implementation
 *  This macro must be used outside of any namespace.
 */
DOCD_CLASS_IMPLEMENT_LOAD_BEGIN(mctools::g4::tracking_action, ocd_)
{
  // The class name :
  ocd_.set_class_name("mctools::g4::tracking_action");

  // The class terse description :
  ocd_.set_class_description("The Geant4 simulation optional tracking action");

  // The library the class belongs to :
  ocd_.set_class_library("mctools_geant4");

  // The class detailed documentation :
  ocd_.set_class_documentation("This is Geant4 simulation engine embedded tracking action.\n"
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

  // Additionnal configuration hints :
  ocd_.set_configuration_hints("Typical configuration is::                          \n"
                               "                                                    \n"
                               " #@description Stacking action logging priority     \n"
                               " logging.priority : string = \"warning\"            \n"
                               "                                                    \n"
                               );

  ocd_.set_validation_support(true);

  // Lock the description:
  ocd_.lock();

  // ... and we are done.
  return;
}
DOCD_CLASS_IMPLEMENT_LOAD_END() // Closing macro for implementation

// Registration macro for class 'mctools::g4::manager' :
DOCD_CLASS_SYSTEM_REGISTRATION(mctools::g4::tracking_action,"mctools::g4::tracking_action")
