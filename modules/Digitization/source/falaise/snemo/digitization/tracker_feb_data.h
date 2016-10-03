// snemo/digitization/tracker_feb_data.h
// Author(s): Yves LEMIERE <lemiere@lpccaen.in2p3.fr>

#ifndef TRACKER_FEB_DATA_H
#define TRACKER_FEB_DATA_H

// Standard library :
#include<string>
#include "stdint.h"
#include <bitset>

// - Bayeux/datatools :
#include <bayeux/datatools/handle.h>
#include <bayeux/datatools/utils.h>


// - Bayeux/geomtools:
#include <geomtools/manager.h>
#include <bayeux/geomtools/base_hit.h>


// This project :
#include <snemo/digitization/electronic_mapping.h>



namespace snemo {
	namespace digitization {


		class tracker_feb_data : public geomtools::base_hit{
			
			
			
		public:

					enum bit_index {
			MODE_STATUS      = 0,
			ANODIC_T0_STATUS = 1,
			ANODIC_T1_STATUS = 2,
			ANODIC_T2_STATUS = 3,
			ANODIC_T3_STATUS = 4,
			ANODIC_T4_STATUS = 5,
			CATHODIC_T5_STATUS = 6,
			CATHODIC_T6_STATUS = 7
					};
		

			

			tracker_feb_data();
			virtual void display();


			void initialize(bool operating_mode_);
 
			bool is_initialized() const;
			
			void reset();

			
			void set_header(int32_t id_,
											const geomtools::geom_id & electronic_id_);
			
			/// Set the data with valid values
			void set_data(uint64_t t0_, uint64_t t1_,uint64_t t2_, uint64_t t3_, uint64_t t4_,uint64_t t5_, uint64_t t6_);			
			
			

			bool is_nemo3_mode();
			bool is_alternative_mode();
			
			//anodic t0
			bool has_anodic_t0() const ;
			inline void set_anodic_t0(uint64_t value_){
				anodic_t0 = value_;
				//	std::clog<<"In method  : "<<std::boolalpha <<has_anodic_t0()<<std::endl;
				_data_description_.set(ANODIC_T0_STATUS,true);
				//std::clog<<"In method  : "<<std::boolalpha <<has_anodic_t0()<<std::endl;
						
				
			}
			inline uint64_t get_anodic_t0(){return anodic_t0;}

			//anodic t1
			bool has_anodic_t1() const;
			inline void set_anodic_t1(uint64_t value_){
				anodic_t1 = value_;
				_data_description_.set(ANODIC_T1_STATUS,true);
			}
			inline uint64_t get_anodic_t1(){return anodic_t1;}


			//anodic t2
			bool has_anodic_t2() const;
			inline void set_anodic_t2(uint64_t value_){
				anodic_t2 = value_;
				_data_description_.set(ANODIC_T2_STATUS,true);
			}
			inline uint64_t get_anodic_t2(){return anodic_t2;}


			//anodic t3
			bool has_anodic_t3() const ;
			inline void set_anodic_t3(uint64_t value_){
				anodic_t3 = value_;
				_data_description_.set(ANODIC_T3_STATUS,true);
			}
			inline uint64_t get_anodic_t3(){return anodic_t3;}

			//anodic t4
			bool has_anodic_t4() const;
			inline void set_anodic_t4(uint64_t value_){
				anodic_t4 = value_;
				_data_description_.set(ANODIC_T4_STATUS,true);
			}
			inline uint64_t get_anodic_t4(){return anodic_t4;}

			//cathodic t5
			bool has_cathodic_t5() const;
			inline void set_cathodic_t5(uint64_t value_){
				cathodic_t5 = value_;
				_data_description_.set(CATHODIC_T5_STATUS,true);
			}
			inline uint64_t get_cathodic_t5(){return cathodic_t5;}
			

			//cathodic t6
			bool has_cathodic_t6() const;
			inline void set_cathodic_t6(uint64_t value_){
				cathodic_t6 = value_;
				_data_description_.set(CATHODIC_T6_STATUS,true);
			}
			inline uint64_t get_cathodic_t6(){return cathodic_t6;}

			void set_anodic_times(uint64_t t0_, uint64_t t1_,uint64_t t2_, uint64_t t3_, uint64_t t4_);
			void set_cathodic_times(uint64_t t5_, uint64_t t6_);
			//void set_times(uint64_t t0_, uint64_t t1_,uint64_t t2_, uint64_t t3_, uint64_t t4_,uint64_t t5_, uint64_t t6_);


			/// Smart print
      virtual void tree_dump(std::ostream      & a_out    = std::clog,
														 const std::string & a_title  = "",
														 const std::string & a_indent = "",
														 bool a_inherit               = false) const;

			
		public : 
			
			static const unsigned int DATE_DESC_SIZE = 8;

			
			




			
						
		private:


			bool _initialized_;


			//HEADER
			//base_hit + _data_description_
			
			std::bitset<DATE_DESC_SIZE> _data_description_; //!< The tracker data description
			                                                      // LSB to  MSB
			                                                      // mode, [0/1] NEMO3/Alternative mode
			                                                      // has_anodic_t0, has_anodic_t1, has_anodic_t2, has_anodic_t3, has_anodic_t4
			                                                      // has_cathodic_t5, has_cathodic_t6 [0/1] missing/exist
			
		
			//DATA
			// 7 geiger timing
			uint64_t anodic_t0;
			uint64_t anodic_t1;
			uint64_t anodic_t2;
			uint64_t anodic_t3;
			uint64_t anodic_t4;
			uint64_t cathodic_t5;
			uint64_t cathodic_t6;
			
	



		};


} // end of namespace digitization

} // end of namespace snemo
#endif /* TRACKER_FEB_DATA_H */

/* 
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
