/// \file mctools/g4/detector_construction.h
/* Author(s) :  Benoit Guillon <guillon@lpccaen.in2p3.fr>
 *              François Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-05-04
 * Last modified: 2015-09-06
 *
 * License:
 *
 * Description:
 *
 *   GEANT 4 detector construction interface
 *
 * History:
 *
 */

#ifndef MCTOOLS_G4_DETECTOR_CONSTRUCTION_H
#define MCTOOLS_G4_DETECTOR_CONSTRUCTION_H 1

// Standard library:
#include <map>
#include <string>
#include <vector>

// Third party:
// - Boost :
#include <boost/scoped_ptr.hpp>
// - Bayeux/datatools :
#include <datatools/multi_properties.h>
#include <datatools/properties.h>
// - Bayeux/mygsl :
#include <mygsl/rng.h>
// G4 stuff:
#include <G4VUserDetectorConstruction.hh>

// This project:
#include <mctools/g4/loggable_support.h>
#include <mctools/g4/region_tools.h>
#include <mctools/step_hit_processor_factory.h>

class G4UserLimits;
class G4VisAttributes;

namespace geomtools {
class manager;
class gdml_writer;
}  // namespace geomtools

namespace emfield {
class electromagnetic_field_manager;
class base_electromagnetic_field;
class emfield_geom_plugin;
}  // namespace emfield

namespace mctools {

namespace g4 {

// Forward declarations:
class manager;
class sensitive_detector;
class magnetic_field;
class electromagnetic_field;
class biasing_manager;
class em_field_g4_stuff;

/// \brief The detector construction Geant4 interface class
class detector_construction : public G4VUserDetectorConstruction, public loggable_support {
 public:
  typedef std::map<std::string, sensitive_detector *> sensitive_detector_dict_type;
  typedef mctools::step_hit_processor_factory SHPF_type;

  /// Default control distance for particle tracking in magnetic field
  static const double DEFAULT_MISS_DISTANCE;

  /// Constructor
  detector_construction();

  /// Constructor
  detector_construction(manager &mgr_);

  /// Destructor
  virtual ~detector_construction();

  /// Check initialization flag
  bool is_initialized() const;

  /// Return a non-mutable reference to the simulation manager
  const manager &get_manager() const;

  /// Return a mutable reference to the simulation manager
  manager &grab_manager();

  /// Check for a referenced geometry manager
  bool has_geometry_manager() const;

  /// Set the reference to a geometry manager
  void set_geometry_manager(const geomtools::manager &gm_);

  /// Return a non-mutable reference to the geometry manager
  const geomtools::manager &get_geometry_manager() const;

  /// Check for a referenced magnetic field manager
  bool has_mag_field_manager() const;

  /// Set the reference to a magnetic field manager
  void set_mag_field_manager(const emfield::electromagnetic_field_manager &);

  /// Return a non-mutable reference to the magnetic field manager
  const emfield::electromagnetic_field_manager &get_mag_field_manager() const;

  /// Check for a referenced electromagnetic field manager
  bool has_em_field_manager() const;

  /// Set the reference to a electromagnetic field manager
  void set_em_field_manager(const emfield::electromagnetic_field_manager &);

  /// Return a non-mutable reference to the electromagnetic field manager
  const emfield::electromagnetic_field_manager &get_em_field_manager() const;

  /// Main initialization method
  void initialize(const datatools::properties &config_);

  /// Main reset method
  void reset();

  /// Return a non-mutable reference to the collection of embedded sensitive detectors
  const sensitive_detector_dict_type &get_sensitive_detectors() const;

  /// Return a mutable reference to the embeded step hit processor factory
  SHPF_type &grab_step_hit_processor_factory();

  void set_materials_geom_plugin_name(const std::string &mpn_);

  void set_emfield_geom_plugin_name(const std::string &fpn_);

  /// G4 interface
  G4VPhysicalVolume *Construct();

  /** Generate the GDML file from the geometry manager */
  void write_tmp_gdml_file();

