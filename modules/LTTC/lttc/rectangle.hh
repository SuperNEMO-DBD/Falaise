#ifndef FALAISE_LTTC_PLUGIN_RECTANGLE_HH
#define FALAISE_LTTC_PLUGIN_RECTANGLE_HH

// Standard library:
#include <iostream>

// This project:
// #include <lttc/i_curve.hh>
#include <lttc/point.hh>

namespace lttc {

  /// \brief Rectangle in a plane with sides parallel to the X and Y axis
  ///
  /// \code
  ///             y
  ///            ^
  ///            :        ur
  ///    +---------------+
  ///    |       :       |
  ///    |       :       |
  /// - -| - - - +- - - -|- - -> x
  ///    |       :       |
  ///    |       :       |
  ///    +---------------+
  ///   ll       :
  ///
  /// \endcode
  ///
  struct rectangle
  {
  
    rectangle() = default;
    
    virtual ~rectangle() = default;

    rectangle(const point2 & ll_, const point2 & ur_);

    rectangle(const point2 & c_, double dx_, double dy_);
    
    bool contains(const point2 & c_, double xtolerance_, double ytolerance_) const; 
    
    bool contains(const point2 & c_, double tolerance_ = 0.0) const; 
    
    bool on_boundary(const point2 & c_, double tolerance_ = 0.0) const; 
    
    bool outside(const point2 & c_, double tolerance_ = 0.0) const; 

    bool overlap(const rectangle & other_, double xtolerance_, double ytolerance_) const; 

    bool overlap(const rectangle & other_, double tolerance_ = 0.0) const; 

    bool contains(const rectangle & other_, double xtolerance_, double ytolerance_) const; 

    bool contains(const rectangle & other_, double tolerance_) const; 
    
    bool unique_intersection(const point2 & from_,
                             const vector2 & direction_,
                             point2 & root_,
                             double tolerance_) const;

    double width() const;

    double height() const;

    double surface() const;
    
    void draw(std::ostream & out_, int tag_ = 0) const;

    void draw_vertexes(std::ostream & out_, int tag_ = 0) const;

    point2 LL() const;
    
    point2 LR() const;
    
    point2 UL() const;
    
    point2 UR() const;
    
    // Attributes:
    point2 ll; ///< Low left corner
    point2 ur; ///< Upper right corner
    
  };
  
} // namespace lttc 

#endif //FALAISE_LTTC_PLUGIN_RECTANGLE_HH
