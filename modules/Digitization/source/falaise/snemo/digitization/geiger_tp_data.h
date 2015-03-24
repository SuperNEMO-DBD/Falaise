// geiger_tp_data.h
// Author(s): Yves LEMIERE <lemiere@lpccaen.in2p3.fr>
// Author(s): Guillaume OLIVIERO <goliviero@lpccaen.in2p3.fr>

#ifndef FALAISE_DIGITIZATION_PLUGIN_SNEMO_DIGITIZATION_GEIGER_TP_DATA_H
#define FALAISE_DIGITIZATION_PLUGIN_SNEMO_DIGITIZATION_GEIGER_TP_DATA_H

// Standard library :
#include <stdexcept>
#include <iostream>
#include <stdlib.h>
#include <cmath>
#include <string>
#include <vector>
#include <stdint.h>
#include <bitset>

// Third party:
// - Boost:
#include <boost/cstdint.hpp>

// - Bayeux/datatools :
#include <bayeux/datatools/handle.h>

// This project :
#include <snemo/digitization/geiger_tp.h>

namespace snemo {
  
  namespace digitization {		

    /// \brief Collection of geiger trigger primitive (geiger_tp)
    class geiger_tp_data : public datatools::i_serializable // or DATATOOLS_SERIALIZABLE_CLASS
		{
		public : 
 
			/// Default constructor
			geiger_tp_data();

			/// Destructor
			virtual ~geiger_tp_data();

			/// Handle to a geiger trigger primitive
			typedef datatools::handle<geiger_tp> geiger_tp_handle_type;

			/// Collection of handles of geiger trigger primitive
			typedef std::vector<geiger_tp_handle_type> geiger_tp_collection_type;
			
			/// Return the index of the geiger TP with minimum clocktick
			int get_clocktick_min_index() const;
			
			/// Return the index of the geiger TP with maximum clocktick
			int get_clocktick_max_index() const;
			
			/// Return the clocktick min for the geiger TP with minimum clocktick (thanks to his index)
			int get_clocktick_min() const;

			/// Return the clocktick max for the geiger TP with maximum clocktick (thanks to his index)
			int get_clocktick_max() const;
			
			/// Return the range between the clocktick min and the clocktick max
			int get_clocktick_range() const;
			
			/// Get a list of geiger trigger primitive which are in the same clocktick
			void get_list_of_gg_tp_per_clocktick(int32_t clocktick_800ns_, geiger_tp_collection_type & my_list_of_gg_tps_per_clocktick_) const;
			
			/// Get a list of geiger trigger primitive which are in the same clocktick and in the same crate
			void get_list_of_gg_tp_per_clocktick_per_crate(int32_t clocktick_800ns_, unsigned int crate_number_, geiger_tp_collection_type & my_list_of_gg_tps_per_clocktick_per_crate_ ) const;

			/// Check the lock status
			bool is_locked_tps() const;

			/// Lock 
			void lock_tps();

			/// Unlock
			void unlock_tps();

			/// Reset the list of tps
			void reset_tps();

			/// Add a geiger trigger primitive at the end of the collection
			geiger_tp & add();

			/// Return the const collection of geigers trigger primitive
			const geiger_tp_collection_type & get_geiger_tps() const;

			/// Reset
			void reset();

			/// Smart print
			virtual void tree_dump(std::ostream      & a_out    = std::clog,
														 const std::string & a_title  = "",
														 const std::string & a_indent = "",
														 bool a_inherit               = false) const;

		protected : 
			/// Check if two geiger TPs do not have the same clocktick AND the same geom ID
			void _check();
			
		private : 

			bool _locked_tps_; //!< TPS lock flag
			geiger_tp_collection_type _geiger_tps_; //!< Collection of geigers tracker primitive

			DATATOOLS_SERIALIZATION_DECLARATION();

		};

  } // end of namespace digitization

} // end of namespace snemo

#include <boost/serialization/export.hpp>
BOOST_CLASS_EXPORT_KEY2(snemo::digitization::geiger_tp_data,
                        "snemo::digitization::geiger_tp_data")


#endif /* FALAISE_DIGITIZATION_PLUGIN_SNEMO_DIGITIZATION_GEIGER_TP_DATA_H */

/* 
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
