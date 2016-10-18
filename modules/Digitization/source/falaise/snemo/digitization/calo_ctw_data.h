// snemo/digitization/calo_ctw_data.h
// Author(s): Yves LEMIERE <lemiere@lpccaen.in2p3.fr>
// Author(s): Guillaume OLIVIERO <goliviero@lpccaen.in2p3.fr>

#ifndef FALAISE_DIGITIZATION_PLUGIN_SNEMO_DIGITIZATION_CALO_CTW_DATA_H
#define FALAISE_DIGITIZATION_PLUGIN_SNEMO_DIGITIZATION_CALO_CTW_DATA_H

// Standard library :
#include <vector>
#include <bitset>

// Third party:
// - Boost:
#include <boost/cstdint.hpp>
// - Bayeux/datatools :
#include <bayeux/datatools/handle.h>

// This project :
#include <snemo/digitization/calo_ctw.h>
#include <snemo/digitization/calo_ctw_constants.h>

namespace snemo {
  
  namespace digitization {		

    /// \brief Collection of calorimeter crate trigger words (C-CTW)
    class calo_ctw_data : public datatools::i_serializable
    {
    public : 
      
      /// Default constructor
      calo_ctw_data();

      /// Destructor
      virtual ~calo_ctw_data();

      /// Handle to a calorimeter crate trigger word
      typedef datatools::handle<calo_ctw> calo_ctw_handle_type;

      /// Collection of handles of calorimeter crate trigger word
      typedef std::vector<calo_ctw_handle_type> calo_ctw_collection_type;
       
      /// Return the index of the calorimeter crate trigger word with minimum clocktick
			unsigned int get_clocktick_min_index() const;
			
      /// Return the index of the calorimeter crate trigger word with maximum clocktick
			unsigned int get_clocktick_max_index() const;
 
      /// Return the clocktick min for the calorimeter crate trigger word with minimum clocktick (thanks to his index)
      uint32_t get_clocktick_min() const;

      /// Return the clocktick max for the calorimeter crate trigger word with maximum clocktick (thanks to his index)
			uint32_t get_clocktick_max() const;
			
      /// Return the range between the clocktick min and the clocktick max
			uint32_t get_clocktick_range() const;
					
      /// Do a list of calorimeter crate trigger word which are in the same clocktick
      void get_list_of_calo_ctw_per_clocktick(uint32_t clocktick_25ns_, calo_ctw_collection_type & ctws_) const;

      /// Reset the list of ctws
      void reset_ctws();

      /// Add a calorimeter crate trigger word at the end of the collection
      calo_ctw & add();

      /// Return the const collection of calorimeter crate trigger
      const calo_ctw_collection_type & get_calo_ctws() const;
		
			/// Check if calo ctw data has ctw(s)
			bool has_calo_ctw() const;
			
      /// Reset
      void reset();

      /// Smart print
      virtual void tree_dump(std::ostream      & a_out    = std::clog,
														 const std::string & a_title  = "",
														 const std::string & a_indent = "",
														 bool a_inherit               = false) const;

    protected : 
      /// Check if two calorimeters crate trigger word do not have the same clocktick AND the same geom ID
      void _check();
			
    private : 
      calo_ctw_collection_type _calo_ctws_; //!< Collection of calorimeters crate trigger

      DATATOOLS_SERIALIZATION_DECLARATION()

    };

  } // end of namespace digitization

} // end of namespace snemo

#include <boost/serialization/export.hpp>
BOOST_CLASS_EXPORT_KEY2(snemo::digitization::calo_ctw_data,
                        "snemo::digitization::calo_ctw_data")


#endif /* FALAISE_DIGITIZATION_PLUGIN_SNEMO_DIGITIZATION_CALO_CTW_DATA_H */

/* 
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
