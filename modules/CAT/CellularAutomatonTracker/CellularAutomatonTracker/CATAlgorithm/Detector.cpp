
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

    void Detector::set_cells(std::vector<Cell> a){
	cells_.clear();
	for(std::vector<Cell>::const_iterator ic=a.begin(); ic!=a.end(); ++ic)
	  cells_[ic->id()] = *ic;
    }

    std::vector<Cell> Detector::cells(){
      std::vector<Cell> cs;
      for(std::map<size_t, Cell>::const_iterator ic=cells_.begin(); ic!=cells_.end(); ++ic)
	cs.push_back(ic->second);
      return cs;
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
	  cells_[c.id()] = c;
	}

      }


    }

    void Detector::draw()
    {

      for(std::map<size_t, Cell>::iterator ic=cells_.begin(); ic!=cells_.end(); ++ic)
	ic->second.draw();
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
      for(std::map<size_t, Cell>::iterator ic=cells_.begin(); ic!=cells_.end(); ++ic){
	distance = ic->second.distance(h);
	if( fabs(distance) < cell_size_/2. ){
	  value = true;
	  angle = atan2(ic->second.ep().y().value() - h.center().y().value(), ic->second.ep().x().value() - h.center().x().value());
	  x = ic->second.ep().x().value() + distance*cos(angle);
	  y = ic->second.ep().y().value() + distance*sin(angle);

	  factor=random->Gaus(0,sigma)*this->cell_size();
	  digitized_distance = fabs(distance)+factor;
	  if( digitized_distance < 0. ) digitized_distance = 0.;

	  ic->second.set_r(digitized_distance);
	  ic->second.set_er(sigma*this->cell_size());
	}
      }

      return value;

    }

    std::vector<Cell> Detector::cells(int itrack){

      std::vector<Cell> cs;
      for(std::map<size_t, Cell>::iterator ic=cells_.begin(); ic!=cells_.end(); ++ic)
	if( ic->second.track_id() == itrack)
	  cs.push_back(ic->second);

      return cs;

    }

    double Detector::average_x(){

      std::vector<Cell> cs = this->cells();
      double average=0.;
      for(size_t i=0; i<cs.size(); i++)
	average += cs[i].ep().x().value();
      average /= cs.size();
      return average;

    }

    double Detector::average_y(){

      std::vector<Cell> cs = this->cells();
      double average=0.;
      for(size_t i=0; i<cs.size(); i++)
	average += cs[i].ep().y().value();
      average /= cs.size();
      return average;

    }

    void Detector::draw_surfaces_rough(Circle *h, bool draw, std::vector<Cell> cs, int ievent){

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
	  for(std::vector<Cell>::const_iterator ic=cs.begin(); ic!=cs.end(); ++ic){
	    surfaces_rough->Fill(x, y, ic->legendre_R(x, y));
	  }
	}
      }

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
	delete surfaces_rough;
      }


    }

    void Detector::draw_surfaces_precise(Circle rough, Circle *h, bool draw, std::vector<Cell> cs, int ievent){

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

      for(size_t ix=1; ix<= (size_t)surfaces_precise->GetNbinsX(); ix++){
	x = surfaces_precise->GetXaxis()->GetBinCenter(ix);
	for(size_t iy=1; iy<= (size_t)surfaces_precise->GetNbinsY(); iy++){
	  y = surfaces_precise->GetYaxis()->GetBinCenter(iy);
	  for(std::vector<Cell>::const_iterator ic=cs.begin(); ic!=cs.end(); ++ic){
	    LR = ic->legendre_R(x, y);
	    r = ic->r().value();
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

      /*
      cout << " X0 " << h->center().x().value() << " +- " << h->center().x().error() << endl;
      cout << " Y0 " << h->center().y().value() << " +- " << h->center().y().error() << endl;
      cout << " R " << h->radius().value() << " +- " << h->radius().error() << endl;
      */

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

    void Detector::assign_reco_points_based_on_circle(std::vector<Cell> cluster, Circle h, size_t itrack){

      double distance;
      double angle;
      double x, y;
      size_t id;

      leftover_cells_.clear();

      for(std::vector<Cell>::iterator ic=cluster.begin(); ic!=cluster.end(); ++ic){
	distance = ic->distance(h);
	if( distance < this->cell_size() ){
	  angle = atan2(ic->ep().y().value() - h.center().y().value(), ic->ep().x().value() - h.center().x().value());
	  x = ic->ep().x().value() + distance*cos(angle);
	  y = ic->ep().y().value() + distance*sin(angle);
	  
	  cells_[ic->id()].set_p_reco(experimental_point(experimental_double(x,0.), experimental_double(y,0.), experimental_double(0.,0.)));
	  cells_[ic->id()].set_track_id(itrack);
	}else{
	  leftover_cells_.push_back(cells_[ic->id()]);
	}
      }

    }

    void Detector::reset(){

      for(std::map<size_t, Cell>::iterator ic=cells_.begin(); ic!=cells_.end(); ++ic)
	ic->second.reset();

    }
  }
}
