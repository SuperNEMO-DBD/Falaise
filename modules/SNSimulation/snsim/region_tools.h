// \file snsim/region_tools.h
/* Author(s) :  François Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2015-09-05
 * Last modified: 2015-09-05
 *
 * License:
 *
 * Description:
 *
 *   Tools to define regions in Geant4
 *
 * History:
 *
 */

#ifndef SNSIM_REGION_TOOLS_H
#define SNSIM_REGION_TOOLS_H

// Standard library:
#include <iostream>
#include <map>
#include <set>
#include <string>

// Third party:
// - Bayeux/datatools :
#include <bayeux/datatools/command_utils.h>
#include <bayeux/datatools/enriched_base.h>
#include <bayeux/datatools/i_tree_dump.h>
#include <bayeux/datatools/properties.h>
// - Bayeux/geomtools :
#include <bayeux/geomtools/manager.h>

namespace snsim {

/// \brief Description of a region
///
/// A Geant4  region can be  defined by  the explicit list  of its
/// logical volumes.   It is  also possible to  specify a  list of
/// materials of which the logical  volumes in the region are made
/// of and also asked for materials with a given range of density,
/// a  specific state  (solid/liquid/gas).  Finally  one can  also
/// explicitely remove some logical volumes  from a region if they
/// have been  initially added  in the  region by  the definitions
/// above.  This allows  to avoid  region collision  when a  given
/// logical volume  is accidentally set in  two different regions,
/// which is forbidden in Geant4.
class region_info : public datatools::enriched_base {
 public:
  /// Default constructor
  region_info();

  /// Destructor
  virtual ~region_info();

  /// Check initialization flag
  bool is_initialized() const;

  /// Reset
  void reset();

  /// Initialize
  void initialize(const datatools::properties &, const geomtools::manager *geom_manager_ = 0);

  /// Check if the region is active (has effective logical volumes)
  bool is_active() const;

  /// Add a logical volume in the region
  void add_logical_volume(const std::string &logical_id_, bool allow_duplicate_ = false);

  /// Remove a logical volume from the region
  void remove_logical_volume(const std::string &logical_id_);

  /// Check if a logical volume belongs to the region given its name
  bool has_logical_volume(const std::string &logical_id_) const;

  /// Return the logical volumes identifiers
  const std::set<std::string> &get_logical_ids() const;

  /// Return the logical volumes identifiers
  std::set<std::string> &grab_logical_ids();

  /// Smart print
  virtual void tree_dump(std::ostream &out_ = std::clog, const std::string &title_ = "",
                         const std::string &indent_ = "", bool inherit_ = false) const;

 private:
  bool _initialized_;                   ///< Initialization flag
  std::set<std::string> _logical_ids_;  ///< Set of logical volumes' names associated to the region
};

/// \brief A dictionary of region description data
typedef std::map<std::string, region_info> region_infos_dict_type;

/// \brief The region setup
class regions_setup : public datatools::i_tree_dumpable {
 public:
  /// Default constructor
  regions_setup();

  /// Destructor
  ~regions_setup();

  /// Initialize
  void initialize(const datatools::properties &);

  /// Reset
  void reset();

  /// Set the reference to the geometry manager
  void set_geometry_manager(const geomtools::manager &);

  /// Return the const reference to the geometry manager
  const geomtools::manager &get_geometry_manager() const;

  /// Return a const reference to the dictionary of region descriptions
  const region_infos_dict_type &get_region_infos() const;

  /// Return a mutable reference to the dictionary of region descriptions
  region_infos_dict_type &grab_region_infos();

  /// Check if a region exists given its name
  bool has_region(const std::string &region_id_) const;

  /// Smart print
  virtual void tree_dump(std::ostream &out_ = std::clog, const std::string &title_ = "",
                         const std::string &indent_ = "", bool inherit_ = false) const;

  // Validate for Geant4...
  // static bool validate_for_geant4(const region_infos_dict_type & region_infos_);

 protected:
  /// Check the validity of the regions setup
  datatools::command::returned_info _check();

 private:
  const geomtools::manager *_geom_manager_;  ///< Handle to an external geometry manager
  region_infos_dict_type _region_infos_;     ///< Dictionary of region descriptions
};

/* Future: add arbitrary user info attached to a region.
/// \brief Base class for user region information data
class base_region_user_info : public G4VUserRegionInformation
{
};
*/

}  // namespace snsim

/// OCD support : interface
#include <bayeux/datatools/ocd_macros.h>
DOCD_CLASS_DECLARATION(snsim::region_info)

#endif  // SNSIM_REGION_TOOLS_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
