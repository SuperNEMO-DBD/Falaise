/* -*- mode: c++ -*- */
#ifndef __CATAlgorithm__ISCENARIO
#define __CATAlgorithm__ISCENARIO
#include <iostream>
#include <cmath>
#include <mybhep/error.h>
#include <mybhep/utilities.h>
#include <mybhep/point.h>
#include <mybhep/hit.h>
#include <mybhep/system_of_units.h>
#include <boost/cstdint.hpp>

#include <CATAlgorithm/tracking_object.h>
#include <CATAlgorithm/experimental_point.h>
#include <CATAlgorithm/experimental_vector.h>
#include <CATAlgorithm/sequence_base.h>
#include <CATAlgorithm/node.h>
#include <CATAlgorithm/calorimeter_hit.h>

namespace CAT {
namespace topology {

class scenario : public tracking_object {
  // a reconstruction scenario is composed of
  // a collection of tracks

 private:
  std::string appname_;

  // chi2
  double helix_chi2_;
  double tangent_chi2_;
  int32_t ndof_;

  // n of free families
  size_t n_free_families_;

  // n of overlapping cells
  size_t n_overlaps_;

 public:
  // tracks
  std::vector<topology::sequence> sequences_;

  //! Default constructor
  scenario();

  //! Default destructor
  virtual ~scenario();

  //! constructor
  scenario(const std::vector<sequence> &seqs, mybhep::prlevel level = mybhep::NORMAL,
           double probmin = 1.e-200);

  /*** dump ***/
  virtual void dump(std::ostream &a_out = std::clog, const std::string &a_title = "",
                    const std::string &a_indent = "", bool a_inherit = false) const;

  //! set experimental_point, radius, error and id;
  void set(const std::vector<sequence> &seqs);

  //! set sequences
  void set_sequences(const std::vector<sequence> &seqs);

  //! set helix_chi2
  void set_helix_chi2(double helix_chi2);

  //! set tangent_chi2
  void set_tangent_chi2(double tangent_chi2);

  //! set n free families
  void set_n_free_families(size_t n);

  //! set n overlaps
  void set_n_overlaps(size_t n);

  //! set ndof
  void set_ndof(int32_t n);

  //! get sequences
  const std::vector<sequence> &sequences() const;

  //! get helix_chi2
  double helix_chi2() const;

  //! get tangent_chi2
  double tangent_chi2() const;

  //! get ndof
  int32_t ndof() const;

  //! get n free families
  size_t n_free_families() const;

  //! get n overlaps
  size_t n_overlaps() const;

  void calculate_n_overlaps(const std::vector<topology::cell> &cells,
                            const std::vector<topology::calorimeter_hit> &calos);

  void calculate_n_free_families(const std::vector<topology::cell> &cells,
                                 const std::vector<topology::calorimeter_hit> &calos);

  void calculate_chi2();

  double helix_Prob() const;

  double tangent_Prob() const;

  bool better_scenario_than(const scenario &s, double limit) const;

  size_t n_of_common_vertexes(double limit) const;

  size_t n_of_ends_on_wire(void) const;
};
}  // namespace topology
}  // namespace CAT

#endif
