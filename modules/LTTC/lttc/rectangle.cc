#include <lttc/rectangle.hh>

// Standard library:
#include <cmath>
#include <stdexcept>

// Third party:
// - Bayeux/datatools :
#include <datatools/exception.h>

namespace lttc {

  rectangle::rectangle(const point & ll_, const point & ur_)
  {
    DT_THROW_IF(ll_.x() > ur_.x(), std::logic_error, "Invalid ll/ur x-coordinates!");
    DT_THROW_IF(ll_.y() > ur_.y(), std::logic_error, "Invalid ll/ur y-coordinates!");
    ll = ll_;
    ur = ur_;
  }

  rectangle::rectangle(const point & c_, double dx_, double dy_)
    : rectangle(c_ - vector2(0.5 * dx_, 0.5 * dy_), c_ + vector2(0.5 * dx_, 0.5 * dy_))
  {
    return;
  }

  point rectangle::LL() const
  {
    return ll;
  }
  
  point rectangle::LR() const
  {
    point lr(ur.x(), ll.y());
    return lr;
  }
  
  point rectangle::UL() const
  {
    point ul(ll.x(), ur.y());
    return ul;
  }
  
  point rectangle::UR() const
  {
    return ur;
  }
     
  bool rectangle::contains(const point & c_, double xtolerance_, double ytolerance_) const
  {
    double xmin = ll.x() + xtolerance_;
    double xmax = ur.x() - xtolerance_;
    double ymin = ll.y() + ytolerance_;
    double ymax = ur.y() - ytolerance_;
    if (c_.x() <= xmin) return false;
    if (c_.x() >= xmax) return false;
    if (c_.y() <= ymin) return false;
    if (c_.y() >= ymax) return false;
    return true;
  }
   
  bool rectangle::contains(const point & c_, double tolerance_) const
  {
    return contains(c_, tolerance_, tolerance_);
  }

  bool rectangle::overlap(const rectangle & o_, double xtolerance_, double ytolerance_) const
  {
    double oxmin = o_.ll.x() + xtolerance_;
    double oxmax = o_.ur.x() - xtolerance_;
    double oymin = o_.ll.y() + ytolerance_;
    double oymax = o_.ur.y() - ytolerance_;
    double xmin = ll.x();
    double xmax = ur.x();
    double ymin = ll.y();
    double ymax = ur.y();
    if (xmax < oxmin) return false;
    if (xmin > oxmax) return false;
    if (ymax < oymin) return false;
    if (ymin > oymax) return false;
    return true;
  }
  
  bool rectangle::overlap(const rectangle & o_, double tolerance_) const
  {
    return overlap(o_, tolerance_, tolerance_);
  }

  bool rectangle::contains(const rectangle & other_, double xtolerance_, double ytolerance_) const
  {
    if (!contains(other_.LL(), xtolerance_, ytolerance_)) return false;
    if (!contains(other_.LR(), xtolerance_, ytolerance_)) return false;
    if (!contains(other_.UL(), xtolerance_, ytolerance_)) return false;
    if (!contains(other_.UR(), xtolerance_, ytolerance_)) return false;
    return true;
  }

  bool rectangle::contains(const rectangle & other_, double tolerance_) const
  {
    return contains(other_, tolerance_, tolerance_);
  }

  double rectangle::width() const
  {
    return ur.x() - ll.x();
  }

  double rectangle::height() const
  {
    return ur.y() - ll.y();
  }

  double rectangle::surface() const
  {
    return width() * height(); 
  }

  void rectangle::draw(std::ostream & out_) const
  {
    out_ << ll.x() << ' ' << ll.y() << '\n';
    out_ << ll.x() << ' ' << ur.y() << '\n';
    out_ << ur.x() << ' ' << ur.y() << '\n';
    out_ << ur.x() << ' ' << ll.y() << '\n';
    out_ << ll.x() << ' ' << ll.y() << '\n';
    out_ << std::endl;
    return;
  }

  void rectangle::draw_vertexes(std::ostream & out_) const
  {
    out_ << LL() << '\n';
    out_ << LR() << '\n';
    out_ << UR() << '\n';
    out_ << UL() << '\n';
    out_ << '\n';
    return;
  }

} // namespace lttc 
