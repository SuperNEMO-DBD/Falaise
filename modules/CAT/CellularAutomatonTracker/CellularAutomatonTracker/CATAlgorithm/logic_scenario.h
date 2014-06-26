/* -*- mode: c++ -*- */
#ifndef __CATAlgorithm__ILOGICSCENARIO
#define __CATAlgorithm__ILOGICSCENARIO
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
#include <CATAlgorithm/scenario.h>
#include <CATAlgorithm/logic_cell.h>
#include <CATAlgorithm/logic_sequence.h>


namespace CAT {
namespace topology{


  class logic_scenario : public tracking_object{

    // a reconstruction scenario is composed of
    // a collection of logic_sequences

  private:
    // chi2
    double chi2_;
    int32_t ndof_;

    // n of free families
    size_t n_free_families_;

    // n of overlapping cells
    size_t n_overlaps_;

  public:

    // tracks
    std::vector<topology::logic_sequence> sequences_;

    //!Default constructor
    logic_scenario()
    {
      sequences_.clear();
      chi2_ = mybhep::small_neg;
      ndof_ = mybhep::default_integer;
      n_free_families_ = mybhep::default_integer;
      n_overlaps_ = mybhep::default_integer;
   }

    //!Default destructor
    virtual ~logic_scenario(){};

    //! constructor
    logic_scenario(const topology::scenario &sc){
      for(std::vector<topology::sequence>::const_iterator iseq=sc.sequences_.begin(); iseq!=sc.sequences_.end(); ++iseq)
        sequences_.push_back(logic_sequence(*iseq));
      chi2_ = sc.chi2();
      ndof_ = sc.ndof();
      n_free_families_ = sc.n_free_families();
      n_overlaps_ = sc.n_overlaps();
    }

    //! get sequences
    const std::vector<logic_sequence> & sequences()const
    {
      return sequences_;
    }

    //!get chi2
    const double& chi2() const
    {
      return chi2_;
    }

    //!get ndof
    const int32_t& ndof() const
    {
      return ndof_;
    }

    //!get n free families
    const size_t& n_free_families() const {return n_free_families_;}

    //!get n overlaps
    const size_t& n_overlaps() const {return n_overlaps_;}


    void calculate_n_overlaps(const std::vector<topology::cell> &cells, const std::vector<topology::calorimeter_hit> &calos){

      std::vector<int> freecells(cells.size());
      fill(freecells.begin(), freecells.end(), 1);

      std::vector<int> freecalos(calos.size());
      fill(freecalos.begin(), freecalos.end(), 1);

      size_t counter = 0;

      for(std::vector<logic_sequence>::iterator iseq = sequences_.begin(); iseq != sequences_.end(); ++iseq){

        for(std::vector<logic_cell>::iterator in = iseq->cells_.begin(); in != iseq->cells_.end(); ++in){
          if( in->id() >= cells.size() ){
            if( print_level() >= mybhep::VVERBOSE )
              std::clog << " problem: cell " << in->id() << " has larger id than n of cells " << cells.size() << std::endl;
            continue;
          }

          if( freecells[in->id()] )
            freecells[in->id()] = 0;
          else
            counter ++;

        }

        if( iseq->has_decay_helix_vertex() && iseq->decay_helix_vertex_type() == "calo" ){
          if( iseq->calo_helix_id() >= calos.size() ){
            if( print_level() >= mybhep::VVERBOSE )
              std::clog << " problem: helix calo " << iseq->calo_helix_id() << " has larger id than n of calos " << calos.size() << std::endl;
            continue;
          }

          if( freecalos[iseq->calo_helix_id()] )
            freecalos[iseq->calo_helix_id()] = 0;
          else
            counter ++;
        }

        if( iseq->has_helix_vertex() && iseq->helix_vertex_type() == "calo" ){
          if( iseq->helix_vertex_id() >= calos.size() ){
            if( print_level() >= mybhep::VVERBOSE )
              std::clog << " problem: helix calo-vertex " << iseq->helix_vertex_id() << " has larger id than n of calos " << calos.size() << std::endl;
            continue;
          }

	  if( iseq->helix_vertex_id() != iseq->calo_helix_id() ){ // avoid double counting if both extrapolations point to the same calo
	    if( freecalos[iseq->helix_vertex_id()] )
	      freecalos[iseq->helix_vertex_id()] = 0;
	    else
	      counter ++;
	  }
        }

        if( iseq->has_decay_tangent_vertex() && iseq->decay_tangent_vertex_type() == "calo" ){
          if( iseq->calo_tangent_id() >= calos.size() ){
            if( print_level() >= mybhep::VVERBOSE )
              std::clog << " problem: tangent calo " << iseq->calo_tangent_id() << " has larger id than n of calos " << calos.size() << std::endl;
            continue;
          }

	  if( iseq->calo_tangent_id() != iseq->calo_helix_id() &&
	      iseq->calo_tangent_id() != iseq->helix_vertex_id() ){ // avoid double counting if both extrapolations point to the same calo
	    if( freecalos[iseq->calo_tangent_id()] )
	      freecalos[iseq->calo_tangent_id()] = 0;
	    else
	      counter ++;
	  }
	}

        if( iseq->has_tangent_vertex() && iseq->tangent_vertex_type() == "calo" ){
          if( iseq->tangent_vertex_id() >= calos.size() ){
            if( print_level() >= mybhep::VVERBOSE )
              std::clog << " problem: tangent calo-vertex " << iseq->tangent_vertex_id() << " has larger id than n of calos " << calos.size() << std::endl;
            continue;
          }

	  if( iseq->tangent_vertex_id() != iseq->calo_helix_id() &&
	      iseq->tangent_vertex_id() != iseq->calo_tangent_id() &&
	      iseq->tangent_vertex_id() != iseq->helix_vertex_id() ){ // avoid double counting if both extrapolations point to the same calo
	    if( freecalos[iseq->tangent_vertex_id()] )
	      freecalos[iseq->tangent_vertex_id()] = 0;
	    else
	      counter ++;
	  }
	}

      }


      n_overlaps_ = counter;

      return;

    }


