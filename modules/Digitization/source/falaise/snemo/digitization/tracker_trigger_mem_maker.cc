// snemo/digitization/tracker_trigger_algorithm.cc
// Author(s): Yves LEMIERE <lemiere@lpccaen.in2p3.fr>
// Author(s): Guillaume OLIVIERO <goliviero@lpccaen.in2p3.fr>

// Ourselves:
#include <snemo/digitization/tracker_trigger_mem_maker.h>

namespace snemo {
  
  namespace digitization {

    tracker_trigger_mem_maker::tracker_trigger_mem_maker()
    {
      _initialized_      = false;
      _mem_type_         = MEM_UNDEFINED;
      _mem_size_type_    = MEM_SIZE_UNDEFINED;
      _mem_algo_type_    = MEM_ALGO_UNDEFINED;
      _min_multiplicity_ = -1;
      _max_gap_          = -1;
      return;
    }   

    tracker_trigger_mem_maker::~tracker_trigger_mem_maker()
    {
      reset();
      return;
    }
    
    void tracker_trigger_mem_maker::initialize()
    {
      DT_THROW_IF(_mem_type_ == MEM_UNDEFINED || _mem_size_type_ == MEM_SIZE_UNDEFINED || _mem_algo_type_ == MEM_ALGO_UNDEFINED, std::logic_error, "Configuration is not (fully) set, object can't be initialized ! ");
      _build();
      _initialized_ = true;
      return;
    }
 
    bool tracker_trigger_mem_maker::is_initialized() const
    {
      return _initialized_;
    }

    void tracker_trigger_mem_maker::reset()
    {
      _initialized_   = false;
      _mem_type_      = MEM_UNDEFINED;
      _mem_size_type_ = MEM_SIZE_UNDEFINED;
      _mem_algo_type_ = MEM_ALGO_UNDEFINED;
      return;
    }
    
 
    void tracker_trigger_mem_maker::configure(const mem_type & memory_type_, 
					      const mem_size_type & memory_size_type_, 
					      const mem_build_algo_type & memory_algo_type_)
    {
      _mem_type_      = memory_type_;
      _mem_size_type_ = memory_size_type_;
      _mem_algo_type_ = memory_algo_type_;
      return;
    }

    void tracker_trigger_mem_maker::set_min_multiplicity(const unsigned int & min_multiplicity_)
    {
      DT_THROW_IF(_mem_algo_type_ != MEM_LVL0_LVL1_ALGO_MIN_MULTIPLICITY, std::logic_error, "Memory algo type value is not good '" << _mem_algo_type_ << "' ! ");
      _min_multiplicity_ = min_multiplicity_;
      return;
    }
			
    void tracker_trigger_mem_maker::set_max_gap(const unsigned int & max_gap_)
    { 
      DT_THROW_IF(_mem_algo_type_ != MEM_LVL0_LVL1_ALGO_MAX_GAP, std::logic_error, "Memory algo type value is not good '" << _mem_algo_type_ << "' ! ");
      _max_gap_ = max_gap_;
      return;
    }

    void tracker_trigger_mem_maker::_build()
    {
      switch (_mem_type_)
	{
	case MEM_UNDEFINED :
	  DT_THROW(std::logic_error, "Undefined memory type ! ");
	  break;

	case MEM_LVL0_LVL1 :
	  _build_lvl0_lvl1_memory();
	  break;

	case MEM_LVL1_LVL2 :
	  _build_lvl1_lvl2_memory();
	  break;

	default :
	  DT_THROW(std::logic_error, "Memory type '" << _mem_type_ << "' is not supported, enter a good value ! ");
	  break;
	};
      
      return;
    }

    void tracker_trigger_mem_maker::_build_lvl0_lvl1_memory()
    {
      switch (_mem_size_type_)
	{
	case MEM_SIZE_UNDEFINED :
	  DT_THROW(std::logic_error, "Undefined memory size type ! ");
	  break;
	  
	case MEM_SIZE_5X1 : 
	  _build_lvl0_lvl1_5X1_memory();
	  break;
	  
	case MEM_SIZE_6X1 :
	  _build_lvl0_lvl1_6X1_memory();
	  break;
      
	default :
	  DT_THROW(std::logic_error, "Memory size '" << _mem_size_type_ << "' is not supported, enter a good value ! ");
	  break;
	};
      
      return;
    }

