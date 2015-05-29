// snemo/digitization/calo_ctw.h
// Author(s): Yves LEMIERE <lemiere@lpccaen.in2p3.fr>
// Author(s): Guillaume OLIVIERO <goliviero@lpccaen.in2p3.fr>

#ifndef FALAISE_DIGITIZATION_PLUGIN_SNEMO_DIGITIZATION_CALO_CTW_H
#define FALAISE_DIGITIZATION_PLUGIN_SNEMO_DIGITIZATION_CALO_CTW_H

// Standard library :
#include <bitset>

// Third party:
// - Boost:
#include <boost/cstdint.hpp>
// - Bayeux/datatools :
#include <bayeux/datatools/bit_mask.h>
#include <bayeux/datatools/handle.h>
// - Bayeux/geomtools :
#include <bayeux/geomtools/base_hit.h>

// This project : 
#include <snemo/digitization/mapping.h>
#include <snemo/digitization/calo_ctw_constants.h>

namespace snemo {
  
  namespace digitization {

    /// \brief The calorimeter crate trigger word (C-CTW)
    class calo_ctw : public geomtools::base_hit
    {
    public : 
			
      /// \brief Masks to automatically tag the attributes to be stored
      enum store_mask_type {
				STORE_WALL           = datatools::bit_mask::bit03,
				STORE_CLOCKTICK_25NS = datatools::bit_mask::bit04, //!< Serialization mask for the clocktick
				STORE_CTW            = datatools::bit_mask::bit05  //!< Serialization mask for the TP
      };

      /// \brief Masks to select specific bits in the calo crate trigger word
      enum tw_mask_type {
				TW_HTM_PC    = datatools::bit_mask::bit00 | datatools::bit_mask::bit01,  //!< High threshold multiplicity per crate (HTM-PC)
				TW_ZONING    = datatools::bit_mask::bit02 | datatools::bit_mask::bit03 | datatools::bit_mask::bit04 | datatools::bit_mask::bit05 | datatools::bit_mask::bit06 | datatools::bit_mask::bit07 | datatools::bit_mask::bit08 | datatools::bit_mask::bit09 | datatools::bit_mask::bit10 | datatools::bit_mask::bit11, //!< Zoning word, depends on the type of the crate (10 bits) 
				TW_LTO_PC    = datatools::bit_mask::bit12, //!< Low trigger only per crate (LTO-PC)
				TW_XT_PC     = datatools::bit_mask::bit13,  //!< External trigger per crate (XT-PC)
				TW_CONTROL   = datatools::bit_mask::bit14 | datatools::bit_mask::bit15 | datatools::bit_mask::bit16 | datatools::bit_mask::bit17 //!< Control bits (4 bits)
			};
 
      /// Default constructor
      calo_ctw();

      /// Destructor
      virtual ~calo_ctw();

			/// Set the header with valid values
			void set_header(int32_t hit_id_,
											const geomtools::geom_id & electronic_id_,	 
											int32_t clocktick_25ns_);

		  /// Return the timestamp of the calo crate trigger word 
			int32_t get_clocktick_25ns() const;
			
			/// Set the timestamp of the calo crate trigger word 
      void set_clocktick_25ns(int32_t value_);

			/// Reset the timestamp of the calo crate trigger word 
			void reset_clocktick_25ns();

			/// Set the high threshold multiplicity (HTM) bits
			void set_htm_pc(unsigned int multiplicity_);

			/// Return the information of the multiplicity for the high threshold
			unsigned int get_htm_pc_info() const;

			/// Check if the htm bits are set
			bool is_htm() const;

			/// Return the complete zoning word
			void get_zoning_word(std::bitset<calo::ctw::ZONING_BITSET_SIZE> & zoning_word_) const;

			/// Set the zoning word 
			void set_zoning_word(std::bitset<calo::ctw::ZONING_BITSET_SIZE> & zoning_word_);

			/// Set one bit of the zoning word
			void set_zoning_bit(int bit_pos_, bool value_);

			/// Compute active zones in a std::set and return the number of active zones
			unsigned int compute_active_zones(std::set<int> & active_zones_) const;

			/// Set the low threshold only (LTO) bit per crate
			void set_lto_pc_bit(bool value_);

			/// Check if the LTO bit is set
			bool is_lto() const;

			/// Set the external trigger (XT) bit
			void set_xt_pc_bit(bool value_);
			
			/// Check if the XT bit is set
			bool is_xt() const;

			/// Return the complete control word
			void get_control_word(std::bitset<calo::ctw::CONTROL_BITSET_SIZE> & control_word_) const;

			/// Set the complete control word
			void set_control_word(std::bitset<calo::ctw::CONTROL_BITSET_SIZE> & control_word_);
 
			/// Return the complete ctw word
			void get_full_word(std::bitset<calo::ctw::FULL_BITSET_SIZE> & full_word_) const;

			/// Set the complete ctw word
			void set_full_word(std::bitset<calo::ctw::FULL_BITSET_SIZE> & control_word_);
			
			/// Check the lock status
      bool is_locked() const;

      /// Lock 
			void lock();

			/// Unlock
			void unlock();

			/// Reset the calorimeter crate TW bitset
			void reset_tw_bitset();

      /// Return the internal state of validity of the calo crate TW is valid
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

			bool _locked_;            //!< CTW lock flag
			int32_t _clocktick_25ns_; //!< The timestamp of the trigger primitive in main clock units (40 MHz)
			std::bitset<calo::ctw::FULL_BITSET_SIZE> _ctw_;    //!< The crate trigger word

      DATATOOLS_SERIALIZATION_DECLARATION();

    };

  } // end of namespace digitization

} // end of namespace snemo

#endif /* FALAISE_DIGITIZATION_PLUGIN_SNEMO_DIGITIZATION_CALO_CTW_H */

/* 
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
