// stacking_action.cc

// Standard library:
#include <cstdlib>
#include <stdexcept>
#include <iostream>
#include <sstream>

// Third party:
// - Boost:
#include <boost/algorithm/string/replace.hpp>
// - Bayeux/datatools:
#include <datatools/properties.h>
#include <datatools/exception.h>
// - Geant4:
#include <globals.hh>

// For C++11 compatibility, remove once Bayeux is C++11 Geant4 10.2 only
#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-register"
#endif
#include <G4TransportationManager.hh>
#ifdef __clang__
#pragma clang diagnostic pop
#endif

#include <G4Track.hh>
#include <G4VPhysicalVolume.hh>
#include <G4ThreeVector.hh>
#include <G4Navigator.hh>
#include <G4LogicalVolumeStore.hh>

// This project:
#include <mctools/g4/stacking_action.h>

namespace mctools {

  namespace g4 {

    stacking_action::stacking_action ()
    {
      _g4_navigator_ = 0;
      _kill_secondary_particles_ = false;
      return;
    }

    stacking_action::~stacking_action ()
    {
      _g4_navigator_ = 0;
      return;
    }

    void stacking_action::initialize (const datatools::properties & config_)
    {
      // parsing configuration properties...
      loggable_support::_initialize_logging_support(config_);

      if (config_.has_key ("kill_particles")) {
        _kill_secondary_particles_ = true;

        if (config_.has_key ("kill_particles.volumes")) {
          config_.fetch ("kill_particles.volumes", _killer_volume_names_);
        }

        if (config_.has_key ("kill_particles.materials")) {
          config_.fetch ("kill_particles.materials", _killer_material_names_);
        }

      }

      _g4_navigator_ = G4TransportationManager::GetTransportationManager ()->GetNavigatorForTracking ();
      return;
    }

    G4ClassificationOfNewTrack stacking_action::ClassifyNewTrack (const G4Track * a_track)
    {
      DT_LOG_TRACE(_logprio(), "Entering...");

      G4ClassificationOfNewTrack classification = fUrgent;

      // If the vector of `secondary particle killer logical volumes' is still empty
      // we fill it :
      if ((_killer_volume_names_.size () > 0) && (_killer_volumes_.size () == 0)) {
        G4LogicalVolumeStore * g4_lv_store = G4LogicalVolumeStore::GetInstance ();
        for (int i = 0; i < (int) _killer_volume_names_.size (); i++) {
          std::ostringstream added_volume_name;
          added_volume_name << _killer_volume_names_[i];
          G4String g4_log_name = added_volume_name.str ().c_str ();
          G4LogicalVolume * g4_volume_log = g4_lv_store->GetVolume (g4_log_name, false);
          DT_THROW_IF (g4_volume_log == 0, std::logic_error,"Logical volume '" << g4_log_name << "' does not exists !");
          DT_LOG_NOTICE(_logprio(), "Add a secondary particle killer logical volume '" << g4_log_name << "'.");
          _killer_volumes_.push_back (g4_volume_log);
        }
      }

      if ((_killer_material_names_.size () > 0) && (_killer_materials_.size () == 0)) {
        //G4LogicalVolumeStore * g4_lv_store = G4LogicalVolumeStore::GetInstance ();
        for (int i = 0; i < (int) _killer_material_names_.size (); i++) {
          std::ostringstream added_material_name;
          added_material_name << _killer_material_names_[i];
          std::string stmp = added_material_name.str ();
          boost::algorithm::replace_all (stmp, ":", "_"); // a trick for GDML does not support ':' in (XML) names.
          G4String g4_material_name = stmp.c_str ();
          G4Material * g4_material = G4Material::GetMaterial (g4_material_name, false);
          DT_THROW_IF (g4_material == 0, std::logic_error, "Material '" << g4_material_name << "' does not exists !");
          DT_LOG_NOTICE(_logprio(), "Add a secondary particle killer material '" << g4_material_name << "'");
          _killer_materials_.push_back (g4_material);
        }
      }


      //string g4_volume_name;
      //g4_volume_name = _g4_navigator_->LocateGlobalPointAndSetup (pos, NULL, false)->GetName ();

      // Another possibility to get the volume name:
      //   int depth=0;
      //   G4TouchableHistory * theTouchable = (G4TouchableHistory*) (a_track->GetTouchable ());
      //   if (theTouchable)
      //     {
      //       depth = theTouchable->GetHistoryDepth ();
      //     }
      //   if (depth > 1)
      //     {
      //       volume_name = theTouchable->GetVolume ()->GetName ();
      //       clog << volume_name << G4endl;
      //     }

      bool kill = false;
      if (_kill_secondary_particles_ && ( a_track->GetParentID () > 0 )) {
        G4ThreeVector pos = a_track->GetPosition ();
        G4VPhysicalVolume * g4_volume_phys = _g4_navigator_->LocateGlobalPointAndSetup (pos, NULL, false);
        G4LogicalVolume * g4_volume_log = g4_volume_phys->GetLogicalVolume ();
        // std::cerr << "DEVEL: mctools::g4::stacking::ClassifyNewTrack: g4_volume_log = '"
        //        << g4_volume_log->GetName () << "'" << endl;

        // Kill by material :
        if (! kill) {
          if (g4_volume_log != 0) {
            G4Material * g4_material = g4_volume_log->GetMaterial();
            if (g4_material != 0) {
              // Killing every particle generated in some specific materials :
              if (find (_killer_materials_.begin (),
                        _killer_materials_.end (),
                        g4_material) != _killer_materials_.end ()) {
                kill = true;
                /*
                  clog << "WARNING: stacking_action::ClassifyNewTrack: "
                  << "New secondary particle '"
                  << a_track->GetDefinition()->GetParticleName ()
                  << "' in material '" << g4_material->GetName ()
                  << "' is killed by SNG4 to speed up." << endl;
                */
              }
            } else {
              /*
                clog << "WARNING: stacking_action::ClassifyNewTrack: "
                << "No step is available from this new track !" << endl;
              */
            }
          }
        }

        // Kill by volume :
        if (! kill) {
          if (g4_volume_log != 0) {
            // Killing every particle generated in some specific volumes :
            if (find (_killer_volumes_.begin (),
                      _killer_volumes_.end (),
                      g4_volume_log) != _killer_volumes_.end ()) {
              kill = true;
              /*
                clog << "WARNING: stacking_action::ClassifyNewTrack: "
                << "New secondary particle '"
                << a_track->GetDefinition()->GetParticleName ()
                << "' in logical volume '" << g4_volume_log->GetName ()
                << "' is killed by SNG4 to speed up." << endl;
              */
            }
          }
        }

      }
      if (kill) {
        classification = fKill;
      }

      DT_LOG_TRACE(_logprio(), "Exiting...");
      return classification;
    }

