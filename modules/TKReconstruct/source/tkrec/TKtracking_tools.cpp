// TK headers
#include "tkrec/TKEvent.h"

#include <boost/multi_array.hpp>

namespace tkrec {

  using namespace std;

  bool debug_mode = false;

  void TKEvent::reconstruct_track_from_hits(const std::vector<TKtrhitHdl>& hits,
					    const TKEventRecConfig & config_)
  {
    bool save_sinograms = config_.save_sinograms;
    // ancient algorithm - does not provide complete likelihood and (r,phi,theta,h) coordinates

    // finds only one track per side of the tracker seperately from given set of tracker hits
    // no uncertainties involved here
	
    // each iteration zooms into a region with 10% size in both directions
    const auto resolution = config_.track_from_hits_resolution;
    const auto iterations = config_.track_from_hits_iterations;

    for(int side = 0; side < 2; side++)
      {	
	vector<double> hits_x;
	vector<double> hits_y;
	vector<double> hits_z;
	vector<double> hits_r;
	vector<double> hits_sigma;	
	vector<int>    index;
		
	for(auto i = 0u; i < hits.size(); i++)
	  {
	    if( side == hits[i]->get_SRL('s'))
	      {
		// not using broken or (wrongly associated) tracker hits with too large radius
		if( hits[i]->get_r() != -1.0 && hits[i]->get_r() < 35.0 )
		  {
		    hits_x.push_back( hits[i]->get_xy('x') );
		    hits_y.push_back( hits[i]->get_xy('y') );
		    hits_z.push_back( hits[i]->get_h() );
		    hits_r.push_back( hits[i]->get_r() );
		    hits_sigma.push_back( hits[i]->get_sigma_R() );
		    index.push_back( i );
		  }
	      }
	  }

	if( hits_x.size() < 3 ) continue;

	// fitting top projections of tracks using Legender transform
	double r = std::numeric_limits<double>::quiet_NaN();
	double theta = std::numeric_limits<double>::quiet_NaN();
	double phi1 = 0.0;
	double phi2 = M_PI;
	double R1 = -2500;
	double R2 = 2500;
		
	double peak_Theta = std::numeric_limits<double>::quiet_NaN();
	double peak_R = std::numeric_limits<double>::quiet_NaN();
	double delta_phi = std::numeric_limits<double>::quiet_NaN();
	double delta_R = std::numeric_limits<double>::quiet_NaN();
		
	for(auto q = 0u; q < iterations; q++)
	  {	
	    double offset = (phi2 - phi1)/(2.0*resolution);
	    TH2F *sinograms = new TH2F("sinograms", "sinograms; theta; r", resolution, phi1+offset, phi2+offset, resolution, R1, R2);		
	    for(auto i = 0u; i < hits_x.size(); i++)
	      {
		for(auto k = 0u; k <= resolution; k++)
		  {
		    theta = phi1 + (k * (phi2 - phi1) / resolution);
		    r = hits_x[i]*sin(theta) - hits_y[i]*cos(theta);
		    sinograms->Fill( theta, r - hits_r[i] );
		    sinograms->Fill( theta, r + hits_r[i] );
		  }	
	      }
								
	    double maximum = 0.0;
	    for(auto i = 1u; i <= resolution; i++)
	      {
		for(auto j = 1u; j <= resolution; j++)
		  {
		    if(maximum < sinograms->GetBinContent(i,j))
		      {
			maximum = sinograms->GetBinContent(i,j);
			peak_Theta = i;
			peak_R = j;
		      }
		  }
	      }
			
	    delta_phi = phi2 - phi1;
	    delta_R = R2 - R1;
			
	    peak_Theta = phi1 + delta_phi * peak_Theta / double(resolution);
	    peak_R = R1 + delta_R * (peak_R-0.5) / double(resolution);
			
	    phi1 = peak_Theta - 0.05*delta_phi;
	    phi2 = peak_Theta + 0.05*delta_phi;
	    R1 = peak_R - 0.05*delta_R;
	    R2 = peak_R + 0.05*delta_R;

	    if( save_sinograms == true ) 
	      {
		TCanvas* c2 = new TCanvas("sinograms");
		sinograms->SetContour(100);
		sinograms->SetStats(0);
		sinograms->Draw("COLZ");
		c2->SaveAs(Form("Events_visu/sinograms-run-%d_event-%d_side-%d_zoom-%d.png", run_number, event_number, side, q));
		c2->Close();
	      }
	    delete sinograms;
	  }
		
	// creating track
	auto track = std::make_shared<TKtrack>(side, peak_Theta, peak_R);	
		
	double a = track->get_a();
	double b = track->get_b();
		
	// associating hits to track
	vector<bool> hits_associated;
	double denominator = sqrt((a*a) + 1);	
	for(auto i = 0u; i < hits_x.size(); i++)
	  {
	    double distance_from_wire = abs(hits_y[i] - a*hits_x[i] - b) / denominator;
	    double association_distance = 3.0 * hits_sigma[i];
	    if( abs(distance_from_wire - hits_r[i]) < association_distance )
	      {
		hits_associated.push_back(true);
		track->add_associated_tr_hit(hits[index[i]]);
		hits[index[i]]->set_associated_track(track);
	      }
	    else
	      {
		hits_associated.push_back(false);
	      }
		
	  }					
	
	// calculating likelihood_R
	double joined_probability = 1.0;
	double probability;
		
	double norm = 1.0 / sqrt(2.0*M_PI);
	for(auto i = 0u; i < hits.size();i++)
	  {	
			
	    probability = exp(-pow(abs( r + hits_x[i]*cos(theta) - hits_y[i]*sin(theta) ) - hits_r[i] , 2)/(2.0*hits_sigma[i]*hits_sigma[i]));
	    joined_probability = joined_probability * probability / hits_sigma[i]; 
	  }		
		
	joined_probability = pow(norm, hits.size()) * joined_probability;
	track->set_likelihood_R(joined_probability);
		
	// fitting z coordinates of hits
	vector<double> hits_p;
	for(auto i = 0u; i < hits_x.size(); i++)
	  {
	    double distance_from_wire = abs(hits_y[i] - a*hits_x[i] - b) / denominator;
	    double projection_distance = pow(hits_x[i], 2.0) + pow((hits_y[i] - b), 2.0) - pow(distance_from_wire, 2.0);
	    projection_distance = pow(projection_distance, 0.5);
			
	    hits_p.push_back(projection_distance);
	  } 
		
	int sum_n = 0;
	double sum_Z = 0.0;
	double sum_P = 0.0;
	double sum_ZP = 0.0;
	double sum_PP = 0.0;
	for(auto i = 0u; i < hits_x.size(); i++)
	  {
	    if(hits_z[i] != 0.0 && hits_associated[i] == true)
	      {
		sum_Z += hits_z[i];
		sum_P += hits_p[i];
		sum_ZP += hits_z[i]*hits_p[i];
		sum_PP += hits_p[i]*hits_p[i];
		sum_n++;		
	      }
	  }
		
	double c = 0.0;
	double d = 0.0;
	denominator = sum_n*sum_PP - sum_P*sum_P; 
	if(sum_n > 0 && denominator != 0.0)
	  {
	    c = (sum_n*sum_ZP - sum_P*sum_Z) / denominator;
	    c = c*(2.0*side-1.0)*pow(1+a*a, 0.5);
	    d = (sum_PP*sum_Z - sum_P*sum_ZP) / denominator;
	  }
		
	track->set_c(c);
	track->set_d(d);
		
	track->set_theta( atan(c/sqrt(a*a+1.0)) ); 
	track->set_h( d - a*b*c/(a*a+1.0) );	
		
	track->update_likelihood();
				
	tracks.push_back(track);
		
	hits_x.erase(hits_x.begin(), hits_x.end());
	hits_y.erase(hits_y.begin(), hits_y.end());
	hits_z.erase(hits_z.begin(), hits_z.end());
	hits_r.erase(hits_r.begin(), hits_r.end());
      }
  }

