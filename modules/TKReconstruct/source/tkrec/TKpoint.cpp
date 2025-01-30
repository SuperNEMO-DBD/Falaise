// TK headers
#include "tkrec/TKpoint.h"

ClassImp(tkrec::TKpoint);

namespace tkrec {

  using namespace std;

  TKpoint::TKpoint(double _x, double _y, double _z)
    : x(_x)
    , y(_y)
    , z(_z)
  {
  }

  void TKpoint::set_x(double _x)
  {
    x = _x;
  }

  void TKpoint::set_y(double _y)
  {
    y = _y;
  }

  void TKpoint::set_z(double _z)
  {
    z = _z;
  }

  double TKpoint::get_x() const
  {
    return x;
  }

  double TKpoint::get_y() const
  {
    return y;
  }

  double TKpoint::get_z() const
  {
    return z;
  }

  void TKpoint::print(std::ostream & out_) const
  {
    out_ << "point (" << x << ", " << y << ", " << z << ")" << std::endl;
  }

  double distance_2D(const TKpoint &point1, const TKpoint &point2)
  {
    double temp = pow(point1.get_x() - point2.get_x(), 2) + pow(point1.get_y() - point2.get_y(), 2);
    return sqrt(temp);
  }

  double distance_3D(const TKpoint &point1, const TKpoint &point2)
  {
    double temp = pow(point1.get_x() - point2.get_x(), 2) + pow(point1.get_y() - point2.get_y(), 2) + pow(point1.get_z() - point2.get_z(), 2);
    return sqrt(temp);
  }

} //  end of namespace tkrec
