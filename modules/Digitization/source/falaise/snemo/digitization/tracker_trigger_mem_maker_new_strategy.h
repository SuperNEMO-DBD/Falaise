// snemo/digitization/tracker_trigger_mem_maker_new_strategy.h
// Author(s): Yves LEMIERE <lemiere@lpccaen.in2p3.fr>
// Author(s): Guillaume OLIVIERO <goliviero@lpccaen.in2p3.fr>

#ifndef FALAISE_DIGITIZATION_PLUGIN_SNEMO_DIGITIZATION_TRACKER_TRIGGER_MEM_MAKER_NEW_STRATEGY_H
#define FALAISE_DIGITIZATION_PLUGIN_SNEMO_DIGITIZATION_TRACKER_TRIGGER_MEM_MAKER_NEW_STRATEGY_H

// Third part :
// - Boost :
#include <boost/scoped_ptr.hpp>

// Ourselves :
#include <snemo/digitization/memory.h>

namespace snemo {

  namespace digitization {

    /// \brief Memory maker for level zero and level one of tracker trigger

    class tracker_trigger_mem_maker_new_strategy{

    public : 

      enum classification_type {
        NO_VTRACK    = 0x0, //!< No track detected    : 00 
        INNER_VTRACK = 0x1, //!< Inner track detected : 01
        OUTER_VTRACK = 0x2, //!< Outer track detected : 10
        FULL_VTRACK  = 0x3, //!< Full track detected  : 11

        NO_HTRACK           = 0x0, //!< No track detected           : 00
        NARROW_RIGHT_HTRACK = 0x1, //!< Right narrow track detected : 01
        NARROW_LEFT_HTRACK  = 0x2, //!< Left narrow track detected  : 10
        WIDE_HTRACK         = 0x3  //!< Wide track detected         : 11
      };

      typedef memory<9,2> mem1_type; // Sliding Zone Vertical memory
      typedef memory<8,2> mem2_type; // Sliding Zone Horizontal memory
      typedef memory<8,2> mem3_type; // Zone Vertical memory
      typedef memory<6,3> mem4_type; // Zone Horizontal memory
      typedef memory<8,3> mem5_type; // Zone Vertical for horizontal memory
			
      enum mem_type
				{
					MEM_UNDEFINED = 0,
					MEM1 = 1,
					MEM2 = 2,			
					MEM3 = 3,
					MEM4 = 4,
					MEM5 = 5,
				};

      /// Default constructor
      tracker_trigger_mem_maker_new_strategy();

      /// Destructor
      virtual ~tracker_trigger_mem_maker_new_strategy();
      
      /// Initialize the object
      void initialize();
      
      /// Check if the object is initialized
      bool is_initialized() const;

      /// Reset
      void reset();
      
      /// Configuration for the algorithm
      void configure(const mem_type & memory_type_);
			
      /// Store the built map in a file
      void store_to_file(const std::string & output_file_,
												 const std::string & description_) const;
      
    protected :
      
      /// Build algorithm in function of the configuration
      void _build();

			/// Build for memory 1 : Sliding Zone Vertical memory
      void _build_mem1();

			/// Build for memory 2 : Sliding Zone Horizontal memory
      void _build_mem2();

			/// Build for memory 3 : Zone Vertical memory
      void _build_mem3();

			/// Build for memory 4 : Zone Horizontal memory
      void _build_mem4();

			/// Build for memory 5 : Zone Vertical for horizontal memory
      void _build_mem5();

    private : 

      // Configuration :
      bool        _initialized_;     //!< Initialized flag
      mem_type    _mem_type_;        //!< Type of memory

      // Working data :
			boost::scoped_ptr<mem1_type> _mem1_;
			boost::scoped_ptr<mem2_type> _mem2_;
			boost::scoped_ptr<mem3_type> _mem3_;
			boost::scoped_ptr<mem4_type> _mem4_;
			boost::scoped_ptr<mem5_type> _mem5_;
    };

  } // end of namespace digitization

} // end of namespace snemo

#endif // FALAISE_DIGITIZATION_PLUGIN_SNEMO_DIGITIZATION_TRACKER_TRIGGER_MEM_MAKER_NEW_STRATEGY_H
/* 
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