  void TKEvent::reconstruct_track(const TKEventRecConfig & config_)
  {
    reconstruct_track_from_hits(tr_hits, config_);
  }

  void TKEvent::reconstruct_multi(const TKEventRecConfig & config_)
  {
    bool save_sinograms = config_.save_sinograms;
    gROOT->SetBatch(kTRUE);

    // resolution of both dimensions of every 2D histogram	
    auto resolution = config_.multi_resolution; 
	
    // number of zooming iterations into histogram
    const auto iterations = config_.multi_iterations;
	
    // how high does a peak has to be in comparison to the highest one in order to be considered as a candidate as well
    const double threshold = config_.multi_threshold;
	
    // into how many segments (in both directions) is histogram divided during each iteration (except the final one)
    // new peak is calculated on each segment 
    const int start_no_segments = config_.multi_start_no_segments; 
	
    // both sides are dealt with separately
    for(int side = 0; side < 2; side++)
      {		
	// filtering good tracker hits for reconstruction (no missing time stamps...)
	vector<TKtrhitHdl> hits_from_side = filter_side(tr_hits, side); 
	vector<TKtrhitHdl> hits = filter_usable(hits_from_side);

	if( hits.size() < 3 ) continue;
		
	// peaks_Theta, peak_R and peaks_value stores information about peak candidates
	// each iteration takes those candidates zooms around them looks for more candidates in that region
	// new set of candidates (more precise ones) comes out from each iteration
	vector<double> peaks_Theta = {M_PI/2.0};
	vector<double> peaks_R = {0.0};
	vector<double> peaks_value = {0.0};
		
	// size of region (delta_phi x delta_R) on witch each peak is calculated more precisely
	double delta_phi = M_PI;
	double delta_R = 5000.0;
		
	int segments = start_no_segments;
	for(auto iter = 0u; iter < iterations; iter++)
	  {
	    // the performance if better with only one segment in final iteration (algorithm looks only for one peak) 
	    if( iter == (iterations - 1) )
	      {
		segments = 1;
	      }
			
	    // stores candidates during iteration
	    vector<double> peaks_Theta_temp;
	    vector<double> peaks_R_temp;
	    vector<double> peaks_value_temp;
			
	    for(auto i = 0u; i < peaks_R.size(); i++)
	      {		
		// XXX 
		// double segment_peaks[segments][segments][3]; // invalid
		// XXX
		// arrays stores information about candidates from each segment
		typedef boost::multi_array<double, 3> array_type;
		typedef array_type::index index;
		array_type segment_peaks(boost::extents[segments][segments][3]);
		// Assign values to the elements:
		for(index ia = 0; ia != segments; ++ia) 
		  for(index ja = 0; ja != segments; ++ja)
		    for(index ka = 0; ka != 3; ++ka)
		      segment_peaks[ia][ja][ka] = 0.0;

		// max_peak - stores value of the highest peak in all segments
		double max_peak = 0.0;
				
		// loop over segments
		for(int seg_r = 0; seg_r < segments; seg_r++)
		  {
		    for(int seg_theta = 0; seg_theta < segments; seg_theta++)
		      {
			// boundaries of segments
			double r_min = peaks_R[i] - (delta_R/2.0) + (seg_r * delta_R/double(segments));
			double r_max = peaks_R[i] - (delta_R/2.0) + ((seg_r+1) * delta_R/double(segments));
			double theta_min = peaks_Theta[i] - (delta_phi/2.0) + (seg_theta * delta_phi/double(segments));
			double theta_max = peaks_Theta[i] - (delta_phi/2.0) + ((seg_theta+1) * delta_phi/double(segments));
						
			// sinograms are calculated for each bin of theta range - (offset = 1/2 of bin widht) 
			double offset = (delta_phi/double(segments))/(2.0*resolution);
			TH2F *sinograms = new TH2F("sinograms", "sinograms; theta; r", resolution, theta_min + offset, theta_max + offset, resolution, r_min, r_max);								
						
			double theta;
			double r;			
			for(auto hit = 0u; hit < hits.size(); hit++)
			  {
			    double sigma = hits[hit]->get_sigma_R();
			    for(auto k = 0u; k <= resolution; k++)
			      {
				theta = theta_min + ( k * (delta_phi/double(segments)) / double(resolution) );
				// r - legendre transform of a center of a circle
				r = hits[hit]->get_xy('x')*sin(theta) - hits[hit]->get_xy('y')*cos(theta);
								
				double weight;
				for(int half = 0; half < 2; half++)
				  {	
				    // mu - legendre transform of half circle (+r/-r)
				    double mu = (r + (2.0*half - 1.0)*hits[hit]->get_r());	
									
				    // gauss is calculated only for -3 to 3 sigma region to cut time							
				    double r1 = mu - 3.0*sigma;
				    double r2 = mu + 3.0*sigma;
									
				    // bin numbers coresponding to r1 and r2 values
				    int bin1 = (double(resolution) * (r1-r_min) / (r_max-r_min)) + 1;
				    int bin2 = (double(resolution) * (r2-r_min) / (r_max-r_min)) + 1;
									
				    // real values of r coresponding to each bin 
				    double r_j1 = r_min + (r_max - r_min) * double(bin1) / double(resolution);
				    double r_j2;
				    for(int binj = bin1; binj < bin2 + 1; binj++)
				      {
					r_j2 = r_j1 + (r_max - r_min) / double(resolution);
										
					// integrated probability
					//weight = (delta_phi / double(resolution * segments)) * ( erf( (r_j2 - mu)/sqrt(2.0*sigma) ) - erf( (r_j1 - mu)/sqrt(2.0*sigma) ) ) / (4.0*sigma); 
										
					// average probability density in a bin given by gauss distribution with mean in mu 
					// (uniformly distributed with respect to theta)
					weight = ( erf( (r_j2 - mu)/(sqrt(2.0)*sigma) ) - erf( (r_j1 - mu)/(sqrt(2.0)*sigma) ) ) / (2.0 * delta_R / double(resolution * segments));
										
					// result is 2D histogram of several sinusoid functions f(theta) in convolution with gauss with respect to R
					sinograms->Fill( theta, (r_j2 + r_j1)/2.0, weight );
					r_j1 = r_j2;			
				      }								
				  }			
			      }	
			  }										
			// sinogram for given segment is complete - next step is looking for peaks 
						
			double maximum = 0.0;
			double peak_Theta = std::numeric_limits<double>::quiet_NaN();
			double peak_R = std::numeric_limits<double>::quiet_NaN();
			for(auto j = 1u; j <= resolution; j++)
			  {
			    for(auto k = 1u; k <= resolution; k++)
			      {
				if(maximum < sinograms->GetBinContent(j,k))
				  {
				    // information (value, bin_theta, bin_R) about peak in given segment
				    maximum = sinograms->GetBinContent(j,k);
				    peak_Theta = j;
				    peak_R = k;
				  }
			      }
			  }
						
			if( save_sinograms == true ) 
			  {
			    TCanvas* c2 = new TCanvas("sinograms");
			    sinograms->SetStats(0);
			    sinograms->SetContour(100);
			    sinograms->Draw("COLZ");
			    c2->SaveAs(Form("Events_visu/sinograms-run-%d_event-%d_side-%d_iter-%d_R-%d_Th-%d.png", run_number, event_number, side, iter, seg_r, seg_theta));
			    c2->Close();
			  }
			delete sinograms;
						
			// converting bin numbers to real values
			peak_Theta = peaks_Theta[i] - (delta_phi/2.0) + (seg_theta * delta_phi/double(segments)) + ( peak_Theta * (delta_phi/double(segments)) / double(resolution) );
			peak_R = peaks_R[i] - (delta_R/2.0) + (seg_r * delta_R/double(segments)) + (delta_R/double(segments)) * (peak_R-0.5) / double(resolution);
						
			segment_peaks[seg_r][seg_theta][0] = peak_R;
			segment_peaks[seg_r][seg_theta][1] = peak_Theta;
			segment_peaks[seg_r][seg_theta][2] = maximum;
			if(maximum > max_peak)
			  {
			    // finding out the value of the highest peak in all segments
			    max_peak = maximum;
			  }
		      }
		  }
				
		for(int seg_r = 0; seg_r < segments; seg_r++)
		  {
		    for(int seg_theta = 0; seg_theta < segments; seg_theta++)
		      {
			// filtering peak candidates obtained from region around given previous peak 
			if(segment_peaks[seg_r][seg_theta][2] >= threshold * max_peak /*&& segment_peaks[seg_r][seg_theta][2] > */)
			  {
			    peaks_R_temp.push_back(segment_peaks[seg_r][seg_theta][0]);
			    peaks_Theta_temp.push_back(segment_peaks[seg_r][seg_theta][1]);
			    peaks_value_temp.push_back(segment_peaks[seg_r][seg_theta][2]);
			  }
		      }
		  }			
	      }
			
	    peaks_R.clear();
	    peaks_Theta.clear();
	    peaks_value.clear();
			
	    // maximum value of all peaks 
	    double maximum = -std::numeric_limits<double>::infinity();
	    for(auto i = 0u; i < peaks_value_temp.size(); i++)
	      {
		if( peaks_value_temp[i] > maximum )
		  {
		    maximum = peaks_value_temp[i];
		  }
	      }
			
	    for(auto i = 0u; i < peaks_value_temp.size(); i++)
	      {
		if( peaks_value_temp[i] > threshold * maximum )
		  {	
		    // if two peaks are so close that they would be duplicit in next iteration one is removed
		    // otherwise we obtain multiple identical peaks
		    bool is_new = true;
		    for(auto j = 0u; j < peaks_R.size(); j++)
		      {
			if(abs(peaks_R_temp[i] - peaks_R[j]) < delta_R/(2.0*segments))
			  {
			    if(abs(peaks_Theta_temp[i] - peaks_Theta[j]) < delta_phi/(2.0*segments))
			      {
				is_new = false;
			      }
			  }
		      }
		    if(is_new)
		      {
			peaks_R.push_back(peaks_R_temp[i]);
			peaks_Theta.push_back(peaks_Theta_temp[i]);
			peaks_value.push_back(peaks_value_temp[i]);						
		      }
		  }
	      }
			
	    // temp vectors are cleared for next iteration
	    peaks_R_temp.clear();
	    peaks_Theta_temp.clear();
	    peaks_value_temp.clear();
			
	    // regions of delta and theta are scaled down
	    delta_phi = delta_phi/double(segments);
	    delta_R = delta_R/double(segments);

			
	    // last iteration - higher resolution to obtain more accuracy without getting more solutions
	    // (not ideal solution) 
	    if( iter == (iterations - 2) )
	      {
		resolution = resolution * 4;
	      }
	  }
	// end of finding peaks
	// next step - creating tracks
	
	for(auto j = 0u; j < peaks_R.size(); j++)
	  {
			
	    double theta = peaks_Theta[j];
	    double r     = peaks_R[j];
			
	    auto track = std::make_shared<TKtrack>(side, theta, r);	
			
	    double a = track->get_a();
	    double b = track->get_b();
			
	    // associating hits to track
	    double denominator = sqrt((a*a) + 1);	
	    for(auto i = 0u; i < hits.size(); i++)
	      {
		double distance_from_wire = abs(hits[i]->get_xy('y') - a*hits[i]->get_xy('x') - b) / denominator;
		double association_distance = 3.0*hits[i]->get_sigma_R();
		if( abs(distance_from_wire - hits[i]->get_r()) < association_distance )
		  {
		    track->add_associated_tr_hit(hits[i]);
		    hits[i]->set_associated_track(track);
		  }
	      }			
			
	    // calculating likelihood_R
	    double joined_probability = 1.0;
	    double probability;
	    double norm = 1.0 / sqrt(2.0*M_PI);
	    for(auto i = 0u; i < hits.size();i++)
	      {	
		double sigma = hits[i]->get_sigma_R();
		double association_distance = 3.0*sigma;
		if(abs( r + hits[i]->get_xy('x')*cos(theta) - hits[i]->get_xy('y')*sin(theta) ) < association_distance )
		  {
		    probability = norm*exp(-pow(abs( r + hits[i]->get_xy('x')*cos(theta) - hits[i]->get_xy('y')*sin(theta) ) - hits[i]->get_r() , 2)/(2.0*sigma*sigma));
		    joined_probability = joined_probability * probability / sigma; 
		  }
	      }		
			
	    track->set_likelihood_R(joined_probability);
	    track->reconstruct_vertical_least_square();
	    tracks.push_back(track);				
	  }
      }	
  }


