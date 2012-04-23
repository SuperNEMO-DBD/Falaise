/* -*- mode: c++ -*- */
#ifndef __CATAlgorithm__ICELLTRIP
#define __CATAlgorithm__ICELLTRIP
#include <iostream>
#include <cmath>
#include <mybhep/error.h>
#include <mybhep/utilities.h>
#include <mybhep/point.h>
#include <mybhep/clhep.h>
#include <CATAlgorithm/experimental_point.h>
#include <CATAlgorithm/experimental_vector.h>
#include <CATAlgorithm/tracking_object.h>
#include <CATAlgorithm/cell.h>
#include <CATAlgorithm/line.h>
#include <CATAlgorithm/joint.h>
#include <CATAlgorithm/cell_couplet.h>
#include <algorithm>
#include <boost/cstdint.hpp>

namespace CAT{
namespace topology{

using namespace std;
using namespace mybhep;

  class cell_triplet : public tracking_object{

    // a cell_triplet is composed of three cells
    // and a list of joints

  protected:
    string appname_;

    // first cell
    cell ca_; 

    // second cell
    cell cb_; 

    // third cell
    cell cc_; 

    // list of chi2 values
    std::vector<double> chi2s_;

    // list of prob values
    std::vector<double> probs_;

  public:   

    // list of joints
    std::vector<joint> joints_;

    // status of cell triplet
    bool free_;

    // begun cell triplet
    bool begun_;

    //!Default constructor     
    cell_triplet()
    {
      appname_= "cell_triplet: ";
      free_ = false;
      begun_ = false;
    }

    //!Default destructor
    virtual ~cell_triplet(){};

    //! constructor
    cell_triplet(cell_couplet cca, cell_couplet ccb){
      appname_= "cell_triplet: ";
      free_ = false;
      begun_ = false;
    }

    //! constructor
    cell_triplet(cell ca, cell cb, cell cc, prlevel level=mybhep::NORMAL, double nsigma=10.){
      set_print_level(level);
      set_nsigma(nsigma);
      appname_= "cell_triplet: ";
      ca_ = ca;
      cb_ = cb;
      cc_ = cc;
      free_ = false;
      begun_ = false;
      chi2s_.clear();
      probs_.clear();
    }

    /*** dump ***/
    virtual void dump (ostream & a_out         = clog,
                            const string & a_title  = "",
                            const string & a_indent = "",
                            bool a_inherit          = false) const{
      string indent;
      if (! a_indent.empty ()) indent = a_indent;
      if (! a_title.empty ())
        {
          a_out << indent << a_title << endl;
        }

      a_out << indent << appname_ << " -------------- " << endl;
      a_out << indent  << " free: " << free() << " begun: " << begun()  << endl;
      a_out << indent  << " first cell " << endl;
      ca().dump(a_out,"", indent + "   ");
      a_out << indent << " second cell " << endl;
      cb().dump(a_out, "",indent + "   ");
      a_out << indent << " third cell " << endl;
      cc().dump(a_out, "",indent + "   ");
      for(vector<joint>::const_iterator ijoint=joints_.begin(); ijoint!=joints_.end(); ++ijoint)
        ijoint->dump(a_out,"",indent + "   ");
      a_out << indent  << " -------------- " << endl;
 
      return;
    }
    
    //! set cells
    void set(cell_couplet cca, cell_couplet ccb){
      cb_ = cca.ca();
      ca_ = cca.cb();
      cc_ = ccb.cb();
      if( cca.ca().id() != ccb.ca().id() ){
          clog << " problem: trying to form a triplet of cell with cells " << cca.ca().id() << " " 
               << cca.cb().id() << " " 
               << ccb.ca().id() << " " 
               << ccb.cb().id() << endl;
        }
    }

    //! set cells
    void set(cell ca, cell cb, cell cc){
      ca_ = ca;
      cb_ = cb;
      cc_ = cc;
    }


    //! set free level
    void set_free(bool free){
      free_ = free;
    }

    //! set begun level
    void set_begun(bool begun){
      begun_ = begun;
    }

