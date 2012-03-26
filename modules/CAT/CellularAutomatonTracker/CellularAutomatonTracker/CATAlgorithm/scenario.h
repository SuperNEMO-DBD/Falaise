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
#include <CATAlgorithm/sequence.h>
#include <CATAlgorithm/node.h>
#include <CATAlgorithm/calorimeter_hit.h>

namespace CAT {
  namespace topology{

    using namespace std;
    using namespace mybhep;

    class scenario : public tracking_object{

      // a reconstruction scenario is composed of 
      // a collection of tracks

    private:
      string appname_;

      // chi2
      double chi2_;
      int32_t ndof_;

      // n of free families
      size_t n_free_families_;

      // n of overlapping cells
      size_t n_overlaps_;

    public:   

      // tracks
      std::vector<topology::sequence> sequences_;

      //!Default constructor 
      scenario()
      {
        appname_= "scenario: ";
        set_print_level(mybhep::NORMAL);
        set_nsigma(10.);
        sequences_.clear();
        chi2_ = small_neg;
        ndof_ = default_integer;
        n_free_families_ = default_integer;
        n_overlaps_ = default_integer;
      }

      //!Default destructor
      virtual ~scenario(){};

      //! constructor
      scenario(std::vector<sequence> seqs, prlevel level=mybhep::NORMAL, double nsigma=10.){
        appname_= "scenario: ";
        set_print_level(mybhep::NORMAL);
        set_nsigma(nsigma);
        sequences_ = seqs;
        chi2_ = small_neg;
        ndof_ = default_integer;
        n_free_families_ = default_integer;
        n_overlaps_ = default_integer;
      }

      /*** dump ***/
      virtual void dump (ostream & a_out         = clog,
                         const string & a_title  = "",
                         const string & a_indent = "",
                         bool a_inherit          = false)const{
        {
          string indent;
          if (! a_indent.empty ()) indent = a_indent;
          if (! a_title.empty ())
            {
              a_out << indent << a_title << endl;
            }

          a_out << indent << appname_ << " -------------- " << endl;
          a_out << indent << "chi2 : " << chi2() << " ndof " << ndof() << " prob " << Prob() << endl;
          a_out << indent << "n free families : " << n_free_families() << endl;
          a_out << indent << "n overlaps : " << n_overlaps() << endl;
          for(std::vector<sequence>::const_iterator iseq = sequences_.begin(); iseq != sequences_.end(); ++iseq)
            iseq->dump();
          a_out << indent << " -------------- " << endl;

          return;
        }
      }



      //! set experimental_point, radius, error and id;
      void set(std::vector<sequence> seqs){
        appname_= "scenario: ";
        sequences_ = seqs;
        chi2_ = small_neg;
        n_free_families_ = default_integer;
        n_overlaps_ = default_integer;
      }

      //! set sequences
      void set_sequences(std::vector<sequence> seqs){
        sequences_ = seqs;
      }

      //! set chi2
      void set_chi2(double chi2){
        chi2_ = chi2;
      }

      //! set n free families
      void set_n_free_families(size_t n){
        n_free_families_ = n;
      }

      //! set n overlaps
      void set_n_overlaps(size_t n){
        n_overlaps_ = n;
      }

      //! set ndof
      void set_ndof(int32_t n){
        ndof_ = n;
      }

      //! get sequences
      const std::vector<sequence> & sequences()const
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