    void tracker_trigger_mem_maker::_build_lvl0_lvl1_5X1_memory()
    {
      switch (_mem_algo_type_)
	{
	case MEM_ALGO_UNDEFINED :
	  DT_THROW(std::logic_error, "Undefined memory algorithm type ! ");
	  break;
	case MEM_LVL0_LVL1_ALGO_MIN_MULTIPLICITY : 
	  _build_lvl0_lvl1_5X1_min_multiplicity_memory();
	  break;
	  
	case MEM_LVL0_LVL1_ALGO_MAX_GAP :
	  _build_lvl0_lvl1_5X1_max_gap_memory();
	  break;
	  
	default :
	  DT_THROW(std::logic_error, "Memory algo type '" << _mem_algo_type_ << "' is not supported, enter a good value ! ");
	  break;
	};
      
      return;
    }

    void tracker_trigger_mem_maker::_build_lvl0_lvl1_5X1_min_multiplicity_memory()
    { 
      DT_THROW_IF(_min_multiplicity_ < 0 || _min_multiplicity_ > 5, std::logic_error, "Minimum multiplicity value is not set or valid '" << _min_multiplicity_ << "' ! ");
      if (_mem_A5D1_.get() == 0)
	{
	  _mem_A5D1_.reset(new memory<5,1>);	
	}
      
      for (unsigned int i = 0; i < 32; i++)
	{   
	  std::bitset<5> my_address_bitset;
	  std::bitset<1> my_data_bitset;
	  my_address_bitset = i;
	  if (my_address_bitset.count() < _min_multiplicity_)
	    {
	      my_data_bitset = 0;
	    }
	  else
	    {  
	      my_data_bitset = 1;
	    }

	  _mem_A5D1_->push(i, my_data_bitset);
	}
      return;
    }

    void tracker_trigger_mem_maker::_build_lvl0_lvl1_5X1_max_gap_memory()
    {
      DT_THROW_IF(_max_gap_ < 0 || _max_gap_ > 5, std::logic_error, "Max gap value is not set or valid '" << _max_gap_ << "' ! ");

      if (_mem_A5D1_.get() == 0)
	{
	  _mem_A5D1_.reset(new memory<5,1>);	
	}

      for (unsigned int i = 0; i < 32; i++)
	{   
	  std::bitset<5> my_address_bitset;
	  std::bitset<1> my_data_bitset;
	  my_address_bitset = i;
	  if (my_address_bitset.count() < 2)
	    {
	      my_data_bitset = 0;
	    }
	  else
	    {  
	      std::vector<unsigned int> bit_activated_index;
	      unsigned int bitset_min_gap = -1;
	      for (unsigned int j = 0; j < 5; j++)
		{
		  if (my_address_bitset.test(j) == true)
		    {
		      bit_activated_index.push_back(j+1);
		    }
		}
	    
	      if (bit_activated_index.size() > 1)
		{
		  unsigned int size = bit_activated_index.size();
		  for (int j = 0; j < bit_activated_index.size(); j++)
		    {
		      unsigned int activated_bit_index_max = bit_activated_index[j+1];
		      unsigned int activated_bit_index_current = bit_activated_index[j];
		      if (activated_bit_index_max != activated_bit_index_current)
			{			  
			  unsigned int gap_between_2_activated_bits = activated_bit_index_max - activated_bit_index_current - 1;	  
			  if (gap_between_2_activated_bits < bitset_min_gap)
			    {
			      bitset_min_gap = gap_between_2_activated_bits;
			    }    
			}

		      if (bitset_min_gap <= _max_gap_)
			{
			  my_data_bitset = 1;
			}
		    }
		}
	    }
	    
	  _mem_A5D1_->push(i, my_data_bitset);
	}
      return;
    }

    void tracker_trigger_mem_maker::_build_lvl0_lvl1_6X1_memory()
    {
      switch (_mem_algo_type_)
	{
	case MEM_ALGO_UNDEFINED :
	  DT_THROW(std::logic_error, "Undefined memory algorithm type ! ");
	  break;
	case MEM_LVL0_LVL1_ALGO_MIN_MULTIPLICITY : 
	  _build_lvl0_lvl1_6X1_min_multiplicity_memory();
	  break;
	  
	case MEM_LVL0_LVL1_ALGO_MAX_GAP :
	  _build_lvl0_lvl1_6X1_max_gap_memory();
	  break;
	  
	default :
	  DT_THROW(std::logic_error, "Memory algo type '" << _mem_algo_type_ << "' is not supported, enter a good value ! ");
	  break;
	};
      
      return;
    }

    void tracker_trigger_mem_maker::_build_lvl0_lvl1_6X1_min_multiplicity_memory()
    { 
      DT_THROW_IF(_min_multiplicity_ < 0 || _min_multiplicity_ > 6, std::logic_error, "Minimum multiplicity value is not set or valid '" << _min_multiplicity_ << "' ! ");
      if (_mem_A6D1_.get() == 0)
	{
	  _mem_A6D1_.reset(new memory<6,1>);	
	}
      
      for (unsigned int i = 0; i < 64; i++)
	{   
	  std::bitset<6> my_address_bitset;
	  std::bitset<1> my_data_bitset;
	  my_address_bitset = i;
	  if (my_address_bitset.count() < _min_multiplicity_)
	    {
	      my_data_bitset = 0;
	    }
	  else
	    {  
	      my_data_bitset = 1;
	    }

	  _mem_A6D1_->push(i, my_data_bitset);
	}
      return;
      return;
    }

