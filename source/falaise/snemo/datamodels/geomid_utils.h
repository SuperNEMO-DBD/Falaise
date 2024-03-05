/// \file falaise/snemo/datamodels/geomid_utils.h
/* Autho : Emmanuel Chauveau <chauveau@lp2ib.in2p3.fr>
 * Creation date: 2024-01-16
 * Last modified: 2024-01-16
 *
 * Description: Some utility to handle GEOMIDs, compute
                OM_num and GG_num, etc.
 */

#ifndef FALAISE_SNEMO_DATAMODELS_GEOMID_UTILS_H
#define FALAISE_SNEMO_DATAMODELS_GEOMID_UTILS_H 1

// Standard library:
#include <string>

// Third party:
#include <bayeux/geomtools/geom_id.h>

namespace snemo {

  namespace datamodel {

		// Compute the OM num (unique integer identifier) from geom_id
		int om_num(const geomtools::geom_id & gid);

		// Compute the GG num (unique integer identifier) from geom_id
		int gg_num(const geomtools::geom_id & gid);

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
