// TK headers
#include "tkrec/TKcluster.h"

// Standard library:
#include <limits> // infinity
#include <cmath> // isnan

#include <datatools/exception.h>

ClassImp(tkrec::TKcluster);

namespace tkrec {

  using namespace std;

  TKcluster::TKcluster(const std::vector<TKtrhitHdl> & tr_hits,
		       double _phi_min,
		       double _phi_max)
    : TKcluster()
  {
    DT_THROW_IF(tr_hits.size() < 1, std::logic_error, "No tracker hits");
    side = tr_hits.at(0)->get_SRL('s');
    phi_min = _phi_min;
    phi_max = _phi_max;
    cluster_tr_hits = tr_hits;
    // for (auto i = 0u; i < tr_hits.size(); i++)
    //   {
    // 	cluster_tr_hits.push_back( tr_hits.at(i) );
    //   }
    return;
  }

  void TKcluster::add_tr_hit(TKtrhitHdl tracker_hit)
  {
    cluster_tr_hits.push_back( tracker_hit );
    return;
  }

  std::vector<TKtrhitHdl> & TKcluster::get_tr_hits()
  {
    return cluster_tr_hits;
  }

  std::vector<ConstTKtrhitHdl> TKcluster::get_tr_hits() const
  {
    std::vector<ConstTKtrhitHdl> thits;
    for (const auto & th : cluster_tr_hits) {
      thits.push_back(th);
    }
    return thits;
  }

  void TKcluster::set_track(const TKtrackHdl & _track)
  {
    track = _track;
    return;
  }

  bool TKcluster::has_track() const
  {
    return track.get() != nullptr;
  }

  TKtrackHdl & TKcluster::get_track()
  {
    return track;
  }

  ConstTKtrackHdl TKcluster::get_track() const
  {
    return track;
  }

  void TKcluster::set_side(double _side)
  {
    side = _side;
    return;
  }

  void TKcluster::set_phi_min(double _phi_min)
  {
    phi_min = _phi_min;
    return;
  }

  void TKcluster::set_phi_max(double _phi_max)
  {
    phi_max = _phi_max;
    return;
  }

  int TKcluster::get_side() const
  {
    return side;
  }

  double TKcluster::get_phi_min() const
  {
    return phi_min;
  }

  double TKcluster::get_phi_max() const
  {
    return phi_max;
  }

  int TKcluster::get_ambiguity_type() const
  {
    return ambiguity_type;
  }

  void TKcluster::detect_ambiguity_type()
  {
    int ambiguity = 0;
    double x0 = cluster_tr_hits[0]->get_xy('x');
    double y0 = cluster_tr_hits[0]->get_xy('y');
    bool ambiguous;
	
    // type 1 == mirror image along line x = x0 
    ambiguous = true;
    for (auto i = 1u; i < cluster_tr_hits.size(); i++)
      {	
	if( x0 != cluster_tr_hits[i]->get_xy('x') )
	  {
	    ambiguous = false;
	    break;
	  }
      }
    if( ambiguous == true )
      {
	ambiguity = 1;
      }
	
    // type 2 == mirror image along line y = y0 
    ambiguous = true;
    for(auto i = 1u; i < cluster_tr_hits.size(); i++)
      {	
	if( y0 != cluster_tr_hits[i]->get_xy('y') )
	  {
	    ambiguous = false;
	    break;
	  }
      }
    if( ambiguous == true )
      {
	ambiguity = 2;
      }
	
    // type 3 == mirror image along line y = x + (y0-x0) 
    ambiguous = true;
    for(auto i = 1u; i < cluster_tr_hits.size(); i++)
      {	
	if( y0-cluster_tr_hits[i]->get_xy('y') != x0-cluster_tr_hits[i]->get_xy('x') )
	  {
	    ambiguous = false;
	    break;
	  }
      }
    if( ambiguous == true )
      {
	ambiguity = 3;
      }
	
    // type 4 == mirror image along line y = -x + (y0-x0) 
    ambiguous = true;
    for(auto i = 1u; i < cluster_tr_hits.size(); i++)
      {	
	if( y0-cluster_tr_hits[i]->get_xy('y') != cluster_tr_hits[i]->get_xy('x')-x0 )
	  {
	    ambiguous = false;
	    break;
	  }
      }
    if( ambiguous == true )
      {
	ambiguity = 4;
      }

    ambiguity_type = ambiguity;
    return;
  }

