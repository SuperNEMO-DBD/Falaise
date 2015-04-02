// snemo/digitization/geiger_signal_data.h
// Author(s): Yves LEMIERE <lemiere@lpccaen.in2p3.fr>
// Author(s): Guillaume OLIVIERO <goliviero@lpccaen.in2p3.fr>

#ifndef FALAISE_DIGITIZATION_PLUGIN_SNEMO_DIGITIZATION_GEIGER_SIGNAL_DATA_H
#define FALAISE_DIGITIZATION_PLUGIN_SNEMO_DIGITIZATION_GEIGER_SIGNAL_DATA_H

// Standard library :
#include <vector>

// Third party:
// - Boost:
#include <boost/cstdint.hpp>
// - Bayeux/datatools :
#include <bayeux/datatools/handle.h>

// This project :
#include <snemo/digitization/geiger_signal.h>

namespace snemo {
  
  namespace digitization {		

    /// \brief Collection of geiger trigger primitive (geiger_signal)
    class geiger_signal_data //: public datatools::i_serializable // or DATATOOLS_SERIALIZABLE_CLASS
    {
    public : 
 
      /// Default constructor
      geiger_signal_data();

      /// Destructor
      virtual ~geiger_signal_data();

      /// Handle to a geiger trigger primitive
      typedef datatools::handle<geiger_signal> geiger_signal_handle_type;

      /// Collection of handles of geiger trigger primitive
      typedef std::vector<geiger_signal_handle_type> geiger_signal_collection_type;			

      /// Check the lock status
      bool is_locked() const;

      /// Lock 
      void lock();

      /// Unlock
      void unlock();

      /// Reset the list of signals
      void reset_signals();

      /// Add a geiger trigger primitive at the end of the collection
      geiger_signal & add();

      /// Return the const collection of geigers trigger primitive
      const geiger_signal_collection_type & get_geiger_signals() const;

      /// Return the mutable collection of geigers trigger primitive			
      geiger_signal_collection_type & grab_geiger_signals();

      /// Reset
      void reset();

      /// Smart print
      virtual void tree_dump(std::ostream      & a_out    = std::clog,
			     const std::string & a_title  = "",
			     const std::string & a_indent = "",
			     bool a_inherit               = false) const;

    protected : 
      /// Check if two geiger signals do not have the same clocktick AND the same geom ID
      void _check();
			
    private : 

      bool _locked_; //!< Geiger signals lock flag
      geiger_signal_collection_type _geiger_signals_; //!< Collection of geigers tracker primitive

      //DATATOOLS_SERIALIZATION_DECLARATION();

    };

  } // end of namespace digitization

} // end of namespace snemo

#include <boost/serialization/export.hpp>
BOOST_CLASS_EXPORT_KEY2(snemo::digitization::geiger_signal_data,
                        "snemo::digitization::geiger_signal_data")


#endif /* FALAISE_DIGITIZATION_PLUGIN_SNEMO_DIGITIZATION_GEIGER_SIGNAL_DATA_H */

/* 
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