    //! set joints
    void set_joints(std::vector<joint> joints){
      joints_ = joints;
    }

    //! set chi2 list
    void set_chi2s(std::vector<double> chi2s){
      chi2s_ = chi2s;
    }

    //! set prob list
    void set_probs(std::vector<double> probs){
      probs_ = probs;
    }

    //! get first cell couplet
    cell_couplet cca()
    {
      cell_couplet cc1(cb_, ca_, print_level(), nsigma());
      return cc1;
    }      

    //! get second cell couplet
    cell_couplet ccb()
    {
      cell_couplet cc2(cb_, cc_, print_level(), nsigma());
      return cc2;
    }      

    //! get joints
    const std::vector<joint>& joints() const
    {
      return joints_;
    }

    //! get first cell
    const cell& ca()const
    {
      return ca_;
    }      

    //! get second cell
    const cell& cb()const
    {
      return cb_;
    }      

    //! get third cell
    const cell& cc()const
    {
      return cc_;
    }      

    //! get list of chi2
    const std::vector<double>& chi2s() const
    {
      return chi2s_;
    }      

    //! get list of prob
    const std::vector<double>& probs() const
    {
      return probs_;
    }      


    //! get free level
    const bool free()const{
      return free_;
    }

    //! get begun level
    const bool begun()const{
      return begun_;
    }

  private:


