#ifndef FALAISE_LTTC_PLUGIN_RECTANGLE_HH
#define FALAISE_LTTC_PLUGIN_RECTANGLE_HH

// Standard library:
#include <iostream>

// This project:
// #include <lttc/i_curve.hh>
#include <lttc/point.hh>

namespace lttc {

  /// \brief Rectangle in a plane
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

    rectangle(const point & ll_, const point & ur_);

    rectangle(const point & c_, double dx_, double dy_);
    
    bool contains(const point & c_, double xtolerance_, double ytolerance_) const; 
    
    bool contains(const point & c_, double tolerance_ = 0.0) const; 

    bool overlap(const rectangle & other_, double xtolerance_, double ytolerance_) const; 

    bool overlap(const rectangle & other_, double tolerance_ = 0.0) const; 

    bool contains(const rectangle & other_, double xtolerance_, double ytolerance_) const; 

    bool contains(const rectangle & other_, double tolerance_) const; 

    double width() const;

    double height() const;

    double surface() const;
    
    void draw(std::ostream & out_) const;

    void draw_vertexes(std::ostream & out_) const;

    point LL() const;
    
    point LR() const;
    
    point UL() const;
    
    point UR() const;
    
    point ll; ///< Low left corner
    point ur; ///< Upper right corner
    
  };
  
} // namespace lttc 

#endif //FALAISE_LTTC_PLUGIN_RECTANGLE_HH
