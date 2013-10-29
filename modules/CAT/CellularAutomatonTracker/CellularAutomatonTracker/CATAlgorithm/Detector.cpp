
#include "RVersion.h"
#include "TRandom.h"
#include "TDirectory.h"
#include "TProcessID.h"
#include "TH3D.h"
#include "TPolyMarker3D.h"
#include "TSpectrum3.h"

#include <ctime>
#include <time.h>
#include <iostream>

#include <CATAlgorithm/Detector.h>

namespace CAT {
  namespace topology{


    using namespace std;

    //______________________________________________________________________________
    Detector::Detector(){
      x0_=0;
      y0_=0;
      Lx_=0;
      Ly_=0;
      cell_size_=0;
      cells_.clear();
      leftover_cells_.clear();
    }

    //______________________________________________________________________________
    Detector::~Detector()
    {
    }

    void Detector::build_cells()
    {

      size_t ncells_x = (int)(2.*Lx_/cell_size_);
      size_t ncells_y = (int)(2.*Ly_/cell_size_);
  
      for(size_t ix=0; ix<ncells_x; ix++){
	double cell_x = x0_ - ncells_x*cell_size_/2. + (ix + 0.5)*cell_size_; 
	for(size_t iy=0; iy<ncells_y; iy++){
	  double cell_y = y0_ - ncells_y*cell_size_/2. + (iy + 0.5)*cell_size_;

	  Cell c;
	  c.set_p(experimental_point(experimental_double(cell_x,0.), experimental_double(cell_y,0.), experimental_double(0.,0.)));
	  c.set_on(false);
	  cells_.push_back(c);
	}

      }


    }

    void Detector::draw()
    {

      for(size_t i=0; i<cells_.size(); i++)
	cells_[i].draw();
    }

    bool Detector::detect(Circle h, double sigma){

      double distance, digitized_distance;
      double angle;
      //std::time_t seed = std::time(0);
      double seed;
      TRandom *random = new TRandom();
      double factor;
      double x, y;
      bool value = false;

      seed = clock();
      random->SetSeed((UInt_t)seed);
      for(size_t i=0; i<cells_.size(); i++){
	distance = cells_[i].distance(h);
	if( fabs(distance) < cell_size_/2. ){
	  value = true;
	  cells_[i].set_on(true);
	  angle = atan2(cells_[i].ep().y().value() - h.center().y().value(), cells_[i].ep().x().value() - h.center().x().value());
	  x = cells_[i].ep().x().value() + distance*cos(angle);
	  y = cells_[i].ep().y().value() + distance*sin(angle);

	  factor=random->Gaus(0,sigma)*this->cell_size();
	  digitized_distance = fabs(distance)+factor;
	  if( digitized_distance < 0. ) digitized_distance = 0.;

	  cells_[i].set_p_true(experimental_point(experimental_double(x,0.), experimental_double(y,0.), experimental_double(0.,0.)));

	  cells_[i].set_r(digitized_distance);
	  cells_[i].set_er(sigma*this->cell_size());
	}
      }

      return value;

    }

    bool Detector::on(){

      for(size_t i=0; i<cells_.size(); i++)
	if( cells_[i].on() )
	  return true;

      return false;
    }

    size_t Detector::n_active_cells(){

      size_t counter = 0;

      for(size_t i=0; i<cells_.size(); i++)
	if( cells_[i].on() )
	  counter ++;

      return counter;

    }

    std::vector<Cell> Detector::active_cells(){

      std::vector<Cell> cs;
      for(size_t i=0; i<cells_.size(); i++)
	if( cells_[i].on() )
	  cs.push_back(cells_[i]);

      return cs;

    }

    double Detector::average_x(){

      std::vector<Cell> cs = this->active_cells();
      double average=0.;
      for(size_t i=0; i<cs.size(); i++)
	average += cs[i].ep().x().value();
      average /= cs.size();
      return average;

    }

    double Detector::average_y(){

      std::vector<Cell> cs = this->active_cells();
      double average=0.;
      for(size_t i=0; i<cs.size(); i++)
	average += cs[i].ep().y().value();
      average /= cs.size();
      return average;

    }

