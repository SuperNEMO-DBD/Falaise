/* -*- mode: c++ -*- */

#include <sultan/calorimeter_hit.h>
#include <mybhep/system_of_units.h>

namespace SULTAN {
namespace topology {

using namespace std;
using namespace mybhep;

//! Default constructor
calorimeter_hit::calorimeter_hit(prlevel level, double probmin) {
  appname_ = "calorimeter_hit: ";
  e_ = experimental_double(small_neg, small_neg);
  t_ = experimental_double(small_neg, small_neg);
  set_print_level(level);
  set_probmin(probmin);
  id_ = default_integer;
  user_id_ = default_integer;
  layer_ = small_neg;
  return;
}

//! Default destructor
calorimeter_hit::~calorimeter_hit() { return; }

//! constructor
calorimeter_hit::calorimeter_hit(const plane& pl, const experimental_double& e,
                                 const experimental_double& t, size_t id, double layer,
                                 prlevel level, double probmin) {
  set_print_level(level);
  set_probmin(probmin);
  appname_ = "calorimeter_hit: ";
  pl_ = pl;
  e_ = e;
  t_ = t;
  id_ = id;
  user_id_ = id;
  layer_ = layer;
}

/*** dump ***/
void calorimeter_hit::dump(ostream& a_out, const std::string& a_title, const std::string& a_indent,
                           bool /* a_inherit */) const {
  std::string indent;
  if (!a_indent.empty()) indent = a_indent;
  if (!a_title.empty()) {
    a_out << indent << a_title << std::endl;
  }

  a_out << indent << appname_ << " -------------- " << std::endl;
  a_out << indent << " id " << id() << " plane " << std::endl;
  pl_.dump(a_out, "", indent + "    ");
  a_out << indent << " energy ";
  this->e().dump(a_out, "", indent + "    ");
  a_out << indent << " time ";
  this->t().dump(a_out, "", indent + "    ");
  a_out << indent << " -------------- " << std::endl;

  return;
}

//! set
void calorimeter_hit::set(const plane& pl, const experimental_double& e,
                          const experimental_double& t, size_t id) {
  pl_ = pl;
  e_ = e;
  t_ = t;
  id_ = id;
}

//! set plane
void calorimeter_hit::set_pl(const plane& pl) { pl_ = pl; }

//! set energy
void calorimeter_hit::set_e(const experimental_double& e) { e_ = e; }

//! set time
void calorimeter_hit::set_t(const experimental_double& t) { t_ = t; }

//! set layer
void calorimeter_hit::set_layer(double layer) { layer_ = layer; }

//! get plane
const plane& calorimeter_hit::pl() const { return pl_; }

//! get energy
const experimental_double& calorimeter_hit::e() const { return e_; }

//! get time
const experimental_double& calorimeter_hit::t() const { return t_; }

//! get id
size_t calorimeter_hit::id() const { return id_; }

int calorimeter_hit::user_id() const { return user_id_; }

//! get layer
double calorimeter_hit::layer() const { return layer_; }

bool calorimeter_hit::same_calo(const calorimeter_hit& c) const {
  double dist = (experimental_vector(pl().center(), c.pl().center())).length().value();

  if (dist < 0.1) return true;

  return false;
}

}  // namespace topology
}  // namespace SULTAN
