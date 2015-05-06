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

      enum mem_type
	{
	  MEM_UNDEFINED = 0,
	  MEM_LVL0_LVL1 = 1
	};

      enum mem_size_type
	{
	  MEM_SIZE_UNDEFINED = 0,
	  MEM_SIZE_4X2 = 1,
	  MEM_SIZE_5X1 = 2,
	  MEM_SIZE_6X1 = 3
	};

      /// Default constructor
      tracker_trigger_mem_maker();
      
    private : 
      // Configuration :
      bool _initialized_;
      mem_type      _mem_type_;
      mem_size_type _mem_size_type_;
      std::string   _output_file_;
      // Working data :
      boost::scoped_ptr<memory<4,2> > _mem_A4D2_;
      boost::scoped_ptr<memory<5,1> > _mem_A5D1_;
      boost::scoped_ptr<memory<6,1> > _mem_A6D1_;
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