  void TKcluster::draw_ML_vertical(int run_number, int event_number)
  {
    double S = 0.0;
    double Sx = 0.0;
    double Sy = 0.0;
    double Sz = 0.0;
    double Sxy = 0.0;
    double Sxz = 0.0;
    double Syz = 0.0;
    double Sxx = 0.0;
    double Syy = 0.0;
    double x,y,z, sigma_Z, const_Z;

    for(auto i = 0u; i < cluster_tr_hits.size(); i++)
      {
	z = cluster_tr_hits[i]->get_h();
	if(z != 0.0)
	  {	
	    x = cluster_tr_hits[i]->get_xy('x');
	    y = cluster_tr_hits[i]->get_xy('y');
	    sigma_Z = cluster_tr_hits[i]->get_sigma_Z();
	    const_Z = pow(sigma_Z, -2.0);
			
	    S = S + const_Z;
	    Sx = Sx + (const_Z*x);
	    Sy = Sy + (const_Z*y);
	    Sz = Sz + (const_Z*z);
	    Sxy = Sxy + (const_Z*x*y);
	    Sxz = Sxz + (const_Z*x*z);
	    Syz = Syz + (const_Z*y*z);
	    Sxx = Sxx + (const_Z*x*x);
	    Syy = Syy + (const_Z*y*y);
	  }
      }
	
		
    const int no_bins = 360;
    double phi;
    double denominator;
    TH1F* hist_chi_squared_Z = new TH1F("chi_squared_Z", "chi_squared_Z", no_bins, 0.0, 2.0*M_PI );
    TH1F* hist_quality_Z = new TH1F("quality_Z", "quality_Z", no_bins, 0.0, 2.0*M_PI );
    TH1F* hist_ML_vertical = new TH1F("ML_vertical", "ML_vertical", no_bins, 0.0, 2.0*M_PI );
    for(auto i = 0u; i < no_bins; i++)
      {
	phi = (i+0.5)*2.0*M_PI/no_bins;
	denominator = (Sy*Sy-S*Syy)*sin(phi)*sin(phi) + 2.0*(Sx*Sy-S*Sxy)*sin(phi)*cos(phi) + (Sx*Sx-S*Sxx)*cos(phi)*cos(phi);
	if( denominator != 0.0)
	  {
	    double tan_theta = ((Sy*Sz-S*Syz)*sin(phi) + (Sx*Sz-S*Sxz)*cos(phi)) / denominator;
	    double h = Sz / S - (Sx*cos(phi) + Sy*sin(phi)) * tan_theta / S;
			
	    double sum = 0.0;
	    int used_hits = 0;
	    double delta_Z; 
	    for(auto j = 0u; j < cluster_tr_hits.size(); j++)
	      {
		z = cluster_tr_hits[j]->get_h();
		if(z != 0.0)
		  {	
		    used_hits++;
		    x = cluster_tr_hits[j]->get_xy('x');
		    y = cluster_tr_hits[j]->get_xy('y');
		    sigma_Z = cluster_tr_hits[j]->get_sigma_Z();
					
		    delta_Z = h + tan_theta*(y*sin(phi) + x*cos(phi)) - z; 
		    sum = sum + pow(delta_Z/sigma_Z, 2.0);
		  }		
	      }
	    if(used_hits > 0)
	      {
			
		double chi_squared_Z = sum/double(used_hits);
		double quality_Z = exp( -0.5*chi_squared_Z );
				
		double con = 1.0; 
		for(auto j = 0u; j < cluster_tr_hits.size(); j++)
		  {
		    z = cluster_tr_hits[j]->get_h();
		    if(z != 0.0)
		      {	
			con = con / (sqrt(2.0*M_PI) * cluster_tr_hits[j]->get_sigma_Z());
		      }
		  }
				
		double likelihood_Z = con * pow(quality_Z, used_hits);
				
		hist_chi_squared_Z->Fill(phi, chi_squared_Z);
		hist_quality_Z->Fill(phi, quality_Z);
		hist_ML_vertical->Fill(phi, likelihood_Z);
	      }		
	  }	
      }
    TCanvas* canvas = new TCanvas("vertical");
    hist_chi_squared_Z->Draw("HIST");
    canvas->SaveAs(Form("Events_visu/chi_squared_Z_run-%d_event-%d_side-%d.png", run_number, event_number, side));
    delete hist_chi_squared_Z;
    hist_quality_Z->Draw("HIST");
    canvas->SaveAs(Form("Events_visu/quality_Z_run-%d_event-%d_side-%d.png", run_number, event_number, side));
    delete hist_quality_Z;
    hist_ML_vertical->Draw("HIST");
    canvas->SaveAs(Form("Events_visu/ML_vertical_run-%d_event-%d_side-%d.png", run_number, event_number, side));
    delete hist_ML_vertical;
	
    canvas->Close();
    return;
  }


