// snemo/digitization/geiger_ctw_data.h
// Author(s): Yves LEMIERE <lemiere@lpccaen.in2p3.fr>
// Author(s): Guillaume OLIVIERO <goliviero@lpccaen.in2p3.fr>

#ifndef FALAISE_DIGITIZATION_PLUGIN_SNEMO_DIGITIZATION_GEIGER_CTW_DATA_H
#define FALAISE_DIGITIZATION_PLUGIN_SNEMO_DIGITIZATION_GEIGER_CTW_DATA_H

// Standard library :
#include <vector>
#include <bitset>

// Third party:
// - Boost:
#include <boost/cstdint.hpp>
// - Bayeux/datatools :
#include <bayeux/datatools/handle.h>

// This project :
#include <snemo/digitization/geiger_ctw.h>

namespace snemo {
  
  namespace digitization {		

    /// \brief Collection of geiger crate trigger words (C-CTW)
    class geiger_ctw_data : public datatools::i_serializable
    {
    public : 
      
      /// Default constructor
      geiger_ctw_data();

      /// Destructor
      virtual ~geiger_ctw_data();

      /// Handle to a geiger crate trigger word
      typedef datatools::handle<geiger_ctw> geiger_ctw_handle_type;

      /// Collection of handles of geiger crate trigger word
      typedef std::vector<geiger_ctw_handle_type> geiger_ctw_collection_type;
       
      /// Return the index of the geiger crate trigger word with minimum clocktick
      int get_clocktick_min_index() const;
			
      /// Return the index of the geiger crate trigger word with maximum clocktick
      int get_clocktick_max_index() const;
 
      /// Return the clocktick min for the geiger crate trigger word with minimum clocktick (thanks to his index)
      int get_clocktick_min() const;

      /// Return the clocktick max for the geiger crate trigger word with maximum clocktick (thanks to his index)
      int get_clocktick_max() const;
			
      /// Return the range between the clocktick min and the clocktick max
      int get_clocktick_range() const;
					
      /// Do a list of geiger crate trigger word which are in the same clocktick
      void get_list_of_geiger_ctw_per_clocktick(int32_t clocktick_25ns_, geiger_ctw_collection_type & ctws_) const;
			
      /// Check the lock status
      bool is_locked() const;

      /// Lock 
      void lock();

      /// Unlock
      void unlock();

      /// Reset the list of ctws
      void reset_ctws();

      /// Add a geiger crate trigger word at the end of the collection
      geiger_ctw & add();

      /// Return the const collection of geiger crate trigger
      const geiger_ctw_collection_type & get_geiger_ctws() const;

      /// Return the collection of geiger crate trigger
      geiger_ctw_collection_type & grab_geiger_ctws() ;

      /// Reset
      void reset();

      /// Smart print
      virtual void tree_dump(std::ostream      & a_out    = std::clog,
														 const std::string & a_title  = "",
														 const std::string & a_indent = "",
														 bool a_inherit               = false) const;

    protected : 
      /// Check if two geigers crate trigger word do not have the same clocktick AND the same geom ID
      void _check();
			
    private : 

      bool _locked_; //!< CTWs lock flag
      geiger_ctw_collection_type _geiger_ctws_; //!< Collection of geigers crate trigger words

      DATATOOLS_SERIALIZATION_DECLARATION();

    };

  } // end of namespace digitization

} // end of namespace snemo

#include <boost/serialization/export.hpp>
BOOST_CLASS_EXPORT_KEY2(snemo::digitization::geiger_ctw_data,
                        "snemo::digitization::geiger_ctw_data")


#endif /* FALAISE_DIGITIZATION_PLUGIN_SNEMO_DIGITIZATION_GEIGER_CTW_DATA_H */

/* 
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