    void tracker_trigger_mem_maker::_build_lvl0_lvl1_6X1_max_gap_memory()
    {

    DT_THROW_IF(_max_gap_ < 0 || _max_gap_ > 6, std::logic_error, "Max gap value is not set or valid '" << _max_gap_ << "' ! ");

      if (_mem_A6D1_.get() == 0)
	{
	  _mem_A6D1_.reset(new memory<6,1>);	
	}

      for (unsigned int i = 0; i < 64; i++)
	{   
	  std::bitset<6> my_address_bitset;
	  std::bitset<1> my_data_bitset;
	  my_address_bitset = i;
	  if (my_address_bitset.count() < 2)
	    {
	      my_data_bitset = 0;
	    }
	  else
	    {  
	      std::vector<unsigned int> bit_activated_index;
	      unsigned int bitset_min_gap = -1;
	      for (unsigned int j = 0; j < 6; j++)
		{
		  if (my_address_bitset.test(j) == true)
		    {
		      bit_activated_index.push_back(j+1);
		    }
		}
	    
	      if (bit_activated_index.size() > 1)
		{
		  unsigned int size = bit_activated_index.size();
		  for (int j = 0; j < bit_activated_index.size(); j++)
		    {
		      unsigned int activated_bit_index_max = bit_activated_index[j+1];
		      unsigned int activated_bit_index_current = bit_activated_index[j];
		      if (activated_bit_index_max != activated_bit_index_current)
			{			  
			  unsigned int gap_between_2_activated_bits = activated_bit_index_max - activated_bit_index_current - 1;	  
			  if (gap_between_2_activated_bits < bitset_min_gap)
			    {
			      bitset_min_gap = gap_between_2_activated_bits;
			    }    
			}

		      if (bitset_min_gap <= _max_gap_)
			{
			  my_data_bitset = 1;
			}
		    }
		}
	    }
	    
	  _mem_A6D1_->push(i, my_data_bitset);
	}
      return;
    }


    void tracker_trigger_mem_maker::_build_lvl1_lvl2_memory()
    {
      switch (_mem_algo_type_)
	{
	case MEM_ALGO_UNDEFINED :
	  DT_THROW(std::logic_error, "Undefined memory algorithm type ! ");
	  break;
	  
	case MEM_LVL1_LVL2_ALGO : 
	  _build_lvl1_lvl2_4X2_memory();
	  break;
      
	default :
	  DT_THROW(std::logic_error, "Memory algo type '" << _mem_algo_type_ << "' is not supported, enter a good value ! ");
	  break;
	};
      
      return;
    }

    void tracker_trigger_mem_maker::_build_lvl1_lvl2_4X2_memory()
    {

      static const std::string VOID       = "00";
      static const std::string PRE_TRACK  = "11";
      static const std::string FULL_TRACK = "01";

      if (_mem_A4D2_.get() == 0)
	{
	  _mem_A4D2_.reset(new memory<4,2>);	
	}

      for (unsigned int i = 0; i < 16; i++)
	{   
	  std::bitset<4> my_address_bitset;
	  std::bitset<2> my_data_bitset;
	  my_address_bitset = i;
	  if (my_address_bitset.count() == 0)
	    {
	      my_data_bitset = std::bitset<2> (VOID);
	    }
	  else if (my_address_bitset.count() == 1)
	    {  
	      my_data_bitset = std::bitset<2> (PRE_TRACK);
	    }
	  else
	    {
	      my_data_bitset = std::bitset<2> (FULL_TRACK);
	    }
	  _mem_A4D2_->push(i, my_data_bitset);
	}
      
      return;
    }

    void tracker_trigger_mem_maker::store(const std::string & output_file_,
					  const std::string & description_) const
    {
      DT_THROW_IF(!is_initialized(), std::logic_error, "Object is not initialized, store can't be done ! ");
      std::string description;
      if (!description_.empty())
	{
	  description = description_;
	}
      if (_mem_A4D2_.get() != 0)
	{
      	  _mem_A4D2_->store_to_file(output_file_, description);
	}
     if (_mem_A5D1_.get() != 0)
	{
      	  _mem_A5D1_->store_to_file(output_file_, description);
	}
     if (_mem_A6D1_.get() != 0)
	{
      	  _mem_A6D1_->store_to_file(output_file_, description);
	}
      return;
    }

  } // end of namespace digitization

} // end of namespace snemo
