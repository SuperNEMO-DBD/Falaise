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
    
    void experimental_legendre_vector::get_neighbours(experimental_helix a, std::vector<experimental_helix>* neighbours){
      experimental_double dx, dy, dz, dR, dH;
      neighbours->clear();
      for(std::vector<experimental_helix>::const_iterator ip = helices_.begin(); ip != helices_.end(); ++ip){
	if( !a.different_cells(*ip) ) continue;
	dx = a.x0() - ip->x0();
	if( fabs(dx.value()) > get_nsigmas()*dx.error() ) continue;
	dy = a.y0() - ip->y0();
	if( fabs(dy.value()) > get_nsigmas()*dy.error() ) continue;
	dz = a.z0() - ip->z0();
	if( fabs(dz.value()) > get_nsigmas()*dz.error() ) continue;
	dR = a.R() - ip->R();
	if( fabs(dR.value()) > get_nsigmas()*dR.error() ) continue;
	dH = a.H() - ip->H();
	if( fabs(dH.value()) > get_nsigmas()*dH.error() ) continue;
	neighbours->push_back(*ip);
      }
      return;
    }
    
    experimental_helix experimental_legendre_vector::max(std::vector<experimental_helix> * neighbours){
      experimental_helix r;
      size_t n = 0;
      size_t nmax = 0;
      std::vector<experimental_helix> neis;
      for(std::vector<experimental_helix>::const_iterator ip = helices_.begin(); ip != helices_.end(); ++ip){
	get_neighbours(*ip, &neis);
	
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


