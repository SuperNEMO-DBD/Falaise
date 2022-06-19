#include <lttc/track.hh>

// Standard library:
// #include <cmath>

// Third party:
// - Bayeux :
#include <geomtools/utils.h>

// This project:
#include <falaise/geometry/rectangle.hh>

namespace lttc {

  using falaise::geometry::point2;
  using falaise::geometry::vector2;
  using falaise::geometry::point3;
  using falaise::geometry::polyline2;
  using falaise::geometry::draw_polyline;
  using falaise::geometry::polyline3;
  using falaise::geometry::rectangle;

  void track2::draw(std::ostream & out_) const
  {
    polyline2 currentPl;
    for (size_t i = 0; i < this->pl.size(); i++) {
      const point2 & pi = this->pl[i];
      double si = this->pl.s[i];
      currentPl.push_back(pi);
      currentPl.s.push_back(si);
      if (this->cuts.count(i + 1)) {
        draw_polyline(out_, currentPl);
        currentPl.clear();
      }
    }
    if (currentPl.size()) {
      draw_polyline(out_, currentPl);
    }
    if (flags & KINK) {
      rectangle kinkRect(kink - vector2(5,5), 10, 10);
      kinkRect.draw(out_);
    }
    return;
  }

  void track2::print(std::ostream & out_, const std::string & indent_, bool inherit_) const
  {
    static const std::string tag{"|-- "};
    static const std::string ltag{"`-- "};
    static const std::string stag{"|   "};
    out_ << indent_ << tag << "ID = " << id << '\n';
    out_ << indent_ << tag << "flags = " << flags << " :";
    if (flags & LINE) {
      out_ << " line";
    }
    if (flags & CIRCLE) {
      out_ << " circle";
    }
    if (flags & KINK) {
      out_ << " kinked";
    }
    out_ << '\n';
    out_ << indent_ << tag << "XY-polyline = " << pl.size() << '\n';
    if (pl.size()) {
      out_ << indent_ << stag << tag  << "first : " << pl[0] << " @s=" << pl.s[0] << '\n';
      out_ << indent_ << stag << ltag << "last  : " << pl[pl.size()-1] << " @s=" << pl.s[pl.size()-1] << '\n';
    }
    out_ << indent_ << tag << "cuts = {";
    for (auto icut : cuts) out_ << icut << ' ';
    out_ << "}" << std::endl;
    out_ << indent_ << tag << "s_start = " << s_start << '\n';
    out_ << indent_ << tag << "s_stop  = " << s_stop << '\n';
    out_ << indent_ << tag << "kink    = " << kink << '\n';
    out_ << indent_ << (inherit_ ? tag : ltag) << "s_kink  = " << s_kink << '\n';
    return;
  }

  // static
  void track3::make_from_xy_track(const track2 & trk2_, track3 & trk3_,
                                  double zstart_, double alpha_)
  {
    bool devel = false;
    //  devel = true;
    if (devel) {
      std::cerr << "[devel] track3::make_from_xy_track:    trk2=" << '\n';
      trk2_.print(std::cerr, "[devel]      ");
    }
    trk3_.id      = trk2_.id;
    trk3_.flags   = trk2_.flags;
    trk3_.pl      = trk2_.pl;
    trk3_.s_start = trk2_.s_start;
    trk3_.s_stop  = trk2_.s_stop;
    trk3_.kink    = trk2_.kink;
    trk3_.s_kink  = trk2_.s_kink;
    trk3_.z.assign(trk3_.pl.size(), 0.0);
    if (trk3_.pl.size()) {
      trk3_.z[0] = zstart_;
      double tanAlpha = std::tan(alpha_);
      if (devel) {
        std::cerr << "[devel] track3::make_from_xy_track:    trk3=" << '\n';
        trk3_.print(std::cerr, "[devel]      ");
        std::cerr << "[devel] track3::make_from_xy_track: trk3_.s_kink=" << trk3_.s_kink << '\n';
        std::cerr << "[devel] track3::make_from_xy_track: trk3_.pl.size()=" << trk3_.pl.size() << '\n';
        std::cerr << "[devel] track3::make_from_xy_track: tanAlpha=" << tanAlpha << '\n';
      }
      for (int i = 1; i < (int) trk3_.pl.size(); i++) {
        double si = trk3_.pl.s[i];
        double sim = trk3_.pl.s[i-1];
        trk3_.z[i] = trk3_.z[i-1] + (trk3_.pl[i] - trk3_.pl[i-1]).mag() * tanAlpha;
        if (devel) {
          std::cerr << "[devel]   sim=" << sim << '\n';
          std::cerr << "[devel]   si="  << si << '\n';
          std::cerr << "[devel]   trk3_.z[i]="  << trk3_.z[i] << '\n';
        }
        if (datatools::is_valid(trk3_.s_kink)) {
          if (not datatools::is_valid(trk3_.z_kink)) {
            if (trk3_.s_kink >= sim and trk3_.s_kink < si) {
              trk3_.z_kink = trk3_.z[i-1] + (trk3_.s_kink - sim) * (trk3_.z[i] - trk3_.z[i-1]) / (si - sim);
              if (devel) {
                std::cerr << "[devel]   trk3_.z_kink ="  << trk3_.z_kink  << '\n';
              }
            }
          }
        }
      }
    }
    return;
  }
  
  void track3::print(std::ostream & out_, const std::string & indent_, bool inherit_) const
  {
    static const std::string tag{"|-- "};
    static const std::string ltag{"`-- "};
    static const std::string stag{"|   "};
    this->track2::print(out_, indent_, true);
    out_ << indent_ << tag << "z-array = " << z.size() << '\n';
    if (pl.size()) {
      out_ << indent_ << stag << tag  << "first : " << z[0] << '\n';
      out_ << indent_ << stag << ltag << "last  : " << z[z.size()-1] << '\n';
    }
    out_ << indent_ << (inherit_ ? tag : ltag) << "z_kink  = " << z_kink << '\n';
    return;
  }
  
  void track3::draw(std::ostream & out_) const
  {
    polyline3 currentPl;
    for (int i = 0; i < (int) this->pl.size(); i++) {
      const point3 & pi = this->pl[i];
      double si = this->pl.s[i];
      currentPl.push_back(pi);
      currentPl.s.push_back(si);
      if (this->cuts.count(i + 1)) {
        draw_polyline(out_, currentPl);
        currentPl.clear();
      }
    }
    if (currentPl.size()) {
      draw_polyline(out_, currentPl);
    }
    // draw_polyline(out_, this->pl);
    return;
  }

  void track3::make(polyline3 & p3_) const
  {
    p3_.clear();
    p3_.reserve(this->pl.size());
    for (int i = 0; i < (int) this->pl.size(); i++) {
      const point2 & p2 = this->pl[i];
      point3 p3(p2.x(), p2.y(), this->z[i]);
      p3_.push_back(p3);
    }
    return;
  }
 
} // namespace lttc 

  