  void TKEvent::reconstruct_single(const TKEventRecConfig & config_)
  {
    reconstruct_single_from_hits(tr_hits, config_);
  }

  void TKEvent::reconstruct_single_from_hits(const std::vector<TKtrhitHdl>& _hits,
					     const TKEventRecConfig & config_)
  {
    bool save_sinograms = config_.save_sinograms; 
    gROOT->SetBatch(kTRUE);

    // resolution of both dimensions of every 2D histogram	
    auto resolution = config_.single_resolution; 
	
    // number of zooming iterations into histogram
    const int iterations = config_.single_iterations;
	
    // both sides are dealt with separately
    for(int side = 0; side < 2; side++)
      {	
	// filtering good tracker hits for reconstruction (no missing time stamps...)
	vector<TKtrhitHdl> hits_from_side = filter_side(_hits, side); 
	vector<TKtrhitHdl> hits = filter_usable(hits_from_side);
	if( hits.size() < 3 ) continue;

	// peaks_Theta, peak_R and peaks_value stores information about peak candidate
	// each iteration takes those candidate and zooms around them
	double peak_Theta = M_PI/2.0;
	double peak_R = 0.0;
	// double peak_value = 0.0;
		
	// size of region (delta_phi x delta_R) on witch each peak is calculated more precisely
	double delta_phi = M_PI;
	double delta_R = 5000.0;
		
	for(int iter = 0; iter < iterations; iter++)
	  {
	    double r_min = peak_R - (delta_R/2.0);
	    double r_max = peak_R + (delta_R/2.0);
	    double theta_min = peak_Theta - (delta_phi/2.0);
	    double theta_max = peak_Theta + (delta_phi/2.0);
			
	    // sinograms are calculated for each bin of theta range - (offset = 1/2 of bin widht) 
	    double offset = (delta_phi)/(2.0*resolution);
	    TH2F *sinograms = new TH2F("sinograms", "sinograms; theta; r", resolution, theta_min + offset, theta_max + offset, resolution, r_min, r_max);								
			
	    double theta;
	    double r;			
	    for(auto hit = 0u; hit < hits.size(); hit++)
	      {
		const auto & hitH = hits[hit];
		double sigma = hitH->get_sigma_R();
		for(auto k = 0u; k <= resolution; k++)
		  {
		    theta = theta_min + ( k * delta_phi / double(resolution) );
		    // r - legendre transform of a center of a circle (Hough transform) - in old coordinates
		    //r = ( -hits[hit]->get_xy('x')*cos(theta) ) - ( -hits[hit]->get_xy('y')*sin(theta) );
		    // in new coordinates
		    r = hitH->get_xy('x')*sin(theta) - hitH->get_xy('y')*cos(theta);
					
		    // double weight;
		    for(int half = 0; half < 2; half++)
		      {	
			// mu - legendre transform of half circle (+r/-r)
			double mu = (r + (2.0*half - 1.0)*hitH->get_r());	
						
			// gauss is calculated only for -3 to 3 sigma region to cut time							
			double r1 = mu - 3.0*sigma;
			double r2 = mu + 3.0*sigma;
						
			// bin numbers coresponding to r1 and r2 values
			int bin1 = (double(resolution) * (r1-r_min) / (r_max-r_min)) + 1;
			int bin2 = (double(resolution) * (r2-r_min) / (r_max-r_min)) + 1;
						
			// real values of r coresponding to each bin 
			double r_j1 = r_min + (r_max - r_min) * double(bin1) / double(resolution);
			double r_j2;
			for(int binj = bin1; binj < bin2 + 1; binj++)
			  {
			    r_j2 = r_j1 + (r_max - r_min) / double(resolution);
							
			    // average probability density in a bin given by gauss distribution with mean in mu 
			    // (uniformly distributed with respect to theta)
			    double weight = ( erf( (r_j2 - mu)/(sqrt(2.0)*sigma) ) - erf( (r_j1 - mu)/(sqrt(2.0)*sigma) ) ) / (2.0 * delta_R / double(resolution));
							
			    // result is 2D histogram of several sinusoid functions f(theta) in convolution with gauss with respect to R
			    sinograms->Fill( theta, (r_j2 + r_j1)/2.0, weight );
			    r_j1 = r_j2;			
			  }								
		      }			
		  }	
	      }										

	    double maximum = -std::numeric_limits<double>::infinity();
	    int peak_Theta_bin = 0;
	    int peak_R_bin = 0;
	    for(auto j = 1u; j <= resolution; j++)
	      {
		for(auto k = 1u; k <= resolution; k++)
		  {
		    if(maximum < sinograms->GetBinContent(j,k))
		      {
			// information (value, bin_theta, bin_R) about peak in given segment
			maximum = sinograms->GetBinContent(j,k);
			peak_Theta_bin = j;
			peak_R_bin = k;
		      }
		  }
	      }
			
	    if( save_sinograms ) 
	      {
		TCanvas* c2 = new TCanvas("sinograms", "sinograms", 2000, 1600);
		sinograms->SetStats(0);
		sinograms->SetContour(100);
		sinograms->Draw("COLZ");
		c2->SaveAs(Form("Events_visu/sinograms-run-%d_event-%d_side-%d_iter-%d.png", run_number, event_number, side, iter));
		c2->Close();
	      }
	    delete sinograms;
			
	    // converting bin numbers to real values
	    peak_Theta = peak_Theta - (delta_phi/2.0) + (double(peak_Theta_bin) * (delta_phi) / double(resolution) );
	    peak_R = peak_R - (delta_R/2.0) + (delta_R) * (double(peak_R_bin) - 0.5) / double(resolution);
			
	    delta_phi = delta_phi*0.1;
	    delta_R = delta_R*0.1;
	  }
	
	// creating track
	double phi = peak_Theta;
	double r = peak_R;
	auto track = std::make_shared<TKtrack>(side, phi, r);	
		
	// double a = track->get_a();
	// double b = track->get_b();
	
	// associating hits to track
	// calculating chi_squared_R, quality_R and likelihood_R
	double sum_R = 0.0;
		
	for(auto i = 0u; i < hits.size(); i++)
	  {
	    const auto & hitH = hits[i]; 
	    double distance = abs( hitH->get_xy('x')*sin(phi) - hitH->get_xy('y')*cos(phi) - r );
	    distance = abs( distance - hitH->get_r() );
	    double sigma = hitH->get_sigma_R();
	    double association_distance = 3.0 * sigma;
	    if( distance < association_distance )
	      {
		track->add_associated_tr_hit(hitH);
		hitH->set_associated_track(track);
				
		sum_R = sum_R + distance/(sigma*sigma);
	      }
	  }	
		
	double no_hits = track->get_associated_tr_hits().size();
	double chi_squared = sum_R / no_hits;
	double quality = exp( -0.5*chi_squared );
		
	double norm = pow( 2.0*M_PI, -no_hits/2.0 ); 
	for(auto i = 0u; i < track->get_associated_tr_hits().size(); i++)
	  {
	    norm = norm / track->get_associated_tr_hits()[i]->get_sigma_R();
	  }
	double likelihood = norm * exp( -0.5*chi_squared*no_hits );
		
	track->set_chi_squared_R( chi_squared );	
	track->set_quality_R( quality );	
	track->set_likelihood_R( likelihood );	

	track->reconstruct_vertical_least_square();
	//track->reconstruct_vertical_MLM();
	tracks.push_back(track);				
      }
    return;
  }

