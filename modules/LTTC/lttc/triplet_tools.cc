#include <lttcc/triplet_tools.hpp>

// Standard library:
#include <cmath>
#include <stdexcept>

// This project:
#include <lttcc/line.hpp>

namespace lttcc {

  hits_ijk::hits_ijk(int i_, int j_, int k_)
  {
    if (i_ == j_) {
      throw std::logic_error("lttcc::hit_ijk::hit_ijk: i==j");
    }
    if (j_ == k_) {
      throw std::logic_error("lttcc::hit_ijk::hit_ijk: j==k");
    }
    if (k_ == i_) {
      throw std::logic_error("lttcc::hit_ijk::hit_ijk: k==i");
    }
    i = i_;
    j = j_;
    k = k_;
    if (j < i) std::swap(i, j);
    if (k < i) std::swap(i, k);
    if (k < j) std::swap(j, k);
    return;
  }

  bool hits_ijk::operator==(const hits_ijk & other_) const
  {
    if (!is_valid() || !other_.is_valid()) return false;
    return (i == other_.i) && (j == other_.j) && (k == other_.k);
  }

  bool hits_ijk::operator<(const hits_ijk & other_) const
  {
    if (!is_valid() || !other_.is_valid()) return false;
    if (i < other_.i) return true;
    if (i > other_.i) return false;
    if (j < other_.j) return true;
    if (j > other_.j) return false;
    if (k < other_.k) return true;
    if (k > other_.k) return false;
    return false;
  }

  // friend
  std::ostream & operator<<(std::ostream & out_, const hits_ijk & ijk_)
  {
    out_ << '[' << ijk_.i << ',' << ijk_.j << ',' << ijk_.k << ']';
    return out_;
  }
 
  void hits_ijk::clear()
  {
    i = -1;
    j = -1;
    k = -1;
    return;
  }
  
  bool hits_ijk::is_valid() const
  {
    if (i < 0) return false;
    if (j < 0) return false;
    if (k < 0) return false;
    return true;
  }

  void triplet_data::clear()
  {
    triplets.clear();
    owner_hits.clear();
    return;
  }

  triplet_tools::triplet_tools(const tracker & sntracker_)
    : sntracker(sntracker_)
  {
    return;
  }
 

