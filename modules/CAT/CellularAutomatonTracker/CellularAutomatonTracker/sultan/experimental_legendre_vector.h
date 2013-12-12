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

      void set_helices(std::vector<experimental_helix> a);

      std::vector<experimental_helix> helices();

      void add_helix(experimental_helix a);

      void reset();

      void get_neighbours(experimental_helix* a, std::vector<experimental_helix>* neighbours);

      experimental_helix max(std::vector<experimental_helix> * neighbours);

    };


  }

}

#endif