      void calculate_n_overlaps(std::vector<topology::cell> cells, std::vector<topology::calorimeter_hit> calos){

        std::vector<int> freecells(cells.size());
        fill(freecells.begin(), freecells.end(), 1);

        std::vector<int> freecalos(calos.size());
        fill(freecalos.begin(), freecalos.end(), 1);
      
        size_t counter = 0;

        for(std::vector<sequence>::iterator iseq = sequences_.begin(); iseq != sequences_.end(); ++iseq){

          for(std::vector<node>::iterator in = iseq->nodes_.begin(); in != iseq->nodes_.end(); ++in){
            if( in->c().id() >= cells.size() ){
              if( print_level() >= mybhep::VVERBOSE )
                clog << " problem: cell " << in->c().id() << " has larger id than n of cells " << cells.size() << endl;
              continue;
            }

            if( freecells[in->c().id()] )
              freecells[in->c().id()] = 0;
            else
              counter ++;

          }

          if( iseq->has_decay_vertex() ){
            if( iseq->calo_id() >= calos.size() ){
              if( print_level() >= mybhep::VVERBOSE )
                clog << " problem: calo " << iseq->calo_id() << " has larger id than n of calos " << calos.size() << endl;
              continue;
            }

            if( freecalos[iseq->calo_id()] )
              freecalos[iseq->calo_id()] = 0;
            else
              counter ++;

          }
        
        }
    

        n_overlaps_ = counter;

        return;
      
      }


      void calculate_n_free_families(std::vector<topology::cell> cells, std::vector<topology::calorimeter_hit> calos){

        std::vector<int> freecells(cells.size());
        fill(freecells.begin(), freecells.end(), 1);

        std::vector<int> freecalos(calos.size());
        fill(freecalos.begin(), freecalos.end(), 1);


        for(std::vector<sequence>::iterator iseq = sequences_.begin(); iseq != sequences_.end(); ++iseq){

          for(std::vector<node>::iterator in = iseq->nodes_.begin(); in != iseq->nodes_.end(); ++in){
            if( in->c().id() >= cells.size() ){
              if( print_level() >= mybhep::VVERBOSE )
                clog << " problem: cell " << in->c().id() << " has larger id than n of cells " << cells.size() << endl;
              continue;
            }
            else{
              freecells[in->c().id()] = 0;
            }
          }

          if( !iseq->has_decay_vertex() ) continue;

          if( iseq->calo_id() >= calos.size() ){
            if( print_level() >= mybhep::VVERBOSE )
              clog << " problem: calo " << iseq->calo_id() << " has larger id than n of calos " << calos.size() << endl;
            continue;
          }
          freecalos[iseq->calo_id()] = 0;

        }

        size_t counter = 0;
        for(std::vector<int>::iterator i=freecells.begin(); i!= freecells.end(); ++i)
          if( *i )
            counter ++;

        for(std::vector<int>::iterator i=freecalos.begin(); i!= freecalos.end(); ++i)
          if( *i )
            counter ++;

        n_free_families_ = counter;

        return;
      }


      void calculate_chi2(){

        double chi2 = 0.;
        int32_t ndof = 0;
        for(std::vector<sequence>::iterator iseq = sequences_.begin(); iseq != sequences_.end(); ++iseq){
          chi2 += iseq->chi2();
          ndof += iseq->ndof();
        }

        chi2_ = chi2;
        ndof_ = ndof;

        return;
      }


      double Prob()const{
        return probof(chi2(), ndof());
      }

      bool better_scenario_than( scenario s)const{

        // - n of recovered cells 
        int deltanfree = n_free_families() - s.n_free_families();

        // n of new overlaps 
        int deltanoverls = n_overlaps() - s.n_overlaps();

        double deltaprob = Prob() - s.Prob();
        double deltachi = chi2() - s.chi2();


        if( print_level() >= mybhep::VVERBOSE ){
          clog << " delta n_free_families = (" << n_free_families()  << " - " << s.n_free_families() << ")= " << deltanfree 
               << " dela n_overlaps = (" << n_overlaps() << " - " << s.n_overlaps() << ")= " << deltanoverls
               << " delta prob = (" << Prob()  << " - " << s.Prob() << ") = " << deltaprob << endl;
        }

        if( deltanoverls < - 2*deltanfree )
          return true;

        if( deltanoverls == - 2*deltanfree ){

          if( deltaprob > 0. )
            return true;

          if( deltaprob == 0. && deltachi < 0. )
            return true;

        }

        return false;
      }

    };
  }
}

#endif
