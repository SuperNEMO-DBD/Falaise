#ifndef FALAISE_LTTC_PLUGIN_SEGMENT_HH
#define FALAISE_LTTC_PLUGIN_SEGMENT_HH

// Standard library:
#include <iostream>
#include <limits>
#include <string>

// This project:
#include <lttc/i_curve.hh>
#include <lttc/point.hh>
#include <lttc/line.hh>

namespace lttc {

  /// \brief Segment in 2D space (non zero length)
  struct segment 
  {
  
    segment();

    segment(const point & first_, const point & last_);
    
    virtual ~segment() = default;

    bool is_valid() const;

    void set(const point & first_, const point & last_);

    const point & get_first() const;

    const point & get_last() const;

    double get_length() const;

    const line & get_line() const;

    bool contains(const point & pos_, double tolerance_) const;

    void reset();

  private:

    point _first_;
    point _last_;
    // Internal data
    double _length_;
    line  _line_;
    
  };
   
} // namespace lttc 

#endif // FALAISE_LTTC_PLUGIN_SEGMENT_HH