  public:
    void calculate_joints(double Ratio, double separation_limit=90., double phi_limit=25., double theta_limit=180.){

      if( print_level() > mybhep::VERBOSE ){
        clog << appname_ << " calculate joints for cells: " << ca_.id() << " " << cb_.id() << " " << cc_.id() << endl;
      }

      joints_.clear();
      std::vector<line> t1 = cca().tangents(); // note: this tangent goes from cell B to cell A
      std::vector<line> t2 = ccb().tangents();

      bool use_ownerror = true;

      experimental_double local_separation;
      for(vector<line>::iterator i1=t1.begin(); i1!=t1.end(); ++i1){

        for(vector<line>::iterator i2=t2.begin(); i2!=t2.end(); ++i2){

          if( print_level() > mybhep::VERBOSE ){
            clog << " tangents " << i1 - t1.begin() << " and " << i2 - t2.begin() << endl;
          }

          bool shall_include_separation = true;
          if( cb_.small() ){
            if( print_level() > mybhep::VERBOSE ){
              clog << " no separation: middle cells is small ";
            }
            shall_include_separation = false;
          }
          else if( cb_.intersect(ca_) ){
            experimental_vector a0 = i1->forward_axis();
            experimental_vector a = a0.hor();
            experimental_vector b0 = cca().forward_axis();
            experimental_vector b = b0.hor();
            double psc = (a*b).value();

            if( fabs(psc) < 0.5 ){ // connection along the intersection
              if( print_level() > mybhep::VERBOSE ){
                clog << " no separation: connect along intersection AB ";
              }
              shall_include_separation = false;

              // keep only the connection with consistent ordering of cells
              experimental_vector c0 = ccb().forward_axis();
              experimental_vector c = c0.hor();
              double psc2 = (b*c).value();
              if( psc2 > 0.1 ){
                if( print_level() > mybhep::VERBOSE ){
                  clog << " rejected because first 2 cells intersect and the ordering is wrong: psc = " << psc2 << endl;
                }
                continue;
              }
              
              // keep only the connection that doesn't invert foward sense
              experimental_vector d0 = i2->forward_axis();
              experimental_vector d = d0.hor();
              double psc3 = (a*d).value();
              if( psc3 > 0. ){
                if( print_level() > mybhep::VERBOSE ){
                  clog << " rejected because direction is reversed: psc = " << psc3 << endl;
                }
                continue;
              }
            } 
          }
          else if( cb_.intersect(cc_) ){
            experimental_vector a0 = i2->forward_axis();
            experimental_vector a = a0.hor();
            experimental_vector b0 = ccb().forward_axis();
            experimental_vector b = b0.hor();
            double psc = (a*b).value();
            if( fabs(psc) < 0.5 ){ // connection along the intersection
              if( print_level() > mybhep::VERBOSE ){
                clog << " no separation: connect along intersection BC ";
              }
              shall_include_separation = false;
              // keep only the connection with consistent ordering of cells
              experimental_vector c0 = cca().forward_axis();
              experimental_vector c = c0.hor();
              double psc2 = (b*c).value();
              if( psc2 > 0.1 ){
                if( print_level() > mybhep::VERBOSE ){
                  clog << " rejected because last 2 cells intersect and the ordering is wrong: psc = " << psc2 << endl;
                }
                continue;
              }

              // keep only the connection that doesn't invert foward sense
              experimental_vector d0 = i1->forward_axis();
              experimental_vector d = d0.hor();
              double psc3 = (a*d).value();
              if( psc3 > 0. ){
                if( print_level() > mybhep::VERBOSE ){
                  clog << " rejected because direction is reversed: psc = " << psc3 << endl;
                }
                continue;
              }

            }

          }
          
          int32_t ndof = 2;  // 2 kink angles, 0 or 1 one separation angle
          if( shall_include_separation )
            ndof ++;

          experimental_point p;
          if( cb_.small() ){
            p = cb_.ep();
            experimental_double newxa = p.x();
            newxa.set_error(cb_.r().error());
            experimental_double newza = p.z();
            newza.set_error(cb_.r().error());
            p.set_x(newxa);
            p.set_z(newza);
          }
          else{
            p = cb_.angular_average(i1->epa(), i2->epa(), &local_separation);
          }

          line newt1(i1->epb(), p, print_level(), get_nsigma());
          line newt2(p, i2->epb(), print_level(), get_nsigma());

          if( print_level() > mybhep::VERBOSE ){
            clog << " p1: "; ca_.dump_point(i1->epb());
            clog << " p2 average point: "; cb_.dump_point(p);
            clog << " p3: "; cc_.dump_point(i2->epb());
            clog << "    separation: "; (local_separation*180/M_PI).dump();
          }

          double chi2 = 0.;
          bool ok = false;

          if( !use_ownerror ){
            if( fabs(local_separation.value()) <= separation_limit ){

              experimental_double phi_kink = newt1.kink_phi(newt2);
              experimental_double theta_kink = newt1.kink_theta(newt2);
              
              if( print_level() > mybhep::VERBOSE ){
                clog << " phi 1st tangent: "; (newt1.phi()*180./M_PI).dump();
                clog << " phi 2nd tangent: "; (newt2.phi()*180./M_PI).dump();
                clog << "    phi_kink " << phi_kink.value()*180/M_PI << " +- " << phi_kink.error()*180/M_PI << endl;
                clog << " theta 1st tangent: "; (newt1.theta()*180./M_PI).dump();
                clog << " theta 2nd tangent: "; (newt2.theta()*180./M_PI).dump();
                clog << "    theta_kink " << theta_kink.value()*180/M_PI << " +- " << theta_kink.error()*180/M_PI << endl;
              }
              
              if(  (fabs(phi_kink.value()) <= phi_limit ) && (fabs((newt1.kink_theta(newt2)).value()) <= theta_limit ) )
                ok = true;
            }
          }
          else{
            bool use_theta_kink = !(ca_.unknown_vertical() || cb_.unknown_vertical() || cc_.unknown_vertical());
            if( !use_theta_kink ) ndof --;

            chi2 = newt1.chi2(newt2, use_theta_kink);

            if( shall_include_separation )
              chi2 += square(local_separation.value()/local_separation.error());

            chi2s_.push_back(chi2);
            double local_prob = probof(chi2, ndof);
            probs_.push_back(local_prob);
            if( local_prob > prob() )
              ok = true;

            if( print_level() > mybhep::VERBOSE ){
              clog << "    chi2 " << chi2 << " prob " << probof(chi2, ndof) << " accepted: " << ok << endl;
            }

          }

          if( ok ){
            joint j(newt1.epa(),p,newt2.epb(), print_level(), get_nsigma());
            j.set_chi2(chi2);
            joints_.push_back(j);
          }
        }  
        
      }


      joints_ = refine(joints_, Ratio);

      return;

    }

