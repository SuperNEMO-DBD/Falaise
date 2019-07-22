
#include "Riostream.h"
#include "TROOT.h"
#include "TFile.h"
#include "TNetFile.h"
#include "TRandom.h"
#include "TTree.h"
#include "TBranch.h"
#include "TClonesArray.h"
#include "TStopwatch.h"
#include "TCanvas.h"
#include "TH1F.h"
#include "TStyle.h"
#include "TMinuit.h"

#include <stdlib.h>
#include <stdio.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <ctime>
#include <time.h>

#include "Cell.h"
#include "Circle.h"
#include "Detector.h"


using namespace std;

Detector detector;
bool use_radius = false;

void fcn(Int_t &npar, Double_t *gin, Double_t &f, Double_t *par, Int_t iflag);

//______________________________________________________________________________
int main()
{

  // initial parameters
  double sigma = 0.03;
  size_t n_true_tracks = 1;
  size_t n_events = 1000;
  bool draw = false;
  double detector_x0=0.;
  double detector_y0=0.;
  double detector_Lx=100.;
  double detector_Ly=100.;
  double detector_cell_size=10.;


  TCanvas c("c","c",800,600);
  gROOT->Reset();
  gROOT->SetStyle("Plain");
  gStyle->SetPalette(1,0);
  gStyle->SetOptStat(0);
  gStyle->SetOptTitle(0);
  TH1F empty("empty","",1,detector_x0-detector_Lx, detector_x0+detector_Lx);
  empty.GetYaxis()->SetRangeUser(detector_y0-detector_Ly, detector_y0+detector_Ly);


  detector.set_x0(detector_x0);
  detector.set_y0(detector_y0);
  detector.set_Lx(detector_Lx);
  detector.set_Ly(detector_Ly);
  detector.set_cell_size(detector_cell_size);
  detector.build_cells();

  TFile of("tracker.root","RECREATE");
  TH1F h_residual_rough("h_residual_rough",";reco - true (units of cell size);n",100,-1.,1.);
  h_residual_rough.SetLineColor(kBlack);
  TH1F h_residual_precise("h_residual_precise",";reco - true (units of cell size);n",100,-1.,1.);
  h_residual_precise.SetLineColor(kRed);
  TH1F h_residual_x0("h_residual_x0",";reco - true (units of cell size);n",100,-2.*detector.cell_size(), 2.*detector.cell_size());
  h_residual_x0.SetLineColor(kBlack);
  TH1F h_residual_y0("h_residual_y0",";reco - true (units of cell size);n",100,-2.*detector.cell_size(), 2.*detector.cell_size());
  h_residual_y0.SetLineColor(kBlack);
  TH1F h_residual_r("h_residual_r",";reco - true (units of cell size);n",100,-2.*detector.cell_size(), 2.*detector.cell_size());
  h_residual_r.SetLineColor(kBlack);
  TH1F h_pull_x0("h_pull_x0",";#frac{reco - true}{#sigma};n",100,-2.,2.);
  h_pull_x0.SetLineColor(kBlack);
  TH1F h_pull_y0("h_pull_y0",";#frac{reco - true}{#sigma};n",100,-2.,2.);
  h_pull_y0.SetLineColor(kBlack);
  TH1F h_pull_r("h_pull_r",";#frac{reco - true}{#sigma};n",100,-2.,2.);
  h_pull_r.SetLineColor(kBlack);

  /*
  // initialize TMinuit with parameters
  size_t npoints = detector.n_active_cells();
  int n_parameters;
  if( use_radius )
    n_parameters =npoints + 3;
  else
    n_parameters = 3;
  TMinuit *gMinuit = new TMinuit(n_parameters);
  //  select verbose level:
  //    default :     (58 lines in this test)
  //    -1 : minimum  ( 4 lines in this test)
  //     0 : low      (31 lines)
  //     1 : medium   (61 lines)
  //     2 : high     (89 lines)
  //     3 : maximum (199 lines in this test)

  gMinuit->SetPrintLevel(3);
  gMinuit->SetFCN(fcn);
  Double_t arglist[10];
  Int_t ierflg = 0;

  arglist[0] = 1;
  gMinuit->mnexcm("SET ERR", arglist ,1,ierflg);

  // Set starting values and step sizes for parameters


  std::time_t seed = std::time(0);  // t is an integer type
  TRandom *random = new TRandom(seed);
  double factor;
  double diagonal = sqrt(pow(detector.Lx(),2) + pow(detector.Ly(),2));

  Cell local_cell;
  vector<Cell> active_cells = detector.active_cells();
  if( use_radius ){
    for(size_t i=0; i<npoints; i++){
      //gMinuit->mnparm(i, Form("phi_%d", (int)i), 0,0.1, -M_PI, M_PI,ierflg);
      local_cell = active_cells[i];
      factor = random->Gaus(1,0.1);
      gMinuit->mnparm(i, Form("phi_%d", (int)i), factor*atan2(local_cell.y_true() - local_cell.y0(), local_cell.x_true() - local_cell.x0()),0.1, -M_PI, M_PI,ierflg);
    }
    //gMinuit->mnparm(npoints, "x0", detector.x0(),detector.cell_size(), 0,0,ierflg);
    //gMinuit->mnparm(npoints+1, "y0", detector.y0(),detector.cell_size(), 0,0,ierflg);
    factor = random->Gaus(1,0.1);
    gMinuit->mnparm(npoints, "x0", factor*h_true.x0(), detector.cell_size(), 0,0,ierflg);
    factor = random->Gaus(1,0.1);
    gMinuit->mnparm(npoints+1, "y0", factor*h_true.y0(),detector.cell_size(), 0,0,ierflg);
    //gMinuit->mnparm(npoints+2, "R", diagonal,diagonal/100., 0., 2.*diagonal,ierflg);
    factor = random->Gaus(1,0.1);
    gMinuit->mnparm(npoints+2, "R", factor*h_true.r(),h_true.r()/100., 0., 2.*diagonal,ierflg);
  }else{
    factor = random->Gaus(1,0.5);
    gMinuit->mnparm(0, "x0", detector.average_x(), detector.cell_size(), 0,0,ierflg);
    //gMinuit->mnparm(0, "x0", factor*h_true.x0(), detector.cell_size(), 0,0,ierflg);
    factor = random->Gaus(1,0.5);
    gMinuit->mnparm(1, "y0", detector.average_y(),detector.cell_size(), 0,0,ierflg);
    //gMinuit->mnparm(1, "y0", factor*h_true.y0(),detector.cell_size(), 0,0,ierflg);
    factor = random->Gaus(1,0.5);
    gMinuit->mnparm(2, "R", diagonal,diagonal/10., 0., 2.*diagonal,ierflg);
    //gMinuit->mnparm(2, "R", factor*h_true.r(),h_true.r()/100., 0., 2.*diagonal,ierflg);
  }


  // Now ready for minimization step
  arglist[0] = 5000; // n of calls
  arglist[1] = 1.; // tolerance
  gMinuit->mnexcm("MIGRAD", arglist ,2,ierflg);

  // Print results
  Double_t amin,edm,errdef;
  Int_t nvpar,nparx,icstat;
  gMinuit->mnstat(amin,edm,errdef,nvpar,nparx,icstat);
  gMinuit->mnprin(3,amin);

  std::cout << "\nPrint results from minuit\n";
  std::vector<double> cell_angle;
  std::vector<double> cell_angle_error;
  double fitted_x0, fitted_x0_error;
  double fitted_y0, fitted_y0_error;
  double fitted_r, fitted_r_error;
  if( use_radius ){
    for(size_t i=0; i<npoints; i++){
      double ca=0.;
      double ca_error=0.;
      gMinuit->GetParameter(i,ca,ca_error);
      cell_angle.push_back(ca);
      cell_angle_error.push_back(ca_error);
    }
    gMinuit->GetParameter(npoints,fitted_x0, fitted_x0_error);
    gMinuit->GetParameter(npoints+1,fitted_y0, fitted_y0_error);
    gMinuit->GetParameter(npoints+2,fitted_r, fitted_r_error);
  }else{
    gMinuit->GetParameter(0,fitted_x0, fitted_x0_error);
    gMinuit->GetParameter(1,fitted_y0, fitted_y0_error);
    gMinuit->GetParameter(2,fitted_r, fitted_r_error);
  }

  if( use_radius ){
    for(size_t i=0; i<npoints; i++){
      cout << " angle " << i << " = " << cell_angle[i] << " +- " << cell_angle_error[i] << " true " << active_cells[i].true_phi()  << endl;
    }
  }
  cout << " x0 = " << fitted_x0 << " +- " << fitted_x0_error << " true " << h_true.x0() << endl;
  cout << " y0 = " << fitted_y0 << " +- " << fitted_y0_error << " true " << h_true.y0() << endl;
  cout << " r = " << fitted_r << " +- " << fitted_r_error << " true " << h_true.r() << endl;
  h_reco.set_x0(fitted_x0);
  h_reco.set_y0(fitted_y0);
  h_reco.set_r(fitted_r);

  */


  std::vector<Circle> hs_true;
  Circle h_true;
  std::vector<Circle> hs_reco_rough;
  Circle h_reco_rough;
  std::vector<Circle> hs_reco_precise;
  Circle h_reco_precise;


  size_t count_events = 0;
  size_t count_true_tracks;
  bool generated_track;
  std::vector<Cell> cells_to_reconstruct;
  while( count_events < n_events ){
    count_true_tracks = 0;
    cells_to_reconstruct.clear();
    hs_reco_rough.clear();
    hs_reco_precise.clear();
    hs_true.clear();
    
    cout << " event " << count_events << endl;

    while(count_true_tracks < n_true_tracks ){

      generated_track = false;
      while( !generated_track ){
	h_true.build_random(detector.x0(),detector.y0(),detector.Lx(),detector.Ly());
	generated_track = detector.detect(h_true,sigma);
      }
      hs_true.push_back(h_true);
      count_true_tracks ++;
    }

    cells_to_reconstruct = detector.active_cells();

    while( cells_to_reconstruct.size() > 3 ){
      detector.draw_surfaces_rough(&h_reco_rough, draw, cells_to_reconstruct,count_events);
      if( draw )
	c.Print("surfaces_rough.eps");

      detector.assign_reco_points_based_on_circle(h_reco_rough);
      detector.fill_residual(&h_residual_rough);

      detector.draw_surfaces_precise(h_reco_rough, &h_reco_precise, draw,count_events);
      if( draw )
	c.Print("surfaces_precise.eps");
      
      detector.assign_reco_points_based_on_circle(h_reco_precise);
      detector.fill_residual(&h_residual_precise);
      detector.fill_residual_circle(&h_residual_x0, &h_residual_y0, &h_residual_r, &h_pull_x0, &h_pull_y0, &h_pull_r, h_true, h_reco_precise);
      
      hs_reco_rough.push_back(h_reco_rough);
      hs_reco_precise.push_back(h_reco_precise);
      cells_to_reconstruct = detector.leftover_cells();
      cout << " reco track with " << cells_to_reconstruct.size() << " leftovers ";
      for(size_t il=0; il<cells_to_reconstruct.size(); il++) cout << " (" << cells_to_reconstruct[il].x0() << ", " << cells_to_reconstruct[il].y0() << ")";
      cout << endl;
    }

    if( draw ){
      empty.Draw();
      /*
      for(size_t it=0; it<hs_true.size(); it++){
	cout << " draw true track " << it << " x0 " << hs_true[it].x0() << " y0 " << hs_true[it].y0() << " r " <<  hs_true[it].r() << endl;
	hs_true[it].draw(kRed);
      }
      */
      for(size_t ir=0; ir<hs_reco_rough.size(); ir++){
	//hs_reco_rough[ir].draw(kBlack);
	hs_reco_precise[ir].draw(kBlue);
      }
      //  empty.Draw("same");
      detector.draw();
    }


    detector.reset();
    count_events ++;

  }

  h_residual_rough.Write();
  h_residual_precise.Write();
  h_residual_x0.Write();
  h_residual_y0.Write();
  h_residual_r.Write();
  h_pull_x0.Write();
  h_pull_y0.Write();
  h_pull_r.Write();

  of.Close();

  c.Print("detector.eps");

  /*
  double xmin = 2.*(detector.x0() - detector.Lx());
  double xmax = 2.*(detector.x0() + detector.Lx());
  double ymin = 2.*(detector.y0() - detector.Ly());
  double ymax = 2.*(detector.y0() + detector.Ly());

  for(size_t i=0; i<active_cells.size(); i++){
    active_cells[i].draw_surface(xmin,xmax,ymin,ymax);
    c.Print(Form("surface_%d.eps", (int)i));
  }
  */

  exit(0);
}


