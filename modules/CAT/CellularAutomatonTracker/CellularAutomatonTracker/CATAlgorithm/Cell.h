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

      // true hit position
      experimental_point p_true_;

      // reconstructed hit position
      experimental_point p_reco_;

      bool on_;

    public:
      Cell();
      virtual ~Cell();

      Cell(mybhep::hit hit, size_t id, bool SuperNemo, mybhep::prlevel level=mybhep::NORMAL, double probmin=1.e-200):cell(hit, id, SuperNemo, level=mybhep::NORMAL, probmin=1.e-200){};

      void set_p_true(experimental_point a);
      void set_p_reco(experimental_point a);
      void set_on(bool a);

      experimental_point p_true(){return p_true_;}
      experimental_point p_reco(){return p_reco_;}
      double phi();
      bool on();
      void draw();
      double distance(Circle h);
      double true_phi();
      double p();
      double p(Circle h);
      int legendre_sign();
      int legendre_sign(Circle h);
      double legendre();
      double legendre(Circle h);
      double delta(Circle h);
      double legendre_R(double X0, double Y0);
      void draw_surface(double xmin, double xmax, double ymin, double ymax);
      void reset();

    };
  }
}

#endif
