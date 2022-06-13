/// \file falaise/snemo/rc/run_event.h
/* Author(s) :    François Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2022-04-27
 * Last modified: 2022-04-27
 *
 * Description: Run event
 */

#ifndef FALAISE_SNEMO_RC_RUN_EVENT_H
#define FALAISE_SNEMO_RC_RUN_EVENT_H

// Standard library:
#include <cstdint>
#include <iostream>
#include <string>

// This project:
#include <falaise/snemo/rc/base_event.h>
#include <falaise/snemo/rc/run_description.h>

namespace snemo {

  namespace rc {

    /// \brief Run event category
    enum class run_event_category : std::uint16_t 
      {
       INDETERMINATE,
       START,
       PAUSE,
       RESUME,
       STOP,
       CRASH
       // EVENT_COUNT
       // IO_BOTTLENECK
      };
    
    std::ostream & operator<<(std::ostream & out_, run_event_category evcat_);
   
    /// \brief Run event
    class run_event
      : public base_event
    {
    public:
   
      run_event() = default;
      
      run_event(const time::time_point & timestamp_,
                run_event_category category_,
                std::int32_t run_id_);
      
      run_event(const time::time_point & timestamp_,
                const time::time_duration & duration_,
                run_event_category category_,
                std::int32_t run_id_);

      virtual ~run_event() = default;
     
      inline run_event_category category() const
      {
        return _category_;
      }
      
      inline std::int32_t run_id() const
      {
        return _run_id_;
      }

      /// Smart print
      virtual void print_tree(std::ostream & out_ = std::clog,
                              const boost::property_tree::ptree & options_ = empty_options()) const override;
      
    private:

      run_event_category _category_ = run_event_category::INDETERMINATE;
      std::int32_t _run_id_ = run_description::INVALID_RUN_ID;

    };
    
  } // end of namespace rc

} // end of namespace snemo

#endif // FALAISE_SNEMO_RC_RUN_EVENT_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