  void TKEvent::reconstruct(const TKEventRecConfig & config_)
  {
    bool save_sinograms = config_.save_sinograms;
    // const bool debug_mode = config_.debug_mode;
    const double chi_square_threshold = config_.chi_square_threshold;
    for(int side = 0; side < 2; side++)
      {
	//if(debug_mode) std::clog << "side: " << side << endl;
	vector<TKtrhitHdl> hits = filter_side(tr_hits, side); 
	hits = filter_usable( hits );
	hits = filter_distant( hits );
	auto no_hits_before = 2034u;
	while( hits.size() > 2 && no_hits_before > hits.size() )
	  {
	    //if(debug_mode) std::clog << "iteration start: " << endl;
	    no_hits_before = hits.size();
	    bool failed = true;
	    TKclusterHdl cluster = find_cluster(hits);			
	    if( cluster.get() != nullptr )
	      {
		//if(debug_mode) std::clog << "find_cluster succes" << endl; 
		cluster->reconstruct_MLM( save_sinograms, run_number, event_number );						
		if( cluster->get_track()->get_chi_squared_R() < chi_square_threshold && cluster->get_track()->get_associated_tr_hits().size() > 3 )
		  {
		    //if(debug_mode) std::clog << "chi_squared good" << endl;
		    failed = false;
		    cluster->detect_ambiguity_type();
		    cluster->reconstruct_ambiguity(); 
		    cluster->get_track()->calculate_tr_hit_points();
		    if(cluster->get_track()->has_mirror_image())
		      {	
			cluster->get_track()->get_mirror_image()->calculate_tr_hit_points();
		      }
		    clusters.push_back( cluster );
		  }
		// else
		//   {
		//     //if(debug_mode) std::clog << "chi_squared bad" << endl; 
		//     delete cluster;
		//   }
	      }
	    if( failed )
	      {
		TKclusterHdl cluster2 = find_cluster_legendre(hits, config_);
		if( cluster2.get() != nullptr )
		  {
		    //if(debug_mode) std::clog << "find_cluster_legendre succes" << endl; 
		    cluster2->reconstruct_MLM( save_sinograms, run_number, event_number );
		    if( cluster2->get_track()->get_chi_squared_R() < chi_square_threshold
			&& cluster->get_track()->get_associated_tr_hits().size() > 3 )
		      {
			//if(debug_mode) std::clog << "chi_squared good" << endl; 
			cluster2->detect_ambiguity_type();
			cluster2->reconstruct_ambiguity();
			cluster2->get_track()->calculate_tr_hit_points();
			if(cluster2->get_track()->has_mirror_image())
			  {	
			    cluster2->get_track()->get_mirror_image()->calculate_tr_hit_points();
			  }
			clusters.push_back( cluster2 );
		      }	
		    // else
		    //   {
		    // 	//if(debug_mode) std::clog << "chi_squared bad" << endl; 
		    // 	delete cluster;
		    //   }
		  }
	      }
	    hits = filter_unassociated( hits );
	    hits = filter_distant( hits );
	    //hits = filter_unclustered( hits );
	  }
      }
    this->build_trajectories();
    //this->extrapolate_trajectories();
    return;
  }

