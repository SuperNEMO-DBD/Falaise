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
#include <CATAlgorithm/cell.h>
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
namespace topology{



  class logic_sequence : public tracking_object{

    // a logic_sequence is composed of a list of logic cells

  public:

    // list of logic cells
    std::vector<logic_cell> cells_;

    // sequence's vertex
    bool has_vertex_;
    size_t vertex_id_;

    // sequence's decay_vertex
    bool has_decay_vertex_;
    size_t calo_id_;

    double chi2_;
    int32_t ndof_;

    //!Default constructor
    logic_sequence()
    {
      cells_.clear();
      has_vertex_ = false;
      vertex_id_ = mybhep::default_integer;
      has_decay_vertex_ = false;
      calo_id_ = mybhep::default_integer;
      chi2_ = mybhep::small_neg;
      ndof_ = mybhep::default_integer;
    }

    //!Default destructor
    virtual ~logic_sequence(){};

    //! constructor from sequence
    logic_sequence(const topology::sequence &s){
      for(std::vector<node>::const_iterator in=s.nodes_.begin(); in!=s.nodes_.end(); ++in)
        cells_.push_back(logic_cell(in->c().id()));
      has_vertex_ = false;
      if( s.has_helix_vertex() ){
        has_vertex_ = true;
        vertex_id_ = s.helix_vertex_id();
      }
      has_decay_vertex_ = false;
      if( s.has_decay_helix_vertex() ){
        has_decay_vertex_ = true;
        calo_id_ = s.calo_helix_id();
      }
      chi2_ = s.chi2();
      ndof_ = s.ndof();

    }

    //! get cells
    const std::vector<logic_cell> cells()const{
      return cells_;
    }

    //! has decay_vertex
    const bool has_decay_vertex()const{
      return has_decay_vertex_;
    }

    //! get calo id
    const size_t calo_id()const{
      return calo_id_;
    }

    //! has vertex
    const bool has_vertex()const{
      return has_vertex_;
    }

    //! get vertex id
    const size_t vertex_id()const{
      return vertex_id_;
    }

    // get chi2 of sequence
    double chi2(){
      return chi2_;
    }

    // get ndof of sequence
    int32_t ndof(){
      return ndof_;
    }

    // get prob of sequence
    double Prob(){
      return probof(chi2(), ndof());

    }


  };
}
}

#endif
