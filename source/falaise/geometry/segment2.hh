#ifndef FALAISE_GEOMETRY_SEGMENT2_HH
#define FALAISE_GEOMETRY_SEGMENT2_HH

// Standard library:
#include <iostream>
#include <limits>
#include <string>

// This project:
#include <falaise/geometry/i_curve2.hh>
#include <falaise/geometry/point.hh>
#include <falaise/geometry/line2.hh>

namespace falaise {
  
  namespace geometry {

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
   
  } // namespace geometry 

} // namespace falaise 

#endif // FALAISE_GEOMETRY_SEGMENT2_HH
