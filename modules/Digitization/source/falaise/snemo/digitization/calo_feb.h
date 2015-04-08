// snemo/digitization/calo_feb.h
// Author(s): Yves LEMIERE <lemiere@lpccaen.in2p3.fr>
// Author(s): Guillaume OLIVIERO <goliviero@lpccaen.in2p3.fr>

#ifndef FALAISE_DIGITIZATION_PLUGIN_SNEMO_DIGITIZATION_CALO_FEB_H
#define FALAISE_DIGITIZATION_PLUGIN_SNEMO_DIGITIZATION_CALO_FEB_H

// Standard library :
#include <iostream>
#include <string>

// Third party:
// - Boost:
#include <boost/cstdint.hpp>

// This project :
#include <snemo/digitization/mapping.h>

namespace snemo {
  
  namespace digitization {

    /// \brief The calorimeter trigger primitive for a single channel
    class calo_feb
    {
    public : 
      
      /// Default constructor
      calo_feb();

      /// Destructor
      virtual ~calo_feb();

      /// Set the header with valid values
      void set_header();

      /// Set the data with valid values
      void set_data();

      /// Initializing
      void initialize();

      /// Check if the algorithm is initialized 
      bool is_initialized() const;

      /// Check the lock status
      bool is_locked() const;

      /// Lock 
      void lock();

      /// Unlock
      void unlock();

      /// Check if the internal data of the TP is valid
      bool is_valid() const;

      /// Reset the internal data of hit
      void reset();

      /// Smart print
      virtual void tree_dump(std::ostream      & a_out    = std::clog,
			     const std::string & a_title  = "",
			     const std::string & a_indent = "",
			     bool a_inherit               = false) const;

    protected : 

      /// Check if a calo TP is available
      void _check();

    private : 

      bool _locked_; //!< TP lock flag
      bool _initialized_; //!< Initialization flag
      // DATATOOLS_SERIALIZATION_DECLARATION();

    };

  } // end of namespace digitization

} // end of namespace snemo

#endif /* FALAISE_DIGITIZATION_PLUGIN_SNEMO_DIGITIZATION_CALO_FEB_H */

/* 
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
