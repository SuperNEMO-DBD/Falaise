// snemo/digitization/tracker_zone.h
// Author(s): Yves LEMIERE <lemiere@lpccaen.in2p3.fr>
// Author(s): Guillaume OLIVIERO <goliviero@lpccaen.in2p3.fr>

#ifndef FALAISE_DIGITIZATION_PLUGIN_SNEMO_DIGITIZATION_TRACKER_ZONE_H
#define FALAISE_DIGITIZATION_PLUGIN_SNEMO_DIGITIZATION_TRACKER_ZONE_H

// Standard library : 
#include <bitset>

// Boost : 
#include <boost/cstdint.hpp>

// Ourselves : 
#include <snemo/digitization/tracker_info.h>

namespace snemo {
  
  namespace digitization {	

    struct tracker_zone
    {
			
      static int start_row(int i_) {
	static const int start_row[snemo::digitization::tracker_info::NZONES]
	  = {0, 9, 21, 33, 45, 57, 68, 80, 92, 104};
	return start_row[i_];
      }

      static int stop_row(int i_) {
	static const int stop_row[snemo::digitization::tracker_info::NZONES]
	  = {8, 20, 32, 44, 56, 67, 79, 91, 103, 112};
	return stop_row[i_];
      }

      static int width(int i_) {
	return stop_row(i_) - start_row(i_) + 1;
      }

      static void print_layout(std::ostream & out_)
      {
	out_ << "Zone layout: " << '\n';
	out_ << "#Zone   Rows    #\n";
	for (int izone = 0; izone < snemo::digitization::tracker_info::NZONES; izone++) {
	  out_ << izone << " " << start_row(izone) << "-" << stop_row(izone)
	       << ' ' << "#" << width(izone) << '\n';
	}
	out_ << std::endl;
	return;
      }

      tracker_zone(){
	reset();
	return;
      }
			
      void reset()
      {
	for (int ilayer = 0; ilayer < snemo::digitization::tracker_info::NLAYERS; ilayer++) {
	  for (int irow = 0; irow < snemo::digitization::tracker_info::ZONE_WIDTH; irow++) {
	    _cells_[ilayer][irow] = false;
	  }
	}
	return;
      }
			
      bool _cells_[snemo::digitization::tracker_info::NLAYERS][snemo::digitization::tracker_info::ZONE_WIDTH];
			
      //

      
      
      // [7 bits] of information per zone after total computing
      std::bitset<snemo::digitization::tracker_info::IO_PATTERN_SIZE>  _data_in_out_pattern_;
      std::bitset<snemo::digitization::tracker_info::LMR_PATTERN_SIZE> _data_left_mid_right_pattern_;
      std::bitset<snemo::digitization::tracker_info::NSZ_PATTERN_SIZE> _data_near_source_;
    };
		
    struct tracker_sliding_zone
    {

      static int start_row(int i_) {
    	return stop_row(i_) - width(i_) + 1;
      }

      static int stop_row(int i_) {
    	int sr = 4 + i_ * 4;
    	if (i_ > 15) sr--;
    	return std::min((int) tracker_info::NROWS - 1, sr);
      }

      // Width of sliding zone = 8 except for sliding zone close to the begining or end of tracker
      static int width(int i_) {
    	int nr = snemo::digitization::tracker_info::SLZONE_WIDTH;
    	if (i_ == 0) nr = 5;
    	if (i_ == (int) tracker_info::NSLZONES - 1) nr = 5;
    	return nr;
      }

      static void print_layout(std::ostream & out_)
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

      // build methods from 4 sliding zones to fill 1 zone

      tracker_sliding_zone()
      {
    	reset();
    	return;
      }

      void reset()
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


      void print(std::ostream & out_) const
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

      int _side_ = -1;
      int _szone_id_ = -1;
      bool _cells_[snemo::digitization::tracker_info::NLAYERS][snemo::digitization::tracker_info::SLZONE_WIDTH];
      
      std::bitset<snemo::digitization::tracker_info::SLZONE_LAYER_PROJ> _addr_layer_proj_;
      std::bitset<snemo::digitization::tracker_info::SLZONE_ROW_PROJ>   _addr_row_proj_;

      std::bitset<snemo::digitization::tracker_info::SLZONE_DATA_LAYER_PROJ> _data_layer_proj_;
      std::bitset<snemo::digitization::tracker_info::SLZONE_DATA_ROW_PROJ>   _data_row_proj_;
      std::bitset<snemo::digitization::tracker_info::NSZ_PATTERN_SIZE>       _data_near_source_;
       
    };

