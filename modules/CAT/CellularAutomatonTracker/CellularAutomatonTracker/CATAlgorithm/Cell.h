/* -*- mode: c++ -*- */
#ifndef __CATAlgorithm__Cell
#define __CATAlgorithm__Cell

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// Cell                                                                //
//                                                                      //
// Description of the cell and track parameters                        //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include <cmath>
#include <CATAlgorithm/Circle.h>
#include <CATAlgorithm/cell.h>

namespace CAT {
  namespace topology{

    class Cell : public cell {

    private:

      // reconstructed hit position
      experimental_point p_reco_;

      int track_id_;

    public:
      Cell();
      virtual ~Cell();

      Cell(mybhep::hit hit, size_t id, bool SuperNemo, mybhep::prlevel level=mybhep::NORMAL, double probmin=1.e-200):cell(hit, id, SuperNemo, level=mybhep::NORMAL, probmin=1.e-200){};

      void set_p_reco(experimental_point a);
      void set_track_id(int a);

      /*** dump ***/
      virtual void dump (std::ostream & a_out         = std::clog,
                         const std::string & a_title  = "",
                         const std::string & a_indent = "",
                         bool a_inherit          = false) const{

	std::string indent;
	if (! a_indent.empty ()) indent = a_indent;
	if (! a_title.empty ())
	  {
	    a_out << indent << a_title << std::endl;
	  }
	
	a_out << indent << " Cell ------------------- " << " id " << id() << std::endl;
	a_out << " wire position: ( " << this->ep().x().value() << ", " << this->ep().y().value() << ")" << std::endl;
	a_out << " measurement: r = " << this->r().value() << " +- " << this->r().error() << ", z = " << this->ep().z().value() << " +- " << this->ep().z().error() << std::endl;
	a_out << " reco position: ( " << this->p_reco().x().value() << " +- " << this->p_reco().x().error() << ", " << this->p_reco().y().value() << " +- " << this->p_reco().y().error() << ", " << this->p_reco().z().value() << " +- " << this->p_reco().z().error() << ")" << std::endl; 

      }


      const experimental_point p_reco()const {return p_reco_;}
      int track_id();
      double phi();
      void draw();
      double distance(Circle h);
      double p(Circle h);
      double delta(Circle h);
      const double legendre_R(double X0, double Y0)const;
      void draw_surface(double xmin, double xmax, double ymin, double ymax);
      void reset();

    };
  }
}

#endif
