// snemo/digitization/memory.h
// Author(s): Yves LEMIERE <lemiere@lpccaen.in2p3.fr>
// Author(s): Guillaume OLIVIERO <goliviero@lpccaen.in2p3.fr>

#ifndef FALAISE_DIGITIZATION_PLUGIN_SNEMO_DIGITIZATION_TRACKER_TRIGGER_MEM_MAKER_H
#define FALAISE_DIGITIZATION_PLUGIN_SNEMO_DIGITIZATION_TRACKER_TRIGGER_MEM_MAKER_H

#include <snemo/digitization/memory.h>

namespace snemo {

  namespace digitization {

    /// \brief Memory maker for level zero and level one of tracker trigger

    class tracker_trigger_mem_maker{

      struct zone_threshold_info 
			{
				static const int32_t INNER_LAYER_THRESHOLD = 2;
				static const int32_t OUTER_LAYER_THRESHOLD = 2;
				static const int32_t LEFT_NROWS_THRESHOLD  = 2;
				static const int32_t RIGHT_NROWS_THRESHOLD = 2;
      };

    public : 

      enum mem_type
				{
					MEM_UNDEFINED = 0,
					MEM_LVL0_LVL1 = 1,
					MEM_LVL1_LVL2 = 2
				};

      enum mem_size_type
				{
					MEM_SIZE_UNDEFINED = 0,
					MEM_SIZE_4X2 = 1,
					MEM_SIZE_5X1 = 2,
					MEM_SIZE_6X1 = 3
				};
      
      enum mem_build_algo_type
				{
					MEM_ALGO_UNDEFINED                  = 0,
					MEM_LVL0_LVL1_ALGO_MIN_MULTIPLICITY = 1,
					MEM_LVL0_LVL1_ALGO_MAX_GAP          = 2,
					MEM_LVL1_LVL2_ALGO                  = 3
				};

      /// Default constructor
      tracker_trigger_mem_maker();

      /// Destructor
      virtual ~tracker_trigger_mem_maker();
      
      /// Initialize the object
      void initialize();
      
      /// Check if the object is initialized
      bool is_initialized() const;

      /// Reset
      void reset();
      
      /// Configuration for the algorithm
      void configure(const mem_type & memory_type_, 
										 const mem_size_type & memory_size_type_,
										 const mem_build_algo_type & memory_algo_type_);
			
			void set_min_multiplicity(const unsigned int & min_multiplicity_);
			
			void set_max_gap(const unsigned int & max_gap_);

      /// Store the built map in a file
      void store(const std::string & output_file_) const;
      
    protected :
      
      /// Build algorithm in function of the configuration
      void _build();

      void _build_lvl0_lvl1_memory();

			void _build_lvl0_lvl1_5X1_memory();

			/// Build the tracker trigger level 0 to level 1 memory with a min multiplicity
			void _build_lvl0_lvl1_5X1_min_multiplicity_memory();

			void _build_lvl0_lvl1_5X1_max_gap_memory();

			void _build_lvl0_lvl1_6X1_memory();

			void _build_lvl0_lvl1_6X1_min_multiplicity_memory();
			
			void _build_lvl0_lvl1_6X1_max_gap_memory();

      void _build_lvl1_lvl2_memory();

      /// Build the tracker trigger level 1 to level 2 memory with multiplicity algo type
      void _build_lvl1_lvl2_4X2_memory();

    private : 
      // Configuration :
      bool                _initialized_;   //!< Initialized flag
      mem_type            _mem_type_;      //!< Type of memory
      mem_size_type       _mem_size_type_; //!< Size of the built memory
      mem_build_algo_type _mem_algo_type_; //!< Algorithm type to build memory
      unsigned int        _min_multiplicity_;
			unsigned int        _max_gap_;

      // Working data :
      boost::scoped_ptr<memory<4,2> > _mem_A4D2_; //!< Pointer to an A4D2 memory
      boost::scoped_ptr<memory<5,1> > _mem_A5D1_; //!< Pointer to an A5D1 memory
      boost::scoped_ptr<memory<6,1> > _mem_A6D1_; //!< Pointer to an A6D1 memory
    };

  } // end of namespace digitization

} // end of namespace snemo

#endif // FALAISE_DIGITIZATION_PLUGIN_SNEMO_DIGITIZATION_TRACKER_TRIGGER_MEM_MAKER_H
/* 
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