  void TKcluster::reconstruct_ambiguity()
  {
    if(track == nullptr) return;
    if( ambiguity_type != 0 )
      {
	double a0 = track->get_a();
	double b0 = track->get_b();
	double x0 = cluster_tr_hits[0]->get_xy('x');
	double y0 = cluster_tr_hits[0]->get_xy('y');
	double a(0.0), b(0.0);
		
	switch(ambiguity_type)
	  {
	  case 1:
	    a = -a0;
	    b = 2.0*a0*x0 + b0;
	    break;
	  case 2:
	    a = -a0;
	    b = 2.0*y0 - b0;
	    break;
	  case 3:
	    a = 1.0/a0;
	    b = y0-x0 - b0/a0 + (y0-x0)/a0;
	    break;
	  case 4:
	    a = 1.0/a0;
	    b = y0+x0 + b0/a0 - (y0+x0)/a0;
	    break;
	  }
		
	auto mirror_image = std::make_shared<TKtrack>(side, a, b, 0.0, 0.0);
	track->set_ambiguity_type( ambiguity_type );
	track->link_mirror_image( mirror_image );
	mirror_image->reconstruct_vertical_MLM();
	
      }
    return;
  }


  void TKcluster::reconstruct_MLM(bool save_sinograms, int run_number, int event_number)
  {
    gROOT->SetBatch(kTRUE);
    // resolution of first iteration is always 0.01rad x 1mm
    const int resolution_phi = (phi_max - phi_min) * 100.0;
    const int resolution_R = 70;
    const int iterations = 2;
	
    double S_r = 0.0;
    double S_rX = 0.0;
    double S_rY = 0.0;
    double const_R;
    for(const auto & hit : cluster_tr_hits)
      {
	const_R = pow(hit->get_sigma_R(), -2.0);
	S_r = S_r + const_R;
	S_rX = S_rX + hit->get_xy('x')*const_R;
	S_rY = S_rY + hit->get_xy('y')*const_R;
      }
	
    double phi1 = phi_min;
    double phi2 = phi_max;
    double R1 = -35.0;
    double R2 = 35.0;
	
    double peak_Phi = std::numeric_limits<double>::quiet_NaN();
    double peak_R = std::numeric_limits<double>::quiet_NaN();
	
    double minimum = std::numeric_limits<double>::quiet_NaN();
    for(int iter = 0; iter < iterations; iter++)
      {	
	minimum = std::numeric_limits<double>::infinity(); // 1e100;
	double delta_phi = phi2 - phi1;
	double delta_R = R2 - R1;
		
	double offset_phi = delta_phi / (2.0 * resolution_phi);
	double offset_R = delta_R / (2.0 * resolution_R);
		
	TH2F *chi_squared = new TH2F("chi_squared", "chi_squared; phi[rad]; r[mm]", resolution_phi, phi1 + offset_phi, phi2 + offset_phi, resolution_R, R1 + offset_R, R2 + offset_R);	
				 
	// double weight; // unused
	double sin_phi, cos_phi;
	double R0;
	double rho;
	double shift;
	double diff_R;
		
	double phi = phi1;
	double r;	
	for(int j = 1; j <= resolution_phi; j++)
	  {
	    phi = phi + (delta_phi / double(resolution_phi));
	    sin_phi = sin(phi);
	    cos_phi = cos(phi);
			
	    // shifting origin to "center of mass" of hits			
	    shift = (S_rX/S_r)*sin_phi - (S_rY/S_r)*cos_phi;
	    for(int k = 1; k <= resolution_R; k++)
	      {
		r = R1 + k*(delta_R / double(resolution_R));
		r = r + shift;
		double diff_R_sum = 0.0;
		for(auto hit = 0u; hit < cluster_tr_hits.size(); hit++)
		  {
		    rho = cluster_tr_hits[hit]->get_xy('x')*sin_phi - cluster_tr_hits[hit]->get_xy('y')*cos_phi;
		    R0 = cluster_tr_hits[hit]->get_r();
		    diff_R = pow( (abs(r-rho) - R0)/cluster_tr_hits[hit]->get_sigma_R(), 2 );
		    diff_R_sum = diff_R_sum + diff_R;
		  }
								
		chi_squared->SetBinContent(j, k, diff_R_sum );
	      }	
	  }
		
	for(int i = 1; i <= resolution_phi; i++)
	  {
	    for(int j = 1; j <= resolution_R; j++)
	      {
		if(minimum > chi_squared->GetBinContent(i,j))
		  {
		    minimum = chi_squared->GetBinContent(i,j);
		    peak_Phi = i;
		    peak_R = j;
		  }
	      }
	  }
		
	if(std::isnan(minimum)) //  == 1e100) 
	  {
	    delete chi_squared;			
	    break;
	  }
		
	if( save_sinograms == true ) 
	  {
	    TCanvas* c2 = new TCanvas("chi_squared","chi_squared", 1000, 1000);
	    c2->SetRightMargin(0.15);
	    chi_squared->SetStats(0);
	    chi_squared->SetContour(100);
	    chi_squared->Draw("COLZ");
	    //gStyle->SetPalette(62);
			
	    // Hough transform of anode wires
	    if( 1 )
	      {
		for(auto hit = 0u; hit < cluster_tr_hits.size(); hit++)
		  {
		    double xi = cluster_tr_hits[hit]->get_xy('x');
		    double yi = cluster_tr_hits[hit]->get_xy('y');
		    auto function = new TF1("function", "[0]*sin(x)-[1]*cos(x)", phi1, phi2);
		    function->SetParameter(0, xi-(S_rX/S_r));
		    function->SetParameter(1, yi-(S_rY/S_r));
		    function->SetLineWidth(2);
		    function->Draw("Same");
		  }
	      }
			
	    c2->SaveAs(Form("Events_visu/chi_squared-run-%d_event-%d_side-%d_zoom-%d.png", run_number, event_number, side, iter));
	    c2->Close();
	  }
	delete chi_squared;
		
	peak_Phi = phi1 + delta_phi * (peak_Phi-0.5) / double(resolution_phi);
	peak_R = R1 + delta_R * (peak_R-0.5) / double(resolution_R);
		
	phi1 = peak_Phi - 0.05*delta_phi;
	phi2 = peak_Phi + 0.05*delta_phi;
	R1 = peak_R - 0.05*delta_R;
	R2 = peak_R + 0.05*delta_R;

	peak_R = peak_R + (S_rX/S_r)*sin(peak_Phi) - (S_rY/S_r)*cos(peak_Phi);
      }
			
    // if (minimum != 1e100)
    if(not std::isnan(minimum))
      {	
	/*
	  if(peak_Phi < 0.0)
	  {
	  peak_Phi = peak_Phi + M_PI;
	  peak_R = -peak_R;
	  }*/
		
	track = std::make_shared<TKtrack>(side, peak_Phi, peak_R);
				
	double a = track->get_a();
	double b = track->get_b();

	// associating tracker hits to track
	double association_distance;
	double denominator = sqrt((a*a) + 1);	
	double distance_from_wire;
	for(auto i = 0u; i < cluster_tr_hits.size(); i++)
	  {
	    const auto & clTrHit = cluster_tr_hits[i];
	    distance_from_wire = abs(clTrHit->get_xy('y') - a*clTrHit->get_xy('x') - b) / denominator;
	    association_distance = 3.0 * clTrHit->get_sigma_R();
	    if( abs(distance_from_wire - clTrHit->get_r()) < association_distance )
	      {
		track->add_associated_tr_hit(clTrHit);
		cluster_tr_hits[i]->set_associated_track(track);
	      }
	  }		
		
	// calculating evuluation factors
	double chi_squared = minimum/double(cluster_tr_hits.size());
		
	double norm_const = pow( 2.0*M_PI, -1.0*double(cluster_tr_hits.size())/2.0 ); 
	for(auto i = 0u; i < cluster_tr_hits.size(); i++)
	  {
	    norm_const = norm_const / cluster_tr_hits[i]->get_sigma_R();
	  }
		
	track->set_chi_squared_R( chi_squared );
	track->set_quality_R( exp(-0.5*chi_squared) );		
	track->set_likelihood_R( norm_const * exp(-0.5*minimum) );
			
	//track->reconstruct_vertical_least_square();
	track->reconstruct_vertical_MLM();
      }
    return;
  }

