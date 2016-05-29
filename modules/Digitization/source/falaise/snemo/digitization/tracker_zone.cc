// snemo/digitization/tracker_zone.cc
// Author(s): Yves LEMIERE <lemiere@lpccaen.in2p3.fr>
// Author(s): Guillaume OLIVIERO <goliviero@lpccaen.in2p3.fr>

// Standard library : 
#include <iostream>

// Ourselves:
#include <falaise/snemo/digitization/tracker_zone.h>

namespace snemo {

  namespace digitization {

    tracker_zone::tracker_zone()
    {
      reset();
      return;
    }

    void tracker_zone::reset()
    {
      side = -1;
      zone_id = -1;
      for (int ilayer = 0; ilayer < snemo::digitization::trigger_info::NLAYERS; ilayer++) {
	for (int irow = 0; irow < snemo::digitization::trigger_info::ZONE_WIDTH; irow++) {
	  cells[ilayer][irow] = false;
	}
      }

      addr_in_out_pattern.reset();
      addr_left_mid_right_pattern.reset();
      addr_left_mid_right_with_in_out_pattern.reset();
      data_in_out_pattern.reset();
      data_left_mid_right_pattern.reset();
      data_near_source.reset();

      return;
    }

    int tracker_zone::start_row(int i_)
    {
      static const int start_row[snemo::digitization::trigger_info::NZONES]
	= {0, 9, 21, 33, 45, 57, 68, 80, 92, 104};
      return start_row[i_];
    }
    
    int tracker_zone::stop_row(int i_)
    {
      static const int stop_row[snemo::digitization::trigger_info::NZONES]
	= {8, 20, 32, 44, 56, 67, 79, 91, 103, 112};
      return stop_row[i_];
    }

    int tracker_zone::width(int i_)
    {
      return stop_row(i_) - start_row(i_) + 1;
    }
   
    void tracker_zone::print_layout(std::ostream & out_)
    {
      out_ << "Zone layout: " << '\n';
      out_ << "#Zone   Rows    #\n";
      for (int izone = 0; izone < snemo::digitization::trigger_info::NZONES; izone++) {
	out_ << izone << " " << start_row(izone) << "-" << stop_row(izone)
	     << ' ' << "#" << width(izone) << '\n';
      }
      out_ << std::endl;
      return;
    }

    void tracker_zone::print_projections(std::ostream & out_) const
    {
      out_ << "Zone projections: " << '\n';
      out_ << "IO pattern : " << addr_in_out_pattern << "\n"; 
      out_ << "LMR pattern : " << addr_left_mid_right_pattern << "\n";
      return;
    }

  } // end of namespace digitization

} // end of namespace snemo
