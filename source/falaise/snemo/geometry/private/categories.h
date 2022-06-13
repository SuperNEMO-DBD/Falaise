#ifndef FALAISE_SNEMO_GEOMETRY_DETAIL_CATEGORIES_H
#define FALAISE_SNEMO_GEOMETRY_DETAIL_CATEGORIES_H

#include <geomtools/box.h>
#include <geomtools/geom_info.h>
#include <geomtools/i_composite_shape_3d.h>
#include <geomtools/intersection_3d.h>
#include <geomtools/subtraction_3d.h>

namespace snemo {
namespace geometry {
namespace detail {

// NB: These must be matched to the entries appearing in the relevant
// geometry mapping resource files!
// At present, no clear direction on who has priority here: code or resources
const char kModuleGIDCategory[] = "module";

// Veto Calorimeters
const char kTrackerSubmoduleGIDCategory[] = "calorimeter_submodule";
const char kGammaVetoOMGIDCategory[] = "gveto_optical_module";
const char kGammaVetoBlockGIDCategory[] = "gveto_block";
const char kGammaVetoWrapperGIDCategory[] = "gveto_wrapper";

// Calorimeter
const char kCaloSubmoduleGIDCategory[] = "calorimeter_submodule";
const char kCaloOMGIDCategory[] = "calorimeter_optical_module";
const char kCaloBlockGIDCategory[] = "calorimeter_block";
const char kCaloWrapperGIDCategory[] = "calorimeter_wrapper";

// XWall Calorimter
// const char kTrackerSubmoduleGIDCategory[] = "calorimeter_submodule";
const char kXCaloOMGIDCategory[] = "xcalo_optical_module";
const char kXCaloBlockGIDCategory[] = "xcalo_block";
const char kXCaloWrapperGIDCategory[] = "xcalo_wrapper";

// Tracker
const char kTrackerVolumeGIDCategory[] = "tracker_volume";
// const char kTrackerLayerGIDCategory[] = "tracker_layer";
const char kDriftCellGIDCategory[] = "drift_cell_core";

//! Given the geom_info for a calorimeter block, return the underlying box object
//  Not entirely clear why this is needed: review what's in resource files...
inline const geomtools::box *getBlockBox(const geomtools::geom_info &blockGI) {
  const geomtools::i_shape_3d *caloBlockShape = &blockGI.get_logical().get_shape();
  if (caloBlockShape->get_shape_name() == "subtraction_3d") {
    const auto &ref_s3d = dynamic_cast<const geomtools::subtraction_3d &>(*caloBlockShape);
    // Example : 'calo_scin_box_model' case :
    const geomtools::i_composite_shape_3d::shape_type &sht1 = ref_s3d.get_shape1();
    const geomtools::i_shape_3d &sh1 = sht1.get_shape();
    DT_THROW_IF(sh1.get_shape_name() != "box", std::logic_error,
                "Do not support non-box shaped block with ID = " << blockGI.get_geom_id() << " !");
    return dynamic_cast<const geomtools::box *>(&sh1);
  }

  if (caloBlockShape->get_shape_name() == "intersection_3d") {
    // Example : 'calo_tapered_scin_box_model' case :
    const auto &ref_i3d = dynamic_cast<const geomtools::intersection_3d &>(*caloBlockShape);
    const geomtools::i_composite_shape_3d::shape_type &sht1 = ref_i3d.get_shape1();
    const geomtools::i_shape_3d &sh1 = sht1.get_shape();
    DT_THROW_IF(sh1.get_shape_name() != "subtraction_3d", std::logic_error,
                "Do not support non-subtraction_3d shaped block with ID = " << blockGI.get_geom_id()
                                                                            << " !");
    const auto &ref_s3d = dynamic_cast<const geomtools::subtraction_3d &>(sh1);
    const geomtools::i_composite_shape_3d::shape_type &sht11 = ref_s3d.get_shape1();
    const geomtools::i_shape_3d &sh11 = sht11.get_shape();
    DT_THROW_IF(sh11.get_shape_name() != "box", std::logic_error,
                "Do not support non-box shaped block with ID = " << blockGI.get_geom_id() << " !");
    return dynamic_cast<const geomtools::box *>(&sh11);
  }

  if (caloBlockShape->get_shape_name() == "box") {
    // Simple box case :
    return dynamic_cast<const geomtools::box *>(caloBlockShape);
  }
  return nullptr;
}
}  // namespace detail
}  // namespace geometry
}  // namespace snemo

#endif