  void TKEvent::reconstruct_simple(const TKEventRecConfig & config_)
  {
    bool save_sinograms = config_.save_sinograms;
    //const bool debug_mode = config_.debug_mode;
    const double chi_square_threshold = config_.single_threshold;
    for(int side = 0; side < 2; side++)
      {
	//if(debug_mode) std::clog << "side: " << side << endl;
	vector<TKtrhitHdl> hits = filter_side(tr_hits, side); 
	hits = filter_usable( hits );
	hits = filter_distant( hits );
	if( hits.size() < 3 ) continue; 
	bool failed = true;
	TKclusterHdl cluster = find_cluster(hits);			
	if( cluster.get() != nullptr )
	  {
	    //if(debug_mode) std::clog << "find_cluster succes" << endl; 
	    cluster->reconstruct_MLM( save_sinograms, run_number, event_number );						
	    if( cluster->get_track()->get_chi_squared_R() < chi_square_threshold && cluster->get_track()->get_associated_tr_hits().size() > 2 )
	      {
		failed = false;
		//if(debug_mode) std::clog << "chi_squared good" << endl;
		cluster->detect_ambiguity_type();
		cluster->reconstruct_ambiguity(); 
		cluster->get_track()->calculate_tr_hit_points();
		if(cluster->get_track()->has_mirror_image())
		  {	
		    cluster->get_track()->get_mirror_image()->calculate_tr_hit_points();
		  }
		clusters.push_back( cluster );
	      }
	    // else
	    //   {
	    // 	//if(debug_mode) std::clog << "chi_squared bad" << endl; 
	    // 	delete cluster;
	    //   }
	  }
	if( failed )
	  {
	    TKclusterHdl cluster2 = find_cluster_legendre(hits, config_);
	    if( cluster2.get() != nullptr )
	      {
		//if(debug_mode) std::clog << "find_cluster_legendre succes" << endl; 
		cluster2->reconstruct_MLM( save_sinograms, run_number, event_number );
		if( cluster2->get_track()->get_chi_squared_R() < chi_square_threshold && cluster2->get_track()->get_associated_tr_hits().size() > 2 )
		  {
		    //if(debug_mode) std::clog << "chi_squared good" << endl; 
		    cluster2->detect_ambiguity_type();
		    cluster2->reconstruct_ambiguity();
		    cluster2->get_track()->calculate_tr_hit_points();
		    if(cluster2->get_track()->has_mirror_image())
		      {	
			cluster2->get_track()->get_mirror_image()->calculate_tr_hit_points();
		      }
		    clusters.push_back( cluster2 );
		  }	
		// else
		//   {
		//     //if(debug_mode) std::clog << "chi_squared bad" << endl; 
		//     delete cluster2;
		//   }
	      }
	  }
      }
    this->build_trajectories();
    this->extrapolate_trajectories();
  }

