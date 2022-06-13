/// \file falaise/snemo/rc/tracker_cell_event.h
/* Author(s) :    François Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2022-04-27
 * Last modified: 2022-04-27
 *
 * Description: Run event
 */

#ifndef FALAISE_SNEMO_RC_TRACKER_CELL_EVENT_H
#define FALAISE_SNEMO_RC_TRACKER_CELL_EVENT_H

// Standard library:
#include <cstdint>
#include <iostream>
#include <string>

// This project:
#include <falaise/snemo/rc/base_event.h>
#include <falaise/snemo/rc/run_description.h>

namespace snemo {

  namespace rc {
    
    /// \brief Tracker cell event category
    enum class tracker_cell_event_category : std::uint16_t 
      {
       INDETERMINATE,
       ON,
       OFF,
       START_NOISY,
       STOP_NOISY,
       START_TRIP,
       STOP_TRIP
      };

    std::ostream & operator<<(std::ostream & out_, tracker_cell_event_category cat_);

    class tracker_cell_event
      : public base_event
    {
    public:
   
      tracker_cell_event() = default;
      
      virtual ~tracker_cell_event() = default;
    
      inline tracker_cell_event_category category() const
      {
        return _category_;
      }
           
      inline const geomtools::geom_id & cell_id() const
      {
        return _cell_id_;
      }

      /// Smart print
      virtual void print_tree(std::ostream & out_ = std::clog,
                              const boost::property_tree::ptree & options_ = empty_options()) const override;

      
    private:

      tracker_cell_event_category _category_ = tracker_cell_event_category::INDETERMINATE;
      geomtools::geom_id _cell_id_;

    };
    
  } // end of namespace rc

} // end of namespace snemo

#endif // FALAISE_SNEMO_RC_TRACKER_CELL_EVENT_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
