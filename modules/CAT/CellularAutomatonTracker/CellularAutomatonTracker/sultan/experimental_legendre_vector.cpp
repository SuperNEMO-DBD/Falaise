/* -*- mode: c++ -*- */

#include <sultan/experimental_legendre_vector.h>

namespace SULTAN {
  namespace topology{

    using namespace std;
    using namespace mybhep;


    void experimental_legendre_vector::set_helices(std::vector<experimental_helix> a){helices_ = a;}
    
    std::vector<experimental_helix> experimental_legendre_vector::helices(){return helices_;}
    
    void experimental_legendre_vector::set_nsigmas(double a){nsigmas_=a;}

    double experimental_legendre_vector::get_nsigmas(){return nsigmas_;}

    void experimental_legendre_vector::add_helix(experimental_helix a){
      helices_.push_back(a);
    }
    
    void experimental_legendre_vector::reset(){
      helices_.clear();
    }
    
    void experimental_legendre_vector::get_neighbours(experimental_helix* a, std::vector<experimental_helix>* neighbours){
      experimental_double dx, dy, dz, dR, dH;
      neighbours->clear();
      for(std::vector<experimental_helix>::const_iterator ip = helices_.begin(); ip != helices_.end(); ++ip){
	if( !a->different_cells(*ip) ) continue;
	dx = a->x0() - ip->x0();
	dy = a->y0() - ip->y0();
	dz = a->z0() - ip->z0();
	dR = a->R() - ip->R();
	dH = a->H() - ip->H();
	
	if( fabs(dx.value()) < get_nsigmas()*dx.error() &&
	    fabs(dy.value()) < get_nsigmas()*dy.error() &&
	    fabs(dz.value()) < get_nsigmas()*dz.error() &&
	    fabs(dR.value()) < get_nsigmas()*dR.error() &&
	    fabs(dH.value()) < get_nsigmas()*dH.error() )
	  neighbours->push_back(*ip);
      }
      a->set_n_neighbours(neighbours->size());
      return;
    }
    
    experimental_helix experimental_legendre_vector::max(std::vector<experimental_helix> * neighbours){
      experimental_helix r;
      size_t n = 0;
      size_t nmax = 0;
      std::vector<experimental_helix> neis;
      for(std::vector<experimental_helix>::iterator ip = helices_.begin(); ip != helices_.end(); ++ip){
	get_neighbours(&(*ip), &neis);
	
	if( neis.size() > nmax ){
	  nmax = neis.size();
	  *neighbours = neis;
	  r = *ip;
	}
      }
      
      return r;
    }
    
    
  }
  
  
}