    //   void print(std::ostream & out_) const
    //   {
    // 	out_ << "Sliding zone (" << _side_ << ',' << _szone_id_ << ") : \n";
    // 	if (_side_ == 0) {
    // 	  out_ << "    " << "  ";
    // 	  for (int irow = 0; irow < width(_szone_id_); irow++) {
    // 	    out_ << irow << ' ';
    // 	  }
    // 	  out_ << '\n';
    // 	  for (int ilayer = tracker_info::NLAYERS - 1; ilayer >= 0; ilayer--) {
    // 	    out_ << ilayer << " = " << "  ";
    // 	    for (int irow = 0; irow < width(_szone_id_); irow++) {
    // 	      out_ << (_cells_[ilayer][irow] ? 'o' : '.') << ' ';
    // 	    }
    // 	    out_ << ' ' << '[' << lproj[ilayer] << ']' << '\n';
    // 	  }
    // 	  out_ << "    " << "[ ";
    // 	  for (int irow = 0; irow < width(_szone_id_); irow++) {
    // 	    out_ << rproj[irow] << ' ';
    // 	  }
    // 	  out_ << ']' << '\n';
    // 	}
    // 	if (_side_ == 1) {
    // 	  out_ << "    " << "[ ";
    // 	  for (int irow = 0; irow < width(_szone_id_); irow++) {
    // 	    out_ << rproj[irow] << ' ';
    // 	  }
    // 	  out_ << ']' << '\n';
    // 	  for (int ilayer = 0; ilayer < tracker_info::NLAYERS; ilayer++) {
    // 	    out_ << ilayer << " = " << "  ";
    // 	    for (int irow = 0; irow < width(_szone_id_); irow++) {
    // 	      out_ << (_cells_[ilayer][irow] ? 'o' : '.') << ' ';
    // 	    }
    // 	    out_ << ' ' << '[' << lproj[ilayer] << ']' << '\n';
    // 	  }
    // 	  out_ << "    " << "  ";
    // 	  for (int irow = 0; irow < width(_szone_id_); irow++) {
    // 	    out_ << irow << ' ';
    // 	  }
    // 	  out_ << '\n';
    // 	}
    // 	out_ << "NSZ[L] = " << '[' << nsz[0] << ']' << "  NSZ[R] = " << '[' << nsz[1] << ']' << '\n';
    // 	out_ << "PATTERN[0-3] = " << '[' << ' ';
    // 	for (int i = 0; i < 4; i++) out_ << pattern[i] << ' ';
    // 	out_ << ']' << '\n';
    // 	return;
    //   }

    //   void compute_lr_proj()
    //   {
    // 	nsz[0] = false;
    // 	nsz[1] = false;
    // 	for (int ilayer = 0; ilayer < tracker_info::NLAYERS; ilayer++) {
    // 	  lproj[ilayer] = false;
    // 	  for (int irow = 0; irow < width(_szone_id_); irow++) {
    // 	    if (_cells_[ilayer][irow]) {
    // 	      lproj[ilayer] = true;
    // 	      if (ilayer < 4) {
    // 		if (irow < 4) {
    // 		  nsz[0] = true;
    // 		} else {
    // 		  nsz[1] = true;
    // 		}
    // 	      }
    // 	      break;
    // 	    }
    // 	  }
    // 	}
    // 	for (int irow = 0; irow < width(_szone_id_); irow++) {
    // 	  rproj[irow] = false;
    // 	  for (int ilayer = 0; ilayer < tracker_info::NLAYERS; ilayer++) {
    // 	    if (_cells_[ilayer][irow]) {
    // 	      rproj[irow] = true;
    // 	      break;
    // 	    }
    // 	  }
    // 	}
    // 	return;
    //   }

    //   void build_pattern(mem1_type & mem1_, mem2_type & mem2_)
    //   {
    // 	// Pattern [ 3 2 1 0 ]
    // 	//           V V H H
    // 	{
    // 	  // Vertical/layer pattern:
    // 	  mem1_type::address_type vaddress;
    // 	  for (int i = 0; i < tracker_info::NLAYERS; i++) {
    // 	    vaddress[i] = lproj[i];
    // 	  }
    // 	  mem1_type::data_type vpattern;
    // 	  mem1_.fetch(vaddress, vpattern);
    // 	  for (int i = 0; i < vpattern.size(); i++) {
    // 	    pattern[2 + i] = vpattern.test(i);
    // 	  }
    // 	}
    // 	{
    // 	  // Horizontal/row pattern:
    // 	  mem2_type::address_type vaddress;
    // 	  for (int i = 0; i < SZONE_WIDTH; i++) {
    // 	    vaddress[i] = rproj[i];
    // 	  }
    // 	  mem2_type::data_type vpattern;
    // 	  mem2_.fetch(vaddress, vpattern);
    // 	  for (int i = 0; i < vpattern.size(); i++) {
    // 	    pattern[i] = vpattern.test(i);
    // 	  }
    // 	}
    // 	return;
    //   }

    //    };
	
  } // end of namespace digitization
	
} // end of namespace snemo

#endif /* FALAISE_DIGITIZATION_PLUGIN_SNEMO_DIGITIZATION_TRACKER_ZONE_H */

/* 
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
