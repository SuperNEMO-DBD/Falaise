// \file falaise/snemo/reconstruction/calorimeter_association_driver.cc

// Ourselves:
#include <ChargedParticleTracking/calorimeter_association_driver.h>

// Standard library:
#include <sstream>

// Third party:
// - Bayeux/datatools:
#include <datatools/properties.h>
#include <datatools/units.h>
// - Bayeux/geomtools
#include <geomtools/manager.h>

// This project (Falaise):
#include <falaise/quantity.h>

#include <falaise/snemo/datamodels/particle_track.h>
#include <falaise/snemo/geometry/calo_locator.h>
#include <falaise/snemo/geometry/gveto_locator.h>
#include <falaise/snemo/geometry/locator_helpers.h>
#include <falaise/snemo/geometry/locator_plugin.h>
#include <falaise/snemo/geometry/xcalo_locator.h>

namespace snemo {

namespace reconstruction {

const std::string& calorimeter_utils::associated_flag() {
  static const std::string s("__associated");
  return s;
}

const std::string& calorimeter_utils::isolated_flag() {
  static const std::string s("__isolated");
  return s;
}

const std::string& calorimeter_utils::neighbor_flag() {
  static const std::string s("__neighbour");
  return s;
}

void calorimeter_utils::flag_as(const snemo::datamodel::calibrated_calorimeter_hit& hit_,
                                const std::string& flag_) {
  auto& mutable_hit = const_cast<snemo::datamodel::calibrated_calorimeter_hit&>(hit_);
  mutable_hit.grab_auxiliaries().update_flag(flag_);
}

bool calorimeter_utils::has_flag(const snemo::datamodel::calibrated_calorimeter_hit& hit_,
                                 const std::string& flag_) {
  return hit_.get_auxiliaries().has_flag(flag_);
}

const std::string& calorimeter_association_driver::get_id() {
  static const std::string s("CAD");
  return s;
}

const geomtools::manager& calorimeter_association_driver::geoManager() const {
  DT_THROW_IF(geoManager_ == nullptr, std::logic_error, "No geometry manager is setup !");
  return *geoManager_;
}

/// Initialize the driver through configuration properties
calorimeter_association_driver::calorimeter_association_driver(const falaise::property_set& ps,
                                                               const geomtools::manager* gm) {
  logPriority_ =
      datatools::logger::get_priority(ps.get<std::string>("logging.priority", "warning"));
  geoManager_ = gm;
  auto lpname = ps.get<std::string>("locator_plugin_name", "");
  geoLocator_ = snemo::geometry::getSNemoLocator(geoManager(), lpname);
  matchTolerance_ = ps.get<falaise::length_t>("matching_tolerance", {50, "mm"})();
}

void calorimeter_association_driver::process(
    const snemo::datamodel::CalorimeterHitHdlCollection& calorimeter_hits_,
    snemo::datamodel::particle_track& particle_) {
  this->_measure_matching_calorimeters_(calorimeter_hits_, particle_);
}

void calorimeter_association_driver::_measure_matching_calorimeters_(
    const snemo::datamodel::CalorimeterHitHdlCollection& calorimeter_hits_,
    snemo::datamodel::particle_track& particle_) {
  namespace snedm = snemo::datamodel;

  if (!particle_.has_vertices()) {
    return;
  }

  // When considering calorimeter hits, there might be some of them that are
  // neighbors. To avoid double count of same calorimeter due to tolerance
  // matching in calorimeter association driver, we first compute the list
  // of neighborhood calorimeter hits and if a vertex matches two of them,
  // we only keep the closest non-associated one.
  //
  //                     |-------
  //                     |      |
  //   particle track 1  |      |
  //  -------------------+      | Here particle track 1 is associated to 2
  //                     |------- calorimeter hits but one is shared with the
  //                     |------- particle 2 : we removed the block already associated
  //                     |      |
  //   particle track 2  |      |
  //  -------------------+      |
  //                     |-------
  //
  // Set the calorimeter locators :
  const snemo::geometry::calo_locator& calo_locator = geoLocator_->caloLocator();
  const snemo::geometry::xcalo_locator& xcalo_locator = geoLocator_->xcaloLocator();
  const snemo::geometry::gveto_locator& gveto_locator = geoLocator_->gvetoLocator();
  // Get a list of neighbouring calorimeter hits
  std::set<geomtools::geom_id> list_of_neighbours;

  for (auto icalo = calorimeter_hits_.begin(); icalo != calorimeter_hits_.end(); ++icalo) {
    const snedm::calibrated_calorimeter_hit& i_calo_hit = icalo->get();
    const geomtools::geom_id& a_current_gid = i_calo_hit.get_geom_id();

    std::vector<geomtools::geom_id> neighbour_ids;
    if (calo_locator.isCaloBlockInThisModule(a_current_gid)) {
      neighbour_ids = calo_locator.getNeighbourGIDs(a_current_gid);
    } else if (xcalo_locator.isCaloBlockInThisModule(a_current_gid)) {
      neighbour_ids = xcalo_locator.getNeighbourGIDs(a_current_gid);
    } else if (gveto_locator.isCaloBlockInThisModule(a_current_gid)) {
      neighbour_ids = gveto_locator.getNeighbourGIDs(a_current_gid);
    }

    for (auto jcalo = std::next(icalo); jcalo != calorimeter_hits_.end(); ++jcalo) {
      const snedm::calibrated_calorimeter_hit& j_calo_hit = jcalo->get();
      const geomtools::geom_id& a_gid = j_calo_hit.get_geom_id();

      if (std::find(neighbour_ids.begin(), neighbour_ids.end(), a_gid) != neighbour_ids.end()) {
        list_of_neighbours.insert(a_gid);
        list_of_neighbours.insert(a_current_gid);
        calorimeter_utils::flag_as(i_calo_hit, calorimeter_utils::neighbor_flag());
        calorimeter_utils::flag_as(j_calo_hit, calorimeter_utils::neighbor_flag());
      }
    }
  }

  // Loop over reconstructed vertices
  snedm::particle_track::vertex_collection_type& the_vertices = particle_.get_vertices();
  for (datatools::handle<geomtools::blur_spot>& a_vertex : the_vertices) {
    // Do not take care of vertex other than the ones on calorimeters
    if (!snedm::particle_track::vertex_is_on_main_calorimeter(*a_vertex) &&
        !snedm::particle_track::vertex_is_on_x_calorimeter(*a_vertex) &&
        !snedm::particle_track::vertex_is_on_gamma_veto(*a_vertex)) {
      continue;
    }

    // Look for matching calorimeters
    using calo_collection_type = std::map<double, snedm::CalorimeterHitHdl>;

    calo_collection_type calo_collection;

    for (const datatools::handle<snedm::calibrated_calorimeter_hit>& a_calo_hit :
         calorimeter_hits_) {
      const geomtools::geom_id& a_current_gid = a_calo_hit->get_geom_id();

      // Getting geometry mapping for parted block
      const geomtools::mapping& the_mapping = geoManager().get_mapping();
      std::vector<geomtools::geom_id> gids;
      the_mapping.compute_matching_geom_id(a_current_gid, gids);

      for (const geomtools::geom_id& a_gid : gids) {
        const geomtools::geom_info* ginfo_ptr = the_mapping.get_geom_info_ptr(a_gid);
        if (ginfo_ptr == nullptr) {
          DT_LOG_WARNING(logPriority_, "Unmapped geom id " << a_gid << "!");
          continue;
        }

        // Tolerance must be understood as 'skin' tolerance so must be
        // multiplied by a factor of 2
        const double tolerance = matchTolerance_;
        if (geomtools::mapping::check_inside(*ginfo_ptr, a_vertex->get_position(), tolerance,
                                             true)) {
          // Compute distance to calorimeter center
          geomtools::vector_3d calo_position = geomtools::invalid_vector_3d();

          if (calo_locator.isCaloBlockInThisModule(a_gid)) {
            calo_position = calo_locator.getBlockPosition(a_gid);
          } else if (xcalo_locator.isCaloBlockInThisModule(a_current_gid)) {
            calo_position = xcalo_locator.getBlockPosition(a_gid);
          } else if (gveto_locator.isCaloBlockInThisModule(a_current_gid)) {
            calo_position = gveto_locator.getBlockPosition(a_gid);
          }
          const double distance = (calo_position - a_vertex->get_position()).mag();
          calo_collection.insert(std::make_pair(distance, a_calo_hit));
        }
      }  // end of calorimeter gids
    }    // end of calorimeter hits

    // 2012-06-15 XG: If no triggered calorimeter has been associated to
    // track, one may try to find one silent calorimeter by using the
    // 'calo_locator' and finding the corresponding calorimeter block. To be
    // continued...
    if (calo_collection.empty()) {
      continue;
    }

    // Keep only closest calorimeter i.e. the one with the smallest distance
    // within calo_collection type
    for (auto& icalo : calo_collection) {
      (icalo.second)->grab_auxiliaries().clean(calorimeter_utils::associated_flag());
    }

    const datatools::handle<snedm::calibrated_calorimeter_hit> a_calo =
        calo_collection.begin()->second;
    calorimeter_utils::flag_as(*a_calo, calorimeter_utils::associated_flag());

    // Store hit in particle, set vertex gid
    particle_.get_associated_calorimeter_hits().push_back(a_calo);
    a_vertex->set_geom_id(a_calo->get_geom_id());
  }  // end of vertices
}

// static
//
void calorimeter_association_driver::init_ocd(datatools::object_configuration_description& ocd_) {
  // Prefix "CAD" stands for "Calorimeter Association Driver" :
  datatools::logger::declare_ocd_logging_configuration(ocd_, "fatal", "CAD.");

  {
    // Description of the 'VED.matching_tolerance' configuration property :
    datatools::configuration_property_description& cpd = ocd_.add_property_info();
    cpd.set_name_pattern("VED.matching_tolerance")
        .set_from("snemo::reconstruction::calorimeter_association_driver")
        .set_terse_description("Set matching length tolerance between vertex track and calo. block")
        .set_traits(datatools::TYPE_REAL)
        .set_mandatory(false)
        .set_explicit_unit(true)
        .set_unit_label("length")
        .set_unit_symbol("mm")
        .set_default_value_real(50.)
        .add_example(
            "Set the default value::                           \n"
            "                                                  \n"
            "  CAD.matching_tolerance : real as length = 50 mm \n"
            "                                                  \n");
  }
}

}  // end of namespace reconstruction

}  // end of namespace snemo

/* OCD support */
#include <datatools/object_configuration_description.h>
DOCD_CLASS_IMPLEMENT_LOAD_BEGIN(snemo::reconstruction::calorimeter_association_driver, ocd_) {
  ocd_.set_class_name("snemo::reconstruction::calorimeter_assocation_driver");
  ocd_.set_class_description("A driver class for calorimeter association algorithm");
  ocd_.set_class_library("Falaise_ChargedParticleTracking");
  ocd_.set_class_documentation(
      "This drivers does the association between a particle track and a calorimter hit. \n");

  // Invoke specific OCD support :
  ::snemo::reconstruction::calorimeter_association_driver::init_ocd(ocd_);

  ocd_.set_validation_support(true);
  ocd_.lock();
  return;
}
DOCD_CLASS_IMPLEMENT_LOAD_END()  // Closing macro for implementation
DOCD_CLASS_SYSTEM_REGISTRATION(snemo::reconstruction::calorimeter_association_driver,
                               "snemo::reconstruction::calorimeter_association_driver")

// end of falaise/snemo/reconstruction/calorimeter_association_driver.cc