  void TKEvent::reconstruct_ML(const TKEventRecConfig & config_)
  {
    bool save_sinograms = config_.save_sinograms;
    for(int side = 0; side < 2; side++)
      {
	vector<TKtrhitHdl> hits = filter_side(tr_hits, side); 
	hits = filter_usable( hits );
	hits = filter_distant( hits );
	if( hits.size() < 3 ) continue;
		
	TKclusterHdl cluster = find_cluster(hits);
	if( cluster.get() != nullptr )
	  {
	    clusters.push_back( cluster );
	    cluster->reconstruct_MLM( save_sinograms, run_number, event_number );
	    cluster->detect_ambiguity_type();
	    cluster->reconstruct_ambiguity();
	  }
      }
    return;
  }


  void TKEvent::reconstruct_ML_3D(const TKEventRecConfig & config_)
  {
    bool save_sinograms = config_.save_sinograms;
    for(int side = 0; side < 2; side++)
      {
	vector<TKtrhitHdl> hits = filter_side(tr_hits, side); 
	hits = filter_usable( hits );
	hits = filter_distant( hits );
	if( hits.size() < 3 ) continue;
		
	TKclusterHdl cluster = find_cluster(hits);
	if( cluster.get() != nullptr )
	  {
	    clusters.push_back( cluster );
	    cluster->reconstruct_MLM_3D( save_sinograms, run_number, event_number );
	    cluster->detect_ambiguity_type();
	    cluster->reconstruct_ambiguity();
	  }
      }
    return;
  }

  TKclusterHdl TKEvent::find_cluster(std::vector<TKtrhitHdl> & hits)
  {
    TKclusterHdl foundClusterHdl;
    //number of different values of phi among which the cluster is being searched for
    const int bins_phi = 100;
    const auto sideId = hits.front()->get_SRL('s');
    for(auto i = 0u; i < hits.size(); i++)
      {
	if( sideId != hits[i]->get_SRL('s'))
	  {
	    std::cerr << "cluster not found - hits from both sides included" << endl;
	    return foundClusterHdl;
	  }
      }

    if( hits.size() < 3 ) 
      {
	std::cerr << "cluster not found - too few usable hits" << endl;
	return foundClusterHdl;
      }
	
    int max_count[bins_phi] = {0};
    double argmax_R[bins_phi];
    int global_max = 0;
    for(int i = 0; i < bins_phi; i++)
      {
	vector<double> boundaries;
	vector<int> hit_count;
	double theta = i * M_PI / double(bins_phi);		
	double boundary_down, boundary_up;
		
	boundaries.push_back(-10000);
	hit_count.push_back(0);
	boundaries.push_back(10000);
		
	for(auto j = 0u; j < hits.size(); j++)
	  {
	    const auto & hitPtr = hits.at(j);
	    double x = hitPtr->get_xy('x');
	    double y = hitPtr->get_xy('y');
	    if(i < bins_phi/2)
	      {
		boundary_down = (x-22.0) * sin(theta) - (y+22.0) * cos(theta);
		boundary_up   = (x+22.0) * sin(theta) - (y-22.0) * cos(theta);
	      }
	    else
	      {		
		boundary_down = (x-22.0) * sin(theta) - (y-22.0) * cos(theta);
		boundary_up   = (x+22.0) * sin(theta) - (y+22.0) * cos(theta);	
	      }
			
	    int k = 0;
	    while( boundary_down > boundaries.at(k) )
	      {
		k++;
	      } 
	    if( boundary_down < boundaries.at(k) )
	      {
		boundaries.insert(boundaries.begin()+k, boundary_down);
		hit_count.insert(hit_count.begin()+k, hit_count.at(k-1));
	      }
	    while( boundary_up > boundaries.at(k) )			
	      {
		hit_count.at(k)++;
		k++;
	      }
	    if( boundary_up < boundaries.at(k) )
	      {
		boundaries.insert(boundaries.begin()+k, boundary_up);
		hit_count.insert(hit_count.begin()+k, hit_count.at(k-1)-1);
	      }

	  }
		
	for(auto j = 0u; j < hit_count.size(); j++)
	  {
	    if(hit_count.at(j) > max_count[i])
	      {
		max_count[i] = hit_count.at(j);
		argmax_R[i] = (boundaries.at(j) + boundaries.at(j+1))/2.0;
	      }
	  }

	if(max_count[i] > global_max)
	  global_max = max_count[i];
      }

    int phi_bin_min = 0;
    int phi_bin_max = 0;	
    double phi_min;
    double phi_max;
	
    if(max_count[0] == global_max)
      {
	while(max_count[phi_bin_max] == global_max)
	  {
	    phi_bin_max++;
	  }
	phi_bin_min = bins_phi-1;
	while(max_count[phi_bin_min] == global_max)
	  {
	    phi_bin_min--;
	  }

	phi_max = phi_bin_max * M_PI / double(bins_phi);
	phi_min = phi_bin_min * M_PI / double(bins_phi) - M_PI;
      }
    else
      {
	while(max_count[phi_bin_min] < global_max)
	  {
	    phi_bin_min++;
	  }		
	phi_bin_max = phi_bin_min;
	while(max_count[phi_bin_max] == global_max)
	  {
	    phi_bin_max++;
	  }

	phi_min = (phi_bin_min-1.0) * M_PI / double(bins_phi);
	phi_max = (phi_bin_max) * M_PI / double(bins_phi);
      }

    double R_0 = argmax_R[phi_bin_max-1];
    double theta_0 = double(phi_bin_max-1) * M_PI / double(bins_phi);
	
    vector<TKtrhitHdl> cluster_hits;
    for(auto j = 0u; j < hits.size(); j++)
      {
	auto & hitPtr = hits.at(j);
	double boundary_down, boundary_up;
	double x = hitPtr->get_xy('x');
	double y = hitPtr->get_xy('y');
	if(phi_bin_max-1 < bins_phi/2)
	  {
	    boundary_down = (x-22.0) * sin(theta_0) - (y+22.0) * cos(theta_0);
	    boundary_up   = (x+22.0) * sin(theta_0) - (y-22.0) * cos(theta_0);
	  }
	else
	  {		
	    boundary_down = (x-22.0) * sin(theta_0) - (y-22.0) * cos(theta_0);
	    boundary_up   = (x+22.0) * sin(theta_0) - (y+22.0) * cos(theta_0);	
	  }
	if(boundary_down <= R_0 && R_0 <= boundary_up)
	  {
	    cluster_hits.push_back(hitPtr);
	  }
      }
    cluster_hits = filter_distant(cluster_hits);//TODO is this good? 
    if(cluster_hits.size() < 3)
      {
	return foundClusterHdl;
      }
    foundClusterHdl = std::make_shared<TKcluster>(cluster_hits, phi_min, phi_max);
    return foundClusterHdl;
  }

