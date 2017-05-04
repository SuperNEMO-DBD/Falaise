
#include "RVersion.h"
#include "TDirectory.h"
#include "TProcessID.h"
#include "TMarker.h"
#include "TEllipse.h"
#include "TF2.h"
#include "TAxis.h"

#include <iostream>

#include <CATAlgorithm/Legendre_function.h>
#include <CATAlgorithm/Cell.h>

namespace CAT {
  namespace topology{


    using namespace std;

    //______________________________________________________________________________
    Cell::Cell(){
      track_id_ = -1;
      circle_phi_ = 0.;
    }

    //______________________________________________________________________________
    Cell::~Cell()
    {
    }

    void Cell::set_p_reco(experimental_point a){
      p_reco_=a;
    }

    void Cell::set_track_id(int a){
      track_id_=a;
    }

    double Cell::phi(){
      // phi of reco point wrt cell center
      return atan2(this->p_reco().y().value() - this->ep().y().value(), this->p_reco().x().value() - this->ep().x().value());
    }

    int Cell::track_id(){
      return track_id_;
    }

    double Cell::distance(Circle h){
      // distance of cell center from circle
      // > 0 if    [circle center] ..... [cell center ] ... [common point]
      // < 0 if    [circle center] ..... [common point] ... [cell center ]

      // distance of cell center from circle center
      double dx = this->ep().x().value() - h.center().x().value();
      double dy = this->ep().y().value() - h.center().y().value();

      double distance = h.radius().value() - sqrt(pow(dx,2) + pow(dy,2));
      return distance;

    }

    double Cell::legendre_R(double X0, double Y0)const{
      // radius of a circle with center in (X0, Y0)
      // that reaches center of cell

      double dx = this->ep().x().value() - X0;
      double dy = this->ep().y().value() - Y0;
      double p = -dx/dy;
      double R = std::abs(p*dx - dy)/sqrt(1+pow(p,2));
      return R;

    }

    void Cell::draw_surface(double xmin, double xmax, double ymin, double ymax){

      Legendre_function *func = new Legendre_function();

      TF2 *f = new TF2("f",func,&Legendre_function::Evaluate,xmin, xmax, ymin, ymax,2,"Legendre_function","Evaluate");
      f->SetParameters(this->ep().x().value(),this->ep().y().value());
      f->GetXaxis()->SetTitle("X0");
      f->GetXaxis()->SetTitleOffset(1.3);
      f->GetYaxis()->SetTitle("Y0");
      f->GetYaxis()->SetTitleOffset(1.4);
      f->GetZaxis()->SetTitle("R");
      f->GetZaxis()->SetTitleOffset(1.3);
      f->Draw("lego2");

    }

    void Cell::reset(){

      this->set_p_reco(experimental_point(experimental_double(0.,0.), experimental_double(0.,0.), experimental_double(0.,0.)));
      this->set_r(0.);
      this->set_er(0.);
      this->set_track_id(-1);
      this->set_circle_phi(0.);

    }

  }
}
