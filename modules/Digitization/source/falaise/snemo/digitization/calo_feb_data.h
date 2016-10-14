// snemo/digitization/calo_feb_data.h
// Author(s): Yves LEMIERE <lemiere@lpccaen.in2p3.fr>

#ifndef CALO_FEB_DATA_H
#define CALO_FEB_DATA_H

// Standard library :
#include<string>
#include "stdint.h"
#include <bitset>
#include <vector>


// - Bayeux/datatools :
#include <bayeux/datatools/handle.h>
#include <bayeux/datatools/utils.h>

// - Bayeux/geomtools:
#include <geomtools/manager.h>
#include <bayeux/geomtools/base_hit.h>


// This project :
///////#include <snemo/digitization/electronic_mapping.h>



namespace snemo {
	namespace digitization {


		class calo_feb_data : public geomtools::base_hit{
			
		private:

			void set_address(uint16_t rack_, uint16_t crate_, uint16_t board_, uint16_t samlong_);
			
		public:


			enum desc_bit_index {
				WAVEFORM_STATUS      = 0
			};
		
			enum treshold_bit_index {
				LOW_TRESHOLD      = 0,
				HIGH_TRESHOLD     = 1
			};
		
			
			calo_feb_data();
			
			void initialize(bool has_waveform_, unsigned int nb_sample_ = 0, unsigned int first_sample_ = 0);

			bool is_initialized() const;
			void reset();
			

			void set_baseline_per_channel(double value_, int channel_);
			inline double get_baseline_per_channel(int channel_){return _baseline[channel_];}
			
			void set_charge_per_channel(double value_, int channel_);
			inline double get_charge_per_channel(int channel_){return _charge[channel_];}

			
			void set_max_per_channel(double value_, int channel_);
			inline double get_max_per_channel(int channel_){return _max_peak_amplitude[channel_];}

			
			void set_time_max_per_channel(uint16_t value_, int channel_);
			inline uint16_t get_time_max_per_channel(int channel_){return _time_max_peak_amplitude[channel_];}
			
			
			void set_falling_per_channel(double value_, int channel_);
			inline double get_falling_per_channel(int channel_){return _falling_time[channel_];}

			
			void set_rising_per_channel(double value_, int channel_);
			inline double get_rising_per_channel(int channel_){return _rising_time[channel_];}


			void set_detected_treshold_per_channel(bool lt_,bool ht_, int channel_);

			inline bool has_low_treshsold_per_channel(int channel_){return _treshold_status_[channel_].test(LOW_TRESHOLD);}
			inline bool has_high_treshsold_per_channel(int channel_){return _treshold_status_[channel_].test(HIGH_TRESHOLD);}
			

			
			
			// void set_header(int32_t id_,
			// 								const geomtools::geom_id & electronic_id_,
			// 								uint64_t data_time_
			// 								);

			void set_header(int32_t id_,
											uint16_t rack_, uint16_t crate_, uint16_t board_, uint16_t samlong_,
											uint64_t data_time_
											);
			
			
			
			/// Set the metadata with valid values for both channels
			void set_metadata(double *baseline_array_,
												double *charge_array_,
												double *max_array_,
												uint16_t *time_max_array_,
												double *falling_array_,
												double *rising_array_
												);			
			
			/// Set the data with valid values for both channels

			void set_data(std::vector<uint16_t> *wvfrm_array_);
			
			/// Smart print
      virtual void tree_dump(std::ostream      & a_out    = std::clog,
														 const std::string & a_title  = "",
														 const std::string & a_indent = "",
														 bool a_inherit               = false) const;

			
		public : 
			
			static const unsigned int DATA_DESC_SIZE      = 1;
			static const unsigned int NB_OF_CHANNEL       = 2;
			static const unsigned int NB_OF_TRESHOLD      = 2;
			static const unsigned int MAX_NB_OF_SAMPLE    = 1024;

			static const unsigned int MAX_SAMPLE_VALUE    = 4096;//2^12
			
			static const unsigned int MAX_RACK_INDEX      = 5;
			static const unsigned int MAX_CRATE_INDEX     = 2;
			static const unsigned int MAX_BOARD_INDEX     = 20;
			static const unsigned int MAX_SAMLONG_INDEX   = 15;

			
		private:


			bool _initialized_;
			uint64_t _data_timestamp;

			//HEADER
			//id_ + _data_timestamp + _threshold_status_ + _data_description_ + address(_rack_id, _crate_id,_board_id,_samlong_id)
			
			std::bitset<DATA_DESC_SIZE> _data_description_; //!< The calo data description
			                                                      // LSB to  MSB
			std::bitset<NB_OF_TRESHOLD> _treshold_status_[NB_OF_CHANNEL];

			uint16_t _rack_id;
			uint16_t _crate_id;
			uint16_t _board_id;
			uint16_t _samlong_id;


			
			//METADATA
			// 
			double _baseline[NB_OF_CHANNEL];
			double _charge[NB_OF_CHANNEL];
			double _max_peak_amplitude[NB_OF_CHANNEL];
			uint16_t _time_max_peak_amplitude[NB_OF_CHANNEL];
			double _falling_time[NB_OF_CHANNEL];
			double _rising_time[NB_OF_CHANNEL];


			//DATA
			//
			std::vector<uint16_t> _waveform[NB_OF_CHANNEL];

			
			//INIT CONFIG
			unsigned int _number_of_sample;
			unsigned int _first_waveform_sample;
	



		};


} // end of namespace digitization

} // end of namespace snemo
#endif /* CALO_FEB_DATA_H */

/* 
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