    std::vector<joint> refine(std::vector<joint> joints, double Ratio){

      std::vector<joint> _joints;

      if( print_level() > mybhep::VERBOSE ){
        clog << " refining " << joints.size() << " joints " << endl;
      }

      for(std::vector<joint>::iterator ijoint=joints.begin(); ijoint != joints.end(); ++ijoint){
        bool found = false;
        for(std::vector<joint>::iterator jjoint=joints.begin(); jjoint != joints.end(); ++jjoint){
          if( jjoint == ijoint ) continue;
          
          if( ca_.same_quadrant(ijoint->epa(), jjoint->epa() ) &&
              !cb_.same_quadrant(ijoint->epb(), jjoint->epb() ) &&
              cc_.same_quadrant(ijoint->epc(), jjoint->epc() ) ){
            experimental_double delta_phi = experimental_fabs(ijoint->kink_phi()) - experimental_fabs(jjoint->kink_phi());
            if( delta_phi.value() > delta_phi.error() ){  // if ijoint has larger kink than jjoint
              if( print_level() > mybhep::VERBOSE ){
                clog << " ... removing joint " << ijoint - joints.begin()  << " because joint " << jjoint - joints.begin()  <<  " has the same initial and final quadrant, a different middle quadrant and better horizontal kink: delta phi = " << delta_phi.value() << endl;
              }
              found = true;
              break;
            }
          }
        }
        
        if( !found )
          _joints.push_back(*ijoint);
        
      }
      
      // order joints in order of increasing chi2 (best joint comes first)
      std::sort(_joints.begin(), _joints.end());

      // only keep 2 best joints
      if( _joints.size() > 2 ){
        _joints.erase(_joints.begin() + 2, _joints.end());
      }


      if( _joints.size() == 2 )
        if( _joints[1].chi2() / _joints[0].chi2() > Ratio ){
          if( print_level() > mybhep::VERBOSE ){
            clog << " erase 2nd best joint ( chi2 = " << _joints[1].chi2() << ") in favour of 1st best joint ( chi2 = " << _joints[0].chi2() << ") limit ratio is: " << Ratio << endl;
          }
          _joints.erase(_joints.begin() + 1);
        }
      
      if( print_level() > mybhep::VERBOSE ){
        clog << " after refining there are " << _joints.size() << " joints " << endl;
      }

      return _joints;

    }

    size_t iteration()const{
      for(vector<joint>::const_iterator i=joints_.begin(); i!=joints_.end(); ++i)
        if( ! i->used() )
          return (size_t)(i - joints_.begin());

      return joints().size();
    }

    cell_triplet invert(){
      cell_triplet inverted;
      inverted.set(ccb(),cca());
      inverted.set_free(free());
      inverted.set_begun(begun());
      inverted.set_chi2s(chi2s());
      inverted.set_probs(probs());

      std::vector<joint> inverted_joints;
      for(vector<joint>::iterator i=joints_.begin(); i!=joints_.end(); ++i){
        inverted_joints.push_back(i->invert());
      }
      inverted.set_joints( inverted_joints );
      return inverted;
    }


    void set_all_used(){
      for(vector<joint>::iterator i=joints_.begin(); i!=joints_.end(); ++i)
        i->set_used(true);
      set_begun(true);
      return;
    }

    friend bool operator==(const cell_triplet& left,
                           const cell_triplet& right)
    {

      return ((left.ca().id() == right.ca().id()) && (left.cc().id() == right.cc().id())) ||
        ((left.ca().id() == right.cc().id()) && (left.cc().id() == right.ca().id()));

    };


    bool same_last_cell(cell c)const{
      return ((this->ca().id() == c.id()) ||
              (this->cc().id() == c.id()) );
      
    }

  };
  
     
}

}

#endif
