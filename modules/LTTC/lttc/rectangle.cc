#include <lttc/rectangle.hh>

// Standard library:
#include <cmath>
#include <stdexcept>

// Third party:
// - Bayeux/datatools :
#include <datatools/exception.h>
#include <datatools/utils.h>
#include <geomtools/utils.h>

// This project:
#include <lttc/line2.hh>

namespace lttc {

  rectangle::rectangle(const point2 & ll_, const point2 & ur_)
  {
    DT_THROW_IF(ll_.x() > ur_.x(), std::logic_error, "Invalid ll/ur x-coordinates!");
    DT_THROW_IF(ll_.y() > ur_.y(), std::logic_error, "Invalid ll/ur y-coordinates!");
    ll = ll_;
    ur = ur_;
  }

  rectangle::rectangle(const point2 & c_, double dx_, double dy_)
    : rectangle(c_ - vector2(0.5 * dx_, 0.5 * dy_), c_ + vector2(0.5 * dx_, 0.5 * dy_))
  {
    return;
  }

  point2 rectangle::LL() const
  {
    return ll;
  }
  
  point2 rectangle::LR() const
  {
    point2 lr(ur.x(), ll.y());
    return lr;
  }
  
  point2 rectangle::UL() const
  {
    point2 ul(ll.x(), ur.y());
    return ul;
  }
  
  point2 rectangle::UR() const
  {
    return ur;
  }
   
  bool rectangle::outside(const point2 & c_, double tolerance_) const
  {
    double xmin = ll.x() - tolerance_;
    double xmax = ur.x() + tolerance_;
    double ymin = ll.y() - tolerance_;
    double ymax = ur.y() + tolerance_;
    if (c_.x() < xmin) return true;
    if (c_.x() > xmax) return true;
    if (c_.y() < ymin) return true;
    if (c_.y() > ymax) return true;
    return false;
  }
   
  bool rectangle::on_boundary(const point2 & c_, double tolerance_) const
  {
    double xmin = ll.x() - tolerance_;
    double xmax = ur.x() + tolerance_;
    double ymin = ll.y() - tolerance_;
    double ymax = ur.y() + tolerance_;
    if (c_.x() < xmin) return false;
    if (c_.x() > xmax) return false;
    if (c_.y() < ymin) return false;
    if (c_.y() > ymax) return false;
    xmin = ll.x() + tolerance_;
    xmax = ur.x() - tolerance_;
    ymin = ll.y() + tolerance_;
    ymax = ur.y() - tolerance_;
    if (c_.x() < xmin) return true;
    if (c_.x() > xmax) return true;
    if (c_.y() < ymin) return true;
    if (c_.y() > ymax) return true;
    return false;
  }
     
  bool rectangle::contains(const point2 & c_, double xtolerance_, double ytolerance_) const
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
   
  bool rectangle::contains(const point2 & c_, double tolerance_) const
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

  void rectangle::draw(std::ostream & out_, int tag_) const
  {
    out_ << ll.x() << ' ' << ll.y() << ' ' << tag_ << '\n';
    out_ << ll.x() << ' ' << ur.y() << ' ' << tag_ << '\n';
    out_ << ur.x() << ' ' << ur.y() << ' ' << tag_ << '\n';
    out_ << ur.x() << ' ' << ll.y() << ' ' << tag_ << '\n';
    out_ << ll.x() << ' ' << ll.y() << ' ' << tag_ << '\n';
    out_ << std::endl;
    return;
  }

  void rectangle::draw_vertexes(std::ostream & out_, int tag_) const
  {
    out_ << ll.x() << ' ' << ll.y() << ' ' << tag_ << '\n';
    out_ << ll.x() << ' ' << ur.y() << ' ' << tag_ << '\n';
    out_ << ur.x() << ' ' << ur.y() << ' ' << tag_ << '\n';
    out_ << ur.x() << ' ' << ll.y() << ' ' << tag_ << '\n';
    out_ << std::endl;
    // out_ << LL() << '\n';
    // out_ << LR() << '\n';
    // out_ << UR() << '\n';
    // out_ << UL() << '\n';
    // out_ << '\n';
    return;
  }
   
  bool rectangle::unique_intersection(const point2 & from_,
                                      const vector2 & direction_,
                                      point2 & root_,
                                      double tolerance_) const
  {
    // std::cerr << "Projection from " << from_ << " along " << direction_ << "\n";
    line2 sides[4];
    sides[0] = line2(1.0, 0.0, -ll.x());
    sides[1] = line2(1.0, 0.0, -ur.x());
    sides[2] = line2(0.0, 1.0, -ll.y());
    sides[3] = line2(0.0, 1.0, -ur.y());
    point2 root;
    geomtools::invalidate(root);
    double distToProj;
    datatools::plus_infinity(distToProj);
    for (int iSide = 0; iSide < 4; iSide++) {
      const line2 & thisSide = sides[iSide];
      // thisSide.print(std::cerr, "[debug] side #" + std::to_string(iSide) + " : ");
      point2 proj;
      if (thisSide.unique_projection(from_, direction_, proj)) {
        // std::cerr << "  Projection on side line is : " << proj << "\n";
        // std::cerr << "  Projection to side " << iSide << " to be validated\n";
        double distToProjSide = (proj - from_).mag();
        if (distToProjSide < distToProj) {
          if (this->on_boundary(proj, tolerance_)) {
            // std::cerr << "    Projection to side " << iSide << " is valid\n";
            distToProj = distToProjSide;
            root = proj;
          } else {
            // std::cerr << "    Projection to side " << iSide << " is not valid\n";
          }
        }
      } else {
      }
    }
    if (geomtools::is_valid(root)) {
      root_ = root;
      return true;
    }
    return false; 
  }
 
} // namespace lttc 
