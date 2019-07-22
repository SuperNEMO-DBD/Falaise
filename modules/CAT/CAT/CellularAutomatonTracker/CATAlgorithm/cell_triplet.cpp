/* -*- mode: c++ -*- */

#include <CATAlgorithm/cell_triplet.h>

namespace CAT {
namespace topology {

using namespace std;
using namespace mybhep;

//! Default constructor
cell_triplet::cell_triplet() {
  appname_ = "cell_triplet: ";
  free_ = false;
  begun_ = false;
}

//! Default destructor
cell_triplet::~cell_triplet() {}

//! constructor
cell_triplet::cell_triplet(cell_couplet & /*cca*/, cell_couplet & /*ccb*/) {
  appname_ = "cell_triplet: ";
  free_ = false;
  begun_ = false;
}

//! constructor
cell_triplet::cell_triplet(const cell &ca, const cell &cb, const cell &cc, prlevel level,
                           double probmin) {
  set_print_level(level);
  set_probmin(probmin);
  appname_ = "cell_triplet: ";
  ca_ = ca;
  cb_ = cb;
  cc_ = cc;
  free_ = false;
  begun_ = false;
  chi2s_.clear();
  probs_.clear();
}

/*** dump ***/
void cell_triplet::dump(std::ostream &a_out, const std::string &a_title,
                        const std::string &a_indent, bool /* a_inherit */) const {
  std::string indent;
  if (!a_indent.empty()) indent = a_indent;
  if (!a_title.empty()) {
    a_out << indent << a_title << std::endl;
  }

  a_out << indent << appname_ << " -------------- " << std::endl;
  a_out << indent << " free: " << free() << " begun: " << begun() << std::endl;
  a_out << indent << " first cell " << std::endl;
  ca().dump(a_out, "", indent + "   ");
  a_out << indent << " second cell " << std::endl;
  cb().dump(a_out, "", indent + "   ");
  a_out << indent << " third cell " << std::endl;
  cc().dump(a_out, "", indent + "   ");
  for (std::vector<joint>::const_iterator ijoint = joints_.begin(); ijoint != joints_.end();
       ++ijoint)
    ijoint->dump(a_out, "", indent + "   ");
  a_out << indent << " -------------- " << std::endl;

  return;
}

/*** dump ***/
void cell_triplet::dump_joint(joint j, std::ostream &a_out, const std::string &a_title,
                              const std::string &a_indent, bool /* a_inherit*/) const {
  std::string indent;
  if (!a_indent.empty()) indent = a_indent;
  if (!a_title.empty()) {
    a_out << indent << a_title << std::endl;
  }

  a_out << indent << appname_ << " -------------- " << std::endl;
  a_out << indent << " first point " << std::endl;
  this->ca().dump_point(j.epa());
  a_out << indent << " second point " << std::endl;
  this->cb().dump_point(j.epb());
  a_out << indent << " third point " << std::endl;
  this->cc().dump_point(j.epc());
  a_out << indent << " -------------- " << std::endl;

  return;
}

//! set cells
void cell_triplet::set(const cell_couplet &cca, const cell_couplet &ccb) {
  cb_ = cca.ca();
  ca_ = cca.cb();
  cc_ = ccb.cb();
  if (cca.ca().id() != ccb.ca().id()) {
    std::clog << " problem: trying to form a triplet of cell with cells " << cca.ca().id() << " "
              << cca.cb().id() << " " << ccb.ca().id() << " " << ccb.cb().id() << std::endl;
  }
}

//! set cells
void cell_triplet::set(const cell &ca, const cell &cb, const cell &cc) {
  ca_ = ca;
  cb_ = cb;
  cc_ = cc;
}

//! set free level
void cell_triplet::set_free(bool free) { free_ = free; }

//! set begun level
void cell_triplet::set_begun(bool begun) { begun_ = begun; }

//! set joints
void cell_triplet::set_joints(const std::vector<joint> &joints) { joints_ = joints; }

//! set chi2 list
void cell_triplet::set_chi2s(const std::vector<double> &chi2s) { chi2s_ = chi2s; }

//! set prob list
void cell_triplet::set_probs(const std::vector<double> &probs) { probs_ = probs; }

//! get first cell couplet
cell_couplet cell_triplet::cca() {
  cell_couplet cc1(cb_, ca_, print_level(), probmin());
  return cc1;
}

//! get second cell couplet
cell_couplet cell_triplet::ccb() {
  cell_couplet cc2(cb_, cc_, print_level(), probmin());
  return cc2;
}

//! get joints
const std::vector<joint> &cell_triplet::joints() const { return joints_; }

//! get first cell
const cell &cell_triplet::ca() const { return ca_; }

//! get second cell
const cell &cell_triplet::cb() const { return cb_; }

//! get third cell
const cell &cell_triplet::cc() const { return cc_; }

//! get list of chi2
const std::vector<double> &cell_triplet::chi2s() const { return chi2s_; }

//! get list of prob
const std::vector<double> &cell_triplet::probs() const { return probs_; }

//! get free level
bool cell_triplet::free() const { return free_; }

//! get begun level
bool cell_triplet::begun() const { return begun_; }

void cell_triplet::calculate_joints(double Ratio, double separation_limit, double phi_limit,
                                    double theta_limit) {
  if (print_level() > mybhep::VERBOSE) {
    std::clog << appname_ << " calculate joints for cells: " << ca_.id() << " " << cb_.id() << " "
              << cc_.id() << std::endl;
  }

  joints_.clear();
  std::vector<line> t1 = cca().tangents();  // note: this tangent goes from cell B to cell A
  std::vector<line> t2 = ccb().tangents();  // this goes from B to C
  bool intersect_ab = ca_.intersect(cb_);
  bool intersect_bc = cb_.intersect(cc_);
  bool intersect_ca = cc_.intersect(ca_);

  bool is_fast = ca_.fast();
  if (!is_fast) {
    phi_limit = std::max(phi_limit, 90.);
    theta_limit = std::max(theta_limit, 180.);
    if (print_level() > mybhep::VERBOSE) {
      std::clog << appname_ << " cells are slow, reset phi_limit " << phi_limit << " theta_limit "
                << theta_limit << std::endl;
    }
  }

  if (print_level() > mybhep::VERBOSE) {
    std::clog << appname_ << " angles of tangents " << ca_.id() << " -> " << cb_.id() << " :"
              << std::endl;
    for (std::vector<line>::iterator i1 = t1.begin(); i1 != t1.end(); ++i1) {
      std::clog << i1 - t1.begin() << ":  phi ";
      ca_.dump_point_phi(i1->epb());
      std::clog << " -> ";
      cb_.dump_point_phi(i1->epa());
      std::clog << " " << std::endl;
    }
    std::clog << appname_ << " angles of tangents " << cb_.id() << " -> " << cc_.id() << " :"
              << std::endl;
    for (std::vector<line>::iterator i2 = t2.begin(); i2 != t2.end(); ++i2) {
      std::clog << i2 - t2.begin() << ":  phi ";
      cb_.dump_point_phi(i2->epa());
      std::clog << " -> ";
      cc_.dump_point_phi(i2->epb());
      std::clog << " " << std::endl;
    }
    if (ca_.small()) std::clog << " cell " << ca_.id() << " is small " << std::endl;
    if (cb_.small()) std::clog << " cell " << cb_.id() << " is small " << std::endl;
    if (cc_.small()) std::clog << " cell " << cc_.id() << " is small " << std::endl;
    if (intersect_ab)
      std::clog << " cells " << ca_.id() << " and " << cb_.id() << " intersect " << std::endl;
    if (intersect_bc)
      std::clog << " cells " << cb_.id() << " and " << cc_.id() << " intersect " << std::endl;
    if (intersect_ca)
      std::clog << " cells " << cc_.id() << " and " << ca_.id() << " intersect " << std::endl;
  }

  bool use_ownerror = true;

  experimental_double local_separation;
  bool shall_include_separation;
  experimental_vector a0, a, b0, b, c0, c, d0, d;
  double psc, psc2, psc3;
  // double psc4;
  int32_t ndof;
  experimental_point p;
  experimental_double newxa, newza;
  double chi2, local_prob, chi2_just_phi, prob_just_phi;
  bool ok, use_theta_kink;
  experimental_double phi_kink, theta_kink;

  for (std::vector<line>::iterator i1 = t1.begin(); i1 != t1.end(); ++i1) {
    for (std::vector<line>::iterator i2 = t2.begin(); i2 != t2.end(); ++i2) {
      if (print_level() > mybhep::VERBOSE) {
        std::clog << " tangents " << i1 - t1.begin() << " and " << i2 - t2.begin() << std::endl;
      }

      shall_include_separation = true;
      a0 = i1->forward_axis();
      a = a0.hor();
      d0 = i2->forward_axis();
      d = d0.hor();

      // keep only the connections that don't invert foward sense
      psc3 = a.kink_phi(d).value() * 180. / M_PI;

      if (std::abs(psc3) < 60.) {
        if (print_level() > mybhep::VERBOSE) {
          std::clog << " rejected because direction is reversed: psc = " << psc3 << std::endl;
        }
        continue;
      }

      if (cb_.small()) {
        if (print_level() > mybhep::VERBOSE) {
          std::clog << " no separation: middle cells is small ";
        }
        shall_include_separation = false;
      } else if (intersect_ab) {
        b0 = cca().forward_axis();
        b = b0.hor();
        psc = a.kink_phi(b).value() * 180. / M_PI;

        if (std::abs(psc - 90.) < 30. || std::abs(psc + 90.) < 30. ||
            std::abs(psc - 270.) < 30.) {  // connection along the intersection
          /*
          if( print_level() > mybhep::VERBOSE ){
            std::clog << " no separation: connect along intersection AB ";
          }
          shall_include_separation = false;
          */
          // keep only the connection with consistent ordering of cells
          c0 = ccb().forward_axis();
          c = c0.hor();
          psc2 = b.kink_phi(c).value() * 180. / M_PI;
          if (std::abs(psc2) < 60.) {
            if (print_level() > mybhep::VERBOSE) {
              std::clog
                  << " rejected because first 2 cells intersect and the ordering is wrong: psc = "
                  << psc2 << std::endl;
            }
            continue;
          }
        }
      } else if (intersect_bc) {
        b0 = ccb().forward_axis();
        b = b0.hor();
        psc = d.kink_phi(b).value() * 180. / M_PI;
        if (std::abs(psc - 90.) < 30. || std::abs(psc + 90.) < 30. ||
            std::abs(psc - 270.) < 30.) {  // connection along the intersection
          /*
          if( print_level() > mybhep::VERBOSE ){
            std::clog << " no separation: connect along intersection BC ";
          }
          shall_include_separation = false;
          */
          // keep only the connection with consistent ordering of cells
          c0 = cca().forward_axis();
          c = c0.hor();
          psc2 = b.kink_phi(c).value() * 180. / M_PI;
          if (std::abs(psc2) < 60.) {
            if (print_level() > mybhep::VERBOSE) {
              std::clog
                  << " rejected because last 2 cells intersect and the ordering is wrong: psc = "
                  << psc2 << std::endl;
            }
            continue;
          }
        }
      }

      ndof = 2;  // 2 kink angles, 0 or 1 one separation angle
      if (shall_include_separation) ndof++;

      if (cb_.small()) {
        p = cb_.ep();
        newxa = p.x();
        newxa.set_error(cb_.r().error());
        newza = p.z();
        newza.set_error(cb_.r().error());
        p.set_x(newxa);
        p.set_z(newza);
      } else {
        p = cb_.angular_average(i1->epa(), i2->epa(), &local_separation);
      }

      line newt1(i1->epb(), p, print_level(), get_probmin());
      line newt2(p, i2->epb(), print_level(), get_probmin());
      phi_kink = newt1.kink_phi(newt2);

      if (print_level() > mybhep::VERBOSE) {
        std::clog << " p1: phi = ";
        ca_.dump_point(i1->epb());
        std::clog << " " << std::endl;
        std::clog << " p2 average point: ";
        cb_.dump_point(p);
        std::clog << " " << std::endl;
        std::clog << " p3: ";
        cc_.dump_point(i2->epb());
        std::clog << " " << std::endl;
        std::clog << "    separation: ";
        (local_separation * 180 / M_PI).dump();
        std::clog << " " << std::endl;
        std::clog << "    phi_kink: " << (phi_kink.value() * 180 / M_PI) << " " << std::endl;
      }

      chi2 = 0.;
      ok = false;

      if (!use_ownerror) {
        if (std::abs(local_separation.value()) <= separation_limit) {
          theta_kink = newt1.kink_theta(newt2);
          if (print_level() > mybhep::VERBOSE) {
            std::clog << " phi 1st tangent: ";
            (newt1.phi() * 180. / M_PI).dump();
            std::clog << " phi 2nd tangent: ";
            (newt2.phi() * 180. / M_PI).dump();
            std::clog << "    phi_kink " << phi_kink.value() * 180 / M_PI << " +- "
                      << phi_kink.error() * 180 / M_PI << std::endl;
            std::clog << " theta 1st tangent: ";
            (newt1.theta() * 180. / M_PI).dump();
            std::clog << " theta 2nd tangent: ";
            (newt2.theta() * 180. / M_PI).dump();
            std::clog << "    theta_kink " << theta_kink.value() * 180 / M_PI << " +- "
                      << theta_kink.error() * 180 / M_PI << std::endl;
          }

          if ((std::abs(phi_kink.value()) * 180. / M_PI <= phi_limit) &&
              (std::abs((newt1.kink_theta(newt2)).value()) * 180. / M_PI <= theta_limit))
            ok = true;
        }
      } else {
        use_theta_kink =
            !(ca_.unknown_vertical() || cb_.unknown_vertical() || cc_.unknown_vertical());
        if (!use_theta_kink) ndof--;

        chi2 = newt1.chi2(newt2, use_theta_kink, &chi2_just_phi);

        // also useul to check just the phi value (adding the theta information reduces the strength
        // of the phi test)
        prob_just_phi = probof(chi2_just_phi, 1);

        if (shall_include_separation)
          chi2 += square(local_separation.value() / local_separation.error());

        chi2s_.push_back(chi2);
        local_prob = probof(chi2, ndof);
        probs_.push_back(local_prob);
        if (local_prob > probmin() && prob_just_phi > probmin() &&
            std::abs(phi_kink.value()) * 180. / M_PI <= phi_limit)
          ok = true;

        if (print_level() > mybhep::VERBOSE) {
          std::clog << "    chi2 " << chi2 << " prob " << local_prob << " prob_just_phi "
                    << prob_just_phi << " phi_kink " << phi_kink.value() * 180. / M_PI << " limit "
                    << phi_limit << " accepted: " << ok << std::endl;
        }
      }

      if (ok) {
        joint j(newt1.epa(), p, newt2.epb(), print_level(), get_probmin());
        j.set_chi2(chi2);
        j.set_ndof(ndof);
        j.set_p(probof(chi2, ndof));
        joints_.push_back(j);
      }
    }
  }

  joints_ = refine(joints_, Ratio);

  return;
}

std::vector<joint> cell_triplet::refine(const std::vector<joint> &joints, double Ratio,
                                        size_t max_njoints) {
  std::vector<joint> _joints;

  if (print_level() > mybhep::VERBOSE) {
    std::clog << " refining " << joints.size() << " joints " << std::endl;
  }
  experimental_double delta_phi;
  bool found;
  for (std::vector<joint>::const_iterator ijoint = joints.begin(); ijoint != joints.end();
       ++ijoint) {
    found = false;
    for (std::vector<joint>::const_iterator jjoint = joints.begin(); jjoint != joints.end();
         ++jjoint) {
      if (jjoint == ijoint) continue;

      if (ca_.same_quadrant(ijoint->epa(), jjoint->epa()) &&
          cc_.same_quadrant(ijoint->epc(), jjoint->epc()) && ijoint->p() < jjoint->p()) {
        if (print_level() > mybhep::VERBOSE) {
          std::clog << " ... removing joint " << ijoint - joints.begin() << " with prob "
                    << ijoint->p() << " because joint " << jjoint - joints.begin() << " with prob "
                    << jjoint->p() << " has the same initial and final quadrant " << std::endl;
        }
        found = true;
        break;
      }
    }

    if (!found) _joints.push_back(*ijoint);
  }

  if (_joints.size() > 1) {
    // order joints in order of increasing chi2 (best joint comes first)
    std::sort(_joints.begin(), _joints.end());
  }

  // only keep best joints
  if (_joints.size() > max_njoints) {
    _joints.erase(_joints.begin() + max_njoints, _joints.end());
  }

  if (_joints.size() >= 2 && !(ca_.intersect(cb_) || ca_.intersect(cc_) || cb_.intersect(cc_))) {
    std::vector<joint>::iterator ijoint = _joints.begin();
    ijoint++;
    while (ijoint != _joints.end()) {
      if ((size_t)(ijoint - _joints.begin() + 1) > _joints.size()) break;
      if (_joints[0].p() / ijoint->p() > Ratio) {
        if (print_level() > mybhep::VERBOSE)
          std::clog << " remove joint with p " << ijoint->p() << " in favor of 1st joint with p "
                    << _joints[0].p() << std::endl;
        _joints.erase(ijoint);
        ijoint = _joints.begin() + (ijoint - _joints.begin());
      } else {
        ijoint++;
      }
    }
  }

  if (print_level() > mybhep::VERBOSE) {
    std::clog << " after refining there are " << _joints.size() << " joints " << std::endl;
    for (std::vector<joint>::const_iterator ij = _joints.begin(); ij != _joints.end(); ++ij) {
      std::clog << " joint " << ij - _joints.begin() << " : ";
      dump_joint(*ij);
    }
  }

  return _joints;
}

size_t cell_triplet::iteration() const {
  for (std::vector<joint>::const_iterator i = joints_.begin(); i != joints_.end(); ++i)
    if (!i->used()) return (size_t)(i - joints_.begin());

  return joints().size();
}

cell_triplet cell_triplet::invert() {
  cell_triplet inverted;
  inverted.set(ccb(), cca());
  inverted.set_free(free());
  inverted.set_begun(begun());
  inverted.set_chi2s(chi2s());
  inverted.set_probs(probs());

  std::vector<joint> inverted_joints;
  for (std::vector<joint>::iterator i = joints_.begin(); i != joints_.end(); ++i) {
    inverted_joints.push_back(i->invert());
  }
  inverted.set_joints(inverted_joints);
  return inverted;
}

void cell_triplet::set_all_used() {
  for (std::vector<joint>::iterator i = joints_.begin(); i != joints_.end(); ++i) i->set_used(true);
  set_begun(true);
  return;
}

bool operator==(const cell_triplet &left, const cell_triplet &right) {
  return ((left.ca().id() == right.ca().id()) && (left.cc().id() == right.cc().id())) ||
         ((left.ca().id() == right.cc().id()) && (left.cc().id() == right.ca().id()));
}

bool cell_triplet::same_last_cell(cell c) const {
  return ((this->ca().id() == c.id()) || (this->cc().id() == c.id()));
}

void cell_triplet::calculate_joints_after_sultan(double Ratio) {
  if (print_level() > mybhep::VERBOSE) {
    std::clog << appname_ << " calculate joints after sultan for cells: " << ca_.id() << " "
              << cb_.id() << " " << cc_.id() << std::endl;
  }

  joints_.clear();
  std::vector<joint> the_joints, the_rejected_joints;
  std::vector<line> t1 = cca().tangents();  // note: this tangent goes from cell B to cell A
  std::vector<line> t2 = ccb().tangents();  // this goes from B to C
  bool intersect_ab = ca_.intersect(cb_);
  bool intersect_bc = cb_.intersect(cc_);
  bool intersect_ca = cc_.intersect(ca_);

  if (print_level() > mybhep::VERBOSE) {
    std::clog << appname_ << " angles of tangents " << ca_.id() << " -> " << cb_.id() << " :"
              << std::endl;
    for (std::vector<line>::iterator i1 = t1.begin(); i1 != t1.end(); ++i1) {
      std::clog << i1 - t1.begin() << ":  phi ";
      ca_.dump_point_phi(i1->epb());
      std::clog << " -> ";
      cb_.dump_point_phi(i1->epa());
      std::clog << " " << std::endl;
    }
    std::clog << appname_ << " angles of tangents " << cb_.id() << " -> " << cc_.id() << " :"
              << std::endl;
    for (std::vector<line>::iterator i2 = t2.begin(); i2 != t2.end(); ++i2) {
      std::clog << i2 - t2.begin() << ":  phi ";
      cb_.dump_point_phi(i2->epa());
      std::clog << " -> ";
      cc_.dump_point_phi(i2->epb());
      std::clog << " " << std::endl;
    }
    if (ca_.small()) std::clog << " cell " << ca_.id() << " is small " << std::endl;
    if (cb_.small()) std::clog << " cell " << cb_.id() << " is small " << std::endl;
    if (cc_.small()) std::clog << " cell " << cc_.id() << " is small " << std::endl;
    if (intersect_ab)
      std::clog << " cells " << ca_.id() << " and " << cb_.id() << " intersect " << std::endl;
    if (intersect_bc)
      std::clog << " cells " << cb_.id() << " and " << cc_.id() << " intersect " << std::endl;
    if (intersect_ca)
      std::clog << " cells " << cc_.id() << " and " << ca_.id() << " intersect " << std::endl;
  }

  experimental_double local_separation;
  bool shall_include_separation;
  experimental_vector a0, a, b0, b, c0, c, d0, d;
  double psc, psc2, psc3;
  int32_t ndof;
  experimental_point p;
  experimental_double newxa, newza;
  double chi2, local_prob, chi2_just_phi, prob_just_phi;
  bool use_theta_kink;
  experimental_double phi_kink, theta_kink;
  bool is_rejected = false;

  for (std::vector<line>::iterator i1 = t1.begin(); i1 != t1.end(); ++i1) {
    for (std::vector<line>::iterator i2 = t2.begin(); i2 != t2.end(); ++i2) {
      if (print_level() > mybhep::VERBOSE) {
        std::clog << " tangents " << i1 - t1.begin() << " and " << i2 - t2.begin() << std::endl;
      }

      is_rejected = false;
      shall_include_separation = true;
      a0 = i1->forward_axis();
      a = a0.hor();
      d0 = i2->forward_axis();
      d = d0.hor();

      // keep only the connections that don't invert foward sense
      psc3 = a.kink_phi(d).value() * 180. / M_PI;

      if (std::abs(psc3) < 60.) {
        if (print_level() > mybhep::VERBOSE) {
          std::clog << " rejected because direction is reversed: psc = " << psc3 << std::endl;
        }
        is_rejected = true;
      }

      if (cb_.small()) {
        if (print_level() > mybhep::VERBOSE) {
          std::clog << " no separation: middle cells is small ";
        }
        shall_include_separation = false;
      } else if (intersect_ab) {
        b0 = cca().forward_axis();
        b = b0.hor();
        psc = a.kink_phi(b).value() * 180. / M_PI;

        if (std::abs(psc - 90.) < 30. || std::abs(psc + 90.) < 30. ||
            std::abs(psc - 270.) < 30.) {  // connection along the intersection

          // keep only the connection with consistent ordering of cells
          c0 = ccb().forward_axis();
          c = c0.hor();
          psc2 = b.kink_phi(c).value() * 180. / M_PI;
          if (std::abs(psc2) < 60.) {
            if (print_level() > mybhep::VERBOSE) {
              std::clog
                  << " rejected because first 2 cells intersect and the ordering is wrong: psc = "
                  << psc2 << std::endl;
            }
            continue;
          }
        }
      } else if (intersect_bc) {
        b0 = ccb().forward_axis();
        b = b0.hor();
        psc = d.kink_phi(b).value() * 180. / M_PI;
        if (std::abs(psc - 90.) < 30. || std::abs(psc + 90.) < 30. ||
            std::abs(psc - 270.) < 30.) {  // connection along the intersection
          c0 = cca().forward_axis();
          c = c0.hor();
          psc2 = b.kink_phi(c).value() * 180. / M_PI;
          if (std::abs(psc2) < 60.) {
            if (print_level() > mybhep::VERBOSE) {
              std::clog
                  << " rejected because last 2 cells intersect and the ordering is wrong: psc = "
                  << psc2 << std::endl;
            }
            continue;
          }
        }
      }

      ndof = 2;  // 2 kink angles, 0 or 1 one separation angle
      if (shall_include_separation) ndof++;

      if (cb_.small()) {
        p = cb_.ep();
        newxa = p.x();
        newxa.set_error(cb_.r().error());
        newza = p.z();
        newza.set_error(cb_.r().error());
        p.set_x(newxa);
        p.set_z(newza);
      } else {
        p = cb_.angular_average(i1->epa(), i2->epa(), &local_separation);
      }

      line newt1(i1->epb(), p, print_level(), get_probmin());
      line newt2(p, i2->epb(), print_level(), get_probmin());
      phi_kink = newt1.kink_phi(newt2);

      if (print_level() > mybhep::VERBOSE) {
        std::clog << " p1: phi = ";
        ca_.dump_point(i1->epb());
        std::clog << " " << std::endl;
        std::clog << " p2 average point: ";
        cb_.dump_point(p);
        std::clog << " " << std::endl;
        std::clog << " p3: ";
        cc_.dump_point(i2->epb());
        std::clog << " " << std::endl;
        std::clog << "    separation: ";
        (local_separation * 180 / M_PI).dump();
        std::clog << " " << std::endl;
        std::clog << "    phi_kink: " << (phi_kink.value() * 180 / M_PI) << " " << std::endl;
      }

      chi2 = 0.;

      use_theta_kink =
          !(ca_.unknown_vertical() || cb_.unknown_vertical() || cc_.unknown_vertical());
      if (!use_theta_kink) ndof--;

      chi2 = newt1.chi2(newt2, use_theta_kink, &chi2_just_phi);

      // also useul to check just the phi value (adding the theta information reduces the strength
      // of the phi test)
      prob_just_phi = probof(chi2_just_phi, 1);

      if (shall_include_separation)
        chi2 += square(local_separation.value() / local_separation.error());

      if (!is_rejected) {
        chi2s_.push_back(chi2);
        local_prob = probof(chi2, ndof);
        probs_.push_back(local_prob);
      }

      if (print_level() > mybhep::VERBOSE) {
        std::clog << "    chi2 " << chi2 << " prob " << local_prob << " prob_just_phi "
                  << prob_just_phi << " phi_kink " << phi_kink.value() * 180. / M_PI << std::endl;
      }

      joint j(newt1.epa(), p, newt2.epb(), print_level(), get_probmin());
      j.set_chi2(chi2);
      j.set_ndof(ndof);
      j.set_p(probof(chi2, ndof));
      if (!is_rejected)
        the_joints.push_back(j);
      else
        the_rejected_joints.push_back(j);
    }
  }

  if (print_level() > mybhep::VERBOSE) {
    std::clog << the_joints.size() << " joints have been produced for triplet and "
              << the_rejected_joints.size() << " rejected,  now keeping 1 or 2 " << std::endl;
  }

  if (the_joints.size() == 0) the_joints = the_rejected_joints;

  size_t max_njoints = 2;
  joints_ = refine(the_joints, Ratio, max_njoints);

  if (print_level() > mybhep::VERBOSE) {
    std::clog << joints_.size() << " joints kept after refining " << std::endl;
  }

  return;
}

}  // namespace topology

}  // namespace CAT