    void Detector::draw_surfaces_rough(Circle *h, bool draw, std::vector<Cell> cs, int ievent){

      leftover_cells_.clear();

      double xmin = 2.*(this->x0() - this->Lx());
      double xmax = 2.*(this->x0() + this->Lx());
      double ymin = 2.*(this->y0() - this->Ly());
      double ymax = 2.*(this->y0() + this->Ly());
      double rmin = 0.;
      double rmax = 2.*sqrt(pow(this->Lx(),2) + pow(this->Ly(),2));

      TH3D* surfaces_rough= new TH3D(Form("surfaces_rough_%d", ievent),"surfaces_rough",
				     (int)((xmax - xmin)/this->cell_size()), xmin, xmax,
				     (int)((ymax - ymin)/this->cell_size()), ymin, ymax,
				     (int)((rmax - rmin)/this->cell_size()), rmin, rmax);
      surfaces_rough->GetXaxis()->SetTitle("X0");
      surfaces_rough->GetXaxis()->SetTitleOffset(1.3);
      surfaces_rough->GetYaxis()->SetTitle("Y0");
      surfaces_rough->GetYaxis()->SetTitleOffset(1.4);
      surfaces_rough->GetZaxis()->SetTitle("R");
      surfaces_rough->GetZaxis()->SetTitleOffset(1.3);


      double x, y;

      for(size_t ix=1; ix<= (size_t)surfaces_rough->GetNbinsX(); ix++){
	x = surfaces_rough->GetXaxis()->GetBinCenter(ix);
	for(size_t iy=1; iy<= (size_t)surfaces_rough->GetNbinsY(); iy++){
	  y = surfaces_rough->GetYaxis()->GetBinCenter(iy);
	  for(size_t ic=0; ic<cs.size(); ic++){
	    surfaces_rough->Fill(x, y, cs[ic].legendre_R(x, y));
	  }
	}
      }

      /*
	TSpectrum3 *s = new TSpectrum3(3,1);
	Int_t nfound = s->Search(surfaces_rough,2,"goff",0.3);
	Float_t *xpeaks = s->GetPositionX();
	Float_t *ypeaks = s->GetPositionY();
	Float_t *zpeaks = s->GetPositionZ();

	cout << " found " << nfound << " peaks " << endl;
	for(size_t ip=0; ip<(size_t)nfound;ip++){
	cout << " peak " << ip << " x " << xpeaks[ip] << " y " << ypeaks[ip] << " z " << zpeaks[ip] << endl;
	}
      */

      int ixmax, iymax, irmax;
      surfaces_rough->GetMaximumBin(ixmax,iymax,irmax);
      double X0 = surfaces_rough->GetXaxis()->GetBinCenter(ixmax);
      double Y0 = surfaces_rough->GetYaxis()->GetBinCenter(iymax);
      double R = surfaces_rough->GetZaxis()->GetBinCenter(irmax);

      h->set_center(experimental_point(experimental_double(X0,0.), experimental_double(Y0,0.), experimental_double(0.,0.)));
      h->set_radius(experimental_double(R, 0.));


      if( draw ){
	TPolyMarker3D *t = new TPolyMarker3D(1, 8);
	t->SetMarkerColor(kRed);
	t->SetPoint(0,X0,Y0,R);
    
	surfaces_rough->Draw("");
	t->Draw("same");
      }else{
	surfaces_rough->Clear();
	//delete surfaces_rough;
      }

      double Rmax = surfaces_rough->GetZaxis()->GetBinUpEdge(irmax);
      double Rmin = surfaces_rough->GetZaxis()->GetBinLowEdge(irmax);
      double cell_R;
      std::vector<Cell> leftover_cs;
      for(size_t ic=0; ic<cs.size(); ic++){
	cell_R = cs[ic].legendre_R(X0, Y0);
	if( cell_R < Rmin || cell_R > Rmax )
	  leftover_cs.push_back(cs[ic]);
      }

      this->set_leftover_cells(leftover_cs);

    }

