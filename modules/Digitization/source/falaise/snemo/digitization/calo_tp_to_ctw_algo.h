// calo_algo_tp_to_ctw.h
// Author(s): Yves LEMIERE <lemiere@lpccaen.in2p3.fr>
// Author(s): Guillaume OLIVIERO <goliviero@lpccaen.in2p3.fr>

#ifndef FALAISE_DIGITIZATION_PLUGIN_SNEMO_DIGITIZATION_CALO_ALGO_TP_TO_CTW_H
#define FALAISE_DIGITIZATION_PLUGIN_SNEMO_DIGITIZATION_CALO_ALGO_TP_TO_CTW_H

// Standard library :
#include <stdexcept>
#include <iostream>
#include <stdlib.h>
#include <sstream>
#include <cmath>
#include <fstream>
#include <string>
#include <vector>
#include <stdint.h>
#include <bitset>

// This project :
#include <snemo/digitization/calo_tp_data.h>
#include <snemo/digitization/calo_ctw_data.h>

// Third party:
// - Boost:
#include <boost/cstdint.hpp>

// - Bayeux/datatools :
#include <bayeux/datatools/handle.h>

namespace snemo {
  
  namespace digitization {		

    /// \brief Algorithm processing. Take a calo_tp_data object (collection of calo_tp) in input used to fill a calo_ctw_data (collection of calo crate trigger word) in output.
    class calo_algo_tp_to_ctw //: public datatools::i_serializable
    {
    public : 
			/// Shift for board index because there is 10FEB/1CB/10FEB in one crate
      static const unsigned int BOARD_SHIFT_INDEX = -1;

      /// Default constructor
      calo_algo_tp_to_ctw();

      /// Destructor
      virtual ~calo_algo_tp_to_ctw();

			///  
      void initialize(const calo_tp_data & my_tp_data_, calo_ctw_data & my_ctw_data_); 

			/// Set the clocktick for one ctw
			void set_ctw_clocktick(const calo_tp & my_calo_tp_, calo_ctw & my_ctw_);

			/// Set the ctw high threshold multiplicity for a given clocktick
			void set_ctw_htm(const calo_tp & my_calo_tp_, calo_ctw & my_ctw_);

			/// Set the ctw zone bit for a given clocktick 
      void set_ctw_zone_bit(const calo_tp & my_calo_tp_, calo_ctw & my_ctw_);

			/// Set the ctw low threshold only for a given clocktick 
			void set_ctw_lto(const calo_tp & my_calo_tp_, calo_ctw & my_ctw_);


			void process(const calo_tp_data & tp_data_,  calo_ctw_data & ctw_data_);


	    void process_for_a_ctw_for_a_clocktick(const std::vector<datatools::handle<calo_tp> > & my_list_of_calo_tp_,  calo_ctw & a_calo_ctw_);

			void fill_a_calo_ctw(const calo_tp & my_calo_tp_, calo_ctw & a_calo_ctw_);

    private :
      
     

    };

  } // end of namespace digitization

} // end of namespace snemo

#endif /* FALAISE_DIGITIZATION_PLUGIN_SNEMO_DIGITIZATION_CALO_CTW_DATA_H */

/* 
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
