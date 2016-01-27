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
      side = -1;
      szone_id = -1;
      for (int ilayer = 0; ilayer < trigger_info::NLAYERS; ilayer++) {
	for (int irow = 0; irow < snemo::digitization::trigger_info::SLZONE_WIDTH; irow++) {
	  cells[ilayer][irow] = false;
	}
      }
      addr_layer_proj.reset();
      addr_row_proj.reset();
      data_IO_proj.reset();
      data_LR_proj.reset();

      return;
    }      

    int tracker_sliding_zone::stop_row(int i_) 
    {
      int sr = 4 + (i_ - 1) * 4;
      if (i_ == 0) sr = 0;
      if (i_ == 1) sr = 4;
      if (i_ == 2) sr = 8;
      if (i_ > 16) sr--;
      return std::min((int) trigger_info::NROWS - 1, sr);
    }

    int tracker_sliding_zone::start_row(int i_) 
    {
      return stop_row(i_) - width(i_) + 1;
    }

    // Width of sliding zone = 8 except for sliding zone close to the begining or end of tracker
    int tracker_sliding_zone::width(int i_) 
    {
      int nr = snemo::digitization::trigger_info::SLZONE_WIDTH;

      if (i_ == 0 || i_ == (int) trigger_info::NSLZONES - 1) nr = 1;
      if (i_ == 1 || i_ == (int) trigger_info::NSLZONES - 2) nr = 5;

      return nr;
    }

    void tracker_sliding_zone::compute_lr_proj()
    {      
      for (int ilayer = 0; ilayer < trigger_info::NLAYERS; ilayer++) 
	{
	  addr_layer_proj.set(ilayer, false);

	  for (int irow = 0; irow < width(szone_id); irow++)
	    {
	      if (cells[ilayer][irow]) 
		{
		  addr_layer_proj.set(ilayer, true);
 		  break;
		}
	    }
	}

      for (int irow = 0; irow < width(szone_id); irow++) 
	{
	  addr_row_proj.set(irow, false);
	  for (int ilayer = 0; ilayer < trigger_info::NLAYERS; ilayer++) 
	    {
	      if (cells[ilayer][irow]) 
		{
		  addr_row_proj.set(irow, true);
		  break;
		}
	    }
	}
      return;
    }

    void tracker_sliding_zone::build_pattern(tracker_trigger_mem_maker_new_strategy::mem1_type & mem1_, tracker_trigger_mem_maker_new_strategy::mem2_type & mem2_)
    {
      // Pattern data : 
      // layer proj :[ 1 0 ]   |  row proj [ 1 0 ] for a sliding zone
      //               O I                   L R
      
      // Vertical/layer pattern:
      tracker_trigger_mem_maker_new_strategy::mem1_type::address_type vaddress;
      vaddress = addr_layer_proj;
	
      tracker_trigger_mem_maker_new_strategy::mem1_type::data_type vdata;
      mem1_.fetch(vaddress, vdata);
      data_IO_proj = vdata;

      // Horizontal/row pattern:
      tracker_trigger_mem_maker_new_strategy::mem2_type::address_type haddress;
      haddress = addr_row_proj;

      tracker_trigger_mem_maker_new_strategy::mem2_type::data_type hdata;
      mem2_.fetch(haddress, hdata);
      data_LR_proj = hdata;
      
      //std::clog << "vaddress = " << vaddress << " vdata = " << data_IO_proj << std::endl;
      // std::clog << "haddress = " << haddress << " hdata = " << data_LR_proj << std::endl;

      return;
    }

    void tracker_sliding_zone::print_layout(std::ostream & out_)
    {
      out_ << "Sliding zone layout: " << '\n';
      out_ << "#S-Zone   Rows    #\n";
      for (int iszone = 0; iszone < trigger_info::NSLZONES; iszone++) {
	out_ << iszone << " " << start_row(iszone) << "-" << stop_row(iszone)
	     << ' ' << "#" << width(iszone) << '\n';
      }
      out_ << std::endl;
      return;
    }

    void tracker_sliding_zone::print(std::ostream & out_) const
    {
      out_ << "Sliding zone (" << side << ',' << szone_id << ") : \n";
      if (side == 0) {
	out_ << "    " << "  ";
	for (int irow = 0; irow < width(szone_id); irow++) {
	  out_ << irow << ' ';
	}
	out_ << '\n';
	for (int ilayer = trigger_info::NLAYERS - 1; ilayer >= 0; ilayer--) {
	  out_ << ilayer << " = " << "  ";
	  for (int irow = 0; irow < width(szone_id); irow++) {
	    out_ << (cells[ilayer][irow] ? 'o' : '.') << ' ';
	  }
	  out_ << ' ' << '[' << addr_layer_proj.test(ilayer) << ']' << '\n';
	}
	out_ << "    " << "[ ";
	for (int irow = 0; irow < width(szone_id); irow++) {
	  out_ << addr_row_proj.test(irow) << ' ';
	}
	out_ << ']' << '\n';
      }
      if (side == 1) {
	out_ << "    " << "[ ";
	for (int irow = 0; irow < width(szone_id); irow++) {
	  out_ << addr_row_proj.test(irow) << ' ';
	}
	out_ << ']' << '\n';
	for (int ilayer = 0; ilayer < trigger_info::NLAYERS; ilayer++) {
	  out_ << ilayer << " = " << "  ";
	  for (int irow = 0; irow < width(szone_id); irow++) {
	    out_ << (cells[ilayer][irow] ? 'o' : '.') << ' ';
	  }
	  out_ << ' ' << '[' << addr_layer_proj.test(ilayer) << ']' << '\n';
	}
	out_ << "    " << "  ";
	for (int irow = 0; irow < width(szone_id); irow++) {
	  out_ << irow << ' ';
	}
	out_ << '\n';
      }
      out_ << "IO = " << '[' << data_IO_proj << ']' << "  LR = " << '[' << data_LR_proj << ']' << '\n';
      out_ << '\n';

      return;
    }


  } // end of namespace digitization

} // end of namespace snemo
