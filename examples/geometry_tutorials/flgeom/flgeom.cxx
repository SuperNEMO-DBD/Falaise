// #define NDEBUG
#include <cassert>

#include <bayeux/datatools/configuration/variant_service.h>
#include <bayeux/datatools/kernel.h>
#include <bayeux/datatools/urn_query_service.h>
#include <bayeux/geomtools/box.h>

#include <falaise/falaise.h>
#include <falaise/snemo/geometry/config.h>
#include <falaise/snemo/geometry/manager.h>
#include <falaise/snemo/geometry/locator_plugin.h>
#include <falaise/snemo/geometry/gg_locator.h>
#include <falaise/snemo/geometry/calo_locator.h>
#include <falaise/snemo/geometry/xcalo_locator.h>
#include <falaise/snemo/geometry/gveto_locator.h>

int main(int argc_, char ** argv_)
{
  /////////////////////
  // Falaise session //
  /////////////////////
  falaise::initialize();

  // Bayeux's kernel and its embeded URN service:
  datatools::kernel & bxKernel = datatools::kernel::instance();
  const datatools::urn_query_service & bxUrnQuery = bxKernel.get_urn_query();

  {
    /////////////////////
    // Variant session //
    /////////////////////

    // Configure the variant service:
    datatools::configuration::variant_service::config variantServiceConfig;
    std::string variantServiceConfigTag = snemo::geometry::default_geometry_variant_tag();
    std::clog << "variantServiceConfigTag = " << variantServiceConfigTag << '\n';
    std::string variantServiceConfigCategory = "configuration";
    std::string variantServiceConfigMime;
    std::string variantServiceConfigPath;
    bxUrnQuery.resolve_urn_to_path(variantServiceConfigTag,
				   variantServiceConfigCategory,
				   variantServiceConfigMime,
				   variantServiceConfigPath);
    std::clog << "variantServiceConfigPath = " << variantServiceConfigPath << " (from URN)\n";
    variantServiceConfig.config_filename = variantServiceConfigPath;
    variantServiceConfig.profile_load = "flgeom.profile";
    datatools::configuration::variant_service variantService;
    variantService.configure(variantServiceConfig);
    variantService.start();
 
    {
      //////////////////////
      // Geometry session //
      //////////////////////
    
      // Configure the geometry service:
      // source code:
      //   {BayeuxSourceDir}/source/bxgeomtools/include/geomtools/manager.h
      geomtools::manager snGeomMgr;
      snemo::geometry::initialize_manager(snGeomMgr);

      //////////////////////////////
      // Use the geometry manager //
      //////////////////////////////
      
      // Access the geometry ID manager:
      // source code:
      //   {BayeuxSourceDir}/source/bxgeomtools/include/geomtools/id_mgr.h
      const  geomtools::id_mgr & geomidMgr = snGeomMgr.get_id_mgr();

      // Extract geometry categories and associated types:
      std::string mainOmCategory = "calorimeter_optical_module";
      assert( geomidMgr.has_category_info(mainOmCategory) );
      uint32_t mainOmType = geomidMgr.get_category_type(mainOmCategory);
      std::clog << "mainOmType = " << mainOmType << '\n';

      std::string mainScinBlockCategory = "calorimeter_block";
      assert( geomidMgr.has_category_info(mainScinBlockCategory) );
      uint32_t mainScinBlockType = geomidMgr.get_category_type(mainScinBlockCategory);
      std::clog << "mainScinBlockType = " << mainScinBlockType << '\n';

      std::string driftCellCoreCategory = "drift_cell_core";
      assert( geomidMgr.has_category_info(driftCellCoreCategory) );
      uint32_t driftCellCoreType = geomidMgr.get_category_type(driftCellCoreCategory);
      std::clog << "driftCellCoreType = " << driftCellCoreType << '\n';

      // Create the geometry ID of a specific scin block in a main calorimeter wall:
      geomtools::geom_id mainScinBlockId;
      std::clog << "mainScinBlockId = " << mainScinBlockId << " (invalid)\n";
      geomidMgr.make_id(mainScinBlockCategory, mainScinBlockId);
      std::clog << "mainScinBlockId = " << mainScinBlockId << " (type set, address unset)\n";
      geomidMgr.set(mainScinBlockId, "module", 0); // SN demonstrator module
      geomidMgr.set(mainScinBlockId, "side", 1);   // France
      geomidMgr.set(mainScinBlockId, "column", 6); // Vertical column
      geomidMgr.set(mainScinBlockId, "row", 4);    // Horizontal row
      geomidMgr.set(mainScinBlockId, "part", 1);   // Front part (tracker side)
      std::clog << "mainScinBlockId = " << mainScinBlockId << " (fully set)\n";

      {
	// Alternative faster set : no need for the ID manager,
	// but you need to know the exact addressing scheme.
	geomtools::geom_id mainScinBlockId(mainScinBlockType, 0, 1, 6, 4, 1);
	std::clog << "mainScinBlockId = " << mainScinBlockId << " (quick set)\n";
      }

      // Access to the geometry mapping:    
      // source code:
      //   {BayeuxSourceDir}/source/bxgeomtools/include/geomtools/geom_map.h
      //   {BayeuxSourceDir}/source/bxgeomtools/include/geomtools/mapping.h
      const geomtools::mapping & geomMapping = snGeomMgr.get_mapping();

      // Extract geometric informations about a main wall scin block from its ID:
      const geomtools::geom_info & mainScinBlockInfo = geomMapping.get_geom_info(mainScinBlockId);
      std::clog << "mainScinBlockInfo = " << mainScinBlockInfo << "\n";

      // Find the placement of the scin block:
      const geomtools::placement & mainScinBlockPlacement = mainScinBlockInfo.get_world_placement();
      std::clog << "mainScinBlockPlacement = " << mainScinBlockPlacement << "\n";

      // Find the logical volume of the scin block:
      const geomtools::logical_volume & mainScinBlockLogVol = mainScinBlockInfo.get_logical();
      std::clog << "mainScinBlockLogVol = " << mainScinBlockLogVol.get_name() << "\n";

      // Find the 3D shape of the scin block:
      const geomtools::i_shape_3d & mainScinBlockShape = mainScinBlockLogVol.get_shape();
      std::clog << "mainScinBlockShape = " << mainScinBlockShape.get_shape_name() << "\n";

      // The 3D box shape and its dimensions:
      const geomtools::box & mainScinBlockBox
	= dynamic_cast<const geomtools::box &>(mainScinBlockShape);
      const double width  = mainScinBlockBox.get_x();
      const double height = mainScinBlockBox.get_y();
      const double depth  = mainScinBlockBox.get_z();
      std::clog << "mainScinBlockBox.x = " << width  / CLHEP::mm << " mm" << "\n";
      std::clog << "mainScinBlockBox.y = " << height / CLHEP::mm << " mm" << "\n";
      std::clog << "mainScinBlockBox.z = " << depth  / CLHEP::mm << " mm" << "\n";

      // Extract the world position of the center of the shape:
      const geomtools::vector_3d & worldPosition = mainScinBlockPlacement.get_translation();
      std::clog << "worldPosition = " << worldPosition / CLHEP::mm << " mm" << "\n";

      // Extract the world rotation of the volume:
      const geomtools::rotation_3d & worldRotation = mainScinBlockPlacement.get_rotation();
      std::clog << "worldRotation = " << worldRotation << "\n";

      /////////////////////////////////////////
      // Mapping's default locator interface //
      /////////////////////////////////////////

      std::clog << "\n=== Default mapping's locator ===\n";

      // Given a world position in the geometry, search for the 
      // geometry ID of a physical volume with a given category:
      geomtools::vector_3d hitWorldPosition(450.5 * CLHEP::mm, 906.5 * CLHEP::mm, 520.0 * CLHEP::mm);
      std::clog << "hitWorldPosition = " << hitWorldPosition / CLHEP::mm  << " mm\n";
      const geomtools::geom_id & foundGeomId = geomMapping.get_geom_id(hitWorldPosition,
								       mainScinBlockCategory,
								       1e-6 * CLHEP::mm);
      if (foundGeomId.is_valid()) {
	std::clog << "foundGeomId = " << foundGeomId << "\n";
      }

      {
	geomtools::vector_3d badWorldPosition(1450.5 * CLHEP::mm, -4506.5 * CLHEP::mm, +3520.0 * CLHEP::mm);
	std::clog << "badWorldPosition = " << badWorldPosition / CLHEP::mm  << " mm\n";
	const geomtools::geom_id & badGeomId = geomMapping.get_geom_id(badWorldPosition,
								       mainScinBlockCategory,
								       1e-6 * CLHEP::mm);
	if (not badGeomId.is_valid()) {
	  std::clog << "badGeomId = " << badGeomId << " (invalid as expected)\n";
	}
      }

      {
	std::clog << "\n=== Bi-207 calibration source ===\n";
	std::string biSourceSpotCategory = "source_calibration_spot";
	geomtools::geom_id biSourceSpotId;
	geomidMgr.make_id(biSourceSpotCategory, biSourceSpotId);
	geomidMgr.set(biSourceSpotId, "module", 0); // SN demonstrator module
	geomidMgr.set(biSourceSpotId, "track", 1);   // Horizontal position
	geomidMgr.set(biSourceSpotId, "position", 3); // Vertical column
	std::clog << "biSourceSpotId = " << biSourceSpotId << "\n";

	const geomtools::geom_info & biSourceSpotInfo = geomMapping.get_geom_info(biSourceSpotId);
	std::clog << "biSourceSpotInfo = " << biSourceSpotInfo << "\n";	
      }

      {
	std::clog << "\n=== LAPP source pad (strip #6, pad#2) ===\n";
	std::string sourcePadCategory = "source_pad";
	geomtools::geom_id sourcePadId;
	geomidMgr.make_id(sourcePadCategory, sourcePadId);
	geomidMgr.set(sourcePadId, "module", 0); // SN demonstrator module
	geomidMgr.set(sourcePadId, "strip", 6);  // Source strip
	geomidMgr.set(sourcePadId, "pad", 2);    // Source pad
	std::clog << "sourcePadId = " << sourcePadId << "\n";
	const geomtools::geom_info & sourcePadInfo = geomMapping.get_geom_info(sourcePadId);
	std::clog << "sourcePadInfo = " << sourcePadInfo << "\n";
      }

      {
	std::clog << "\n=== ITEP source pad (strip #2, pad #0) ===\n";
	std::string sourcePadCategory = "source_pad";
	geomtools::geom_id sourcePadId;
	geomidMgr.make_id(sourcePadCategory, sourcePadId);
	geomidMgr.set(sourcePadId, "module", 0); // SN demonstrator module
	geomidMgr.set(sourcePadId, "strip", 2);  // Source strip
	geomidMgr.set(sourcePadId, "pad", 0);    // Source pad
	std::clog << "sourcePadId = " << sourcePadId << "\n";
	const geomtools::geom_info & sourcePadInfo = geomMapping.get_geom_info(sourcePadId);
	std::clog << "sourcePadInfo = " << sourcePadInfo << "\n";
      }

      ////////////////////////////////////
      // Use the embeded locator plugin //
      ////////////////////////////////////
   
      // source code:
      //   {FalaiseSourceDir}/source/falaise/snemo/geometry/locator_plugin.h
      const snemo::geometry::locator_plugin & locatorPlugin =
	snGeomMgr.get_plugin<snemo::geometry::locator_plugin>(snemo::geometry::default_locators_driver_name());
      
      // Access specific and faster locators:

      // Locator for Geiger cell and tracker components:
      // source code:
      //   {FalaiseSourceDir}/source/falaise/snemo/geometry/gg_locator.h
      const snemo::geometry::gg_locator & ggLocator = locatorPlugin.geigerLocator();

      std::clog << "\n=== Geiger locator ===\n";
      std::clog << "numberOfSides  = " << ggLocator.numberOfSides() << "\n";
      std::clog << "numberOfLayers = " << ggLocator.numberOfLayers(0) << "\n";
      std::clog << "numberOfRows   = " << ggLocator.numberOfRows(0) << "\n";

      uint32_t ggCellIdType = ggLocator.cellGIDType();
      std::clog << "ggCellIdType = " << ggCellIdType << "\n";

      double ggCellLength = ggLocator.cellLength();
      double ggCellRadius = ggLocator.cellRadius();
      double ggAnodeWireDiameter = ggLocator.anodeWireDiameter();
      double ggFieldWireDiameter = ggLocator.fieldWireDiameter();
      std::clog << "ggCellLength = " << ggCellLength / CLHEP::mm << " mm\n";
      std::clog << "ggCellRadius = " << ggCellRadius / CLHEP::mm << " mm\n";
      std::clog << "ggAnodeWireDiameter = " << ggAnodeWireDiameter / CLHEP::mm << " mm\n";
      std::clog << "ggFieldWireDiameter = " << ggFieldWireDiameter / CLHEP::mm << " mm\n";

      double italyLayerX0 = ggLocator.getXCoordOfLayer(0, 0);
      double italyLayerX8 = ggLocator.getXCoordOfLayer(0, 8);
      std::clog << "X for layer #0 = " << italyLayerX0 / CLHEP::mm << " mm (Italy)\n";
      std::clog << "X for layer #8 = " << italyLayerX8 / CLHEP::mm << " mm (Italy)\n";
    
      // Locators for calorimeter components:
      // source code:
      //   {FalaiseSourceDir}/source/falaise/snemo/geometry/calo_locator.h
      //   {FalaiseSourceDir}/source/falaise/snemo/geometry/xcalo_locator.h
      //   {FalaiseSourceDir}/source/falaise/snemo/geometry/gveto_locator.h
      const snemo::geometry::calo_locator  & caloLocator  = locatorPlugin.caloLocator();
      const snemo::geometry::xcalo_locator & xcaloLocator = locatorPlugin.xcaloLocator();
      const snemo::geometry::gveto_locator & gvetoLocator = locatorPlugin.gvetoLocator();

    
      // Terminate the geometry manager:
      snGeomMgr.reset();
    } // End of geometry session
  
    variantService.stop();

  } // End of variant session
  
  falaise::terminate();
  // End of Falaise session
  return 0;
}
