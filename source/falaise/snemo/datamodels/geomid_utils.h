/// \file falaise/snemo/datamodels/geomid_utils.h
/* Authors: Emmanuel Chauveau <chauveau@lp2ib.in2p3.fr>,
 *          Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2024-01-16
 * Last modified: 2024-08-27
 *
 * Description: Some utility to handle GEOMIDs, compute OM_num and GG_num, etc.
 */

#ifndef FALAISE_SNEMO_DATAMODELS_GEOMID_UTILS_H
#define FALAISE_SNEMO_DATAMODELS_GEOMID_UTILS_H 1

// Standard library:
#include <string>
#include <map>

// Third party:
#include <bayeux/geomtools/geom_id.h>

namespace snemo {

  namespace datamodel {

    // Compute the OM num (unique integer identifier) from geom_id (unique geometry identifier)
    int om_num(const geomtools::geom_id & gid);

    // Compute the OM geom_id (unique geometry identifier) from the OM num (unique integer identifier)
    geomtools::geom_id om_gid(const int num_, bool block_ = false, bool back_part_ = false);

		// Return a singleton num->geom_id map for OMs
		const std::map<int, geomtools::geom_id> & om_map();

    // Compute the GG num (unique integer identifier) from geom_id (unique geometry identifier)
    int gg_num(const geomtools::geom_id & gid);

    // Compute the GG geom_id (unique geometry identifier) from the GG num (unique integer identifier)
    geomtools::geom_id gg_gid(const int num_, bool cell_core_ = false);

		// Return a singleton num->geom_id map for GG cells
		const std::map<int, geomtools::geom_id> & gg_map();

    // Provide the OM label from geom_id (example: "X:1.0.5.19")
    std::string om_label(const geomtools::geom_id & gid);

    // Provide the GG label from geom_id (example: "GG:1.64.7")
    std::string gg_label(const geomtools::geom_id & gid);

  } // end of namespace datamodel

} // end of namespace snemo

#endif // FALAISE_SNEMO_DATAMODELS_GEOMID_UTILS_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
