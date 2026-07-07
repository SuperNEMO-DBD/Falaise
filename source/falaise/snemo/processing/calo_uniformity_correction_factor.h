/** \file falaise/snemo/processing/calo_uniformity_correction_factor.h
 * Author (s) :    F.Mauger <mauger@lpccaen.in2p3.fr>
 *                 E.Chauveau <chauveau@cenbg.in2p3.fr>
 * Creation date: 2026-05-18
 * Last modified: 2026-05-18
 * Description:
 *
 *   Model the Birks-Cerenkov correction factor for energy deposit by electrons in scin block.
 *
 */
#ifndef FALAISE_SNEMO_PROCESSING_CALO_UNIFORMITY_CORRECTION_FACTOR_H
#define FALAISE_SNEMO_PROCESSING_CALO_UNIFORMITY_CORRECTION_FACTOR_H 1

#include <vector>

#include <bayeux/geomtools/utils.h>
#include <bayeux/geomtools/geom_id.h>

namespace snemo {

  namespace processing {

    struct CaloUniformityCorrection
    {
      // 2026-05-18 FM: Blocks dimensions and geom categories are hard-coded:
      static constexpr int mwallCategory = 1302;
      static constexpr int xwallCategory = 1232;
      static constexpr int gvetoCategory = 1252;
      // unit is mm:
      static constexpr double mwallScinFrontBlockX_mm = 256.0;
      static constexpr double mwallScinFrontBlockY_mm = 256.0;
      static constexpr double mwallScinFrontBlockZ_mm = 31.0;
      static constexpr double xwallScinBlockX_mm = 200.0;
      static constexpr double xwallScinBlockY_mm = 208.5;
      static constexpr double xwallScinBlockZ_mm = 150.0;
      static constexpr double gvetoScinBlockX_mm = 290.0;
      static constexpr double gvetoScinBlockY_mm = 308.0;
      static constexpr double gvetoScinBlockZ_mm = 150.0;

      CaloUniformityCorrection();

      /** Return the correction factor at some position within the scintillator block
       * geomtools::geom_id scinGid = ... ; // geom ID of the scintillator block 
       * geomtools::vector_3d scinPos = ... ; // world position of the block center
       * double corr = correction_factor(scinGid, scinPos);
       *
       */
      
      double correction_factor(const geomtools::geom_id & scin_gid_,
			       const geomtools::vector_3d & position_in_scin_block_) const;

    public:
      
      std::vector<double> parameters_mwall_8inch; //!< Polynomial parameters for the uniformity correction for MWall 8"
      std::vector<double> parameters_mwall_5inch; //!< Polynomial parameters for the uniformity correction for MWall 5"
      std::vector<double> parameters_xwall;       //!< Polynomial parameters for the uniformity correction for XWall
      std::vector<double> parameters_gveto;       //!< Polynomial parameters for the uniformity correction for GVeto

    };
      
  } // end of namespace processing

} // end of namespace snemo

#endif // FALAISE_SNEMO_PROCESSING_CALO_UNIFORMITY_CORRECTION_FACTOR_H
