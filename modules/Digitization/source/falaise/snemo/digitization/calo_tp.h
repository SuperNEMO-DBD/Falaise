// snemo/digitization/calo_tp.h
// Author(s): Yves LEMIERE <lemiere@lpccaen.in2p3.fr>
// Author(s): Guillaume OLIVIERO <goliviero@lpccaen.in2p3.fr>

#ifndef FALAISE_DIGITIZATION_PLUGIN_SNEMO_DIGITIZATION_CALO_TP_H
#define FALAISE_DIGITIZATION_PLUGIN_SNEMO_DIGITIZATION_CALO_TP_H

// Standard library :
#include <bitset>

// Third party:
// - Boost:
#include <boost/cstdint.hpp>
// - Bayeux/datatools :
#include <bayeux/datatools/bit_mask.h>
// - Bayeux/geomtools :
#include <bayeux/geomtools/base_hit.h>

// This project :
#include <snemo/digitization/mapping.h>

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

      /// \brief Masks to select specific bits in the trigger primitive bitset
      enum tp_mask_type {
				TP_HTM        = datatools::bit_mask::bit00 | datatools::bit_mask::bit01,  //!< High threshold multiplicity (HTM) bit mask 
				TP_LTO        = datatools::bit_mask::bit02, //!< Low threshold only (LTO) bit mask 
				TP_EXT_TRIG   = datatools::bit_mask::bit03, //!< External trigger bit mask
				TP_SPARE      = datatools::bit_mask::bit04  //!< Spare bit mask
      };

			/// Position of each bits in the TP bitset word (5 bits)
			enum tp_bit_pos {
				HTM_BIT0  = 0,
				HTM_BIT1  = 1,
				LTO_BIT   = 2,
				XT_BIT    = 3,
				SPARE_BIT = 4
			};
			
      /// Maximum number of channels by front-end board (FEB)
			static const unsigned int MAX_NUMBER_OF_CHANNELS = 16;
			
			/// Full size for the calorimter TP bitset
			static const unsigned int FULL_SIZE = 5;
			
			/// Size of htm bitset for calorimeter TP
			static const unsigned int HTM_SIZE  = 2;			

			/// Value of the low threshold to set bits
			static constexpr double LOW_THRESHOLD = 10.0; // (mV units problem maybe)

			/// Value of the low threshold to set bits
			static constexpr double HIGH_THRESHOLD = 20.0; // (mV units problem maybe)

      /// Default constructor
      calo_tp();

      /// Destructor
      virtual ~calo_tp();

			/// Set the header with valid values
			void set_header(int32_t hit_id_,
											const geomtools::geom_id & electronic_id_,
											int32_t clocktick_25ns_);

			/// Set the data with valid values
			void set_data(const double amplitude,
										const bool xt_bit_,
										const bool spare_bit);
			
			/// Update data when a second calo hit is in the same calo TP
			void update_data(const double amplitude,
											 const bool xt_bit_,
											 const bool spare_bit);
  
      /// Return the timestamp of the trigger primitive
      int32_t get_clocktick_25ns() const;

      /// Set the timestamp of the trigger primitive
      void set_clocktick_25ns(const int32_t clocktick_25_);
			
			/// Check if a clocktick is defined
			bool has_clocktick_25ns() const;

      /// Reset the timestamp of the trigger primitive
      void reset_clocktick_25ns();

      /// Return the const reference to the TP bitset
      const std::bitset<FULL_SIZE> & get_tp_bitset() const;

      /// Reset the TP bitset
      void reset_tp_bitset();
			
			/// Set the high threshold multiplicity (HTM) bits
			void set_htm(const unsigned int multiplicity_);

			/// Return the information of the multiplicity for the high threshold
			unsigned int get_htm() const;

			/// Get the high threshold multiplicity (HTM) word (2 bits)
			std::bitset<HTM_SIZE> get_htm_bits() const;
			
			/// Check if the htm bits are set 
			bool is_htm() const;

			/// Set the low threshold only (LTO) bit
			void set_lto_bit(const bool value_);

			/// Check if the LTO bit is set
			bool is_lto() const;

			/// Set the external trigger (XT) bit
			void set_xt_bit(const bool value_);

			/// Check if the XT bit is set
			bool is_xt() const;

			/// Set the spare bit
			void set_spare_bit(const bool value_);

			/// Check if the spare bit is set
			bool is_spare() const;

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

			bool _locked_;               //!< TP lock flag
      int32_t _clocktick_25ns_;    //!< The timestamp of the trigger primitive in main clock units (40 MHz)
      std::bitset<FULL_SIZE> _tp_; //!< The trigger primitive bitset

      DATATOOLS_SERIALIZATION_DECLARATION()

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
