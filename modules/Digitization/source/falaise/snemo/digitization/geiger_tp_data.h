// snemo/digitization/geiger_tp_data.h
// Author(s): Yves LEMIERE <lemiere@lpccaen.in2p3.fr>
// Author(s): Guillaume OLIVIERO <goliviero@lpccaen.in2p3.fr>

#ifndef FALAISE_DIGITIZATION_PLUGIN_SNEMO_DIGITIZATION_GEIGER_TP_DATA_H
#define FALAISE_DIGITIZATION_PLUGIN_SNEMO_DIGITIZATION_GEIGER_TP_DATA_H

// Standard library :
#include <vector>

// Third party:
// - Boost:
#include <boost/cstdint.hpp>
// - Bayeux/datatools :
#include <bayeux/datatools/handle.h>
// - Bayeux/geomtools:
#include <bayeux/geomtools/geom_id.h>

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
			unsigned int get_clocktick_min_index() const;
			
			/// Return the index of the geiger TP with maximum clocktick
			unsigned int get_clocktick_max_index() const;
			
			/// Return the clocktick min for the geiger TP with minimum clocktick (thanks to his index)
			uint32_t get_clocktick_min() const;

			/// Return the clocktick max for the geiger TP with maximum clocktick (thanks to his index)
			uint32_t get_clocktick_max() const;
			
			/// Return the range between the clocktick min and the clocktick max
			uint32_t get_clocktick_range() const;
			
			/// Get a list of geiger trigger primitive which are in the same clocktick
			void get_list_of_gg_tp_per_clocktick(uint32_t clocktick_800ns_, geiger_tp_collection_type & my_list_of_gg_tps_per_clocktick_) const;
			
			/// Get a list of geiger trigger primitive which are in the same clocktick and in the same crate
			void get_list_of_gg_tp_per_clocktick_per_crate(uint32_t clocktick_800ns_, unsigned int crate_number_, geiger_tp_collection_type & my_list_of_gg_tps_per_clocktick_per_crate_ ) const;

			/// Get a list of geiger trigger primitive which are in the same electronic ID
			void get_list_of_gg_tp_per_eid(const geomtools::geom_id & electronic_id_, geiger_tp_collection_type & my_list_of_gg_tps_per_eid_) const;

			/// Reset the list of tps
			void reset_tps();

			/// Add a geiger trigger primitive at the end of the collection
			geiger_tp & add();

			/// Return the const collection of geigers trigger primitive
			const geiger_tp_collection_type & get_geiger_tps() const;

			/// Return the mutable collection of geigers trigger primitive			
			geiger_tp_collection_type & grab_geiger_tps();

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

			geiger_tp_collection_type _geiger_tps_; //!< Collection of geigers tracker primitive

			DATATOOLS_SERIALIZATION_DECLARATION()

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
