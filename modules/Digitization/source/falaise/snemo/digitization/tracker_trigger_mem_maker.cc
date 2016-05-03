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
      return;
    }   

    tracker_trigger_mem_maker::~tracker_trigger_mem_maker()
    {
      reset();
      return;
    }
    
    void tracker_trigger_mem_maker::initialize()
    {
      DT_THROW_IF(_mem_type_ == MEM_UNDEFINED, std::logic_error, "Configuration is not (fully) set, object can't be initialized ! ");
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
      return;
    }    
 
    void tracker_trigger_mem_maker::configure(const mem_type & memory_type_)
    {
      _mem_type_      = memory_type_;
      return;
    }

    void tracker_trigger_mem_maker::_build()
    {
      switch (_mem_type_)
	{
	case MEM_UNDEFINED :
	  DT_THROW(std::logic_error, "Undefined memory type ! ");
	  break;

	case MEM1 :
	  _build_mem1();
	  break;

	case MEM2 :
	  _build_mem2();
	  break;

	case MEM3 :
	  _build_mem3();
	  break;

	case MEM4 :
	  _build_mem4();
	  break;

	case MEM5 :
	  _build_mem5();
	  break;

	default :
	  DT_THROW(std::logic_error, "Memory type '" << _mem_type_ << "' is not supported, enter a good value ! ");
	  break;
	};
      
      return;
    }

    void tracker_trigger_mem_maker::_build_mem1()
    {
      if (_mem1_.get() == 0) _mem1_.reset(new mem1_type);

      std::size_t vfull_layer_min  = 0;
      std::size_t vfull_layer_max  = 8;
      std::size_t vfull_mult_min   = 6;
      std::size_t vfull_mult_max   = 9;
      std::size_t vinner_layer_min = 0;
      std::size_t vinner_layer_max = 5;
      std::size_t vinner_mult_min  = 3;
      std::size_t vinner_mult_max  = 6;
      std::size_t vouter_layer_min = 4;
      std::size_t vouter_layer_max = 8;
      std::size_t vouter_mult_min  = 3;
      std::size_t vouter_mult_max  = 5;

      for (unsigned long addr = 0; addr < _mem1_->get_number_of_addresses(); addr++) {
        mem1_type::address_type address = addr;

        // Default NO_VTRACK pattern:
        classification_type clsf = NO_VTRACK;

        // Search for OUTER_VTRACK pattern:
        {
          std::size_t vouter_count = 0;
          for (std::size_t ilayer = vouter_layer_min; ilayer <= vouter_layer_max; ilayer++) {
            if (address.test(ilayer)) vouter_count++;
          }
          if (vouter_count >= vouter_mult_min && vouter_count <= vouter_mult_max) {
            clsf = OUTER_VTRACK;
          }
        }

        // Search for INNER_VTRACK pattern:
        {
          std::size_t vinner_count = 0;
          for (std::size_t ilayer = vinner_layer_min; ilayer <= vinner_layer_max; ilayer++) {
            if (address.test(ilayer)) vinner_count++;
          }
          if (vinner_count >= vinner_mult_min && vinner_count <= vinner_mult_max) {
            clsf = INNER_VTRACK;
          }
        }

        // Search for FULL_VTRACK pattern:
        {
          std::size_t vfull_count = 0;
          for (std::size_t ilayer = vfull_layer_min; ilayer <= vfull_layer_max; ilayer++) {
            if (address.test(ilayer)) vfull_count++;
          }
          if (vfull_count >= vfull_mult_min && vfull_count <= vfull_mult_max) {
            clsf = FULL_VTRACK;
          }
        }
        mem1_type::data_type data = clsf;
        _mem1_->push(address, data);
      }
      //      _mem1_->memory_map_display();
      
      return;
    }
    
    void tracker_trigger_mem_maker::_build_mem2()
    {
      if (_mem2_.get() == 0) _mem2_.reset(new mem2_type);

      std::size_t wide_row_min          = 0;
      std::size_t wide_row_max          = 7;
      std::size_t wide_mult_min         = 6;
      std::size_t wide_mult_max         = 8;
      std::size_t narrow_left_row_min   = 0;
      std::size_t narrow_left_row_max   = 4;
      std::size_t narrow_left_mult_min  = 3;
      std::size_t narrow_left_mult_max  = 5;
      std::size_t narrow_right_row_min  = 3;
      std::size_t narrow_right_row_max  = 7;
      std::size_t narrow_right_mult_min = 3;
      std::size_t narrow_right_mult_max = 5;

      for (unsigned long addr = 0; addr < _mem2_->get_number_of_addresses(); addr++) {
        mem2_type::address_type address = addr;
        // Default NO_TRACK pattern:
        classification_type clsf = NO_HTRACK;
        std::size_t full_count = 0;
        for (int irow = 0; irow < 8; irow++) {
          if (address.test(irow)) {
            full_count++;
          }
        }
        // Search for patterns:
        std::string addr_str = address.to_string();
        std::string left_str = addr_str;
        std::reverse(left_str.begin(), left_str.end());
        std::string right_str = addr_str;

        // Left:
        std::size_t lpos0 = left_str.find("1111");
        std::size_t lpos1 = left_str.find("111");
        std::size_t lpos2 = left_str.find("1101");
        std::size_t lpos3 = left_str.find("1011");
        bool left = false;
        if (!left && lpos0 != left_str.npos) {
          if (lpos0 <= 2) {
            left = true;
          }
        }
        if (!left && lpos1 != left_str.npos) {
          if (lpos1 <= 2) {
            left = true;
          }
        }
        if (!left && lpos2 != left_str.npos) {
          if (lpos2 <= 2) {
            left = true;
          }
        }
        if (!left && lpos3 != left_str.npos) {
          if (lpos3 <= 2) {
            left = true;
          }
        }

        // Right:
        std::size_t rpos0 = right_str.find("1111");
        std::size_t rpos1 = right_str.find("111");
        std::size_t rpos2 = right_str.find("1101");
        std::size_t rpos3 = right_str.find("1011");
        bool right = false;
        if (!right && rpos0 != right_str.npos) {
          if (rpos0 <= 1) { // Trick to favor left classification
            right = true;
          }
        }
        if (!right && rpos1 != right_str.npos) {
          if (rpos1 <= 2) {
            right = true;
          }
        }
        if (!right && rpos2 != right_str.npos) {
          if (rpos2 <= 1) {
            right = true;
          }
        }
        if (!right && rpos3 != right_str.npos) {
          if (rpos3 <= 1) {
            right = true;
          }
        }

        char c12n2 = ' ';
        if (c12n2 == ' ' && full_count >= 6) c12n2 = 'W';
        if (c12n2 == ' ' && left && right) c12n2 = 'W';
        if (c12n2 == ' ' && left && ! right) c12n2 = 'L';
        if (c12n2 == ' ' && ! left && right) c12n2 = 'R';
        if (c12n2 == ' ') c12n2 = 'V';

        if (full_count < 6 && c12n2 == 'W') {
          if (right_str.substr(0,2) == "00" && left_str.substr(0,2) != "00") c12n2 = 'L';
          if (left_str.substr(0,2) == "00" && right_str.substr(0,2) != "00") c12n2 = 'R';
          if (left_str.substr(0,2) == "00" && right_str.substr(0,2) == "00") c12n2 = 'L';
        }

        if (c12n2 == 'V') {
          clsf = NO_HTRACK;
        } else if (c12n2 == 'R') {
          clsf = NARROW_RIGHT_HTRACK;
        } else if (c12n2 == 'L') {
          clsf = NARROW_LEFT_HTRACK;
        } else if (c12n2 == 'W') {
          clsf = WIDE_HTRACK;
        }
        mem2_type::data_type data = clsf;
        _mem2_->push(address, data);
      }
 
      return;
    }    

    void tracker_trigger_mem_maker::_build_mem3()
    {
      if (_mem3_.get() == 0) _mem3_.reset(new mem3_type);

      std::size_t szD_BIT_INNER = 0;
      std::size_t szD_BIT_OUTER = 1;
      std::size_t szC_BIT_INNER = 2;
      std::size_t szC_BIT_OUTER = 3;
      std::size_t szB_BIT_INNER = 4;
      std::size_t szB_BIT_OUTER = 5;
      std::size_t szA_BIT_INNER = 6;
      std::size_t szA_BIT_OUTER = 7;
  
      std::size_t data_BIT_INNER = 0;
      std::size_t data_BIT_OUTER = 1;

      bool debug = false;

      //                  O I O I O I O I
      // bitset address : 7 6 5 4 3 2 1 0
      //   Sliding Zone : A A B B C C D D
      
      for (unsigned long addr = 0; addr < _mem3_->get_number_of_addresses(); addr++) 
	{
	  
	  mem3_type::address_type address = addr; 

	  // Default VVOID pattern:
	  mem3_type::data_type data = NO_VTRACK;

	  if (address.test(szB_BIT_INNER) || address.test(szC_BIT_INNER) || address.test(szD_BIT_INNER) || address.test(szA_BIT_INNER)) data.set(data_BIT_INNER);
	  if (address.test(szB_BIT_OUTER) || address.test(szC_BIT_OUTER) || address.test(szD_BIT_OUTER) || address.test(szA_BIT_OUTER)) data.set(data_BIT_OUTER);

	  if (address.to_string() == "10000001") {
	    data.set(data_BIT_INNER);
	    data.set(data_BIT_OUTER, 0);
	  }

	  if (address.to_string() == "01000010") {
	    data.set(data_BIT_INNER);
	    data.set(data_BIT_OUTER, 0);
	  }
	  _mem3_->push(address, data);
        }   
      return;
    }

    void tracker_trigger_mem_maker::_build_mem4()
    {
      if (_mem4_.get() == 0) _mem4_.reset(new mem4_type);

      std::size_t ADDR_szD_BIT_LEFT   = 0;
      std::size_t ADDR_szC_BIT_RIGHT  = 1;
      std::size_t ADDR_szC_BIT_LEFT   = 2;
      std::size_t ADDR_szB_BIT_RIGHT  = 3;
      std::size_t ADDR_szB_BIT_LEFT   = 4;
      std::size_t ADDR_szA_BIT_RIGHT  = 5;  

      std::size_t DATA_BIT_RIGHT  = 0;
      std::size_t DATA_BIT_MIDDLE = 1;
      std::size_t DATA_BIT_LEFT   = 2;

      //                   R L R L R L 
      // bitset address :  5 4 3 2 1 0
      //   Sliding Zone :  A B B C C D   
      // A6 D3 memory   :
      //                    L M R
      //    bitset data :   2 1 0

      for (unsigned long addr = 0; addr < _mem4_->get_number_of_addresses(); addr++) 
	{
	  mem4_type::address_type address = addr;
	  mem4_type::data_type data = 0x0;

	  // Default NO_HTRACK pattern:   
	  classification_type szA_clsf = NO_HTRACK;
	  classification_type szB_clsf = NO_HTRACK;
	  classification_type szC_clsf = NO_HTRACK;
	  classification_type szD_clsf = NO_HTRACK;
	  
	  std::string addr_str = address.to_string();  

	  if (address.test(ADDR_szA_BIT_RIGHT) || address.test(ADDR_szB_BIT_LEFT)) data.set(DATA_BIT_LEFT);
	  if (address.test(ADDR_szB_BIT_RIGHT) || address.test(ADDR_szC_BIT_LEFT)) data.set(DATA_BIT_MIDDLE);
	  if (address.test(ADDR_szC_BIT_RIGHT) || address.test(ADDR_szD_BIT_LEFT)) data.set(DATA_BIT_RIGHT);

	  _mem4_->push(address, data);
	  std::string data_str = data.to_string();
	}
      
      return;
    }

    void tracker_trigger_mem_maker::_build_mem5()
    {
      if (_mem5_.get() == 0) _mem5_.reset(new mem5_type);

      std::size_t szD_BIT_INNER = 0;
      std::size_t szD_BIT_OUTER = 1;
      std::size_t szC_BIT_INNER = 2;
      std::size_t szC_BIT_OUTER = 3;
      std::size_t szB_BIT_INNER = 4;
      std::size_t szB_BIT_OUTER = 5;
      std::size_t szA_BIT_INNER = 6;
      std::size_t szA_BIT_OUTER = 7; 

      std::size_t data_BIT_RIGHT  = 0;
      std::size_t data_BIT_MIDDLE = 1;
      std::size_t data_BIT_LEFT   = 2;

      // This memory is build with the reverse logic, data is set to 111
      // Vertical address:
      //                   O I O I O I O I             L M  R
      // vertical bitset : 7 6 5 4 3 2 1 0 ---> data : 2 1  0
      //   Sliding Zone  : A A B B C C D D             A BC D

      for (unsigned long addr = 0; addr < _mem5_->get_number_of_addresses(); addr++) 
	{
	  mem5_type::address_type address = addr;
	  // Reverse logic, data is set to 111
	  mem5_type::data_type data = 0x7;

	  // We only see when data is not left, not middle or not right

	  if (!(address.test(szA_BIT_INNER) ||
		address.test(szA_BIT_OUTER)))
	    {
	      data.set(data_BIT_LEFT, false);
	    }

	  if (!(address.test(szD_BIT_INNER) ||
		address.test(szD_BIT_OUTER)))
	    {
	      data.set(data_BIT_RIGHT, false);
	    }
	  
	  if (!(address.test(szB_BIT_INNER) ||
		address.test(szB_BIT_OUTER) ||
		address.test(szC_BIT_INNER) ||
		address.test(szC_BIT_OUTER)))
	    {
	      data.set(data_BIT_MIDDLE,false);
	    }
	  std::string data_str = data.to_string();
	  _mem5_->push(address, data);
	}
 
      return;
    }

    void tracker_trigger_mem_maker::store_to_file(const std::string & output_file_,
							       const std::string & description_) const
    {
      DT_THROW_IF(!is_initialized(), std::logic_error, "Object is not initialized, store can't be done ! ");
      std::string description;
      if (!description_.empty())
	{
	  description = description_;
	}

      if (_mem1_.get() != 0) _mem1_->store_to_file(output_file_, description);
      if (_mem2_.get() != 0) _mem2_->store_to_file(output_file_, description);
      if (_mem3_.get() != 0) _mem3_->store_to_file(output_file_, description);
      if (_mem4_.get() != 0) _mem4_->store_to_file(output_file_, description);
      if (_mem5_.get() != 0) _mem5_->store_to_file(output_file_, description);

      return;
    }


  } // end of namespace digitization

} // end of namespace snemo
