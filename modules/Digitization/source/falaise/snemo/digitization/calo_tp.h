// calo_tp.h
// Author(s): Yves LEMIERE <lemiere@lpccaen.in2p3.fr>
// Author(s): Guillaume OLIVIERO <goliviero@lpccaen.in2p3.fr>
//

#ifndef FALAISE_DIGITIZATION_PLUGIN_SNEMO_DIGITIZATION_CALO_TP_H
#define FALAISE_DIGITIZATION_PLUGIN_SNEMO_DIGITIZATION_CALO_TP_H

// Standard library :
#include <stdexcept>
#include <iostream>
#include <stdlib.h>
#include <sstream>
#include <math.h>
#include <cmath>
#include <stdlib.h>
#include <fstream>
#include <string>
#include <vector>
#include <stdint.h>
#include <bitset>

// Third party:
// - Boost:
#include <boost/cstdint.hpp>

// - Bayeux/datatools :
#include <bayeux/datatools/bit_mask.h>

// - Bayeux/geomtools :
#include <bayeux/geomtools/base_hit.h>

namespace snemo {
  
  namespace digitization {

    /// \brief The calorimeter trigger primitive for a single channel
    class calo_tp : public geomtools::base_hit
    {
    public : 

      /// \brief Masks to automatically tag the attributes to be stored
      enum store_mask_type {
	STORE_CLOCKTICK_25NS = datatools::bit_mask::bit03, //!< Serialization mask for the clocktick
	STORE_TP             = datatools::bit_mask::bit04  //!< Serialization mask for the TP
      };

      /// Default constructor
      calo_tp();

      /// Destructor
      virtual ~calo_tp();
  
      /// Return the timestamp of the trigger primitive
      int32_t get_clocktick_25ns() const;

      /// Set the timestamp of the trigger primitive
      void set_clocktick_25ns(int32_t value_);

      /// Reset the timestamp of the trigger primitive
      void reset_clocktick_25ns();

      /// Return the const reference to the TP bitset
      const std::bitset<5> & get_tp() const;

      /// Return the mutable reference to the TP bitset
      std::bitset<5> & grab_tp();
      
      /// Set the TP bitset
      void set_tp(const std::bitset<5> &);

      /// Reset the TP bitset
      void reset_tp();

      /// Check if the internal data of the TP are valid
      bool is_valid() const;

      /// Reset the internal data of hit
      void reset();

      /// Smart print
      virtual void tree_dump(std::ostream      & a_out    = std::clog,
			     const std::string & a_title  = "",
			     const std::string & a_indent = "",
			     bool a_inherit               = false) const;

    private : 

      int32_t _clocktick_25ns_; //!< The timestamp of the trigger primitive in main clock units (40 MHz)
      std::bitset<5> _tp_; //!< The trigger primitive bitset

      DATATOOLS_SERIALIZATION_DECLARATION();

    };

  } // end of namespace digitization

} // end of namespace snemo

#endif /* FALAISE_DIGITIZATION_PLUGIN_SNEMO_DIGITIZATION_CALO_TP_H */

/* 
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
