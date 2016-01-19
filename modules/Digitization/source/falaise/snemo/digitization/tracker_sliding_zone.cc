// snemo/digitization/tracker_zone.cc
// Author(s): Yves LEMIERE <lemiere@lpccaen.in2p3.fr>
// Author(s): Guillaume OLIVIERO <goliviero@lpccaen.in2p3.fr>


// Standard library : 
#include <iostream>
#include <algorithm>

// Ourselves:
#include <falaise/snemo/digitization/tracker_sliding_zone.h>

namespace snemo {

  namespace digitization {

    tracker_sliding_zone::tracker_sliding_zone()
    {
      reset();
      return;
    }

    void tracker_sliding_zone::reset()
    {
      _side_ = -1;
      _szone_id_ = -1;
      for (int ilayer = 0; ilayer < tracker_info::NLAYERS; ilayer++) {
	for (int irow = 0; irow < snemo::digitization::tracker_info::SLZONE_WIDTH; irow++) {
	  _cells_[ilayer][irow] = false;
	}
      }
      _addr_layer_proj_.reset();
      _addr_row_proj_.reset();
      _data_layer_proj_.reset();
      _data_row_proj_.reset();
      _data_near_source_.reset();

      return;
    }

    int tracker_sliding_zone::start_row(int i_) 
    {
      return stop_row(i_) - width(i_) + 1;
    }

    int tracker_sliding_zone::stop_row(int i_) 
    {
      int sr = 4 + i_ * 4;
      if (i_ > 15) sr--;
      return std::min((int) tracker_info::NROWS - 1, sr);
    }

    // Width of sliding zone = 8 except for sliding zone close to the begining or end of tracker
    int tracker_sliding_zone::width(int i_) 
    {
      int nr = snemo::digitization::tracker_info::SLZONE_WIDTH;
      if (i_ == 0) nr = 5;
      if (i_ == (int) tracker_info::NSLZONES - 1) nr = 5;
      return nr;
    }

    void tracker_sliding_zone::compute_lr_proj()
    {
      _data_near_source_.reset();
      _data_layer_proj_.reset();
      _data_row_proj_.reset();
      
      for (int ilayer = 0; ilayer < tracker_info::NLAYERS; ilayer++) 
	{
	  _addr_layer_proj_.set(ilayer, false);

	  for (int irow = 0; irow < width(_szone_id_); irow++)
	    {
	      if (_cells_[ilayer][irow]) 
		{
		  _addr_layer_proj_.set(ilayer, true);
		  if (ilayer < tracker_info::NUMBER_OF_LAYERS_HIT_FOR_NEAR_SOURCE_BIT) 
		    {
		      if (irow < tracker_info::LEFT_RIGHT_ROW_SEPARATION_FOR_NEAR_SOURCE_BIT) _data_near_source_.set(DATA_NEAR_SOURCE_BIT_LEFT, true);
		      else _data_near_source_.set(DATA_NEAR_SOURCE_BIT_RIGHT, true);
		    }
		  break;
		}
	    }
	}

      for (int irow = 0; irow < width(_szone_id_); irow++) 
	{
	  _addr_row_proj_.set(irow, false);
	  for (int ilayer = 0; ilayer < tracker_info::NLAYERS; ilayer++) 
	    {
	      if (_cells_[ilayer][irow]) 
		{
		  _addr_row_proj_.set(irow, true);
		  break;
		}
	    }
	}
      return;
    }

    void tracker_sliding_zone::build_pattern(tracker_trigger_mem_maker_new_strategy::mem1_type & mem1_, tracker_trigger_mem_maker_new_strategy::mem2_type & mem2_)
    {
      // A refaire après recode les mémoires

      // // Pattern [ 3 2 1 0 ] for a sliding zone
      // //           O I L R
      // {
      // 	// Vertical/layer pattern:
      // 	mem1_type::address_type vaddress;
        
	

      // 	for (int i = 0; i < tracker_info::NLAYERS; i++) {
	  
      // 	  vaddress[i] = lproj[i];
      // 	}
      // 	mem1_type::data_type vpattern;
      // 	mem1_.fetch(vaddress, vpattern);
      // 	for (int i = 0; i < vpattern.size(); i++) {
      // 	  pattern[2 + i] = vpattern.test(i);
      // 	}
      // }
      // {
      // 	// Horizontal/row pattern:
      // 	mem2_type::address_type vaddress;
      // 	for (int i = 0; i < SZONE_WIDTH; i++) {
      // 	  vaddress[i] = rproj[i];
      // 	}
      // 	mem2_type::data_type vpattern;
      // 	mem2_.fetch(vaddress, vpattern);
      // 	for (int i = 0; i < vpattern.size(); i++) {
      // 	  pattern[i] = vpattern.test(i);
      // 	}
      // }
      // return;
    }

    void tracker_sliding_zone::print_layout(std::ostream & out_)
    {
      out_ << "Sliding zone layout: " << '\n';
      out_ << "#S-Zone   Rows    #\n";
      for (int iszone = 0; iszone < tracker_info::NSLZONES; iszone++) {
	out_ << iszone << " " << start_row(iszone) << "-" << stop_row(iszone)
	     << ' ' << "#" << width(iszone) << '\n';
      }
      out_ << std::endl;
      return;
    }

    void tracker_sliding_zone::print(std::ostream & out_) const
    {
      out_ << "Sliding zone (" << _side_ << ',' << _szone_id_ << ") : \n";
      if (_side_ == 0) {
	out_ << "    " << "  ";
	for (int irow = 0; irow < width(_szone_id_); irow++) {
	  out_ << irow << ' ';
	}
	out_ << '\n';
	for (int ilayer = tracker_info::NLAYERS - 1; ilayer >= 0; ilayer--) {
	  out_ << ilayer << " = " << "  ";
	  for (int irow = 0; irow < width(_szone_id_); irow++) {
	    out_ << (_cells_[ilayer][irow] ? 'o' : '.') << ' ';
	  }
	  out_ << ' ' << '[' << _addr_layer_proj_.test(ilayer) << ']' << '\n';
	}
	out_ << "    " << "[ ";
	for (int irow = 0; irow < width(_szone_id_); irow++) {
	  out_ << _addr_row_proj_.test(irow) << ' ';
	}
	out_ << ']' << '\n';
      }
      if (_side_ == 1) {
	out_ << "    " << "[ ";
	for (int irow = 0; irow < width(_szone_id_); irow++) {
	  out_ << _addr_row_proj_.test(irow) << ' ';
	}
	out_ << ']' << '\n';
	for (int ilayer = 0; ilayer < tracker_info::NLAYERS; ilayer++) {
	  out_ << ilayer << " = " << "  ";
	  for (int irow = 0; irow < width(_szone_id_); irow++) {
	    out_ << (_cells_[ilayer][irow] ? 'o' : '.') << ' ';
	  }
	  out_ << ' ' << '[' << _addr_layer_proj_.test(ilayer) << ']' << '\n';
	}
	out_ << "    " << "  ";
	for (int irow = 0; irow < width(_szone_id_); irow++) {
	  out_ << irow << ' ';
	}
	out_ << '\n';
      }
      out_ << "NSZ[L] = " << '[' << _data_near_source_.test(0) << ']' << "  NSZ[R] = " << '[' << _data_near_source_.test(1) << ']' << '\n';
      out_ << ']' << '\n';
      return;
    }


  } // end of namespace digitization

} // end of namespace snemo