void fcn(Int_t &npar, Double_t *gin, Double_t &f, Double_t *par, Int_t iflag){
  //calculate chisquare
  Double_t chi2 = 0.;
  Double_t delta_x=0.;
  Double_t delta_y=0.;
  Double_t phi, xcell, ycell, xcircle, ycircle;
  Double_t errorx = 1.;
  Double_t errory = 1.;
  std::vector<Cell> cells = detector.cells();
  size_t npoints = cells.size();
  for (size_t i=0;i<npoints; i++) {
    if( cells[i].on() ){
      if( use_radius ){
	xcell = cells[i].x0() + cells[i].r()*cos(par[i]);
	ycell = cells[i].y0() + cells[i].r()*sin(par[i]);
	
	phi = atan2(ycell - par[npoints+1], xcell - par[npoints]);
	
	xcircle = par[npoints] + par[npoints+2]*cos(phi);
	ycircle = par[npoints+1] + par[npoints+2]*sin(phi);
	
      }else{
	xcell = cells[i].x0();
	ycell = cells[i].y0();
	
	phi = atan2(ycell - par[1], xcell - par[0]);
	
	xcircle = par[0] + par[2]*cos(phi);
	ycircle = par[1] + par[2]*sin(phi);

	errorx = cells[i].r();
	errory = cells[i].r();
	
      }
      delta_x  = xcell - xcircle;
      delta_y  = ycell - ycircle;
      chi2 += pow(delta_x/errorx,2) + pow(delta_y/errory,2);
    }
  }
  f = chi2;
}

