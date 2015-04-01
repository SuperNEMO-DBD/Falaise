// snemo/digitization/geiger_signal.h
// Author(s): Yves LEMIERE <lemiere@lpccaen.in2p3.fr>
// Author(s): Guillaume OLIVIERO <goliviero@lpccaen.in2p3.fr>

#ifndef FALAISE_DIGITIZATION_PLUGIN_SNEMO_DIGITIZATION_GEIGER_SIGNAL_H
#define FALAISE_DIGITIZATION_PLUGIN_SNEMO_DIGITIZATION_GEIGER_SIGNAL_H

// Standard library :
#include <bitset>

// Third party:
// - Bayeux/geomtools :
#include <bayeux/geomtools/base_hit.h>

// This project : 
#include <snemo/digitization/mapping.h>

namespace snemo {
  
  namespace digitization {

    /// \brief The geiger tracker signal
    class geiger_signal : public geomtools::base_hit
    {
    public : 

      /// Default constructor
      geiger_signal();

      /// Destructor
      virtual ~geiger_signal();
      
      /// Return the const time of avalanche on the anode
      double get_anode_avalanche_time() const;
     
      // Set the time of avalanche on the anode
      void set_anode_avalanche_time(double & anode_avalanche_time_);

      /// Return the const time of plasma on top cathode
      double get_plasma_top_time_() const;

      /// Set the time plasma on top cathode
      void set_plasma_top_time_(double & plasma_top_time_);

      /// Return the const time of plasma on bottom cathode
      double get_plasma_bottom_time_() const;

      /// Set the time plasma on bottom cathode
      void set_plasma_bottom_time_(double & plasma_bottom_time_);

      /// Check the lock status
      bool is_locked() const;

      /// Lock 
      void lock();

      /// Unlock
      void unlock();

      /// Check if the internal data of the geiger signal is valid
      bool is_valid() const;

      /// Reset the internal data of geiger signal
      void reset();

      /// Smart print
      virtual void tree_dump(std::ostream      & a_out    = std::clog,
			     const std::string & a_title  = "",
			     const std::string & a_indent = "",
			     bool a_inherit               = false) const;

    protected : 

      /// Check if a geiger signal is available
      void _check();

    private : 

      bool _locked_; //!< geiger signal lock flag
      double _anode_avalanche_time_; //!< Time of the anode avalanche
      double _plasma_top_time_; //!< Time of plasma on top cathode
      double _plasma_bottom_time_; //!< Time of plasma on bottom cathode

      DATATOOLS_SERIALIZATION_DECLARATION();

    };

  } // end of namespace digitization

} // end of namespace snemo

#endif /* FALAISE_DIGITIZATION_PLUGIN_SNEMO_DIGITIZATION_GEIGER_SIGNAL_H */

/* 
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
