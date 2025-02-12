#ifndef FALAISE_TKRECONSTRUCT_TKPOINT_H
#define FALAISE_TKRECONSTRUCT_TKPOINT_H

// Standard headers
#include <iostream>
#include <cmath>
#include <memory>

// ROOT headers
// #include "TObject.h"

namespace tkrec {

  class TKpoint
  //  : public TObject
  {
  private:
		
    double x = 0.0;
    double y = 0.0;
    double z = 0.0;

  public:
		
    TKpoint() = default;
    TKpoint(double _x, double _y, double _z);
    virtual ~TKpoint() = default;
		
    void set_x(double _x);
    void set_y(double _y);
    void set_z(double _z);
		
    double get_x() const;
    double get_y() const;
    double get_z() const;

    void print(std::ostream & out_ = std::clog) const;
		
    // ClassDef(TKpoint,1);
				
  };

  double distance_2D(const TKpoint &point1, const TKpoint &point2);
  double distance_3D(const TKpoint &point1, const TKpoint &point2);

  typedef std::shared_ptr<TKpoint> TKpointHdl;
  typedef std::shared_ptr<const TKpoint> ConstTKpointHdl;

} //  end of namespace tkrec

#endif // FALAISE_TKRECONSTRUCT_TKPOINT_H
