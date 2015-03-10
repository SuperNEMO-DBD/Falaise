// calo_crate_tw.h
// Author(s): Yves LEMIERE <lemiere@lpccaen.in2p3.fr>
// Author(s): Guillaume OLIVIERO <goliviero@lpccaen.in2p3.fr>
//

#ifndef FALAISE_DIGITIZATION_PLUGIN_SNEMO_DIGITIZATION_CALO_CRATE_TW_H
#define FALAISE_DIGITIZATION_PLUGIN_SNEMO_DIGITIZATION_CALO_CRATE_TW_H

// Standard library :
#include <stdexcept>
#include <iostream>
#include <stdlib.h>
#include <sstream>
#include <math.h>
#include <cmath>
#include <fstream>
#include <string>
#include <set>
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

    /// \brief The calorimeter crate trigger word
    class calo_crate_tw : public geomtools::base_hit
    {
    public : 
    
			// A revoir plus tard pour la provenance du mot de 18 bits (main wall ou x-gamma veto )
			// enum crate_type {
			// 	CRATE_INVALID = 0,
			// 	CRATE_MAIN = 1,
			// 	CRATE_XG = 2	 
			// };
			
      /// \brief Masks to select specific bits in the calo crate trigger word
      enum tw_mask_type {
				TW_HTM_PC    = datatools::bit_mask::bit00 | datatools::bit_mask::bit01,  //!< High threshold multiplicity per crate (HTM-PC)
				TW_ZONING    = datatools::bit_mask::bit02 | datatools::bit_mask::bit03 | datatools::bit_mask::bit04 | datatools::bit_mask::bit05 | datatools::bit_mask::bit06 | datatools::bit_mask::bit07 | datatools::bit_mask::bit08 | datatools::bit_mask::bit09 | datatools::bit_mask::bit10 | datatools::bit_mask::bit11, //!< Zoning word, depends on the type of the crate (10 bits) 
				TW_LTO_PC    = datatools::bit_mask::bit12, //!< Low trigger only per crate (LTO-PC)
				TW_XT_PC     = datatools::bit_mask::bit13,  //!< External trigger per crate (XT-PC)
				TW_CONTROL   = datatools::bit_mask::bit14 | datatools::bit_mask::bit15 | datatools::bit_mask::bit16 | datatools::bit_mask::bit17 //!< Control bits (4 bits)
			};

			enum tw_bit_pos{
				HTM_PC_BIT0  = 0,
				HTM_PC_BIT1  = 1,
				ZONING_BIT0  = 2,
				ZONING_BIT1  = 3,
				ZONING_BIT2  = 4,
				ZONING_BIT3  = 5,
				ZONING_BIT4  = 6,
				ZONING_BIT5  = 7,
				ZONING_BIT6  = 8,
				ZONING_BIT7  = 9,
				ZONING_BIT8  = 10,
				ZONING_BIT9  = 11,
				LTO_PC_BIT   = 12,
				XT_PC_BIT    = 13,
				CONTROL_BIT0 = 14,
				CONTROL_BIT1 = 15,
				CONTROL_BIT2 = 16,
				CONTROL_BIT3 = 17,
			};

      /// Default constructor
      calo_crate_tw();

      /// Destructor
      virtual ~calo_crate_tw();

		  /// Return the timestamp of the calo crate trigger word 
			int32_t get_clocktick_25ns() const;
			
			/// Set the timestamp of the calo crate trigger word 
      void set_clocktick_25ns(int32_t value_);

			/// Reset the timestamp of the calo crate trigger word 
			void reset_clocktick_25ns();

			/// Set the high threshold multiplicity (HTM) bits
			void set_htm_pc_info(unsigned int multiplicity_);

			/// Return the information of the multiplicity for the high threshold
			unsigned int get_htm_pc_info() const;

			/// Check if the htm bits are set
			bool is_htm() const;

			/// Return the complete zoning word
			void get_zoning_word(std::bitset<10> & zoning_word_) const;

			/// Set the zoning word 
			void set_zoning_word(std::bitset<10> & zoning_word_);

			/// Compute 
			unsigned int compute_active_zones(std::set<int> & active_zones_) const;

			/// Set the low threshold only (LTO) bit
			void set_lto_pc_bit(bool value_);

			/// Check if the LTO bit is set
			bool is_lto() const;

			/// Set the external trigger (XT) bit
			void set_xt_pc_bit(bool value_);
			
			/// Check if the XT bit is set
			bool is_xt() const;

			/// Return the complete control word
			void get_control_word(std::bitset<4> & control_word_) const;

			/// Set the complete control word
			void set_control_word(std::bitset<4> & control_word_);
 
			/// Check the lock status
      bool is_locked_ctw() const;

      /// Lock 
			void lock_ctw();

			/// Unlock
			void unlock_ctw();

			/// Reset the calorimeter crate TW bitset
			void reset_tw_bitset();

      /// Check if the internal data ofthe calo crate TW is valid
      bool is_valid() const;

      /// Reset the internal data of the calo crate TW
      void reset();

      /// Smart print
      virtual void tree_dump(std::ostream      & a_out    = std::clog,
														 const std::string & a_title  = "",
														 const std::string & a_indent = "",
														 bool a_inherit               = false) const;

		protected : 
			
			/// Check if a calo crate TW is available
			void _check();

    private : 

			bool _locked_ctw_; //!< CTW lock flag
			//	crate_type _crate_; //!< The type of the crate (from main wall or X Wall - Gamma veto)
			int32_t _clocktick_25ns_; //!< The timestamp of the trigger primitive in main clock units (40 MHz)
			std::bitset<18> _ctw_; //!< The crate trigger word

      //DATATOOLS_SERIALIZATION_DECLARATION();

    };

  } // end of namespace digitization

} // end of namespace snemo

#endif /* FALAISE_DIGITIZATION_PLUGIN_SNEMO_DIGITIZATION_CALO_CRATE_TW_H */

/* 
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
