// snemo/digitization/signal_data.h
// Author(s): Yves LEMIERE <lemiere@lpccaen.in2p3.fr>
// Author(s): Guillaume OLIVIERO <goliviero@lpccaen.in2p3.fr>

#ifndef FALAISE_DIGITIZATION_PLUGIN_SNEMO_DIGITIZATION_SIGNAL_DATA_H
#define FALAISE_DIGITIZATION_PLUGIN_SNEMO_DIGITIZATION_SIGNAL_DATA_H

// Standard library :
#include <vector>

// Third party:
// - Bayeux/datatools :
#include <bayeux/datatools/handle.h>
// - Bayeux/mctools :
#include <mctools/base_step_hit.h>

// This project :
#include <snemo/digitization/geiger_signal.h>
#include <snemo/digitization/calo_signal.h>

namespace snemo {
  
  namespace digitization {		

    /// \brief Collection of signal data. Both calorimeter signals and geiger signals.
    class signal_data //: public datatools::i_serializable // or DATATOOLS_SERIALIZABLE_CLASS
    {
    public : 
 
      /// Default constructor
      signal_data();

      /// Destructor
      virtual ~signal_data();

      /// Handle to a geiger trigger primitive
      typedef datatools::handle<geiger_signal> geiger_signal_handle_type;

      /// Collection of handles of geiger trigger primitive
      typedef std::vector<geiger_signal_handle_type> geiger_signal_collection_type;

      /// Handle to a calo trigger primitive
      typedef datatools::handle<calo_signal> calo_signal_handle_type;

      /// Collection of handles of calo trigger primitive
      typedef std::vector<calo_signal_handle_type> calo_signal_collection_type;	
			
      /// Check the lock status
      bool is_locked() const;

      /// Lock 
      void lock();

      /// Unlock
      void unlock();

      /// Reset the list of geiger signals
      void reset_geiger_signals();

      /// Reset the list of calorimeter signals
      void reset_calo_signals();

      /// Add a geiger trigger primitive at the end of the collection
      geiger_signal & add_geiger_signal();

      /// Add a geiger trigger primitive at the end of the collection
      calo_signal & add_calo_signal();

      /// Return the const collection of geigers trigger primitive
      const geiger_signal_collection_type & get_geiger_signals() const;

      /// Return the mutable collection of geigers trigger primitive			
      geiger_signal_collection_type & grab_geiger_signals();

      /// Return the const collection of geigers trigger primitive
      const calo_signal_collection_type & get_calo_signals() const;

      /// Return the mutable collection of geigers trigger primitive			
      calo_signal_collection_type & grab_calo_signals();
			
			/// Check if signal data has geiger signals
			bool has_geiger_signals();
			
			/// Check if signal data has calo signals
			bool has_calo_signals();

      /// Reset
      void reset();

      /// Smart print
      virtual void tree_dump(std::ostream      & a_out    = std::clog,
														 const std::string & a_title  = "",
														 const std::string & a_indent = "",
														 bool a_inherit               = false) const;

    protected : 
      /// Check if two geiger or two calo signals do not have the same geom ID
      void _check();
			
    private : 

      bool _locked_; //!< Geiger signals lock flag
      geiger_signal_collection_type _geiger_signals_; //!< Collection of geigers tracker primitive
      calo_signal_collection_type _calo_signals_; //!< Collection of calos tracker primitive

      //DATATOOLS_SERIALIZATION_DECLARATION();

    };

  } // end of namespace digitization

} // end of namespace snemo

#include <boost/serialization/export.hpp>
BOOST_CLASS_EXPORT_KEY2(snemo::digitization::signal_data,
                        "snemo::digitization::signal_data")


#endif /* FALAISE_DIGITIZATION_PLUGIN_SNEMO_DIGITIZATION_SIGNAL_DATA_H */

/* 
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
