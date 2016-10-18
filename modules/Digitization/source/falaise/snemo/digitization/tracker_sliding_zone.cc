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
      for (unsigned int ilayer = 0; ilayer < trigger_info::NLAYERS; ilayer++) {
	for (unsigned int irow = 0; irow < snemo::digitization::trigger_info::SLZONE_WIDTH; irow++) {
	  cells[ilayer][irow] = false;
	}
      }
      addr_layer_proj.reset();
      addr_row_proj.reset();
      data_IO_proj.reset();
      data_LR_proj.reset();

      return;
    }

    unsigned int tracker_sliding_zone::stop_row(unsigned int i_)
    {
      unsigned int sr = 4 + (i_ - 1) * 4;
      if (i_ == 0) sr = 0;
      if (i_ == 1) sr = 4;
      if (i_ == 2) sr = 8;
      if (i_ > 16) sr--;
      return std::min((unsigned int) trigger_info::NROWS - 1, sr);
    }

    unsigned int tracker_sliding_zone::start_row(unsigned int i_)
    {
      return stop_row(i_) - width(i_) + 1;
    }

    // Width of sliding zone = 8 except for sliding zone close to the begining or end of tracker
    unsigned int tracker_sliding_zone::width(unsigned int i_)
    {
      unsigned int nr = snemo::digitization::trigger_info::SLZONE_WIDTH;

      if (i_ == 0 || i_ == (unsigned int) trigger_info::NSLZONES - 1) nr = 1;
      if (i_ == 1 || i_ == (unsigned int) trigger_info::NSLZONES - 2) nr = 5;

      return nr;
    }

    void tracker_sliding_zone::compute_lr_proj()
    {
      for (unsigned int ilayer = 0; ilayer < trigger_info::NLAYERS; ilayer++)
	{
	  addr_layer_proj.set(ilayer, false);

	  for (unsigned int irow = 0; irow < width(szone_id); irow++)
	    {
	      if (cells[ilayer][irow])
		{
		  addr_layer_proj.set(ilayer, true);
 		  break;
		}
	    }
	}

      for (unsigned int irow = 0; irow < width(szone_id); irow++)
	{
	  // Include a shift for SLZA & SLZB zone 0 (left bits are unused for projections)
	  if (szone_id == 0 || szone_id == 1) addr_row_proj.set(irow + 8 - width(szone_id), false);
	  else addr_row_proj.set(irow, false);
	  for (unsigned int ilayer = 0; ilayer < trigger_info::NLAYERS; ilayer++)
	    {
	      if (cells[ilayer][irow])
		{
		  if (szone_id == 0 || szone_id == 1)
		    {
		      std::size_t pos = irow + 8 - width(szone_id);
		      addr_row_proj.set(pos, true);
		    }
		  else addr_row_proj.set(irow, true);
		  // addr_row_proj.set(irow, true);
		  break;
		}
	    }
	}

      return;
    }

    void tracker_sliding_zone::build_pattern(tracker_trigger_mem_maker::mem1_type & mem1_, tracker_trigger_mem_maker::mem2_type & mem2_)
    {
      // Pattern data :
      // layer proj :[ 1 0 ]   |  row proj [ 1 0 ] for a sliding zone
      //               O I                   L R

      // Vertical/layer pattern:
      tracker_trigger_mem_maker::mem1_type::address_type vaddress;
      vaddress = addr_layer_proj;

      tracker_trigger_mem_maker::mem1_type::data_type vdata;
      mem1_.fetch(vaddress, vdata);
      data_IO_proj = vdata;

      // Horizontal/row pattern:
      tracker_trigger_mem_maker::mem2_type::address_type haddress;
      haddress = addr_row_proj;

      tracker_trigger_mem_maker::mem2_type::data_type hdata;
      mem2_.fetch(haddress, hdata);
      data_LR_proj = hdata;
      return;
    }

    void tracker_sliding_zone::print_layout(std::ostream & out_)
    {
      out_ << "Sliding zone layout: " << '\n';
      out_ << "#S-Zone   Rows    #\n";
      for (unsigned int iszone = 0; iszone < trigger_info::NSLZONES; iszone++) {
	out_ << iszone << " " << start_row(iszone) << "-" << stop_row(iszone)
	     << ' ' << "#" << width(iszone) << '\n';
      }
      out_ << std::endl;
      return;
    }

    void tracker_sliding_zone::print(std::ostream & out_) const
    {
      if (szone_id == 0 || szone_id == 1) out_ << "Problem with display for projections due to an initial shift for these sliding zones" << std::endl;
      out_ << "Sliding zone (" << side << ',' << szone_id << ") : \n";
      if (side == 0) {
	out_ << "    " << "  ";
	for (unsigned int irow = 0; irow < width(szone_id); irow++) {
	  out_ << irow << ' ';
	}
	out_ << '\n';
	for (unsigned int ilayer = trigger_info::NLAYERS - 1; ilayer <= 0; ilayer--) {
	  out_ << ilayer << " = " << "  ";
	  for (unsigned int irow = 0; irow < width(szone_id); irow++) {
	    out_ << (cells[ilayer][irow] ? 'o' : '.') << ' ';
	  }
	  out_ << ' ' << '[' << addr_layer_proj.test(ilayer) << ']' << '\n';
	}
	out_ << "    " << "[ ";
	for (unsigned int irow = 0; irow < width(szone_id); irow++) {
	  out_ << addr_row_proj.test(irow) << ' ';
	}
	out_ << ']' << '\n';
      }
      if (side == 1) {
	out_ << "    " << "[ ";
	for (unsigned int irow = 0; irow < width(szone_id); irow++) {
	  out_ << addr_row_proj.test(irow) << ' ';
	}
	out_ << ']' << '\n';
	for (unsigned int ilayer = 0; ilayer < trigger_info::NLAYERS; ilayer++) {
	  out_ << ilayer << " = " << "  ";
	  for (unsigned int irow = 0; irow < width(szone_id); irow++) {
	    out_ << (cells[ilayer][irow] ? 'o' : '.') << ' ';
	  }
	  out_ << ' ' << '[' << addr_layer_proj.test(ilayer) << ']' << '\n';
	}
	out_ << "    " << "  ";
	for (unsigned int irow = 0; irow < width(szone_id); irow++) {
	  out_ << irow << ' ';
	}
	out_ << '\n';
      }
      out_ << "IO = " << '[' << data_IO_proj << ']' << "  LR = " << '[' << data_LR_proj << ']' << '\n';
      out_ << '\n';

      return;
    }

    void tracker_sliding_zone::print_projections(std::ostream & out_) const
    {
      out_ << "Sliding zone (" << side << ',' << szone_id << ") \n";
      std::string addr_row_proj_string =  addr_row_proj.to_string();
      std::reverse(addr_row_proj_string.begin(), addr_row_proj_string.end());
      std::string addr_layer_proj_string =  addr_layer_proj.to_string();
      std::reverse(addr_layer_proj_string.begin(), addr_layer_proj_string.end());

      out_ << "PR : Addr : [" <<  addr_row_proj_string << "]  Data : [" << data_LR_proj << "] \n";
      out_ << "PL : Addr : [" << addr_layer_proj_string << "] Data : [" << data_IO_proj << "] \n";

      // for (unsigned int i =  snemo::digitization::trigger_info::SLZONE_ROW_PROJ; i > 0; i--)
      // 	{
      // 	  if (i == snemo::digitization::trigger_info::SLZONE_ROW_PROJ) out_ << "PR \n";
      // 	  if (i != 4) out_ << addr_row_proj[i] << "\n";
      // 	  if (i == 4) out_ << addr_row_proj[i] << "  ===> " << data_LR_proj << "\n";
      // 	}
      // for (unsigned int i = snemo::digitization::trigger_info::SLZONE_LAYER_PROJ; i > 0; i--)
      // 	{
      // 	  if (i == snemo::digitization::trigger_info::SLZONE_LAYER_PROJ) out_ << "PL : ";
      // 	  if (i != 0 && i != snemo::digitization::trigger_info::SLZONE_LAYER_PROJ - 1) out_ << addr_layer_proj[i];
      // 	  if (i == 0) out_ << " ==> " << data_IO_proj << "\n";
      // 	}

    }


  } // end of namespace digitization

} // end of namespace snemo
