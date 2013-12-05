/* -*- mode: c++ -*- */
#ifndef __sultan__EXPERIMENTALLEGENDRE_VECTOR
#define __sultan__EXPERIMENTALLEGENDRE_VECTOR
#include <iostream>
#include <cmath>
#include <mybhep/utilities.h>
#include <sultan/experimental_helix.h>


namespace SULTAN {
  namespace topology{


    class experimental_legendre_vector : public tracking_object {

    private:

      std::string appname_;

      std::vector<experimental_helix> helices_;

    public:


      //!Default constructor
      experimental_legendre_vector(mybhep::prlevel level=mybhep::NORMAL, double probmin=1.e-200)
      {
        appname_= "experimental_legendre_vector: ";
	helices_.clear();
        set_print_level(level);
        set_probmin(probmin);
      }

      //!Default destructor
      virtual ~experimental_legendre_vector(){};

      /*** dump ***/
      virtual void dump (std::ostream & a_out         = std::clog,
                         const std::string & a_title  = "",
                         const std::string & a_indent = "",
                         bool a_inherit          = false){
        {
          std::string indent;
          if (! a_indent.empty ()) indent = a_indent;
          if (! a_title.empty ())
            {
              a_out << indent << a_title << std::endl;
            }
          a_out << indent << appname_  << std::endl;
	  for(std::vector<experimental_helix>::const_iterator ip = helices_.begin(); ip != helices_.end(); ++ip){
	    ip->dump(); 
	  }

          return;
        }
      }


      void set_helices(std::vector<experimental_helix> a){helices_ = a;}

      std::vector<experimental_helix> helices(){return helices_;}

      void add_helix(experimental_helix a){
	helices_.push_back(a);
      }

      void reset(){
	helices_.clear();
      }

      void get_neighbours(experimental_helix* a, std::vector<experimental_helix>* neighbours){
	experimental_double dx, dy, dz, dR, dH;
	neighbours->clear();
	for(std::vector<experimental_helix>::const_iterator ip = helices_.begin(); ip != helices_.end(); ++ip){
	  if( !a->different_cells(*ip) ) continue;
	  dx = a->x0() - ip->x0();
	  dy = a->y0() - ip->y0();
	  dz = a->z0() - ip->z0();
	  dR = a->R() - ip->R();
	  dH = a->H() - ip->H();

	  if( fabs(dx.value()) < dx.error() &&
	      fabs(dy.value()) < dy.error() &&
	      fabs(dz.value()) < dz.error() &&
	      fabs(dR.value()) < dR.error() &&
	      fabs(dH.value()) < dH.error() )
	    neighbours->push_back(*ip);
	}
	a->set_n_neighbours(neighbours->size());
	return;
      }

      experimental_helix max(std::vector<experimental_helix> * neighbours){
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


    };


  }

}

#endif
