/// \file falaise/snemo/rc/base_event.h
/* Author(s) :    François Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2022-04-27
 * Last modified: 2022-04-27
 *
 * Description: Base running condition event
 */

#ifndef FALAISE_SNEMO_RC_BASE_EVENT_H
#define FALAISE_SNEMO_RC_BASE_EVENT_H

// Bayeux:
#include <bayeux/datatools/i_tree_dump.h>

// This project:
#include <falaise/snemo/time/time_utils.h>

namespace snemo {

  namespace rc {

    /// \brief Base class for RC event
    class base_event
      : public datatools::i_tree_dumpable
    {
    public:

      base_event() = default;
      
      base_event(const time::time_point & timestamp_);
      
      base_event(const time::time_point & timestamp_, const time::time_duration & duration_);

      virtual ~base_event() = default;

      void set_timestamp(const time::time_point & timestamp_);
      
      inline const time::time_point & timestamp() const
      {
        return _timestamp_;
      }

      void set_duration(const time::time_duration & duration_);

      void reset();
      
      inline const time::time_duration & duration() const
      {
        return _duration_;
      }

      inline bool is_instantaneous() const
      {
        return _duration_.is_not_a_date_time(); // or _duration_.is_zero();
      }

      /// Smart print
      virtual void print_tree(std::ostream & out_ = std::clog,
                              const boost::property_tree::ptree & options_ = empty_options()) const override;
 
    private:
      
      time::time_point _timestamp_{time::not_a_date_time};
      time::time_duration _duration_{time::not_a_date_time};

    };
    
  } // end of namespace rc

} // end of namespace snemo

#endif // FALAISE_SNEMO_RC_BASE_EVENT_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
