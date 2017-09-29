/// \file falaise/snemo/electronics/cabling.h
/* Author(s): Yves LEMIERE <lemiere@lpccaen.in2p3.fr>,
 *  Guillaume OLIVIERO <goliviero@lpccaen.in2p3.fr>,
 *  Francois MAUGER <mauger@lpccaen.in2p3.fr>
 * Creation date: 2016-01-01
 * Last modified: 2016-12-06
 *
 */

#ifndef FALAISE_SNEMO_ELECTRONICS_CABLING_H
#define FALAISE_SNEMO_ELECTRONICS_CABLING_H

// Standard library :
#include <set>

// - Boost:
#include <boost/bimap.hpp>
#include <boost/cstdint.hpp>

// - Bayeux/geomtools:
#include <bayeux/geomtools/geom_id.h>
#include <bayeux/geomtools/id_mgr.h>
#include <bayeux/geomtools/manager.h>
// - Bayeux/datatools :
#include <bayeux/datatools/logger.h>

// This project
#include <snemo/geometry/calo_locator.h>
#include <snemo/geometry/gg_locator.h>
#include <snemo/geometry/gveto_locator.h>
#include <snemo/geometry/xcalo_locator.h>
// #include <snemo/geometry/ID_convertor.h>

namespace snemo {

namespace electronics {

/// \brief Electronics channel/geometry elements cabling
class cabling {
 public:
  typedef boost::bimap<geomtools::geom_id, geomtools::geom_id> ID_bimap;
  typedef ID_bimap::value_type ID_doublet;

  /// Default constructor
  cabling();

  /// Destructor
  virtual ~cabling();

  /// Return logging priority
  datatools::logger::priority get_logging() const;

  /// Set logging priority
  void set_logging(const datatools::logger::priority p_);

  /// Precontruct the map for a given type
  void add_preconstructed_type(int type_);

  /// Set the geometry manager
  void set_geo_manager(const geomtools::manager& mgr_);

  /// Check if the geometry manager is set
  bool has_geo_manager() const;

  /// Set the module number
  void set_module_number(unsigned int module_number_);

  /// Check if the module number is set
  bool has_module_number() const;

  /// Check if the object is initialized
  bool is_initialized() const;

  /// Initializing
  void initialize();

  /// Reset the object :
  void reset();

  /// Return the set of supported types
  const std::set<int32_t>& get_supported_geom_types() const;

  /// Convert geometric ID into electronic ID
  void convert_GID_to_EID(const geomtools::geom_id& geom_id_,
                          geomtools::geom_id& electronic_id_) const;

  /// Convert electronic ID into geometric ID
  void convert_EID_to_GID(const geomtools::geom_id& electronic_id_,
                          geomtools::geom_id& geom_id_) const;

 protected:
  /// Initializing
  void _initialize();

  /// Construct supported geom types
  void _init_supported_geom_types();

  /// Construct the bimap for geiger category type
  void _init_geiger();

  /// Construct the bimap for main calorimeters category type
  void _init_mcalo();

  /// Construct the bimap for x-wall calorimeters category type
  void _init_xcalo();

  /// Construct the bimap for gamma-veto calorimeters category type
  void _init_gveto();

  /// Protected convert geometric ID into electronic ID
  void _convert_GID_to_EID(const geomtools::geom_id& geom_id_, geomtools::geom_id& electronic_id_);

  /// Protected convert electronic ID into geometric ID
  void _convert_EID_to_GID(const geomtools::geom_id& electronic_id_, geomtools::geom_id& geom_id_);

  /// Set default values for attributes
  void _set_defaults();

 private:
  // Management:
  bool _initialized_ = false;  //!< Initialization flag

  // External resources:
  const geomtools::manager* _geo_manager_ = nullptr;  //!< Eternal geom manager

  // Configuration parameters:
  datatools::logger::priority _logging_ = datatools::logger::PRIO_FATAL;
  std::set<int32_t> _supported_geom_types_;
  unsigned int _module_number_;  //!< Module number (demonstrator module number for the moment)

  // Working internal data/resources:
  std::set<int32_t> _pre_constructed_types_;  //!< Set of pre constructed types ("geiger", "mcalo",
                                              //!< "xcalo", "gveto" are supported)
  const geomtools::id_mgr::category_info* _mcalo_cat_info_ = nullptr;
  const geomtools::id_mgr::category_info* _xcalo_cat_info_ = nullptr;
  const geomtools::id_mgr::category_info* _gveto_cat_info_ = nullptr;
  const geomtools::id_mgr::category_info* _gg_cat_info_ = nullptr;
  const snemo::geometry::calo_locator* _mcalo_locator_ = nullptr;
  const snemo::geometry::gveto_locator* _gveto_locator_ = nullptr;
  const snemo::geometry::xcalo_locator* _xcalo_locator_ = nullptr;
  const snemo::geometry::gg_locator* _gg_locator_ = nullptr;

  // ID_convertor _ID_convertor_; //!< ID convertor (GID to EID)
  ID_bimap _geiger_id_bimap_;  //!< Bidirectionnal map between geiger cells geom ID and
                               //!< corresponding electronic channel ID
  ID_bimap _mcalo_id_bimap_;   //!< Bidirectionnal map between main calorimeter geom ID and
                               //!< corresponding electronic channel ID
  ID_bimap _xcalo_id_bimap_;   //!< Bidirectionnal map between x-wall calorimeter geom ID and
                               //!< corresponding electronic channel ID
  ID_bimap _gveto_id_bimap_;   //!< Bidirectionnal map between gamma-veto geom ID and corresponding
                               //!< electronic channel ID
};

}  // namespace electronics

}  // end of namespace snemo

#endif  // FALAISE_SNEMO_ELECTRONICS_CABLING_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
