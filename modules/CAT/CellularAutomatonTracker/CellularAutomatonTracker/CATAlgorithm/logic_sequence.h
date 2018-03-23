/* -*- mode: c++ -*- */
#ifndef __CATAlgorithm__ILOGICSEQUENCE
#define __CATAlgorithm__ILOGICSEQUENCE
#include <iostream>
#include <cmath>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <mybhep/error.h>
#include <mybhep/utilities.h>
#include <mybhep/point.h>
#include <mybhep/clhep.h>
#include <boost/cstdint.hpp>
#include <CATAlgorithm/experimental_point.h>
#include <CATAlgorithm/experimental_vector.h>
#include <CATAlgorithm/cell_base.h>
#include <CATAlgorithm/line.h>
#include <CATAlgorithm/cell_couplet.h>
#include <CATAlgorithm/cell_triplet.h>
#include <CATAlgorithm/node.h>
#include <CATAlgorithm/helix.h>
#include <CATAlgorithm/plane.h>
#include <CATAlgorithm/cluster.h>
#include <CATAlgorithm/calorimeter_hit.h>
#include <CATAlgorithm/logic_cell.h>

namespace CAT {
namespace topology {

class logic_sequence : public tracking_object {
  // a logic_sequence is composed of a list of logic cells

 public:
  // list of logic cells
  std::vector<logic_cell> cells_;

  // sequence's vertex
  bool has_helix_vertex_;
  size_t helix_vertex_id_;
  std::string helix_vertex_type_;
  bool has_tangent_vertex_;
  size_t tangent_vertex_id_;
  std::string tangent_vertex_type_;

  // sequence's decay_vertex
  bool has_decay_helix_vertex_;
  size_t calo_helix_id_;
  std::string decay_helix_vertex_type_;
  bool has_decay_tangent_vertex_;
  size_t calo_tangent_id_;
  std::string decay_tangent_vertex_type_;

  double helix_chi2_;
  double tangent_chi2_;
  int32_t ndof_;

  //! Default constructor
  logic_sequence() {
    cells_.clear();
    has_helix_vertex_ = false;
    helix_vertex_id_ = mybhep::default_integer;
    helix_vertex_type_ = "default";
    has_tangent_vertex_ = false;
    tangent_vertex_id_ = mybhep::default_integer;
    tangent_vertex_type_ = "default";
    has_decay_helix_vertex_ = false;
    calo_helix_id_ = mybhep::default_integer;
    decay_helix_vertex_type_ = "default";
    has_decay_tangent_vertex_ = false;
    calo_tangent_id_ = mybhep::default_integer;
    decay_tangent_vertex_type_ = "default";
    helix_chi2_ = mybhep::small_neg;
    tangent_chi2_ = mybhep::small_neg;
    ndof_ = mybhep::default_integer;
  }

  //! Default destructor
  virtual ~logic_sequence(){};

  //! constructor from sequence
  logic_sequence(const topology::sequence &s) {
    for (std::vector<node>::const_iterator in = s.nodes_.begin(); in != s.nodes_.end(); ++in)
      cells_.push_back(logic_cell(in->c().id()));
    has_helix_vertex_ = false;
    helix_vertex_id_ = mybhep::default_integer;
    helix_vertex_type_ = "default";
    has_tangent_vertex_ = false;
    tangent_vertex_id_ = mybhep::default_integer;
    tangent_vertex_type_ = "default";
    has_decay_helix_vertex_ = false;
    calo_helix_id_ = mybhep::default_integer;
    decay_helix_vertex_type_ = "default";
    has_decay_tangent_vertex_ = false;
    calo_tangent_id_ = mybhep::default_integer;
    decay_tangent_vertex_type_ = "default";
    if (s.has_helix_vertex()) {
      has_helix_vertex_ = true;
      helix_vertex_id_ = s.helix_vertex_id();
      helix_vertex_type_ = s.helix_vertex_type();
    }
    if (s.has_decay_helix_vertex()) {
      has_decay_helix_vertex_ = true;
      calo_helix_id_ = s.calo_helix_id();
      decay_helix_vertex_type_ = s.decay_helix_vertex_type();
    }
    if (s.has_tangent_vertex()) {
      has_tangent_vertex_ = true;
      tangent_vertex_id_ = s.tangent_vertex_id();
      tangent_vertex_type_ = s.tangent_vertex_type();
    }
    if (s.has_decay_tangent_vertex()) {
      has_decay_tangent_vertex_ = true;
      calo_tangent_id_ = s.calo_tangent_id();
      decay_tangent_vertex_type_ = s.decay_tangent_vertex_type();
    }
    helix_chi2_ = s.helix_chi2();
    tangent_chi2_ = s.chi2();
    ndof_ = s.ndof();
  }

  //! get cells
  const std::vector<logic_cell> cells() const { return cells_; }

  //! has decay_vertex
  bool has_decay_helix_vertex() const { return has_decay_helix_vertex_; }
  bool has_decay_tangent_vertex() const { return has_decay_tangent_vertex_; }
  const std::string &decay_helix_vertex_type() const { return decay_helix_vertex_type_; }
  const std::string &decay_tangent_vertex_type() const { return decay_tangent_vertex_type_; }

  //! get calo id
  size_t calo_helix_id() const { return calo_helix_id_; }
  size_t calo_tangent_id() const { return calo_tangent_id_; }

  //! has vertex
  bool has_helix_vertex() const { return has_helix_vertex_; }
  bool has_tangent_vertex() const { return has_tangent_vertex_; }
  const std::string &helix_vertex_type() const { return helix_vertex_type_; }
  const std::string &tangent_vertex_type() const { return tangent_vertex_type_; }

  //! get vertex id
  size_t helix_vertex_id() const { return helix_vertex_id_; }
  size_t tangent_vertex_id() const { return tangent_vertex_id_; }

  // get chi2 of sequence
  double tangent_chi2() { return tangent_chi2_; }

  // get chi2 of sequence
  double helix_chi2() { return helix_chi2_; }

  // get ndof of sequence
  int32_t ndof() { return ndof_; }

  // get prob of sequence
  double helix_Prob() { return probof(helix_chi2(), ndof()); }

  // get prob of sequence
  double tangent_Prob() { return probof(tangent_chi2(), ndof()); }
};
}  // namespace topology
}  // namespace CAT

#endif
