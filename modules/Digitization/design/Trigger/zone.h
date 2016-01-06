#ifndef SNEMO_TRIGGER_ZONE_H
#define SNEMO_TRIGGER_ZONE_H

#include <iostream>
#include <algorithm>

#include "tracker.h"
#include "utils.h"

namespace snemo {
  namespace trigger {

    // Zone:
    struct zone
    {
      static const std::size_t ZWIDTH = 12;

      static int start_row(int i_) {
	static constexpr int _start_row[tracker::NZONES]
	  = {0,  9, 21, 33, 45, 57, 68, 80,  92, 104 };
	return _start_row[i_];
      }

      static int stop_row(int i_) {
	static constexpr int _stop_row[tracker::NZONES]
	  = {8, 20, 32, 44, 56, 67, 79, 91, 103, 112 };
	return _stop_row[i_];
      }

      static int width(int i_) {
	return stop_row(i_) - start_row(i_) + 1;
      }

      static void print_layout(std::ostream & out_)
      {
	out_ << "Zone layout: " << '\n';
	out_ << "#Zone   Rows    #\n";
	for (int izone = 0; izone < tracker::NZONES; izone++) {
	  out_ << izone << " " << start_row(izone) << "-" << stop_row(izone)
	       << ' ' << "#" << width(izone) << '\n';
	}
	out_ << std::endl;
	return;
      }

      zone(){
	reset();
	return;
      }

      void reset()
      {
	for (int ilayer = 0; ilayer < tracker::NLAYERS; ilayer++) {
	  for (int irow = 0; irow < ZWIDTH; irow++) {
	    cells[ilayer][irow] = false;
	  }
	}
	return;
      }

      bool cells[tracker::NLAYERS][ZWIDTH];

    };

    // Sliding zone:
    struct szone
    {
      static const std::size_t SZWIDTH = 8;

      static int stop_row(int i_) {
	int sr = 4 + i_ * 4;
	if (i_ > 15) sr--;
	return std::min((int) tracker::NROWS - 1, sr);
      }

      static int start_row(int i_) {
	return stop_row(i_) - width(i_) + 1;
      }

      static int width(int i_) {
	int nr = 8;
	if (i_ == 0) nr = 5;
	if (i_ == (int) tracker::NSZONES - 1) nr = 5;
	return nr;
      }

      static void print_layout(std::ostream & out_)
      {
	out_ << "Sliding zone layout: " << '\n';
	out_ << "#S-Zone   Rows    #\n";
	for (int iszone = 0; iszone < tracker::NSZONES; iszone++) {
	  out_ << iszone << " " << start_row(iszone) << "-" << stop_row(iszone)
	       << ' ' << "#" << width(iszone) << '\n';
	}
	out_ << std::endl;
	return;
      }

      szone()
      {
	reset();
	return;
      }

      void reset()
      {
	side = -1;
	szone_id = -1;
	for (int ilayer = 0; ilayer < tracker::NLAYERS; ilayer++) {
	  lproj[ilayer] = false;
	  for (int irow = 0; irow < SZWIDTH; irow++) {
	    cells[ilayer][irow] = false;
	  }
	}
	for (int irow = 0; irow < SZWIDTH; irow++) {
	  rproj[irow] = false;
	}
	nsz[0] = false;
	nsz[1] = false;
	for (int i = 0; i < 4; i++) pattern[i] = false;
	return;
      }