  TKclusterHdl TKEvent::find_cluster_legendre(const std::vector<TKtrhitHdl> & hits,
					      const TKEventRecConfig & config_)
  {
    TKclusterHdl clusterPtr;
    bool save_sinograms = config_.save_sinograms;
    gROOT->SetBatch(kTRUE);

    if( hits.size() < 3 ) 
      {
	std::cerr << "cluster not found - too few usable hits" << endl;
	return clusterPtr;
      }

    const double distance_limit = config_.legendre_distance_limit;
    const double limit_angle = config_.legendre_limit_angle;

    // resolution of both dimensions of every 2D histogram	
    auto resolution = config_.legendre_resolution; 
	
    // number of zooming iterations into histogram
    const auto iterations = 2u;

    // peaks_phi, peak_R and peaks_value stores information about peak candidate
    // each iteration takes those candidate and zooms around them
    double peak_phi = M_PI / 2.0;
    double peak_R = 0.0;
    // double peak_value = 0.0;
	
    // size of region (delta_phi x delta_R) on witch each peak is calculated more precisely
    double delta_phi = M_PI;
    double delta_R = 5000.0;
	
    for(auto iter = 0u; iter < iterations; iter++)
      {
	double r_min = peak_R - (delta_R/2.0);
	double r_max = peak_R + (delta_R/2.0);
	double phi_min = peak_phi - (delta_phi/2.0);
	double phi_max = peak_phi + (delta_phi/2.0);
		
	// sinograms are calculated for each bin of phi range - (offset = 1/2 of bin widht) 
	double offset = (delta_phi)/(2.0*resolution);
	TH2F *sinograms = new TH2F("sinograms", "sinograms; phi; r", resolution, phi_min + offset, phi_max + offset, resolution, r_min, r_max);
	double phi;
	double r;			
	for(auto hit = 0u; hit < hits.size(); hit++)
	  {
	    const auto & hitPtr = hits[hit];
	    double sigma = hitPtr->get_sigma_R();
	    for(auto k = 0u; k <= resolution; k++)
	      {
		phi = phi_min + ( k * delta_phi / double(resolution) );
		// r - legendre transform of a center of a circle (Hough transform)
		r = hitPtr->get_xy('x')*sin(phi) - hitPtr->get_xy('y')*cos(phi);
				
		double weight;
		for(int half = 0; half < 2; half++)
		  {	
		    // mu - legendre transform of half circle (+r/-r)
		    double mu = (r + (2.0*half - 1.0)*hitPtr->get_r());	
					
		    // gauss is calculated only for -3 to 3 sigma region to cut time							
		    double r1 = mu - 3.0*sigma;
		    double r2 = mu + 3.0*sigma;
					
		    // bin numbers coresponding to r1 and r2 values
		    int bin1 = (double(resolution) * (r1-r_min) / (r_max-r_min)) + 1;
		    int bin2 = (double(resolution) * (r2-r_min) / (r_max-r_min)) + 1;
					
		    // real values of r coresponding to each bin 
		    double r_j1 = r_min + (r_max - r_min) * double(bin1) / double(resolution);
		    double r_j2;
		    for(int binj = bin1; binj < bin2 + 1; binj++)
		      {
			r_j2 = r_j1 + (r_max - r_min) / double(resolution);
						
			// average probability density in a bin given by gauss distribution with mean in mu 
			// (uniformly distributed with respect to phi)
			weight = ( erf( (r_j2 - mu)/(sqrt(2.0)*sigma) ) - erf( (r_j1 - mu)/(sqrt(2.0)*sigma) ) ) / (2.0 * delta_R / double(resolution));
						
			// result is 2D histogram of several sinusoid functions f(phi) in convolution with gauss with respect to R
			sinograms->Fill( phi, (r_j2 + r_j1)/2.0, weight );
			r_j1 = r_j2;			
		      }								
		  }			
	      }	
	  }										

	// Get bin number of maximum value
	int maxBin = sinograms->GetMaximumBin();

	// Get X and Y values corresponding to the maximum bin
	int bin_phi, bin_R, bin_Z;
	sinograms->GetBinXYZ(maxBin, bin_phi, bin_R, bin_Z);
	peak_phi = sinograms->GetXaxis()->GetBinCenter(bin_phi);
	peak_R = sinograms->GetYaxis()->GetBinCenter(bin_R);
	delta_phi = delta_phi*0.1;
	delta_R = delta_R*0.1;

	if( save_sinograms ) 
	  {
	    TCanvas* c2 = new TCanvas("sinograms", "sinograms", 2000, 1600);
	    sinograms->SetStats(0);
	    sinograms->SetContour(100);
	    sinograms->Draw("COLZ");
	    c2->SaveAs(Form("Events_visu/clustering-run-%d_event-%d_iter-%d.png", run_number, event_number, iter));
	    c2->Close();
	    delete c2;
	  }
	delete sinograms;
      }
	
    vector<TKtrhitHdl> cluster_candidate = filter_close_hits(hits, peak_phi, peak_R, distance_limit);
	
    cluster_candidate = filter_distant(cluster_candidate); //TODO is this good?
    if(cluster_candidate.size() >= 3)
      {
	clusterPtr = std::make_shared<TKcluster>(cluster_candidate,
						 peak_phi - limit_angle,
						 peak_phi + limit_angle);
      }
    return clusterPtr;
  }

