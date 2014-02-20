
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

      x0_=0.;
      y0_=0.;
      Lx_=0.;
      Ly_=0.;
      cell_size_=0;
      InnerRadius_ =0.;
      OuterRadius_ =0.;
      FoilRadius_ =0.;
      nofflayers_ =0;

      SuperNemo_ =false;
      SuperNemoChannel_ =false;

      cells_.clear();
      leftover_cells_.clear();

      surfaces_rough = NULL;
      surfaces_precise = NULL;

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
      // return active cells in the detector

      std::vector<Cell> cs;
      for(std::map<size_t, Cell>::const_iterator ic=cells_.begin(); ic!=cells_.end(); ++ic)
        cs.push_back(ic->second);
      return cs;
    }

    std::vector<Cell> Detector::cells(int itrack){
      // return active cells associated with track itrack

      std::vector<Cell> cs;
      for(std::map<size_t, Cell>::iterator ic=cells_.begin(); ic!=cells_.end(); ++ic)
        if( ic->second.track_id() == itrack)
          cs.push_back(ic->second);

      return cs;

    }

    void Detector::setup_histograms(){

      double xmin = 2.*(this->x0() - this->Lx());
      double xmax = 2.*(this->x0() + this->Lx());
      double ymin = 2.*(this->y0() - this->Ly());
      double ymax = 2.*(this->y0() + this->Ly());
      double rmin = 0.;
      double rmax = 2.*sqrt(pow(this->Lx(),2) + pow(this->Ly(),2));

      if( surfaces_rough )
        delete surfaces_rough;

      surfaces_rough = new TH3D("surfaces_rough","surfaces_rough",
                                     (int)((xmax - xmin)/this->cell_size()), xmin, xmax,
                                     (int)((ymax - ymin)/this->cell_size()), ymin, ymax,
                                     (int)((rmax - rmin)/this->cell_size()), rmin, rmax);
      surfaces_rough->GetXaxis()->SetTitle("X0");
      surfaces_rough->GetXaxis()->SetTitleOffset(1.3);
      surfaces_rough->GetYaxis()->SetTitle("Y0");
      surfaces_rough->GetYaxis()->SetTitleOffset(1.4);
      surfaces_rough->GetZaxis()->SetTitle("R");
      surfaces_rough->GetZaxis()->SetTitleOffset(1.3);

      int nbins = 100;
      if( surfaces_precise )
        delete surfaces_precise;

      surfaces_precise= new TH3D("surfaces_precise","surfaces_precise",
                                 nbins,xmin,xmax,
                                 nbins,ymin,ymax,
                                 nbins,rmin,rmax);
      surfaces_precise->GetXaxis()->SetTitle("X0");
      surfaces_precise->GetXaxis()->SetTitleOffset(1.3);
      surfaces_precise->GetYaxis()->SetTitle("Y0");
      surfaces_precise->GetYaxis()->SetTitleOffset(1.4);
      surfaces_precise->GetZaxis()->SetTitle("R");
      surfaces_precise->GetZaxis()->SetTitleOffset(1.3);
    }

    void Detector::fill_surfaces_rough(std::vector<Cell>* cs){
      // fill the rough histogram with all the legendre surfaces
      // belonging to each cell in cs

      clock.start(" Detector: fill_surfaces_rough ", "cumulative");

      m.message(" build rough surfaces for ", cs->size(), " cells ", mybhep::VERBOSE);

      double x, y;

      // reset histograms (option "M" to reset minimum and maximum)
      surfaces_rough->Reset("M");

      // loop over (X, Y) bins
      for(size_t ix=1; ix<= (size_t)surfaces_rough->GetNbinsX(); ix++){
        x = surfaces_rough->GetXaxis()->GetBinCenter(ix);
        for(size_t iy=1; iy<= (size_t)surfaces_rough->GetNbinsY(); iy++){
          y = surfaces_rough->GetYaxis()->GetBinCenter(iy);

          // loop over cells in cs
          for(std::vector<Cell>::const_iterator ic=cs->begin(); ic!=cs->end(); ++ic){
            surfaces_rough->Fill(x, y, ic->legendre_R(x, y));
          }
        }
      }

      clock.stop(" Detector: fill_surfaces_rough ");

    }


    void Detector::fill_surfaces_precise(Circle *h, std::vector<Cell>* cs){
      // fill the precise histogram with all the legendre surfaces
      // belonging to each cell in cs

      clock.start(" Detector: fill_surfaces_precise ", "cumulative");

      m.message(" build precise surfaces for ", cs->size(), " cells ", mybhep::VERBOSE);

      // reset histograms (option "M" to reset minimum and maximum)
      surfaces_precise->Reset("M");

      int ncells = 3;
      int nbins = 100;
      double xmin = h->center().x().value() - ncells*this->cell_size();
      double xmax = h->center().x().value() + ncells*this->cell_size();
      double ymin = h->center().y().value() - ncells*this->cell_size();
      double ymax = h->center().y().value() + ncells*this->cell_size();
      double rmin = h->radius().value() - ncells*this->cell_size();
      double rmax = h->radius().value() + ncells*this->cell_size();

      surfaces_precise->GetXaxis()->Set(nbins,xmin,xmax);
      surfaces_precise->GetYaxis()->Set(nbins,ymin,ymax);
      surfaces_precise->GetZaxis()->Set(nbins,rmin,rmax);

      double x, y, LR, r;

      // loop over (X, Y) bins
      for(size_t ix=1; ix<= (size_t)surfaces_precise->GetNbinsX(); ix++){
        x = surfaces_precise->GetXaxis()->GetBinCenter(ix);
        for(size_t iy=1; iy<= (size_t)surfaces_precise->GetNbinsY(); iy++){
          y = surfaces_precise->GetYaxis()->GetBinCenter(iy);

          // loop over cells in cs
          for(std::vector<Cell>::const_iterator ic=cs->begin(); ic!=cs->end(); ++ic){
            LR = ic->legendre_R(x, y);
            r = ic->r().value();
            surfaces_precise->Fill(x, y, LR + r);
            surfaces_precise->Fill(x, y, LR - r);
          }
        }
      }


      clock.stop(" Detector: fill_surfaces_precise ");

    }

    void Detector::set_messenger(mybhep::prlevel l){
      level = l;
      m = mybhep::messenger(level);
    }

    void Detector::draw_surfaces(Circle *h, std::vector<Cell>* cs, size_t itrack){

      clock.start(" Detector: draw_surfaces ", "cumulative");

      double X0, Y0, R, sigma_X0, sigma_Y0, sigma_R;
      int ixmax_rough, iymax_rough, irmax_rough, ixmax_precise, iymax_precise, irmax_precise;
      //int nbins = 100;
      double nentries;

      std::vector<Cell> assigned_cells;
      std::vector<Cell> leftover_cells;
      bool found_good_circle = false;
      bool first = true;


      // fill rough histogram
      fill_surfaces_rough(cs);

      // keep looking until a good circle is found
      while (!found_good_circle ){

        if( !first ){
          // remove dominant peak from histogram
          surfaces_rough->SetBinContent(ixmax_rough,iymax_rough,irmax_rough,0);
        }

        // find dominant peak in rough histogram
        surfaces_rough->GetMaximumBin(ixmax_rough,iymax_rough,irmax_rough);

        nentries = surfaces_rough->GetBinContent(ixmax_rough,iymax_rough,irmax_rough);
        if( nentries < 3 ){
          // less than 3 cells cannot define a circle
          leftover_cells.clear();
          assigned_cells.clear();
          break;
        }

        // get circle parameters
        X0 = surfaces_rough->GetXaxis()->GetBinCenter(ixmax_rough);
        Y0 = surfaces_rough->GetYaxis()->GetBinCenter(iymax_rough);
        R = surfaces_rough->GetZaxis()->GetBinCenter(irmax_rough);

        m.message(" first try ", first, " rough intersection: (", X0, ", ", Y0, ", ", R, ") entries ", nentries, mybhep::VERBOSE);


        // assign parameters to circle h
        h->set_center(experimental_point(experimental_double(X0,0.), experimental_double(Y0,0.), experimental_double(0.,0.)));
        h->set_radius(experimental_double(R, 0.));


        // fill precise histogram
        fill_surfaces_precise(h, cs);

        // find dominant peak in precise histogram
        surfaces_precise->GetMaximumBin(ixmax_precise,iymax_precise,irmax_precise);

        // get circle parameters
        nentries = surfaces_precise->GetBinContent(ixmax_precise,iymax_precise,irmax_precise);
        X0 = surfaces_precise->GetXaxis()->GetBinCenter(ixmax_precise);
        Y0 = surfaces_precise->GetYaxis()->GetBinCenter(iymax_precise);
        R = surfaces_precise->GetZaxis()->GetBinCenter(irmax_precise);

        // get circle parameters errors
        sigma_X0 = surfaces_precise->GetRMS(1);
        sigma_Y0 = surfaces_precise->GetRMS(2);
        sigma_R = surfaces_precise->GetRMS(3);


        // assign parameters to circle h
        h->set_center(experimental_point(experimental_double(X0,sigma_X0), experimental_double(Y0,sigma_Y0), experimental_double(0.,0.)));
        h->set_radius(experimental_double(R, sigma_R));


        m.message(" precise intersection: (", X0, ", ", Y0, ", ", R, ") entries ", nentries, mybhep::VERBOSE);

        // split points in cs into "leftover" and "assigned"
        // (the latter get a reconstructed point)
        assign_reco_points_based_on_circle(cs, h, &assigned_cells, &leftover_cells);

        // check if assigned cells form a contious track
        found_good_circle = continous(&assigned_cells);

        if( !found_good_circle){
          first = false;
        }

      }

      if( found_good_circle ){

        leftover_cells_ = leftover_cells;

        for(vector<Cell>::iterator ic = assigned_cells.begin(); ic != assigned_cells.end(); ++ic){
          cells_[ic->id()] = *ic;
          cells_[ic->id()].set_track_id(itrack);
        }

      }


      clock.stop(" Detector: draw_surfaces ");

    }


    bool Detector::continous(std::vector<Cell>* cs){
      // check if cells in cs form a continous strip

      clock.start(" Detector: continous ", "cumulative");

      size_t n_breaks = 0;
      size_t next_index;

      // order cells based on their angle wrt circle center
      std::sort(cs->begin(),cs->end(),Cell::circle_order);

      // loop over cells
      for(std::vector<Cell>::const_iterator ic=cs->begin(); ic!=cs->end(); ++ic){

        next_index = ic - cs->begin() + 1;
        if( next_index == cs->size() )
          next_index = 0;

        if( near_level(*ic, cs->at(next_index) ) == 0 )
          n_breaks ++;

      }

      bool ok = (n_breaks == 1 );

      m.message(" the ", cs->size(), " cells have ", n_breaks, " breaks, continous: ", ok, mybhep::VERBOSE);

      clock.stop(" Detector: continous ");

      return ok;
    }


  void Detector::assign_reco_points_based_on_circle(std::vector<Cell>* cluster, Circle *h, std::vector<Cell> * cs, std::vector<Cell> * ls){
    // calculate reconstructed points for cells in cluster based on circle h

      double distance;
      double angle;
      double x, y;
      Cell c;

      cs->clear();
      ls->clear();

      // loop over cells in cluster
      for(std::vector<Cell>::iterator ic=cluster->begin(); ic!=cluster->end(); ++ic){

        // distance of cell from helix
        distance = ic->distance(*h);

        // if the circle goes through the cell...
        if( distance < this->cell_size()/2. ){

          // angle of cell center wrt circle center
          angle = atan2(ic->ep().y().value() - h->center().y().value(), ic->ep().x().value() - h->center().x().value());

          // reconstructed point of cell
          x = ic->ep().x().value() + distance*cos(angle);
          y = ic->ep().y().value() + distance*sin(angle);

          // copy cell and assign reco point
          c = *ic;
          c.set_p_reco(experimental_point(experimental_double(x,0.), experimental_double(y,0.), experimental_double(0.,0.)));
          c.set_circle_phi(angle);
          cs->push_back(c);

        // if the circle does not go through the cell
        }else{
          ls->push_back(*ic);
        }
      }

    }

    void Detector::reset(){

      for(std::map<size_t, Cell>::iterator ic=cells_.begin(); ic!=cells_.end(); ++ic)
        ic->second.reset();

    }

    size_t Detector::near_level( const topology::cell & c1, const topology::cell & c2 ){

      // returns 0 for far-away cell
      // 1 for cells separated by nofflayers
      // 2 for diagonal cells
      // 3 for side-by-side cells

      // side-by-side connection: distance = 1
      // diagonal connection: distance = sqrt(2) = 1.41
      // skip 1 connection, side: distance = 2
      // skip 1 connection, tilt: distance = sqrt(5) = 2.24
      // skip 1 connection, diag: distance = 2 sqrt(2) = 2.83

      topology::experimental_double distance = topology::experimental_vector(c1.ep(),c2.ep()).hor().length();

      double limit_side;
      double limit_diagonal;
      if (SuperNemo() && SuperNemoChannel())
        {
          limit_side = this->cell_size();
          limit_diagonal = sqrt(2.)*this->cell_size();
        }
      else
        {
          double factor = cos(M_PI/8.); // 0.923879532511287 // octogonal factor = 0.92
          limit_side = factor*this->cell_size();
          limit_diagonal = sqrt(2.)*factor*this->cell_size(); // new factor = 1.31
        }
      double precision = 0.15*limit_side;


      if( fabs(distance.value() - limit_side) < precision )
        return 3;

      if( fabs(distance.value() - limit_diagonal) < precision )
        return 2;

      if( distance.value() < limit_diagonal*(1. + nofflayers()) )
        return 1;

      return 0;


    }

    void Detector::finalize(){
      clock.dump();
      if( surfaces_rough )
        delete surfaces_rough;
      if( surfaces_precise )
        delete surfaces_precise;
    }



  }
}