    void Detector::draw_surfaces_precise(Circle rough, Circle *h, bool draw, int ievent){

      int ncells = 3;

      double xmin = rough.center().x().value() - ncells*this->cell_size();
      double xmax = rough.center().x().value() + ncells*this->cell_size();
      double ymin = rough.center().y().value() - ncells*this->cell_size();
      double ymax = rough.center().y().value() + ncells*this->cell_size();
      double rmin = rough.radius().value() - ncells*this->cell_size();
      double rmax = rough.radius().value() + ncells*this->cell_size();

      int nbins = 100;

      TH3D* surfaces_precise= new TH3D(Form("surfaces_precise_%d", ievent),"surfaces_precise",
				       nbins,xmin,xmax,
				       nbins,ymin,ymax,
				       nbins,rmin,rmax);
      surfaces_precise->GetXaxis()->SetTitle("X0");
      surfaces_precise->GetXaxis()->SetTitleOffset(1.3);
      surfaces_precise->GetYaxis()->SetTitle("Y0");
      surfaces_precise->GetYaxis()->SetTitleOffset(1.4);
      surfaces_precise->GetZaxis()->SetTitle("R");
      surfaces_precise->GetZaxis()->SetTitleOffset(1.3);

      double x, y, LR, r, LRup, LRdown;
      std::vector<Cell> cs = this->active_cells();

      for(size_t ix=1; ix<= (size_t)surfaces_precise->GetNbinsX(); ix++){
	x = surfaces_precise->GetXaxis()->GetBinCenter(ix);
	for(size_t iy=1; iy<= (size_t)surfaces_precise->GetNbinsY(); iy++){
	  y = surfaces_precise->GetYaxis()->GetBinCenter(iy);
	  for(size_t ic=0; ic<cs.size(); ic++){
	    LR = cs[ic].legendre_R(x, y);
	    r = cs[ic].r().value();
	    LRup = LR + r;
	    LRdown = LR - r;
	    surfaces_precise->Fill(x, y, LRup);
	    surfaces_precise->Fill(x, y, LRdown);
	  }
	}
      }

      int ixmax, iymax, irmax;
      surfaces_precise->GetMaximumBin(ixmax,iymax,irmax);
      double X0 = surfaces_precise->GetXaxis()->GetBinCenter(ixmax);
      double Y0 = surfaces_precise->GetYaxis()->GetBinCenter(iymax);
      double R = surfaces_precise->GetZaxis()->GetBinCenter(irmax);

      double sigma_X0 = surfaces_precise->GetRMS(1);
      double sigma_Y0 = surfaces_precise->GetRMS(2);
      double sigma_R = surfaces_precise->GetRMS(3);

      h->set_center(experimental_point(experimental_double(X0,sigma_X0), experimental_double(Y0,sigma_Y0), experimental_double(0.,0.)));
      h->set_radius(experimental_double(R, sigma_R));

      cout << " X0 " << h->center().x().value() << " +- " << h->center().x().error() << endl;
      cout << " Y0 " << h->center().y().value() << " +- " << h->center().y().error() << endl;
      cout << " R " << h->radius().value() << " +- " << h->radius().error() << endl;

      if( draw ){
	TPolyMarker3D *t = new TPolyMarker3D(1, 8);
	t->SetMarkerColor(kRed);
	t->SetPoint(0,X0,Y0,R);
    
	surfaces_precise->Draw("");
	t->Draw("same");
      }else{
	surfaces_precise->Clear();
	delete surfaces_precise;
      }


    }

    void Detector::assign_reco_points_based_on_circle(Circle h){

      double distance;
      double angle;
      double x, y;

      std::vector<Cell> cs = this->cells();
      for(size_t i=0; i<cs.size(); i++){
	if( cs[i].on() ){
	  distance = cs[i].distance(h);
	  angle = atan2(cs[i].ep().y().value() - h.center().y().value(), cs[i].ep().x().value() - h.center().x().value());
	  x = cs[i].ep().x().value() + distance*cos(angle);
	  y = cs[i].ep().y().value() + distance*sin(angle);
      
	  cells_[i].set_p(experimental_point(experimental_double(x,0.), experimental_double(y,0.), experimental_double(0.,0.)));
	}
      }

    }

    void Detector::fill_residual(TH1F* h){

      double truepos, recopos, residual;

      std::vector<Cell> cs = this->active_cells();
      for(size_t i=0; i<cs.size(); i++){
	truepos = cs[i].p_true().x().value();
	recopos = cs[i].p_reco().x().value();
	residual = recopos - truepos;
	h->Fill(residual/this->cell_size());

	truepos = cs[i].p_true().y().value();
	recopos = cs[i].p_reco().y().value();
	residual = recopos - truepos;
	h->Fill(residual/this->cell_size());
      }

    }

    void Detector::fill_residual_circle(TH1F* x0, TH1F* y0, TH1F* r, TH1F* px0, TH1F* py0, TH1F* pr, Circle htrue, Circle hreco){

      double truepos, recopos, residual, pull;

      truepos=htrue.center().x().value();
      recopos=hreco.center().x().value();
      residual = recopos - truepos;
      pull = residual / hreco.center().x().error();
      x0->Fill(residual);
      px0->Fill(residual);

      truepos=htrue.center().y().value();
      recopos=hreco.center().y().value();
      residual = recopos - truepos;
      pull = residual / hreco.center().y().error();
      y0->Fill(residual);
      py0->Fill(residual);

      truepos=htrue.radius().value();
      recopos=hreco.radius().value();
      residual = recopos - truepos;
      pull = residual / hreco.radius().error();
      r->Fill(residual);
      pr->Fill(residual);

    }

    void Detector::reset(){

      std::vector<Cell> cs = this->cells();
      for(size_t i=0; i<cs.size(); i++)
	cells_[i].reset();

    }
  }
}
