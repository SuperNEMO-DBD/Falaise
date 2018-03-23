/* -*- mode: c++ -*- */
#include <sultan/sequence.h>

#include <sultan/experimental_point.h>
#include <sultan/experimental_vector.h>
#include <sultan/cell.h>
#include <sultan/node.h>
#include <sultan/experimental_helix.h>

#include <sys/time.h>

namespace SULTAN {
namespace topology {

//! Default constructor
sequence::sequence() {
  appname_ = "sequence: ";
  nodes_.clear();
  name_ = "default";
  calo_id_ = mybhep::default_integer;
  helix_momentum_ = experimental_vector(mybhep::small_neg, mybhep::small_neg, mybhep::small_neg,
                                        mybhep::small_neg, mybhep::small_neg, mybhep::small_neg);
}

//! Default destructor
sequence::~sequence() {}

//! constructor from std::vector of nodes
sequence::sequence(const std::vector<node> &nodes, mybhep::prlevel level, double probmin) {
  set_print_level(level);
  set_probmin(probmin);
  appname_ = "sequence: ";
  nodes_ = nodes;
  name_ = "default";
  calo_id_ = mybhep::default_integer;
  helix_momentum_ = experimental_vector(mybhep::small_neg, mybhep::small_neg, mybhep::small_neg,
                                        mybhep::small_neg, mybhep::small_neg, mybhep::small_neg);
}

/*** dump ***/
void sequence::dump(std::ostream &a_out, const std::string &a_title, const std::string &a_indent,
                    bool /* a_inherit */) const {
  std::string indent;
  if (!a_indent.empty()) indent = a_indent;
  if (!a_title.empty()) {
    a_out << indent << a_title << std::endl;
  }

  a_out << indent << appname_ << " ------------------- " << std::endl;
  a_out << indent << name_ << ". Number of nodes: " << nodes().size() << std::endl;
  for (std::vector<node>::const_iterator inode = nodes_.begin(); inode != nodes_.end(); ++inode)
    inode->dump(a_out, "", indent + "     ");

  return;
}

//! set nodes
void sequence::set_nodes(const std::vector<node> &nodes) { nodes_ = nodes; }

//! set name
void sequence::set_name(const std::string &name) { name_ = name; }

//! set helix
void sequence::set_helix(const experimental_helix &h) { helix_ = h; }

//! set helix_momentum
void sequence::set_helix_momentum(const experimental_vector &mom) { helix_momentum_ = mom; }

//! get calo id
size_t sequence::calo_id() const { return calo_id_; }

//! get nodes
const std::vector<node> &sequence::nodes() const { return nodes_; }

//! get name
const std::string &sequence::name() const { return name_; }

//! get sequence
const experimental_helix &sequence::get_helix() const { return helix_; }

//! get helix_momentum
const experimental_vector &sequence::helix_momentum() const { return helix_momentum_; }

bool sequence::has_cell(const cell &c) const {
  if (std::find(nodes_.begin(), nodes_.end(), c) != nodes_.end()) return true;

  return false;
}

bool sequence::is_contained_in(const sequence &s) const {
  for (std::vector<node>::const_iterator inode = nodes_.begin(); inode != nodes_.end(); ++inode) {
    if (!s.has_cell(inode->c())) return false;
  }

  return true;
}

bool sequence::contains(const sequence &s) const {
  std::vector<node> nodes = s.nodes();
  for (std::vector<node>::const_iterator inode = nodes.begin(); inode != nodes.end(); ++inode) {
    if (!this->has_cell(inode->c())) return false;
  }

  return true;
}

experimental_vector sequence::initial_helix_dir(bool SuperNEMO, double ref_value) const {
  size_t s = nodes_.size();
  if (s < 1) {
    if (print_level() >= mybhep::NORMAL) {
      std::clog << " problem: sultan asking for initial helix dir of sequence with " << s
                << " nodes " << std::endl;
      return experimental_vector();
    }
  }

  experimental_point p_first = nodes_[0].ep();
  experimental_point p_last = nodes_[s - 1].ep();
  double dist_first, dist_last;

  if (SuperNEMO) {
    dist_first = std::abs(p_first.x().value() - ref_value);
    dist_last = std::abs(p_last.x().value() - ref_value);
    experimental_vector foil_normal(1., 0., 0., 0., 0., 0.);
    double sign = 1.;
    if (dist_first < dist_last) {
      experimental_vector helix_dir = helix_.direction_at(p_first);
      if (nodes_[0].c().ep().x().value() < 0.) foil_normal *= -1.;
      double sp = (helix_dir * foil_normal).value();  // the helix could be run backwards
      if (sp < 0) sign = -1.;
      return helix_dir * sign;
    }
    experimental_vector helix_dir = helix_.direction_at(p_last);
    if (nodes_[s - 1].c().ep().x().value() < 0.) foil_normal *= -1.;
    double sp = (helix_dir * foil_normal).value();
    if (sp < 0) sign = -1.;
    return helix_dir * sign;
  }

  dist_first = std::abs(p_first.radius().value() - ref_value);
  dist_last = std::abs(p_last.radius().value() - ref_value);
  experimental_point origin(0., 0., 0., 0., 0., 0.);
  double sign = 1.;
  if (dist_first < dist_last) {
    experimental_vector foil_normal = (nodes_[0].c().ep() - origin).hor().unit();
    if (nodes_[0].c().ep().radius().value() - ref_value < 0.) foil_normal *= -1.;
    experimental_vector helix_dir = helix_.direction_at(p_first);
    double sp = (helix_dir * foil_normal).value();
    if (sp < 0) sign = -1;
    return helix_dir * sign;
  }

  experimental_vector foil_normal = (nodes_[s - 1].c().ep() - origin).hor().unit();
  if (nodes_[s - 1].c().ep().radius().value() - ref_value < 0.) foil_normal *= -1.;
  experimental_vector helix_dir = helix_.direction_at(p_last);
  double sp = (helix_dir * foil_normal).value();
  if (sp < 0) sign = -1;
  return helix_dir * sign;
}

void sequence::calculate_momentum(double bfield, bool SuperNEMO, double ref_value) {
  experimental_double mom =
      experimental_sqrt(experimental_square(helix_.R()) + experimental_square(helix_.H())) * 0.3 *
      bfield;

  helix_momentum_ = mom * initial_helix_dir(SuperNEMO, ref_value);

  if (print_level() >= mybhep::VVERBOSE) {
    std::clog << " sequence radius " << helix_.R().value() << " pitch " << helix_.H().value()
              << " bfield " << bfield << " helix_mom " << mom.value() << " dir ("
              << initial_helix_dir(SuperNEMO, ref_value).x().value() << ", "
              << initial_helix_dir(SuperNEMO, ref_value).y().value() << ", "
              << initial_helix_dir(SuperNEMO, ref_value).z().value() << ")" << std::endl;
  }
}

}  // namespace topology
}  // namespace SULTAN

// end
