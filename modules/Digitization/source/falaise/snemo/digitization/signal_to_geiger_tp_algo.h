// snemo/digitization/signal_to_geiger_tp_algo.h
// Author(s): Yves LEMIERE <lemiere@lpccaen.in2p3.fr>
// Author(s): Guillaume OLIVIERO <goliviero@lpccaen.in2p3.fr>

#ifndef FALAISE_DIGITIZATION_PLUGIN_SNEMO_DIGITIZATION_SIGNAL_TO_GEIGER_TP_ALGO_H
#define FALAISE_DIGITIZATION_PLUGIN_SNEMO_DIGITIZATION_SIGNAL_TO_GEIGER_TP_ALGO_H

// Standard library :
#include <stdexcept>
#include <algorithm>

// Third party:
// - Bayeux/datatools :
#include <datatools/logger.h>
// - Bayeux/mctools:
#include <mctools/simulated_data.h>
// - Bayeux/geomtools:
#include <geomtools/manager.h>

// This project :
#include <snemo/digitization/geiger_tp_data.h>
#include <snemo/digitization/signal_data.h>
#include <snemo/digitization/ID_convertor.h>

namespace snemo {
  
  namespace digitization {		

    /// \brief Algorithm processing. Take simulated datas and fill geiger trigger primitive data object.
    class signal_to_geiger_tp_algo : boost::noncopyable
    {
    public :
			
			class signal_to_tp_working_data
			{
			public:
        signal_to_tp_working_data();
				void reset();
 				bool operator<(const signal_to_tp_working_data &) const;
     
				const geiger_signal * signal_ref;
				geomtools::geom_id    feb_id;
				int32_t               clocktick_800;
				double                shift_800;
				int                   bit_index;
			}; 
			
			typedef std::vector<signal_to_tp_working_data> working_data_collection_type;
			
			static const unsigned int TP_SIZE = 55;
			
      /// Default constructor
      signal_to_geiger_tp_algo();

      /// Destructor
      virtual ~signal_to_geiger_tp_algo();
      
      /// Initializing
      void initialize(const ID_convertor & my_ID_convertor_);

      /// Check if the algorithm is initialized 
      bool is_initialized() const;

      /// Reset the object
      void reset(); 
			
      /// Set the clocktick reference for the algorithm
      void set_clocktick_reference(int32_t clocktick_ref_);
			
      /// Set the clocktick shift
      void set_clocktick_shift(int32_t clocktick_shift_);
			
      /// Process to fill a geiger tp data object from simulated data
      int process(const signal_data & signal_data_,
									geiger_tp_data & my_geiger_tp_data_);

    protected: 

			/// Prepare the working data collection (sort by clocktick)
			int _prepare_working_data(const signal_data & signal_data_,
															 working_data_collection_type & wd_collection_);

			/// Sort working data by clocktick
			void _sort_working_data(working_data_collection_type & wd_collection_);

			/// Create geiger tp from working data collection
			int _geiger_tp_process(const working_data_collection_type & wd_collection_,
														geiger_tp_data & my_geiger_tp_data_);

      ///  Process to fill a geiger tp data object from signal data
      int _process(const signal_data & signal_data_,
									 geiger_tp_data & my_geiger_tp_data_);

    private :
      
      bool _initialized_; //!< Initialization flag
      int32_t _clocktick_ref_; //!< Clocktick reference of the algorithm
      int32_t _clocktick_shift_; //!< Clocktick shift between [0:25]
      const ID_convertor * _ID_convertor_; //!< Convert geometric ID into electronic ID
			unsigned int _active_bits_counter_[TP_SIZE];
    };

  } // end of namespace digitization

} // end of namespace snemo


#endif // FALAISE_DIGITIZATION_PLUGIN_SNEMO_DIGITIZATION_SIGNAL_TO_GEIGER_TP_ALGO_H

/* 
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