      void print(std::ostream & out_) const
      {
	out_ << "Sliding zone (" << side << ',' << szone_id << ") : \n";
	if (side == 0) {
	  out_ << "    " << "  ";
	  for (int irow = 0; irow < width(szone_id); irow++) {
	    out_ << irow << ' ';
	  }
	  out_ << '\n';
	  for (int ilayer = tracker::NLAYERS - 1; ilayer >= 0; ilayer--) {
	    out_ << ilayer << " = " << "  ";
	    for (int irow = 0; irow < width(szone_id); irow++) {
	      out_ << (cells[ilayer][irow] ? 'o' : '.') << ' ';
	    }
	    out_ << ' ' << '[' << lproj[ilayer] << ']' << '\n';
	  }
	  out_ << "    " << "[ ";
	  for (int irow = 0; irow < width(szone_id); irow++) {
	    out_ << rproj[irow] << ' ';
	  }
	  out_ << ']' << '\n';
	}
	if (side == 1) {
	  out_ << "    " << "[ ";
	  for (int irow = 0; irow < width(szone_id); irow++) {
	    out_ << rproj[irow] << ' ';
	  }
	  out_ << ']' << '\n';
	  for (int ilayer = 0; ilayer < tracker::NLAYERS; ilayer++) {
	    out_ << ilayer << " = " << "  ";
	    for (int irow = 0; irow < width(szone_id); irow++) {
	      out_ << (cells[ilayer][irow] ? 'o' : '.') << ' ';
	    }
	    out_ << ' ' << '[' << lproj[ilayer] << ']' << '\n';
	  }
	  out_ << "    " << "  ";
	  for (int irow = 0; irow < width(szone_id); irow++) {
	    out_ << irow << ' ';
	  }
	  out_ << '\n';
	}
	out_ << "NSZ[L] = " << '[' << nsz[0] << ']' << "  NSZ[R] = " << '[' << nsz[1] << ']' << '\n';
	out_ << "PATTERN[0-3] = " << '[' << ' ';
	for (int i = 0; i < 4; i++) out_ << pattern[i] << ' ';
	out_ << ']' << '\n';
	return;
      }

      void compute_lr_proj()
      {
	nsz[0] = false;
	nsz[1] = false;
	for (int ilayer = 0; ilayer < tracker::NLAYERS; ilayer++) {
	  lproj[ilayer] = false;
	  for (int irow = 0; irow < width(szone_id); irow++) {
	    if (cells[ilayer][irow]) {
	      lproj[ilayer] = true;
	      if (ilayer < 4) {
		if (irow < 4) {
		  nsz[0] = true;
		} else {
		  nsz[1] = true;
		}
	      }
	      break;
	    }
	  }
	}
	for (int irow = 0; irow < width(szone_id); irow++) {
	  rproj[irow] = false;
	  for (int ilayer = 0; ilayer < tracker::NLAYERS; ilayer++) {
	    if (cells[ilayer][irow]) {
	      rproj[irow] = true;
	      break;
	    }
	  }
	}
	return;
      }

      void build_pattern(mem1_type & mem1_, mem2_type & mem2_)
      {
	// Pattern [ 3 2 1 0 ]
	//           V V H H
	{
	  // Vertical/layer pattern:
	  mem1_type::address_type vaddress;
	  for (int i = 0; i < tracker::NLAYERS; i++) {
	    vaddress[i] = lproj[i];
	  }
	  mem1_type::data_type vpattern;
	  mem1_.fetch(vaddress, vpattern);
	  for (int i = 0; i < vpattern.size(); i++) {
	    pattern[2 + i] = vpattern.test(i);
	  }
	}
	{
	  // Horizontal/row pattern:
	  mem2_type::address_type vaddress;
	  for (int i = 0; i < SZWIDTH; i++) {
	    vaddress[i] = rproj[i];
	  }
	  mem2_type::data_type vpattern;
	  mem2_.fetch(vaddress, vpattern);
	  for (int i = 0; i < vpattern.size(); i++) {
	    pattern[i] = vpattern.test(i);
	  }
	}
	return;
      }

      int side;
      int szone_id;
      bool cells[tracker::NLAYERS][SZWIDTH];
      bool lproj[tracker::NLAYERS];
      bool rproj[SZWIDTH];
      bool nsz[2];
      bool pattern[4];
    };

  }
}

#endif // SNEMO_TRIGGER_ZONE_H
