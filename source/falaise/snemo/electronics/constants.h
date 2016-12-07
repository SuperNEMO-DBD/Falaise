/// \file snemo/electronics/constants.h
/* Author(s)     : Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Author(s)     : Guillaume Oliviéro <goliviero@lpccaen.in2p3.fr>
 * Creation date : 2016-12-06
 * Last modified : 2016-12-06
 *
 * Copyright (C) 2016 Francois Mauger <mauger@lpccaen.in2p3.fr>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 * Description:
 *
 *   SuperNEMO electronics constants.
 *
 */

#ifndef FALAISE_SNEMO_ELECTRONICS_CONSTANTS_H
#define FALAISE_SNEMO_ELECTRONICS_CONSTANTS_H

// Standard library:
#include <cstddef>
#include <cstdint>
#include <limits>

namespace snemo {

  namespace electronics {

    class constants
    {
    public :
      
			static const std::size_t NSIDES  = 2;
			static const std::size_t NLAYERS = 9;
			static const std::size_t NROWS   = 113;
			static const std::size_t NZONES  = 10;
			
      static const std::size_t TRACKER_DATA_FULL_BITSET_SIZE = 7;
			
			static const uint32_t INVALID_CLOCKTICK = std::numeric_limits<uint32_t>::max(); //!< Invalid value for clocktick


			/// Size of each bitset in the Calo summary record
			enum calo_summary_record_bitset_size {
				FULL_BITSET_SIZE    = 35,
				ZONING_BITSET_SIZE  = 10,
				HTM_BITSET_SIZE     = 2,
				XT_INFO_BITSET_SIZE = 3
			};
			
			enum L2_trigger_mode {
				INVALID      = 0,
				CALO_ONLY    = 1,
				CALO_TRACKER_TIME_COINC = 2,
				CARACO       = 3,
				OPEN_DELAYED = 4,
				APE          = 5,
				DAVE         = 6					
			};


    };

    
  }  // end of namespace electronics

}  // end of namespace snemo

#include <datatools/ocd_macros.h>

// Declare the OCD interface of the module
DOCD_CLASS_DECLARATION(snemo::electronics::constants)

#endif // FALAISE_SNEMO_ELECTRONICS_CONSTANTS_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
