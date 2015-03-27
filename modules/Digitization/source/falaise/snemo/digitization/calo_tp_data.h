// calo_tp_data.h
// Author(s): Yves LEMIERE <lemiere@lpccaen.in2p3.fr>
// Author(s): Guillaume OLIVIERO <goliviero@lpccaen.in2p3.fr>

#ifndef FALAISE_DIGITIZATION_PLUGIN_SNEMO_DIGITIZATION_CALO_TP_DATA_H
#define FALAISE_DIGITIZATION_PLUGIN_SNEMO_DIGITIZATION_CALO_TP_DATA_H

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
#include <snemo/digitization/calo_tp.h>

namespace snemo {
  
  namespace digitization {		

    /// \brief Collection of calorimeter trigger primitive (calo_tp)
    class calo_tp_data : public datatools::i_serializable // or DATATOOLS_SERIALIZABLE_CLASS
    {
    public : 
 
      /// Default constructor
      calo_tp_data();

      /// Destructor
      virtual ~calo_tp_data();

			/// Handle to a calorimeter trigger primitive
			typedef datatools::handle<calo_tp> calo_tp_handle_type;

			/// Collection of handles of calorimeter trigger primitive
			typedef std::vector<calo_tp_handle_type> calo_tp_collection_type;
			
			/// Return the index of the calo TP with minimum clocktick
			int get_clocktick_min_index() const;
			
			/// Return the index of the calo TP with maximum clocktick
			int get_clocktick_max_index() const;
			
			/// Return the clocktick min for the calo TP with minimum clocktick (thanks to his index)
			int get_clocktick_min() const;

			/// Return the clocktick max for the calo TP with maximum clocktick (thanks to his index)
			int get_clocktick_max() const;
			
			/// Return the range between the clocktick min and the clocktick max
			int get_clocktick_range() const;
			
			/// Get a list of calo trigger primitive which are in the same clocktick
			void get_list_of_tp_per_clocktick(int32_t clocktick_25ns_, calo_tp_collection_type & my_list_of_tps_per_clocktick_) const;
			
			/// Get a list of calo trigger primitive which are in the same clocktick and in the same crate
			void get_list_of_tp_per_clocktick_per_crate(int32_t clocktick_25ns_, unsigned int crate_number_, calo_tp_collection_type & my_list_of_tps_per_clocktick_per_crate_ ) const;

			/// Check the lock status
      bool is_locked_tps() const;

      /// Lock 
			void lock_tps();

			/// Unlock
			void unlock_tps();

			/// Reset the list of tps
			void reset_tps();

			/// Add a calorimeter trigger primitive at the end of the collection
			calo_tp & add();

			/// Return the const collection of calorimeters trigger primitive
			const calo_tp_collection_type & get_calo_tps() const;

			/// Return the mutable collection of calorimeters trigger primitive
			calo_tp_collection_type & grab_calo_tps();

      /// Reset
      void reset();

      /// Smart print
      virtual void tree_dump(std::ostream      & a_out    = std::clog,
														 const std::string & a_title  = "",
														 const std::string & a_indent = "",
														 bool a_inherit               = false) const;

		protected : 
			/// Check if two calo TPs do not have the same clocktick AND the same geom ID
			void _check();
			
    private : 

			bool _locked_tps_; //!< TPS lock flag
			calo_tp_collection_type _calo_tps_; //!< Collection of calorimeters tracker primitive

      DATATOOLS_SERIALIZATION_DECLARATION();

    };

  } // end of namespace digitization

} // end of namespace snemo

#include <boost/serialization/export.hpp>
BOOST_CLASS_EXPORT_KEY2(snemo::digitization::calo_tp_data,
                        "snemo::digitization::calo_tp_data")


#endif /* FALAISE_DIGITIZATION_PLUGIN_SNEMO_DIGITIZATION_CALO_TP_DATA_H */

/* 
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
