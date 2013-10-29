
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
      on_=false;
    }

    //______________________________________________________________________________
    Cell::~Cell()
    {
    }

    void Cell::set_p_true(experimental_point a){
      p_true_=a;
    }
    void Cell::set_p_reco(experimental_point a){
      p_reco_=a;
    }

    void Cell::set_on(bool a){
      on_=a;
    }

    double Cell::phi(){
      return atan2(this->p_reco().y().value() - this->ep().y().value(), this->p_reco().x().value() - this->ep().x().value());
    }

    bool Cell::on(){
      return on_;
    }

    void Cell::draw(){

      if( !this->on() ){
	TMarker *t = new TMarker(this->ep().x().value(),this->ep().y().value(),8);
	t->SetMarkerColor(kBlack);
	t->Draw("same");
      }else{
	TEllipse *t = new TEllipse(this->ep().x().value(),this->ep().y().value(),this->r().value(),this->r().value());
	t->SetLineColor(kBlue);
	t->SetFillColor(0);
	t->Draw("same");
	TMarker *c = new TMarker(this->ep().x().value(),this->ep().y().value(),8);
	c->SetMarkerColor(kBlack);
	c->Draw("same");
	TMarker *p = new TMarker(this->p_true().x().value(),this->p_true().y().value(),7);
	p->SetMarkerColor(kRed);
	p->Draw("same");
      }
    }

    double Cell::distance(Circle h){

      double distance = h.radius().value() - sqrt(pow(this->ep().x().value() - h.center().x().value(),2) + pow(this->ep().y().value() - h.center().y().value(),2));
      return distance;

    }

    double Cell::true_phi(){
      return atan2(this->p_true().y().value() - this->ep().y().value(), this->p_true().x().value() - this->ep().x().value());
    }


    int Cell::legendre_sign(){
      if( this->p_true().y().value() > this->ep().y().value() )
	return 1;

      return -1;
    }

    int Cell::legendre_sign(Circle h){
      if( this->p_true().y().value() > h.center().y().value() )
	return 1;

      return -1;
    }

    double Cell::p(){
  
      double dx = this->p_true().x().value() - this->ep().x().value();
      double dy = this->p_true().y().value() - this->ep().y().value();
      double val = - this->legendre_sign()*dx/fabs(dy);
      return val;

    }

    double Cell::legendre(){

      double x0 = this->ep().x().value();
      double y0 = this->ep().y().value();
      double r = this->r().value();
      double p = this->p();

      return p*x0  - y0 - this->legendre_sign() * r * sqrt(1 + pow(p,2));
      //return p*x0  - y0 - r * sqrt(1 + pow(p,2));

    }

    double Cell::p(Circle h){
  
      double dx = this->ep().x().value() - h.center().x().value();
      double dy = this->ep().y().value() - h.center().y().value();
      double val = - dx/dy;
      return val;

    }

    double Cell::legendre(Circle h){

      double x0 = h.center().x().value();
      double y0 = h.center().y().value();
      double r = h.radius().value();
      double p = this->p(h);

      return p*x0  - y0 - this->legendre_sign(h)*r * sqrt(1 + pow(p,2));

    }

    double Cell::delta(Circle h){

      double r = this->r().value();
      double R = h.radius().value();
      double dy = this->ep().y().value() - h.center().y().value();
      return - 2.*r*(R+r)/dy;

    }


    double Cell::legendre_R(double X0, double Y0){

      double dx = this->ep().x().value() - X0;
      double dy = this->ep().y().value() - Y0;
      double p = -dx/dy;
      double R = fabs(p*dx - dy)/sqrt(1+pow(p,2));
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

      this->set_p_true(experimental_point(experimental_double(0.,0.), experimental_double(0.,0.), experimental_double(0.,0.)));
      this->set_p_reco(experimental_point(experimental_double(0.,0.), experimental_double(0.,0.), experimental_double(0.,0.)));
      this->set_r(0.);
      this->set_er(0.);
      this->set_on(false);

    }

  }
}
