// snemo/digitization/clock_utils.h
// Author(s): Yves LEMIERE <lemiere@lpccaen.in2p3.fr>
// Author(s): Guillaume OLIVIERO <goliviero@lpccaen.in2p3.fr>

#ifndef FALAISE_DIGITIZATION_PLUGIN_SNEMO_DIGITIZATION_CLOCK_UTILS_TP_H
#define FALAISE_DIGITIZATION_PLUGIN_SNEMO_DIGITIZATION_CLOCK_UTILS_TP_H

// - Bayeux/datatools:
#include <datatools/clhep_units.h>

// - Bayeux/GSL:
#include <mygsl/rng.h>

// - Boost:
#include <boost/cstdint.hpp>

namespace snemo {
  
  namespace digitization {
		
		class clock_utils 
		{
		public :
			
			static const int32_t NUMBER_OF_25_CLOCK_IN_800 = 32; //!< Number of 25 time clock in 800
			static const int32_t MAIN_CLOCKTICK = 25; //!< Main clocktick, 40 MHz => 25ns.
			static const int32_t TRACKER_CLOCKTICK = 800; //!< Clocktick for tracker, 800ns.
			static const int32_t INVALID_CLOCKTICK = -1; //!< Invalid value for clocktick
      
			/// Default constructor
			clock_utils();

			/// Destructor
			virtual ~clock_utils();

			/// Initializing
      void initialize();

			/// Check if the object is initialized 
			bool is_initialized();

			/// Reset the object : 
			void reset();

			/// Get the clocktick 25ns reference
			int32_t get_clocktick_25_ref();

			/// Get the clocktick 800ns reference
			int32_t get_clocktick_800_ref();

			/// Get the clocktick 25ns shift
			double get_clocktick_25_shift();

			/// Get the clocktick 800ns shift
			double get_clocktick_800_shift();

			/// Compute clockticks reference
			void compute_clockticks_ref(mygsl::rng & prng_);

		protected :
		
			/// Randomize clocktick reference 25 & 800 between [0;INT32_MAX]
			void _randomize_clockticks_ref(mygsl::rng & prng_);
			
			/// Clocktick shift uniform randomize for 25 and 800ns clockticks
			void _randomize_clockticks_shift(mygsl::rng & prng_);
		private :
			
			bool    _initialized_;         //!< Initialization flag
			int32_t _clocktick_25_ref_;    //!< Clocktick 25 ns reference
			int32_t _clocktick_800_ref_;   //!< Clocktick 800 ns reference
			double  _clocktick_25_shift_;  //!< Clocktick 25 ns shift
			double  _clocktick_800_shift_; //!< Clocktick 800 ns shift
		};
	       
  } // end of namespace digitization

} // end of namespace snemo

#endif /* FALAISE_DIGITIZATION_PLUGIN_SNEMO_DIGITIZATION_MAPPING_TP_H */

/* 
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