    void calculate_n_free_families(const std::vector<topology::cell> &cells, const std::vector<topology::calorimeter_hit> &calos){

      std::vector<int> freecells(cells.size());
      fill(freecells.begin(), freecells.end(), 1);

      std::vector<int> freecalos(calos.size());
      fill(freecalos.begin(), freecalos.end(), 1);

      for(std::vector<logic_sequence>::iterator iseq = sequences_.begin(); iseq != sequences_.end(); ++iseq){

        for(std::vector<logic_cell>::iterator in = iseq->cells_.begin(); in != iseq->cells_.end(); ++in){
          if( in->id() >= cells.size() ){
            if( print_level() >= mybhep::VVERBOSE )
              std::clog << " problem: cell " << in->id() << " has larger id than n of cells " << cells.size() << std::endl;
            continue;
          }
          else{
            freecells[in->id()] = 0;
          }
        }

        if( iseq->has_decay_helix_vertex() && iseq->decay_helix_vertex_type() == "calo" ){

	  if( iseq->calo_helix_id() >= calos.size() ){
	    if( print_level() >= mybhep::VVERBOSE )
	      std::clog << " problem: helix calo " << iseq->calo_helix_id() << " has larger id than n of calos " << calos.size() << std::endl;
	    continue;
	  }
	  freecalos[iseq->calo_helix_id()] = 0;
	}

        if( iseq->has_helix_vertex() && iseq->helix_vertex_type() == "calo" ){

	  if( iseq->helix_vertex_id() >= calos.size() ){
	    if( print_level() >= mybhep::VVERBOSE )
	      std::clog << " problem: helix calo-vertex " << iseq->helix_vertex_id() << " has larger id than n of calos " << calos.size() << std::endl;
	    continue;
	  }
	  freecalos[iseq->helix_vertex_id()] = 0;
	}

        if( iseq->has_decay_tangent_vertex() && iseq->decay_tangent_vertex_type() == "calo" ){

	  if( iseq->calo_tangent_id() >= calos.size() ){
	    if( print_level() >= mybhep::VVERBOSE )
	      std::clog << " problem: tangent calo " << iseq->calo_tangent_id() << " has larger id than n of calos " << calos.size() << std::endl;
	    continue;
	  }
	  freecalos[iseq->calo_tangent_id()] = 0;
	}

        if( iseq->has_tangent_vertex() && iseq->tangent_vertex_type() == "calo" ){

	  if( iseq->tangent_vertex_id() >= calos.size() ){
	    if( print_level() >= mybhep::VVERBOSE )
	      std::clog << " problem: tangent calo-vertex " << iseq->tangent_vertex_id() << " has larger id than n of calos " << calos.size() << std::endl;
	    continue;
	  }
	  freecalos[iseq->tangent_vertex_id()] = 0;
	}

      }

      size_t counter = 0;
      for(std::vector<int>::iterator i=freecells.begin(); i!= freecells.end(); ++i)
        if( *i ){
          counter ++;
	}

      for(std::vector<int>::iterator i=freecalos.begin(); i!= freecalos.end(); ++i)
        if( *i ){
          counter ++;
	}

      n_free_families_ = counter;

      return;
    }


    void calculate_chi2(){

      double chi2 = 0.;
      int32_t ndof = 0;
      for(std::vector<logic_sequence>::iterator iseq = sequences_.begin(); iseq != sequences_.end(); ++iseq){
        chi2 += iseq->chi2();
        ndof += iseq->ndof();
      }

      chi2_ = chi2;
      ndof_ = ndof;

      return;
    }


    double Prob() const{
      return probof(chi2(), ndof());
    }

    bool better_scenario_than(const logic_scenario &s){

      // - n of recovered cells
      int deltanfree = n_free_families() - s.n_free_families();

      // n of new overlaps
      int deltanoverls = n_overlaps() - s.n_overlaps();

      double deltaprob = Prob() - s.Prob();


      if( print_level() >= mybhep::VVERBOSE ){
        std::clog << " delta n_free_families = (" << n_free_families()  << " - " << s.n_free_families() << ")= " << deltanfree
             << " dela n_overlaps = (" << n_overlaps() << " - " << s.n_overlaps() << ")= " << deltanoverls
             << " delta prob = (" << Prob()  << " - " << s.Prob() << ") = " << deltaprob << std::endl;
      }

      if( deltanoverls < - 2*deltanfree )
        return true;

      if( deltanoverls == - 2*deltanfree ){

        if( deltaprob > 0. )
            return true;
      }

      return false;
    }

  };
}
}

#endif