  void TKcluster::reconstruct_MLM_3D(bool save_sinograms, int run_number, int event_number)
  {
    gROOT->SetBatch(kTRUE);
    // resolution is 2*10^(-iterations)mm and C*10^(-iterations)degrees where term C differs for every event (aproximately 2) 
    const int resolution_phi = (phi_max - phi_min) * 100.0;
    const int resolution_R = 35;
    const int iterations = 3;

    // calculation data sums
    double R = 0.0;
    double Rx = 0.0;
    double Ry = 0.0;
    double Z = 0.0;
    double Zx = 0.0;
    double Zy = 0.0;
    double Zz = 0.0;
    double Zxy = 0.0;
    double Zxz = 0.0;
    double Zyz = 0.0;
    double Zxx = 0.0;
    double Zyy = 0.0;
    double Zzz = 0.0;
    double x,y,z;
    double const_R, const_Z;
    auto n = cluster_tr_hits.size();
    int no_Z = 0;
    for(auto i = 0u; i < n; i++)
      {
	x = cluster_tr_hits[i]->get_xy('x');
	y = cluster_tr_hits[i]->get_xy('y');
	z = cluster_tr_hits[i]->get_h();
		
	const_R = pow(cluster_tr_hits[i]->get_sigma_R(), -2.0);
	
	R = R + const_R;
	Rx = Rx + x*const_R;
	Ry = Ry + y*const_R;
	
	if(z != 0.0)
	  {	
	    const_Z = pow(cluster_tr_hits[i]->get_sigma_Z(), -2.0);

	    no_Z++;
	    Z = Z + const_Z;
	    Zx = Zx + x*const_Z;
	    Zy = Zy + y*const_Z;
	    Zz = Zz + z*const_Z;
	    Zxy = Zxy + x*y*const_Z;
	    Zxz = Zxz + x*z*const_Z;
	    Zyz = Zyz + y*z*const_Z;
	    Zxx = Zxx + x*x*const_Z;
	    Zyy = Zyy + y*y*const_Z;
	    Zzz = Zzz + z*z*const_Z;
	  }
      }

    // initial histogram boundaries
    double phi1 = phi_min;
    double phi2 = phi_max;
    double R1 = -35.0;
    double R2 = 35.0;
	
    double peak_Phi = std::numeric_limits<double>::quiet_NaN();
    double peak_R = std::numeric_limits<double>::quiet_NaN();
	
    double minimum = std::numeric_limits<double>::infinity();
    for(int iter = 0; iter < iterations; iter++)
      {	
	// minimum = 1e100;
	minimum = std::numeric_limits<double>::infinity();
	double delta_phi = phi2 - phi1;
	double delta_R = R2 - R1;
		
	double offset_phi = delta_phi/(2.0*resolution_phi);
	double offset_R = delta_R/(2.0*resolution_R);
		
	TH2F *chi_squared = new TH2F("chi_squared_3D", "chi_squared_3D; phi[rad]; r[mm]", resolution_phi, phi1 + offset_phi, phi2 + offset_phi, resolution_R, R1 + offset_R, R2 + offset_R);	
				 
	double sin_phi, cos_phi;
	double shift;
	double denominator;
		
	double phi = phi1;
	for(int j = 1; j <= resolution_phi; j++)
	  {
	    phi = phi + (delta_phi / double(resolution_phi));
	    sin_phi = sin(phi);
	    cos_phi = cos(phi);
			
	    // calculating chi-squared of vertical component
	    double diff_Z_sum = 0.0;
	    denominator = (Zy*Zy-Z*Zyy)*sin_phi*sin_phi + 2.0*(Zx*Zy-Z*Zxy)*sin_phi*cos_phi + (Zx*Zx-Z*Zxx)*cos_phi*cos_phi;
	    if( denominator != 0.0 )
	      {
		// calculation parameters theta0 and h0 from maximum likelihood 
		double tan_theta0 = ((Zy*Zz-Z*Zyz)*sin_phi + (Zx*Zz-Z*Zxz)*cos_phi) / denominator;
		double h0 = Zz / Z - (Zx*cos_phi + Zy*sin_phi) * tan_theta0 / Z;
				
		diff_Z_sum = tan_theta0*tan_theta0 * (Zxx*cos_phi*cos_phi + 2.0*Zxy*sin_phi*cos_phi + Zyy*sin_phi*sin_phi);
		diff_Z_sum = diff_Z_sum + 2.0*tan_theta0*(h0*(Zx*cos_phi + Zy*sin_phi) - Zxz*cos_phi - Zyz*sin_phi);
		diff_Z_sum = diff_Z_sum + h0*h0*Z - 2.0*h0*Zz + Zzz;
	      }
			
	    /*
	    // alternative computing of diff_Z_sum
	    double diff_Z; 
	    for(int hit = 0; hit < n; hit++)
	    {	
	    z = cluster_tr_hits[hit]->get_h();
	    if(z != 0.0)
	    {	
	    x = cluster_tr_hits[hit]->get_xy('x');
	    y = cluster_tr_hits[hit]->get_xy('y');
					
	    diff_Z = h0 + tan_theta0*(y*sin_phi + x*cos_phi) - z; 
	    diff_Z = diff_Z * diff_Z;
	    diff_Z_sum = diff_Z_sum + diff_Z;
	    }		
	    }
	    */
			
	    // shifting origin to "center of mass" of hits			
	    shift = (Rx/R)*sin_phi - (Ry/R)*cos_phi;
	    double r = R1 + shift;	
	    for(int k = 1; k <= resolution_R; k++)
	      {
		r = r + (delta_R / double(resolution_R));
		
		double R0;
		double rho;
		double diff_R_sum = 0.0;
		for(auto hit = 0u; hit < n; hit++)
		  {
		    x = cluster_tr_hits[hit]->get_xy('x');
		    y = cluster_tr_hits[hit]->get_xy('y');
		    R0 = cluster_tr_hits[hit]->get_r();
					
		    rho = x*sin_phi - y*cos_phi;
	
		    diff_R_sum = diff_R_sum + pow( (abs(r-rho) - R0)/cluster_tr_hits[hit]->get_sigma_R(), 2 );
		  }
				
		chi_squared->SetBinContent(j, k, diff_R_sum + diff_Z_sum);
	      }	
	  }
		
	for(int i = 1; i <= resolution_phi; i++)
	  {
	    for(int j = 1; j <= resolution_R; j++)
	      {
		if(minimum > chi_squared->GetBinContent(i,j))
		  {
		    minimum = chi_squared->GetBinContent(i,j);
		    peak_Phi = i;
		    peak_R = j;
		  }
	      }
	  }
		
	// if (minimum >= 1e100) 
	if(std::isnan(minimum))
	  {
	    delete chi_squared;		
	    return;
	  }
		
	if( save_sinograms == true ) 
	  {
	    TCanvas* c2 = new TCanvas("chi_squared_3D","chi_squared_3D", 1000, 1000);
	    c2->SetRightMargin(0.15);
	    chi_squared->SetStats(0);
	    chi_squared->SetContour(100);
	    chi_squared->Draw("COLZ");
	    //gStyle->SetPalette(62);
			
	    // Hough transform of anode wires
	    if( 1 )
	      {
		for(auto hit = 0u; hit < n; hit++)
		  {
		    double xi = cluster_tr_hits[hit]->get_xy('x');
		    double yi = cluster_tr_hits[hit]->get_xy('y');
		    auto function = new TF1("function", "[0]*sin(x)-[1]*cos(x)", phi1, phi2);
		    function->SetParameter(0, xi-(Rx/R));
		    function->SetParameter(1, yi-(Ry/R));
		    function->SetLineWidth(2);
		    function->Draw("Same");
		  }
	      }
			
	    c2->SaveAs(Form("Events_visu/chi_squared_3D-run-%d_event-%d_side-%d_zoom-%d.png", run_number, event_number, side, iter));
	    c2->Close();
	  }
				
	delete chi_squared;
		
	peak_Phi = phi1 + delta_phi * (peak_Phi-0.5) / double(resolution_phi);
	peak_R = R1 + delta_R * (peak_R-0.5) / double(resolution_R);
		
	phi1 = peak_Phi - 0.05*delta_phi;
	phi2 = peak_Phi + 0.05*delta_phi;
	R1 = peak_R - 0.05*delta_R;
	R2 = peak_R + 0.05*delta_R;

	peak_R = peak_R + (Rx/R)*sin(peak_Phi) - (Ry/R)*cos(peak_Phi);
      }
		
    /*
      if(peak_Phi < 0.0)
      {
      peak_Phi = peak_Phi + M_PI;
      peak_R = -peak_R;
      }*/
	
    track = std::make_shared<TKtrack>(side, peak_Phi, peak_R);
			
    double a = track->get_a();
    double b = track->get_b();

    double denominator = sqrt((a*a) + 1);	
    for(auto i = 0u; i < n; i++)
      {
	double association_distance = 3.0*cluster_tr_hits[i]->get_sigma_R();
	double distance_from_wire = abs(cluster_tr_hits[i]->get_xy('y') - a*cluster_tr_hits[i]->get_xy('x') - b) / denominator;
	if( abs(distance_from_wire - cluster_tr_hits[i]->get_r()) < association_distance )
	  {
	    track->add_associated_tr_hit(cluster_tr_hits[i]);
	    cluster_tr_hits[i]->set_associated_track(track);
	  }
      }			
	
    // calculating evuluation factors
    double no_of_measurements = n + no_Z;
    double chi_squared = minimum / no_of_measurements;
	
    track->set_chi_squared( chi_squared );
    track->set_quality( exp(-0.5*chi_squared) );
		
    // TODO: is this compromising accuracy of norm_const?
    double norm_const = pow( 2.0*M_PI, -no_of_measurements/2.0 ); 
    for(auto i = 0u; i < cluster_tr_hits.size(); i++)
      {
	norm_const = norm_const / cluster_tr_hits[i]->get_sigma_R();
	if(cluster_tr_hits[i]->get_h() != 0.0)
	  {
	    norm_const = norm_const / cluster_tr_hits[i]->get_sigma_Z();
	  }
      }
	
    track->set_likelihood( norm_const * exp(-0.5*minimum) );
		
    //track->reconstruct_vertical_least_square();
    track->reconstruct_vertical_MLM();
    return;
  }


  void TKcluster::print(std::ostream & out_) const
  {
    out_ << "Cluster | side "       << side
	 << ", size "       << cluster_tr_hits.size()
	 << ", phi_min: " << phi_min 
	 << ", phi_max: "   << phi_max 
	 << ", ambiguity type: " << ambiguity_type << std::endl;
    return;
  }

} //  end of namespace tkrec
