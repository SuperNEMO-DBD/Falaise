/// \file falaise/snemo/rc/tracker_cell_ppt.h
/* Author(s) :    François Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2026-02-18
 * Last modified: 2026-02-18
 *
 * Description: Tracker cell PPT informations
 */

#ifndef FALAISE_SNEMO_RC_TRACKER_CELL_PPT_H
#define FALAISE_SNEMO_RC_TRACKER_CELL_PPT_H

// Standard library:
#include <limits>
#include <vector>
#include <string>

// Bayeux/datatools:
#include <bayeux/geomtools/geom_id.h>

// This project:
#include "falaise/snemo/time/time_utils.h"

namespace snemo {
  
  namespace rc {
   
    enum class tracker_cell_ppt_type
      { 
				normal         = 0,
				missing_bottom = 1,
				missing_top    = 2,
				inoperative    = 3
      };

    std::string to_string(const tracker_cell_ppt_type ppt_type_);

    struct tracker_cell_ppt_info
    {
			static tracker_cell_ppt_info make_inoperative();
      bool is_valid() const;
      tracker_cell_ppt_type type = tracker_cell_ppt_type::normal;
      double ppt = std::numeric_limits<double>::quiet_NaN();
      double ppt_error = std::numeric_limits<double>::quiet_NaN();
			friend std::ostream & operator<<(std::ostream & out_, const tracker_cell_ppt_info & record_);
   };
 
    struct tracker_cell_ppt_record
    {
      time::time_period period = snemo::time::invalid_period();
      tracker_cell_ppt_info info;
      friend std::ostream & operator<<(std::ostream & out_, const tracker_cell_ppt_record & record_);
    };
 		
    class tracker_cell_ppt_history
    {
    public:
      tracker_cell_ppt_history() = default;
      tracker_cell_ppt_history(const std::vector<tracker_cell_ppt_record> & records_);
      void set(const std::vector<tracker_cell_ppt_record> & records_);
      void add(const time::time_period & period_, const tracker_cell_ppt_info & info_);
      void clear();
      const std::vector<tracker_cell_ppt_record> & records() const;
      const tracker_cell_ppt_info & get_ppt_info(const time::time_point & t_) const;
      void print(std::ostream & out_, const std::string & indent_ = "") const;

    private:
      std::vector<tracker_cell_ppt_record> _records_;
    };
 
    typedef std::map<geomtools::geom_id, snemo::rc::tracker_cell_ppt_history> tracker_cell_ppt_history_map;

  } // end of namespace rc
  
} // end of namespace snemo

#endif // FALAISE_SNEMO_RC_TRACKER_CELL_PPT_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