  void TKEvent::build_trajectories()
  {
    //if(debug_mode) std::cerr << "building trajectory" << endl;
    vector<TKtrackHdl> all_tracks = this->get_all_tracks();
	
    for(int side = 0; side < 2; side++)
      {
	//if(debug_mode) std::cerr << "building side: " << side << endl;
	vector<TKtrackHdl> all_tracks_from_side;
	for(auto i = 0u; i < all_tracks.size(); i++)
	  {
	    if(all_tracks[i]->get_side() == side)
	      {
		if(all_tracks[i]->get_associated_tr_hit_points().size() > 1) {
		  all_tracks_from_side.push_back(all_tracks[i]);
		}
	      }
	  }		
	const auto no_tracks = all_tracks_from_side.size();
	//if(debug_mode) std::cerr << no_tracks << " tracks available" << endl;

	// XXX
	// bool connections[no_tracks][no_tracks];
	// XXX
	
	// arrays stores information about candidates from each segment
	typedef boost::multi_array<bool, 2> array_type;
	typedef array_type::index index;
	array_type connections(boost::extents[no_tracks][no_tracks]);
	// Assign values to the elements:
	for(index ia = 0; ia != (int) no_tracks; ++ia) 
	  for(index ja = 0; ja != (int) no_tracks; ++ja)
	    connections[ia][ja] = false;
	// for(auto i = 0u; i < no_tracks; i++)
	//   {
	//     for(auto j = 0u; j < no_tracks; j++)
	//       {
	// 	connections[i][j] = false;
	//       }
	//   }
	for(auto i = 0u; i < no_tracks; i++)
	  {
	    const ConstTKtrackHdl & track1 = all_tracks_from_side[i];
	    for(auto j = i+1; j < no_tracks; j++)
	      {
		const ConstTKtrackHdl & track2 = all_tracks_from_side[j];
		if(track1->get_mirror_image().get() == track2.get()) {
		  continue;
		}
		double a1 = track1->get_a();
		double a2 = track2->get_a();
		double b1 = track1->get_b();
		double b2 = track2->get_b();
		double x = (b2-b1)/(a1-a2);
		double y = a1*x + b1;
		if(y < -2486.0 || y > 2486.0) continue;
		if(x < -425.0 || x > 425.0) continue;
		if(side == 0 && x > -29.0) continue;
		if(side == 1 && x < 29.0) continue;				
		double z1 = track1->get_c()*x + track1->get_d();
		double z2 = track2->get_c()*x + track2->get_d();
		if(z1 == 0 || z2 == 0) continue;
		if(abs(z2 - z1) > 40.0) continue;			
		connections[i][j] = true;
		connections[j][i] = true;			
	      }
	  }
	// int connection_counter[no_tracks];	
	std::vector<int> connection_counter(no_tracks, 0);
	for(auto i = 0u; i < no_tracks; i++)
	  {
	    // connection_counter[i] = 0;
	    for(auto j = 0u; j < no_tracks; j++)
	      {
		if(connections[j][i])
		  {
		    connection_counter[i]++;
		  }
	      }
	  }

	// XXX
	// bool trajectorized[no_tracks];
	// XXX
	std::vector<bool> trajectorized(no_tracks, false);
	
	//if(debug_mode) std::cerr << "number of connections for each segment:" << endl;
	for(auto i = 0u; i < no_tracks; i++)
	  {
	    trajectorized[i] = false;
	    if(debug_mode) std::cerr << connection_counter[i] << endl; 
	  }
	/*
	  if(debug_mode)
	  {
	  std::cerr << "connection matrix:" << endl;
	  for(int i = 0; i < no_tracks; i++)
	  {
	  for(int j = 0; j < no_tracks; j++)
	  {
	  std::cerr << connections[i][j] << " ";	
	  }
	  std::cerr << endl;
	  }
	  std::cerr << endl;
	  }*/
		
	for(auto i = 0u; i < no_tracks; i++)
	  {
	    //if(debug_mode) std::cerr << "connecting track number: " << i << endl; 
	    if(connection_counter[i] == 0)
	      {
		//if(all_tracks_from_side[i]->get_associated_tr_hits().size() > 1) // in ideal case should be unnecessary
		trajectories.push_back(std::make_shared<TKtrajectory>(all_tracks_from_side[i]));
		trajectorized[i] = true;
		//if(debug_mode) std::cerr << i << " trajectorized" << endl;
	      }
	    else if(connection_counter[i] == 1 && not trajectorized[i])
	      {
		vector<TKtrackHdl> composite_track;
		composite_track.push_back(all_tracks_from_side[i]);
		trajectorized[i] = true;
		//if(debug_mode) std::cerr << i << " trajectorized" << endl;
		int next_index;
		bool is_valid = false;
		for(auto j = 0u; j < no_tracks; j++)
		  {
		    if(connections[i][j] && not trajectorized[j])
		      {
			next_index = j;
			is_valid = true;
			composite_track.push_back(all_tracks_from_side[next_index]);
			trajectorized[next_index] = true;
			//if(debug_mode) std::cerr << next_index << " trajectorized" << endl;
			break;
		      }
		  }
				
		while(connection_counter[next_index] >= 2 && is_valid)
		  {
		    is_valid = false;
		    for(auto j = 0u; j < no_tracks; j++)
		      {
			if(connections[next_index][j] && not trajectorized[j])
			  {
			    next_index = j;
			    is_valid = true;
			    composite_track.push_back(all_tracks_from_side[next_index]);
			    trajectorized[next_index] = true;
			    //if(debug_mode) std::cerr << next_index << " trajectorized" << endl;
			    break;
			  }
		      }
		  }
		if(composite_track.size() > 1)
		  {
		    // 000
		    trajectories.push_back(std::make_shared<TKtrajectory>(composite_track));
		  }
		else if(composite_track.size() == 1)
		  {
		    //if(composite_track[0]->get_associated_tr_hits().size() > 1) // in ideal case should be unnecessary
		    trajectories.push_back(std::make_shared<TKtrajectory>(composite_track.front()));
		  }
	      }
	  }
      }
    /*
      if(debug_mode)
      {
      std::cerr << "trajectory:" << endl;
      for(int i = 0; i < trajectories.size(); i++)
      {
      trajectories[i]->print();
      }
      }*/
	
    return;
  }

  void TKEvent::extrapolate_trajectories()
  {
    for(auto& trajectory : trajectories)
      {
	trajectory->extrapolate();
      }
    return;
  }

  void TKEvent::calculate_tr_hit_points()
  {
    vector<TKtrackHdl> all_tracks = this->get_all_tracks();
    for(auto & trackHdl : all_tracks)
      {
	trackHdl->calculate_tr_hit_points();
      }
    return;
  }

} //  end of namespace tkrec