 protected:
  /** This method automatically creates and setups G4 sensitive detectors
   * with associated collections of simulated step hits.
   *
   * Two sources of informations are used:
   *
   *  - the geometry model (using the model factory from the geometry manager)
   *    provides some associations between logical volumes and
   *    sensitive detector categories
   *    These rules for sensitive detector definition are considered
   *    as 'official' for they are implemented in the main
   *    geometry model.
   *
   *  - the step hit processor factory (aka SHPF) enables to
   *    add some additionnal (non-official) sensitive detectors
   *
   */
  void _construct_sensitive_detectors();

  /** Construct regions */
  void _construct_regions();

  /** Construct biasing */
  void _construct_biasing();

  /** Setup electromagnetic field */
  void _construct_electromagnetic_field();

  /** Destroy electromagnetic field */
  void _destroy_electromagnetic_field();

  /** This method automatically setup G4 visualization attributes
   * from the main geometry model.
   */
  void _set_visualization_attributes();

  /** Setup user limits */
  void _set_user_limits();

  /** Hidden construction method */
  virtual G4VPhysicalVolume *_g4_construct();

 protected:
  void _set_default();

 private:
  bool _initialized_ = false;  //!< Initialization flag
  bool _abort_on_error_ =
      true;  //!< Flag to force abortion on error while building the geometry setup

  //! Main Geant4 manager:
  manager *_g4_manager_ = nullptr;  //!< Handle to the Geant4 simulation manager

  //! Geometry manager:
  const geomtools::manager *_geom_manager_ = nullptr;  //!< Reference to the geometry manager

  //! Magnetic field manager:
  bool _using_em_field_ = false;           //!< Flag to use the Geant4 EM field system
  std::string _emfield_geom_plugin_name_;  //!< The name of the EM field manager geometry plugin
  const emfield::electromagnetic_field_manager *_em_field_manager_ =
      nullptr;                           //!< Reference to the EM field manager
  datatools::properties _em_field_aux_;  //!< Auxiliary properties related to EM field
  double _miss_distance_unit_;           //!< Default miss distance length unit
  double _general_miss_distance_;        //!< Default general miss distance

  //! \brief Type alias for dictionary of G4 EM field working data
  typedef std::map<std::string, em_field_g4_stuff *> em_field_g4_data_type;
  em_field_g4_data_type _em_field_g4_data_;  //!< Dictionary of G4 EM field working data

  //! User limits:
  bool _using_user_limits_ = true;
  datatools::properties _limits_config_;
  std::list<G4UserLimits *> _user_limits_col_;

  // GDML stuff:
  bool _generate_gdml_file_;
  std::string _materials_geom_plugin_name_;
  std::string _gdml_file_dir_;
  std::string _gdml_schema_;
  bool _gdml_validation_;
  std::string _gdml_filename_;

  // Visualization:
  bool _using_vis_attributes_ = true;
  std::map<std::string, G4VisAttributes *> _vis_attributes_;

  // Sensitive detectors:
  bool _using_sensitive_detectors_ = true;
  sensitive_detector_dict_type _sensitive_detectors_;
  datatools::multi_properties _SD_params_;

  // Regions:
  bool _using_regions_ = true;
  regions_setup _region_definitions_;

  // Step hit processor factory stuff (from mctools::step_hit_processor_factory) :
  datatools::properties _SHPF_config_;
  SHPF_type _SHPF_;

  bool _using_biasing_ = false;            //!< Flag to use the Geant4 biasing system
  datatools::properties _biasing_config_;  //!< Configuration properties for the biasing manager
  boost::scoped_ptr<biasing_manager> _biasing_manager_;  //!< Biaising manager
};

}  // end of namespace g4

}  // end of namespace mctools

/// OCD support : interface
#include <datatools/ocd_macros.h>
DOCD_CLASS_DECLARATION(mctools::g4::detector_construction)

#endif  // MCTOOLS_G4_DETECTOR_CONSTRUCTION_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
