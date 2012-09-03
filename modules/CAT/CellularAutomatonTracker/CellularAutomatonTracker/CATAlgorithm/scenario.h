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


    class scenario : public tracking_object{

      // a reconstruction scenario is composed of
      // a collection of tracks

    private:
      std::string appname_;

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
        //sequences_.clear();
        chi2_ = mybhep::small_neg;
        ndof_ = mybhep::default_integer;
        n_free_families_ = mybhep::default_integer;
        n_overlaps_ = mybhep::default_integer;
      }

      //!Default destructor
      virtual ~scenario(){};

      //! constructor
      scenario(const std::vector<sequence> & seqs, mybhep::prlevel level=mybhep::NORMAL, double nsigma=10.){
        appname_= "scenario: ";
        set_print_level(mybhep::NORMAL);
        set_nsigma(nsigma);
        sequences_ = seqs;
        chi2_ = mybhep::small_neg;
        ndof_ = mybhep::default_integer;
        n_free_families_ = mybhep::default_integer;
        n_overlaps_ = mybhep::default_integer;
      }

      /*** dump ***/
      virtual void dump (std::ostream & a_out         = std::clog,
                         const std::string & a_title  = "",
                         const std::string & a_indent = "",
                         bool a_inherit          = false)const{
        {
          std::string indent;
          if (! a_indent.empty ()) indent = a_indent;
          if (! a_title.empty ())
            {
              a_out << indent << a_title << std::endl;
            }

          a_out << indent << appname_ << " -------------- " << std::endl;
          a_out << indent << "chi2 : " << chi2() << " ndof " << ndof() << " prob " << Prob() << std::endl;
          a_out << indent << "n free families : " << n_free_families() << std::endl;
          a_out << indent << "n overlaps : " << n_overlaps() << std::endl;
          for(std::vector<sequence>::const_iterator iseq = sequences_.begin(); iseq != sequences_.end(); ++iseq)
            iseq->dump();
          a_out << indent << " -------------- " << std::endl;

          return;
        }
      }



      //! set experimental_point, radius, error and id;
      void set(const std::vector<sequence> & seqs){
        appname_= "scenario: ";
        sequences_ = seqs;
        chi2_ = mybhep::small_neg;
        n_free_families_ = mybhep::default_integer;
        n_overlaps_ = mybhep::default_integer;
      }

      //! set sequences
      void set_sequences(const std::vector<sequence> & seqs){
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
      double chi2() const
      {
        return chi2_;
      }

      //!get ndof
      int32_t ndof() const
      {
        return ndof_;
      }

      //!get n free families
      size_t n_free_families() const {return n_free_families_;}

      //!get n overlaps
      size_t n_overlaps() const {return n_overlaps_;}


      void calculate_n_overlaps(const std::vector<topology::cell> & cells,
                                const std::vector<topology::calorimeter_hit> & calos){

        std::vector<int> freecells(cells.size());
        fill(freecells.begin(), freecells.end(), 1);

        std::vector<int> freecalos(calos.size());
        fill(freecalos.begin(), freecalos.end(), 1);

        size_t counter = 0;

        for(std::vector<sequence>::iterator iseq = sequences_.begin(); iseq != sequences_.end(); ++iseq){

          for(std::vector<node>::iterator in = iseq->nodes_.begin(); in != iseq->nodes_.end(); ++in){
            if( in->c().id() >= cells.size() ){
              if( print_level() >= mybhep::VVERBOSE )
                std::clog << " problem: cell " << in->c().id() << " has larger id than n of cells " << cells.size() << std::endl;
              continue;
            }

            if( freecells[in->c().id()] )
              freecells[in->c().id()] = 0;
            else
              counter ++;

          }

          if( iseq->has_decay_helix_vertex() ){
            if( iseq->calo_helix_id() >= calos.size() ){
              if( print_level() >= mybhep::VVERBOSE )
                std::clog << " problem: calo " << iseq->calo_helix_id() << " has larger id than n of calos " << calos.size() << std::endl;
              continue;
            }

            if( freecalos[iseq->calo_helix_id()] )
              freecalos[iseq->calo_helix_id()] = 0;
            else
              counter ++;

          }

        }


        n_overlaps_ = counter;

        return;

      }


      void calculate_n_free_families(const std::vector<topology::cell> &cells,
                                     const std::vector<topology::calorimeter_hit> & calos){

        std::vector<int> freecells(cells.size());
        fill(freecells.begin(), freecells.end(), 1);

        std::vector<int> freecalos(calos.size());
        fill(freecalos.begin(), freecalos.end(), 1);


        for(std::vector<sequence>::iterator iseq = sequences_.begin(); iseq != sequences_.end(); ++iseq){

          for(std::vector<node>::iterator in = iseq->nodes_.begin(); in != iseq->nodes_.end(); ++in){
            if( in->c().id() >= cells.size() ){
              if( print_level() >= mybhep::VVERBOSE )
                std::clog << " problem: cell " << in->c().id() << " has larger id than n of cells " << cells.size() << std::endl;
              continue;
            }
            else{
              freecells[in->c().id()] = 0;
            }
          }

          if( !iseq->has_decay_helix_vertex() ) continue;

          if( iseq->calo_helix_id() >= calos.size() ){
            if( print_level() >= mybhep::VVERBOSE )
              std::clog << " problem: calo " << iseq->calo_helix_id() << " has larger id than n of calos " << calos.size() << std::endl;
            continue;
          }
          freecalos[iseq->calo_helix_id()] = 0;

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

      bool better_scenario_than( const scenario & s)const{

        // - n of recovered cells
        int deltanfree = n_free_families() - s.n_free_families();

        // n of new overlaps
        int deltanoverls = n_overlaps() - s.n_overlaps();

        double deltaprob = Prob() - s.Prob();
        double deltachi = chi2() - s.chi2();


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

          if( deltaprob == 0. && deltachi < 0. )
            return true;

        }

        return false;
      }

    };
  }
}

#endif