  void triplet_tools::make_from_hits(const hit_collection & hits_,
                                     const std::vector<int> & removed_hits_,
                                     triplet_data & tdata_) const
  {
    bool debug = false;
    // debug = true;
    tdata_.triplets.clear();
    size_t nhits = hits_.size();
    int nt = nhits * (nhits - 1) * (nhits - 2) / 6;
    if (debug) std::cerr << "[debug] lttcc::triplet_tools::make_from_hits: "
                         << "nt=" << nt << " expected triplets" << '\n';
    tdata_.triplets.reserve(nt);
    tdata_.owner_hits.reserve(nt);
    tdata_.circles.reserve(nt);
    for (int ihit1 = 0; ihit1 < (int) hits_.size(); ihit1++) {
      if (debug) std::cerr << "[debug] lttcc::triplet_tools::make_from_hits: "
                           << "ihit1=" << ihit1 << '\n';
      if (removed_hits_[ihit1] >= 0) continue;
      const hit & h1 = hits_[ihit1];
      point hp1 = sntracker.cell_position(h1.ilayer, h1.irow);
      for (int ihit2 = ihit1+1; ihit2 < (int) hits_.size(); ihit2++) {
        if (debug) std::cerr << "[debug] lttcc::triplet_tools::make_from_hits: "
                             << "  ihit2=" << ihit2 << '\n';
        if (removed_hits_[ihit2] >= 0) continue;
        const hit & h2 = hits_[ihit2];
        point hp2 = sntracker.cell_position(h2.ilayer, h2.irow);
        double d2hp12 = (hp2 - hp1).mag2();
        for (int ihit3 = ihit2+1; ihit3 < (int) hits_.size(); ihit3++) {
          if (debug) std::cerr << "[debug] lttcc::triplet_tools::make_from_hits: "
                               << "    ihit3=" << ihit3 << '\n';
          if (removed_hits_[ihit3] >= 0) continue;
          if (debug) std::cerr << "[debug] lttcc::triplet_tools::make_from_hits: "
                               << "(i,j,k)=(" << ihit1 << ',' << ihit2 << ',' << ihit3 << ")" << '\n';
          const hit & h3 = hits_[ihit3];
          point hp3 = sntracker.cell_position(h3.ilayer, h3.irow);
          double d2hp23 = (hp3 - hp2).mag2();
          double d2hp31 = (hp3 - hp1).mag2();
          int ieff = ihit1;
          int jeff = ihit3;
          int keff = ihit2;
          const point * p1 = &hp1;
          const point * p2 = &hp3;
          const point * p3 = &hp2;
          double d2max = d2hp12;
          if (d2hp23 > d2max) {
            ieff = ihit3;
            jeff = ihit1;
            keff = ihit2;
            p1 = &hp3;
            p2 = &hp1;
            p3 = &hp2;
          }
          if (d2hp31 > d2max) {
            ieff = ihit1;
            jeff = ihit2;
            keff = ihit3;
            p1 = &hp1;
            p2 = &hp2;
            p3 = &hp3;
          }
          std::cerr << "[debug] lttcc::triplet_tools::make_from_hits: "
                    << "(i,j,k)=(" << ihit1 << ',' << ihit2 << ',' << ihit3 << ")" << '\n';

          vec2 p1p2(*p1, *p2);
          vec2 p1p3(*p1, *p3);
          vec2 p2p3(*p2, *p3);
          vec2 perpP1p2 = p1p2.perp();
          vec2 perpP1p3 = p1p3.perp();
          vec2 perpP2p3 = p2p3.perp();
          double r1     = hits_[ieff].drift_radius;
          double r1_err = hits_[ieff].drift_radius_err;
          double r2     = hits_[jeff].drift_radius;
          double r2_err = hits_[jeff].drift_radius_err;
          double r3     = hits_[keff].drift_radius;
          double r3_err = hits_[keff].drift_radius_err;
          double t1_err = std::sqrt(2 * r1_err / r1);
          double t2_err = std::sqrt(2 * r2_err / r2);
          double t3_err = std::sqrt(2 * r3_err / r3);
          /// XXx
          line lp1p3(*p1, *p3);
          line lp1p2(*p1, *p2);
          line lp2p3(*p2, *p3);
          std::cerr << "[debug] lttcc::triplet_tools::make_from_hits: "
                    << "Line lp1p3 : " << '\n';
          lp1p3.print(std::cerr, "[debug] ");
          vec2 perp13Dir(lp1p3.a, lp1p3.b);
          vec2 perp12Dir(lp1p2.a, lp1p2.b);
          vec2 perp23Dir(lp2p3.a, lp2p3.b);
          std::vector<point> vpbt[3];
          vpbt[0].push_back(*p1 - perpP1p2 * r1);
          vpbt[0].push_back(*p1 + perpP1p2 * r1);
          vpbt[1].push_back(*p2 - perpP1p3 * r2);
          vpbt[1].push_back(*p2 + perpP1p3 * r2);
          vpbt[2].push_back(*p3 - perpP2p3 * r3);
          vpbt[2].push_back(*p3 + perpP2p3 * r3);
          // point pbt[3][2];
          // pbt[0][0] = *p1 - perpP1p2 * r1;
          // pbt[0][1] = *p1 + perpP1p2 * r1;
          // pbt[1][0] = *p2 - perpP1p3 * r2;
          // pbt[1][1] = *p2 + perpP1p3 * r2;
          // pbt[2][0] = *p3 - perpP2p3 * r3;
          // pbt[2][1] = *p3 + perpP2p3 * r3;
          for (int ip = 0; ip < (int) vpbt[0].size(); ip++) {
            for (int jp = 0; jp < (int) vpbt[1].size(); jp++) {
              for (int kp = 0; kp < (int) vpbt[2].size(); kp++) {
                point mid = vpbt[1][jp];
                if (are_aligned(vpbt[0][ip], mid, vpbt[2][kp])) {
                  if (jp == 0) {
                    mid = *p2 - perp13Dir * (r2 + r2_err);
                  } else {
                    mid = *p2 + perp13Dir * (r2 + r2_err);
                  }
                }
                triplet t123(vpbt[0][ip], mid, vpbt[2][kp]);
                tdata_.triplets.push_back(t123);
                circle cijk = circle::make_circle(t123);
                double cp1 = cijk.dist(*p1);
                double cp2 = cijk.dist(*p2);
                double cp3 = cijk.dist(*p3);
                if (cp1 < r1 - nrsig * r1_err) continue; 
                if (cp2 < r2 - nrsig * r2_err) continue; 
                if (cp3 < r3 - nrsig * r3_err) continue; 
                tdata_.circles.push_back(cijk);
                hits_ijk hijk(ieff, jeff, keff);
                tdata_.owner_hits.push_back(hijk);
              }
            }
          }
          std::cerr << "[debug] lttcc::triplet_tools::make_from_hits: "
                    << "Done (i,j,k)=(" << ihit1 << ',' << ihit2 << ',' << ihit3 << ")" << '\n';
       }    
      }    
    }
    return;
  }
  
} // namespace lttcc 
