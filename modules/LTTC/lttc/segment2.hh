#ifndef FALAISE_LTTC_PLUGIN_SEGMENT2_HH
#define FALAISE_LTTC_PLUGIN_SEGMENT2_HH

// Standard library:
#include <iostream>
#include <limits>
#include <string>

// This project:
#include <lttc/i_curve2.hh>
#include <lttc/point.hh>
#include <lttc/line2.hh>

namespace lttc {

  /// \brief Segment in 2D space (non zero length)
  struct segment2 
  {
  public:
    
    segment2();

    segment2(const point2 & first_, const point2 & last_);
    
    virtual ~segment2() = default;

    bool is_valid() const;

    void set(const point2 & first_, const point2 & last_);

    const point2 & get_first() const;

    const point2 & get_last() const;

    double get_length() const;

    const line2 & get_line() const;

    bool contains(const point2 & pos_, double tolerance_) const;

    void reset();

  private:

    point2 _first_;
    point2 _last_;
    // Internal data
    double _length_;
    line2  _line_;
    
  };
   
} // namespace lttc 

#endif // FALAISE_LTTC_PLUGIN_SEGMENT2_HH