    void stacking_action::NewStage ()
    {
      return;
    }

    void stacking_action::PrepareNewEvent ()
    {
      return;
    }

  } // end of namespace g4

} // end of namespace mctools


/** Opening macro for implementation
 *  This macro must be used outside of any namespace.
 */
DOCD_CLASS_IMPLEMENT_LOAD_BEGIN(mctools::g4::stacking_action,ocd_)
{
  // The class name :
  ocd_.set_class_name ("mctools::g4::stacking_action");

  // The class terse description :
  ocd_.set_class_description ("The Geant4 simulation optional stacking action");

  // The library the class belongs to :
  ocd_.set_class_library ("mctools_g4");

  // The class detailed documentation :
  ocd_.set_class_documentation ("This is Geant4 simulation engine embedded stacking action.      \n"
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
    // Description of the 'kill_particles' configuration property :
    datatools::configuration_property_description & cpd
      = ocd_.add_property_info();
    cpd.set_name_pattern("kill_particles")
      .set_terse_description("Flag to activate secondary particle killing")
      .set_traits(datatools::TYPE_BOOLEAN)
      .set_mandatory(false)
      .set_long_description("Default value: ``0``            \n"
                            "                                \n"
                            "Example::                       \n"
                            "                                \n"
                            "  kill_particles : boolean = 1  \n"
                            "                                \n"
                            )
      ;
  }

  {
    // Description of the 'kill_particles.volumes' configuration property :
    datatools::configuration_property_description & cpd
      = ocd_.add_property_info();
    cpd.set_name_pattern("kill_particles.volumes")
      .set_terse_description("The list of logical volumes where secondary particle killing is activated")
      .set_traits(datatools::TYPE_STRING,
                  configuration_property_description::ARRAY)
      .set_mandatory(false)
      .set_triggered_by_flag("kill_particles")
      .set_long_description("Default value: empty            \n"
                            "                                \n"
                            "Example::                       \n"
                            "                                \n"
                            " kill_particles.volumes : string[2] = \"calorimeter.log\" \"beam_dump.log\"  \n"
                            "                                \n"
                            )
      ;
  }

  {
    // Description of the 'kill_particles.materials' configuration property :
    datatools::configuration_property_description & cpd
      = ocd_.add_property_info();
    cpd.set_name_pattern("kill_particles.materials")
      .set_terse_description("The list of materials of which logical volumes are made and where secondary particle killing is activated")
      .set_traits(datatools::TYPE_STRING,
                  configuration_property_description::ARRAY)
      .set_mandatory(false)
      .set_triggered_by_flag("kill_particles")
      .set_long_description("Default value: empty            \n"
                            "                                \n"
                            "Example::                       \n"
                            "                                \n"
                            " kill_particles.materials : string[2] = \"iron\" \"water\"  \n"
                            "                                \n"
                            )
      ;
  }

  // Additionnal configuration hints :
  ocd_.set_configuration_hints("Typical configuration is::                                             \n"
                               "                                                                       \n"
                               " #@description Stacking action logging priority                        \n"
                               " logging.priority : string = \"warning\"                               \n"
                               "                                                                       \n"
                               " #@description Activate the killing of secondary particles within some volumes \n"
                               " kill_particles : boolean = 1                                          \n"
                               "                                                                       \n"
                               " #@description Kill secondary particles within some given volumes      \n"
                               " kill_particles.volumes : string[1] = \"scintillator_block.model.log\" \n"
                               "                                                                       \n"
                               " #@description Kill secondary particles within some given materials    \n"
                               " kill_particles.materials : string[2] = \"gas\" \"water\"              \n"
                               "                                                                       \n"
                               );

  ocd_.set_validation_support(true);

  // Lock the description:
  ocd_.lock();

  // ... and we are done.
  return;
}
DOCD_CLASS_IMPLEMENT_LOAD_END() // Closing macro for implementation

// Registration macro for class 'mctools::g4::manager' :
DOCD_CLASS_SYSTEM_REGISTRATION(mctools::g4::stacking_action,"mctools::g4::stacking_action")
