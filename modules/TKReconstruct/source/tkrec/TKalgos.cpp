// Interface from Falaise
#include "tkrec/TKalgos.h"

// Standard headers
#include <iomanip>

// Boost:
#include <boost/multi_array.hpp>

// Bayeux:
#include <bayeux/datatools/exception.h>
#include <bayeux/datatools/clhep_units.h>

namespace tkrec {
 
  EventRecMode event_recmode_from_label(const std::string & label_)
  {
    if (label_ == "full") return EventRecMode::full;
    if (label_ == "simple") return EventRecMode::simple;
    if (label_ == "basic") return EventRecMode::basic;
    if (label_ == "single") return EventRecMode::single;
    if (label_ == "multi") return EventRecMode::multi;
    if (label_ == "ML") return EventRecMode::ML;
    if (label_ == "ML3D") return EventRecMode::ML3D;
    return EventRecMode::undefined;
  }
 
  void TKEventRecConfig::parse(const datatools::properties & config_)
  {
    if (config_.has_key("verbosity")) {
      std::string verbosityLabel = config_.fetch_string("verbosity");
      auto parsedVerb = datatools::logger::get_priority(verbosityLabel);
      DT_THROW_IF(parsedVerb == datatools::logger::PRIO_UNDEFINED,
                  std::logic_error,
                  "Undefined verbosity label " << std::quoted(verbosityLabel));
      this->verbosity = parsedVerb;
    }

    if (config_.has_key("mode")) {
      std::string evRecModeLabel = config_.fetch_string("mode");
      auto evRecMode = event_recmode_from_label(evRecModeLabel);
      DT_THROW_IF(evRecMode == EventRecMode::undefined,
                  std::logic_error,
                  "Undefined event reconstruction mode " << std::quoted(evRecModeLabel));
      this->mode = evRecMode;
    }
    
    if (config_.has_flag("visualization")) {
      this->visualization = true;
    }
  
    if (config_.has_flag("save_sinograms")) {
      this->save_sinograms = true;
    }
  
    if (config_.has_flag("force_default_sigma_r")) {
      this->force_default_sigma_r = true;
    }
  
    if (config_.has_key("default_sigma_r")) {
      auto value = config_.fetch_real_with_explicit_dimension("default_sigma_r",
							      "length");
      DT_THROW_IF(value <= 1.0e-3 * CLHEP::mm or value >= 50. * CLHEP::mm,
                  std::logic_error,
                  "Invalid default sigma for drift radius"); 
      this->default_sigma_r = value / CLHEP::mm;
    }
  
    if (config_.has_key("chi_square_threshold")) {
      this->chi_square_threshold =
        config_.fetch_dimensionless_real("chi_square_threshold");
      DT_THROW_IF(this->chi_square_threshold < 1.0,
		  std::logic_error,
		  "Invalid chi_square_threshold value");
    }

    if (mode == EventRecMode::basic) {
      if (config_.has_key("basic_resolution")) {
	this->basic_resolution =
	  config_.fetch_positive_integer("basic_resolution");
      }
    
      if (config_.has_key("basic_iterations")) {
	this->basic_iterations =
	  config_.fetch_positive_integer("basic_iterations");
      }
    }
    
    if (mode == EventRecMode::multi) {
      if (config_.has_key("multi_resolution")) {
	this->multi_resolution =
	  config_.fetch_positive_integer("multi_resolution");
      }
 
      if (config_.has_key("multi_iterations")) {
	this->multi_iterations =
	  config_.fetch_positive_integer("multi_iterations");
      }
 
      if (config_.has_key("multi_start_no_segments")) {
	this->multi_start_no_segments =
	  config_.fetch_positive_integer("multi_start_no_segments");
      }
   
      if (config_.has_key("multi_threshold")) {
	this->multi_threshold =
	  config_.fetch_dimensionless_real("multi_threshold");
      }
    }
    
    if (mode == EventRecMode::single) {
      if (config_.has_key("single_resolution")) {
	this->single_resolution =
	  config_.fetch_positive_integer("single_resolution");
      }
 
      if (config_.has_key("single_iterations")) {
	this->single_iterations =
	  config_.fetch_positive_integer("single_iterations");
      }
   
      if (config_.has_key("single_threshold")) {
	this->single_threshold =
	  config_.fetch_dimensionless_real("single_threshold");
      }
    }
    
    if (config_.has_key("legendre_resolution")) {
      this->legendre_resolution =
        config_.fetch_positive_integer("legendre_resolution");
    }
 
    if (config_.has_key("legendre_iterations")) {
      this->legendre_iterations =
        config_.fetch_positive_integer("legendre_iterations");
    }

    if (config_.has_key("legendre_distance_limit")) {
      auto value =
        config_.fetch_real_with_explicit_dimension("legendre_distance_limit",
                                                   "length");
      this->legendre_distance_limit = value / CLHEP::mm;
    }
 
    if (config_.has_key("legendre_limit_angle")) {
      auto value =
        config_.fetch_real_with_explicit_dimension("legendre_limit_angle",
                                                   "angle");
      this->legendre_limit_angle = value / CLHEP::radian;
    }
     
    return;
  }
 
  TKalgos::TKalgos(const TKgeom & geom_)
    : _geom_(geom_) 
  {
    return;
  }
 
  TKalgos::~TKalgos()
  {
    if (is_initialized()) {
      reset();
    }
    return;
  }

  void TKalgos::set_event(TKEvent & event_)
  {
    _event_ = &event_;
    if (_visu_) {
      _visu_->set_event(*_event_);
    }
    return;
  }

  bool TKalgos::has_event() const
  {
    return _event_ != nullptr;
  }

  bool TKalgos::is_initialized() const
  {
    return _config_.mode != EventRecMode::undefined;
  }
  
  void TKalgos::initialize(const TKEventRecConfig & evrecconf_)
  {
    _config_ = evrecconf_;
    DT_THROW_IF(_config_.mode == EventRecMode::undefined,
                std::logic_error,
                "Undefined reconstruction mode");

    if (_config_.visualization) {
      _visu_ = std::make_unique<TKvisu>(_geom_);
    }
    
    return;
  }
  
  void TKalgos::reset()
  {
    _event_ = nullptr;
    _config_ = TKEventRecConfig();
    return;
  }
  
  void TKalgos::process(TKEvent & event_)
  {
    set_event(event_);
    if (_config_.mode == EventRecMode::full) {
      DT_LOG_DEBUG(_config_.verbosity, "Processing 'full' mode...");
      _process_full_();
    } else if (_config_.mode == EventRecMode::simple) {
      _process_simple_();
    } else if (_config_.mode == EventRecMode::basic) {
      _process_basic_();
    } else if (_config_.mode == EventRecMode::single) {
      _process_single_();
    } else if (_config_.mode == EventRecMode::multi) {
      _process_multi_();
    } else if (_config_.mode == EventRecMode::ML) {
      _process_ML_();
    } else if (_config_.mode == EventRecMode::ML3D) {
      _process_ML3D_();
    }

    //event->reconstruct_ML();
    //event->calculate_tr_hit_points();
    //event->build_trajectories();		// TKEvent -> TKtrack
    //event->print();
    //event->make_top_projection(3, 2);
    //event->build_event();
    if (_visu_) {
      _visu_->make_top_projection();
      _visu_->build_event();
    }
    return;
  }

  void TKalgos::_process_full_()
  {
    reconstruct_full();
    return;
  }

  void TKalgos::_process_simple_()
  {
    reconstruct_simple();
    return;
  }

  void TKalgos::_process_basic_()
  {
    reconstruct_track();
    return;
  }
  
  void TKalgos::_process_single_()
  {
    reconstruct_single();
    return;
  }
  
  void TKalgos::_process_multi_()
  {
    reconstruct_multi();
    return;
  }
  
  void TKalgos::_process_ML_()
  {
    reconstruct_ML();
    return;
  }
   
  void TKalgos::_process_ML3D_()
  {
    reconstruct_ML_3D();
    return;
  }
  
  void TKalgos::hough_transform(const std::vector<TKtrhitHdl>& hits,
                                double phi_min,
                                double phi_max,
                                double R_min,
                                double R_max,
                                int ID)
  {
    gROOT->SetBatch(kTRUE);

    const int resolution = 1000;
    double r, theta;

    double S_r = 0.0;
    double S_rX = 0.0;
    double S_rY = 0.0;
    double const_R;
    for(auto i = 0u; i < hits.size();i++)
      {
        const auto & hitPtr = hits[i];
        const_R = pow(hitPtr->get_sigma_R(), -2.0);
        S_r = S_r + const_R;
        S_rX = S_rX + hitPtr->get_xy('x')*const_R;
        S_rY = S_rY + hitPtr->get_xy('y')*const_R;
      }
                                        
    // double peak_Theta;
    // double peak_R;
    // double delta_phi, delta_R;

    double offset1 = (phi_max - phi_min)/(2.0*resolution);
    double offset2 = (R_max - R_min)/(2.0*resolution);
    TCanvas canvas("canvas", "canvas", 1000, 1000);
    canvas.cd();
    TH2F hough("hough", "hough; theta; r", resolution, phi_min+offset1, phi_max+offset1, resolution, R_min+offset2, R_max+offset2);
        
    double boundary_down_1;
    double boundary_up_1;
    double boundary_down_2;             
    double boundary_up_2;

    double shift;
        
    for(auto i = 0u; i < hits.size(); i++)
      { 
        const auto & hitPtr = hits[i];
        double x = hitPtr->get_xy('x');
        double y = hitPtr->get_xy('y');
        for(auto k = 0u; k < resolution; k++)
          {
            theta = phi_min + k * (phi_max - phi_min) / double(resolution);                     
            shift = (S_rX/S_r)*std::sin(theta) - (S_rY/S_r)*std::cos(theta);
                                
            boundary_down_1 = (x-22.0) * std::sin(theta) - (y+22.0) * std::cos(theta);
            boundary_up_1   = (x+22.0) * std::sin(theta) - (y-22.0) * std::cos(theta);
            boundary_down_2 = (x-22.0) * std::sin(theta) - (y-22.0) * std::cos(theta);
            boundary_up_2   = (x+22.0) * std::sin(theta) - (y+22.0) * std::cos(theta);
                                                
            for(auto l = 0u; l < resolution; l++)
              { 
                r = R_min + l * (R_max - R_min) / double(resolution);
                r = r + shift;
                if(theta <= M_PI/2.0)
                  {
                    if(boundary_down_1 <= r && r <= boundary_up_1  )
                      {
                        hough.SetBinContent(k, l,  hough.GetBinContent(k, l)+1);
                      }
                  }
                else
                  {
                    if(boundary_down_2 <= r && r <= boundary_up_2  )
                      {
                        hough.SetBinContent(k, l,  hough.GetBinContent(k, l)+1);
                      }                         
                  }     
              }
          }     
      }
        
    int side = hits.front()->get_SRL('S');
    hough.SetStats(0);
    //gStyle->SetPalette(62);
    hough.Draw("COLZ");
    // Hough transform of anode wires
    if( true )
      {
        for(auto ihit = 0u; ihit < hits.size(); ihit++)
          {
            const auto & hitPtr = hits[ihit];
            double xi = hitPtr->get_xy('x');
            double yi = hitPtr->get_xy('y');
            TF1 function("function", "[0]*sin(x)-[1]*cos(x)", phi_min, phi_max);
            function.SetParameter(0, xi-(S_rX/S_r));
            function.SetParameter(1, yi-(S_rY/S_r));
            function.SetLineWidth(2);
            function.Draw("Same");
          }
      }
    canvas.SaveAs(Form("Events_visu/Hough_transform-run-%d_event-%d_side-%d_%d.png",
                        _event_->get_run_number(),
                        _event_->get_event_number(),
                        side,
                        ID));
    canvas.Close();
    return;
  }

  void TKalgos::draw_likelihood()
  {
    gROOT->SetBatch(kTRUE);
        
    const int resolution = 1250;
    const int iterations = 3;

    for(int side = 0; side < 2; side++)
      {
        std::vector<TKtrhitHdl> hits_from_side = TKEvent::filter_side(_event_->get_tr_hits(), side); 
        std::vector<TKtrhitHdl> hits = TKEvent::filter_usable(hits_from_side);

        if( hits.size() < 3 ) continue;
        
        double phi1 = 0.0;
        double phi2 = 2.0*M_PI;
        double R1 = 0.0;
        double R2 = 2500.0;
                
        double peak_Theta = std::numeric_limits<double>::quiet_NaN();
        double peak_R = std::numeric_limits<double>::quiet_NaN();
        double delta_phi = std::numeric_limits<double>::quiet_NaN();
        double delta_R = std::numeric_limits<double>::quiet_NaN();
                
        for(auto iter = 0u; iter < iterations; iter++)
          {     
            double offset_phi = (phi2 - phi1)/(2.0*resolution);
            double offset_R = (R2 - R1)/(2.0*resolution);
            TH2F sinogram("sinogram", "sinogram; phi[rad]; r[mm]",
			  resolution,
			  phi1 + offset_phi,
			  phi2 + offset_phi,
			  resolution,
			  R1 + offset_R,
			  R2 + offset_R);             
                                
            for(auto hit = 0u; hit < hits.size(); hit++)
              {
                const auto & hitH = hits[hit];
                // double R0 = hitH->get_r();
                double sigma_R = hitH->get_sigma_R();
                double norm_const = 1.0 / (std::sqrt(2.0*M_PI)*sigma_R); 
                                
                double weight;
                double r, theta;
                for(auto j = 0u; j <= resolution; j++)
                  {
                    theta = phi1 + (j * (phi2 - phi1) / resolution);
                    double rho = hitH->get_xy('x')*std::sin(theta) - hitH->get_xy('y')*std::cos(theta);
                                        
                    for(auto k = 0u; k <= resolution; k++)
                      {
                        r = R1 + (k * (R2 - R1) / resolution);
                                                
                        weight = std::exp( -std::pow( std::abs(r-rho) - hitH->get_r() , 2 ) / (2.0*sigma_R*sigma_R) );
                        weight = weight * norm_const;
                                                
                        if(hit != 0)
                          {     
                            weight = weight * sinogram.GetBinContent(j, k);
                          }
                        sinogram.SetBinContent(j, k, weight );
                      }
                  }     
              }
                                                        
            double maximum = 0.0;
            for(auto i = 1u; i <= resolution; i++)
              {
                for(auto j = 1u; j <= resolution; j++)
                  {
                    if(maximum < sinogram.GetBinContent(i,j))
                      {
                        maximum = sinogram.GetBinContent(i,j);
                        peak_Theta = i;
                        peak_R = j;
                      }
                  }
              }                     
                        
            if(maximum == 0.0) 
              {
                 break;
              }
                        
            if( true ) 
              {
                TCanvas c2("sinogram","sinogram", 2000, 1600);
		c2.cd();
                c2.SetRightMargin(0.15);
                sinogram.SetStats(0);
                sinogram.SetContour(100);
                sinogram.Draw("COLZ");
                //gStyle->SetPalette(62);
                                
                                
                // Hough transform of anode wires
                if( 0 )
                  {
                    for(auto hit = 0u; hit < hits.size(); hit++)
                      {
                        const auto & hitH = hits[hit];
                        double xi = hitH->get_xy('x');
                        double yi = hitH->get_xy('y');
                        TF1 function("function", "[0]*sin(x)-[1]*cos(x)", phi1, phi2);
                        function.SetParameter(0, xi);
                        function.SetParameter(1, yi);
                        function.SetLineWidth(1);
                        function.Draw("Same");
                      }
                  }
                                
                c2.SaveAs(Form("Events_visu/sinogram-run-%d_event-%d_side-%d_zoom-%d.png",
                                _event_->get_run_number(),
                                _event_->get_event_number(),
                                side,
                                iter));
                c2.Close();
              }
                        
            delta_phi = phi2 - phi1;
            delta_R = R2 - R1;
                        
            peak_Theta = phi1 + delta_phi * (peak_Theta-0.5) / double(resolution);
            peak_R = R1 + delta_R * (peak_R-0.5) / double(resolution);
                                                            
            phi1 = peak_Theta - 0.05*delta_phi;
            phi2 = peak_Theta + 0.05*delta_phi;
            R1 = peak_R - 0.05*delta_R;
            R2 = peak_R + 0.05*delta_R;

            DT_LOG_DEBUG(_config_.verbosity,
                         "Event: " << _event_->get_event_number() << " r: "
                         << peak_R << "     phi: " << peak_Theta);
                        
            // double a = tan(peak_Theta);
            // double b = -peak_R / cos(peak_Theta);
                        
            double joined_probability = 1.0;
            double probability;
            double norm_const = pow(2.0*M_PI, -0.5*hits.size());
            for(auto i = 0u; i < hits.size(); i++)
              { 
                const auto & hitH = hits[i];
                double sigma_R = hitH->get_sigma_R();
                norm_const = norm_const / sigma_R;
                probability = std::exp(-std::pow(std::abs( peak_R + hitH->get_xy('x')*std::cos(peak_Theta) - hitH->get_xy('y')*std::sin(peak_Theta) ) - hitH->get_r() , 2)/(2.0*sigma_R*sigma_R));
                joined_probability = joined_probability * probability; 
              }         
                        
            joined_probability = norm_const * joined_probability;

            DT_LOG_DEBUG(_config_.verbosity, "likelihood R: " << joined_probability);
          }
      }
    return;
  }

  void TKalgos::draw_likelihood_centred()
  {
    gROOT->SetBatch(kTRUE);
        
    const int resolution = 1000;
    const int iterations = 3;

    for(int side = 0; side < 2; side++)
      {
        std::vector<TKtrhitHdl> hits_from_side =
          TKEvent::filter_side(_event_->get_tr_hits(), side); 
        std::vector<TKtrhitHdl> hits = TKEvent::filter_usable(hits_from_side);
        if( hits.size() < 3 ) continue;

        double S_r = 0.0;
        double S_rX = 0.0;
        double S_rY = 0.0;
        double const_R;
        for(auto i = 0u; i < hits.size();i++)
          {
            const auto hitPtr = hits[i];
            const_R = hitPtr->get_sigma_R();
            S_r = S_r + const_R;
            S_rX = S_rX + hitPtr->get_xy('x')*const_R;
            S_rY = S_rY + hitPtr->get_xy('y')*const_R;
          }
                
        double phi1 = 0.0;
        double phi2 = M_PI;
        double R1 = -150.0;
        double R2 = 150.0;
                
        double peak_Theta = std::numeric_limits<double>::quiet_NaN();
        double peak_R = std::numeric_limits<double>::quiet_NaN();
                
        for(auto iter = 0u; iter < iterations; iter++)
          {     
            double delta_phi = phi2 - phi1;
            double delta_R = R2 - R1;
                        
            double offset_phi = delta_phi/(2.0*resolution);
            double offset_R = delta_R/(2.0*resolution);
                        
            TH2F sinogram_centred("sinogram_centred",
				  "sinogram_centred; phi[rad]; r[mm]",
				  resolution,
				  phi1 + offset_phi,
				  phi2 + offset_phi,
				  resolution,
				  R1 + offset_R,
				  R2 + offset_R);   
                        
            double r, theta;
            for(auto j = 0u; j <= resolution; j++)
              {
                theta = phi1 + (j * delta_phi / resolution);
                                        
                for(auto k = 0u; k <= resolution; k++)
                  {
                    r = R1 + (k * delta_R / resolution);
                    // shifting origin to "center of mass" of hits
                    r = r + (S_rX/S_r)*std::sin(theta) - (S_rY/S_r)*std::cos(theta);
                                        
                    double weight = 1.0;
                    double temp;
                    for(auto hit = 0u; hit < hits.size(); hit++)
                      {
                        double rho = hits[hit]->get_xy('x')*std::sin(theta) - hits[hit]->get_xy('y')*std::cos(theta);
                        // double R0 = hits[hit]->get_r();
                        double sigma_R = hits[hit]->get_sigma_R();
                        double norm_const = 1.0 / (std::sqrt(2.0*M_PI)*sigma_R); 
                                                
                        temp = std::exp( -std::pow( std::abs(r-rho) - hits[hit]->get_r() , 2 ) / (2.0*sigma_R*sigma_R) );
                        temp = temp * norm_const;
                                                
                        weight = weight * temp;
                                                
                      }
                    sinogram_centred.SetBinContent(j, k, log(weight) );
                  }     
              }
                                                        
            double maximum = -std::numeric_limits<double>::infinity();
            for(auto i = 1u; i <= resolution; i++)
              {
                for(auto j = 1u; j <= resolution; j++)
                  {
                    if(maximum < sinogram_centred.GetBinContent(i,j))
                      {
                        maximum = sinogram_centred.GetBinContent(i,j);
                        peak_Theta = i;
                        peak_R = j;
                      }
                  }
              }
            if(std::isinf(maximum)) 
              {
                // delete sinogram_centred;                        
                break;
              }
                        
                        
            if( true ) 
              {
                TCanvas c2("sinogram_centred","sinogram_centred", 2000, 1600);
		c2.cd();
                c2.SetRightMargin(0.15);
                sinogram_centred.SetStats(0);
                sinogram_centred.SetContour(100);
                sinogram_centred.Draw("COLZ");
                //gStyle->SetPalette(62);
                                
                // Hough transform of anode wires
                if( 1 )
                  {
                    for(auto hit = 0u; hit < hits.size(); hit++)
                      {
                        double xi = hits[hit]->get_xy('x');
                        double yi = hits[hit]->get_xy('y');
                        TF1 function("function", "[0]*sin(x)-[1]*cos(x)", phi1, phi2);
                        function.SetParameter(0, xi-(S_rX/S_r));
                        function.SetParameter(1, yi-(S_rY/S_r));
                        function.SetLineWidth(2);
                        function.Draw("Same");
                      }
                  }
                                                            
                c2.SaveAs(Form("Events_visu/sinogram_centred-run-%d_event-%d_side-%d_zoom-%d.png",
                                _event_->get_run_number(),
                                _event_->get_event_number(),
                                side,
                                iter));
                c2.Close();
              }
            // delete sinogram_centred;
                        
            peak_Theta = phi1 + delta_phi * (peak_Theta-0.5) / double(resolution);
            peak_R = R1 + delta_R * (peak_R-0.5) / double(resolution);
                        
            phi1 = peak_Theta - 0.06*delta_phi;
            phi2 = peak_Theta + 0.06*delta_phi;
            R1 = peak_R - 0.06*delta_R;
            R2 = peak_R + 0.06*delta_R;

            peak_R = peak_R + (S_rX/S_r)*std::sin(peak_Theta) - (S_rY/S_r)*std::cos(peak_Theta);
            DT_LOG_DEBUG(_config_.verbosity, "Event: " << _event_->get_event_number()
                         << " r: " << peak_R
                         << "     phi: " << peak_Theta);
                                                
            double joined_probability = 1.0;
            double probability;
            double norm_const = pow(2.0*M_PI, -0.5*hits.size());
            for(auto i = 0u; i < hits.size(); i++)
              {
                const auto & hitPtr = hits[i];
                double sigma_R = hitPtr->get_sigma_R();
                norm_const = norm_const / sigma_R;
                probability = std::exp(-std::pow(std::abs( peak_R + hitPtr->get_xy('x')*std::cos(peak_Theta) - hitPtr->get_xy('y')*std::sin(peak_Theta) ) - hitPtr->get_r() , 2)/(2.0*sigma_R*sigma_R));
                joined_probability = joined_probability * probability; 
              } 
                        
            joined_probability = norm_const * joined_probability;
                        
            DT_LOG_DEBUG(_config_.verbosity, "likelihood R: " << joined_probability);
                        
            if(iter == iterations-1)
              {
                auto trackPtr = std::make_shared<TKtrack>(side, peak_Theta, peak_R);
                trackPtr->set_likelihood_R(joined_probability);
                _event_->get_tracks().push_back(trackPtr);
              }
          }
      }
    return;
  }

  void TKalgos::draw_sinusoids()
  {
    gROOT->SetBatch(kTRUE);
        
    for(int side = 0; side < 2; side++)
      {
        std::vector<TKtrhitHdl> hits_from_side =
          TKEvent::filter_side(_event_->get_tr_hits(), side); 
        std::vector<TKtrhitHdl> hits = TKEvent::filter_usable(hits_from_side);
        if( hits.size() < 1 ) continue;
                
        TCanvas canvas("sinusoids");
	canvas.cd();
        for(auto hit = 0u; hit < hits.size(); hit++)
          {
            const auto & hitH = hits[hit];
            double xi = hitH->get_xy('x');
            double yi = hitH->get_xy('y');
            TF1 function("function", "[0]*sin(x)-[1]*cos(x)", 0.0, 2.0*M_PI);
            function.SetParameter(0, xi);
            function.SetParameter(1, yi);
            function.SetLineWidth(1);
                        
            if(hit != 0)
              {
                function.Draw("Same");
              }
            else
              {
                function.Draw();
              }
          }
        canvas.SaveAs(Form("Events_visu/sinusoids-run-%d_event-%d_side-%d.png",
                            _event_->get_run_number(),
                            _event_->get_event_number(),
                            side));
        canvas.Close();                
      }
    return;
  }


  void TKalgos::_reconstruct_track_from_hits_(const std::vector<TKtrhitHdl> & hits)
  {
    bool save_sinograms = _config_.save_sinograms;
    // ancient algorithm - does not provide complete likelihood and (r,phi,theta,h) coordinates

    // finds only one track per side of the tracker seperately from given set of tracker hits
    // no uncertainties involved here
        
    // each iteration zooms into a region with 10% size in both directions
    const auto resolution = _config_.basic_resolution;
    const auto iterations = _config_.basic_iterations;

    for(int side = 0; side < 2; side++)
      { 
        std::vector<double> hits_x;
        std::vector<double> hits_y;
        std::vector<double> hits_z;
        std::vector<double> hits_r;
        std::vector<double> hits_sigma;      
        std::vector<int>    index;
                
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
            TH2F sinograms("sinograms", "sinograms; theta; r",
			   resolution,
			   phi1+offset,
			   phi2+offset,
			   resolution,
			   R1,
			   R2);           
            for(auto i = 0u; i < hits_x.size(); i++)
              {
                for(auto k = 0u; k <= resolution; k++)
                  {
                    theta = phi1 + (k * (phi2 - phi1) / resolution);
                    r = hits_x[i]*std::sin(theta) - hits_y[i]*std::cos(theta);
                    sinograms.Fill( theta, r - hits_r[i] );
                    sinograms.Fill( theta, r + hits_r[i] );
                  }     
              }
                                                                
            double maximum = 0.0;
            for(auto i = 1u; i <= resolution; i++)
              {
                for(auto j = 1u; j <= resolution; j++)
                  {
                    if(maximum < sinograms.GetBinContent(i,j))
                      {
                        maximum = sinograms.GetBinContent(i,j);
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

            if( save_sinograms ) 
              {
                TCanvas c2("sinograms");
		c2.cd();
                sinograms.SetContour(100);
                sinograms.SetStats(0);
                sinograms.Draw("COLZ");
                c2.SaveAs(Form("Events_visu/sinograms-run-%d_event-%d_side-%d_zoom-%d.png",
				_event_->get_run_number(),
				_event_->get_event_number(),
				side,
				q));
                c2.Close();
              }
          }
                
        // creating track
        auto track = std::make_shared<TKtrack>(side, peak_Theta, peak_R);       
                
        double a = track->get_a();
        double b = track->get_b();
                
        // associating hits to track
        std::vector<bool> hits_associated;
        double denominator = std::sqrt((a*a) + 1);   
        for(auto i = 0u; i < hits_x.size(); i++)
          {
            double distance_from_wire = std::abs(hits_y[i] - a*hits_x[i] - b) / denominator;
            double association_distance = 3.0 * hits_sigma[i];
            if( std::abs(distance_from_wire - hits_r[i]) < association_distance )
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
                
        double norm = 1.0 / std::sqrt(2.0*M_PI);
        for(auto i = 0u; i < hits.size();i++)
          {     
                        
            probability = std::exp(-std::pow(std::abs( r + hits_x[i]*std::cos(theta) - hits_y[i]*std::sin(theta) ) - hits_r[i] , 2)/(2.0*hits_sigma[i]*hits_sigma[i]));
            joined_probability = joined_probability * probability / hits_sigma[i]; 
          }             
                
        joined_probability = std::pow(norm, hits.size()) * joined_probability;
        track->set_likelihood_R(joined_probability);
                
        // fitting z coordinates of hits
        std::vector<double> hits_p;
        for(auto i = 0u; i < hits_x.size(); i++)
          {
            double distance_from_wire = std::abs(hits_y[i] - a*hits_x[i] - b) / denominator;
            double projection_distance = std::pow(hits_x[i], 2.0) + std::pow((hits_y[i] - b), 2.0) - std::pow(distance_from_wire, 2.0);
            projection_distance = std::pow(projection_distance, 0.5);
                        
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
            c = c*(2.0*side-1.0)*std::pow(1+a*a, 0.5);
            d = (sum_PP*sum_Z - sum_P*sum_ZP) / denominator;
          }
                
        track->set_c(c);
        track->set_d(d);
                
        track->set_theta( std::atan(c/std::sqrt(a*a+1.0)) ); 
        track->set_h( d - a*b*c/(a*a+1.0) );    
                
        track->update_likelihood();
                                
        _event_->get_tracks().push_back(track);
                
        hits_x.erase(hits_x.begin(), hits_x.end());
        hits_y.erase(hits_y.begin(), hits_y.end());
        hits_z.erase(hits_z.begin(), hits_z.end());
        hits_r.erase(hits_r.begin(), hits_r.end());
      }
    return;
  }

  void TKalgos::reconstruct_track()
  {
    _reconstruct_track_from_hits_(_event_->get_tr_hits());
    return;
  }

  void TKalgos::reconstruct_multi()
  {
    bool save_sinograms = _config_.save_sinograms;
    gROOT->SetBatch(kTRUE);

    // resolution of both dimensions of every 2D histogram      
    auto resolution = _config_.multi_resolution; 
        
    // number of zooming iterations into histogram
    const auto iterations = _config_.multi_iterations;
        
    // how high does a peak has to be in comparison to the highest one in order to be considered as a candidate as well
    const double threshold = _config_.multi_threshold;
        
    // into how many segments (in both directions) is histogram divided during each iteration (except the final one)
    // new peak is calculated on each segment 
    const int start_no_segments = _config_.multi_start_no_segments; 
        
    // both sides are dealt with separately
    for(int side = 0; side < 2; side++)
      {         
        // filtering good tracker hits for reconstruction (no missing time stamps...)
        std::vector<TKtrhitHdl> hits_from_side = TKEvent::filter_side(_event_->get_tr_hits(), side); 
        std::vector<TKtrhitHdl> hits = TKEvent::filter_usable(hits_from_side);

        if( hits.size() < 3 ) continue;
                
        // peaks_Theta, peak_R and peaks_value stores information about peak candidates
        // each iteration takes those candidates zooms around them looks for more candidates in that region
        // new set of candidates (more precise ones) comes out from each iteration
        std::vector<double> peaks_Theta = {M_PI/2.0};
        std::vector<double> peaks_R = {0.0};
        std::vector<double> peaks_value = {0.0};
                
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
            std::vector<double> peaks_Theta_temp;
            std::vector<double> peaks_R_temp;
            std::vector<double> peaks_value_temp;
                        
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
                        TH2F sinograms("sinograms",
				       "sinograms; theta; r",
				       resolution,
				       theta_min + offset,
				       theta_max + offset,
				       resolution,
				       r_min,
				       r_max);
                                                
                        double theta;
                        double r;                       
                        for(auto hit = 0u; hit < hits.size(); hit++)
                          {
                            double sigma = hits[hit]->get_sigma_R();
                            for(auto k = 0u; k <= resolution; k++)
                              {
                                theta = theta_min + ( k * (delta_phi/double(segments)) / double(resolution) );
                                // r - legendre transform of a center of a circle
                                r = hits[hit]->get_xy('x')*std::sin(theta) - hits[hit]->get_xy('y')*std::cos(theta);
                                                                
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
                                        weight = ( std::erf( (r_j2 - mu)/(std::sqrt(2.0)*sigma) ) - std::erf( (r_j1 - mu)/(std::sqrt(2.0)*sigma) ) ) / (2.0 * delta_R / double(resolution * segments));
                                                                                
                                        // result is 2D histogram of several sinusoid functions f(theta) in convolution with gauss with respect to R
                                        sinograms.Fill( theta, (r_j2 + r_j1)/2.0, weight );
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
                                if(maximum < sinograms.GetBinContent(j,k))
                                  {
                                    // information (value, bin_theta, bin_R) about peak in given segment
                                    maximum = sinograms.GetBinContent(j,k);
                                    peak_Theta = j;
                                    peak_R = k;
                                  }
                              }
                          }
                                                
                        if( save_sinograms == true ) 
                          {
                            TCanvas c2("sinograms");
			    c2.cd();
                            sinograms.SetStats(0);
                            sinograms.SetContour(100);
                            sinograms.Draw("COLZ");
                            c2.SaveAs(Form("Events_visu/sinograms-run-%d_event-%d_side-%d_iter-%d_R-%d_Th-%d.png",
					    _event_->get_run_number(),
					    _event_->get_event_number(),
					    side,
					    iter,
					    seg_r,
					    seg_theta));
                            c2.Close();
                          }
                                                
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
            double denominator = std::sqrt((a*a) + 1);       
            for(auto i = 0u; i < hits.size(); i++)
              {
                double distance_from_wire = std::abs(hits[i]->get_xy('y') - a*hits[i]->get_xy('x') - b) / denominator;
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
            double norm = 1.0 / std::sqrt(2.0*M_PI);
            for(auto i = 0u; i < hits.size();i++)
              { 
                double sigma = hits[i]->get_sigma_R();
                double association_distance = 3.0*sigma;
                if(std::abs( r + hits[i]->get_xy('x')*std::cos(theta) - hits[i]->get_xy('y')*std::sin(theta) ) < association_distance )
                  {
                    probability = norm*std::exp(-std::pow(abs( r + hits[i]->get_xy('x')*std::cos(theta) - hits[i]->get_xy('y')*std::sin(theta) ) - hits[i]->get_r() , 2)/(2.0*sigma*sigma));
                    joined_probability = joined_probability * probability / sigma; 
                  }
              }         
                        
            track->set_likelihood_R(joined_probability);
            track->reconstruct_vertical_least_square();
            _event_->get_tracks().push_back(track);                            
          }
      }
    return;
  }

  void TKalgos::reconstruct_single()
  {
    _reconstruct_single_from_hits_(_event_->get_tr_hits());
    return;
  }

  void TKalgos::_reconstruct_single_from_hits_(const std::vector<TKtrhitHdl>& _hits)
  {
    bool save_sinograms = _config_.save_sinograms; 
    gROOT->SetBatch(kTRUE);

    // resolution of both dimensions of every 2D histogram      
    auto resolution = _config_.single_resolution; 
        
    // number of zooming iterations into histogram
    const int iterations = _config_.single_iterations;
        
    // both sides are dealt with separately
    for(int side = 0; side < 2; side++)
      { 
        // filtering good tracker hits for reconstruction (no missing time stamps...)
        std::vector<TKtrhitHdl> hits_from_side = TKEvent::filter_side(_hits, side); 
        std::vector<TKtrhitHdl> hits = TKEvent::filter_usable(hits_from_side);
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
            TH2F sinograms("sinograms", "sinograms; theta; r",
			   resolution,
			   theta_min + offset,
			   theta_max + offset,
			   resolution,
			   r_min,
			   r_max);
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
                    r = hitH->get_xy('x')*std::sin(theta) - hitH->get_xy('y')*std::cos(theta);
                                        
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
                            double weight = ( std::erf( (r_j2 - mu)/(std::sqrt(2.0)*sigma) ) - std::erf( (r_j1 - mu)/(std::sqrt(2.0)*sigma) ) ) / (2.0 * delta_R / double(resolution));
                                                        
                            // result is 2D histogram of several sinusoid functions f(theta) in convolution with gauss with respect to R
                            sinograms.Fill( theta, (r_j2 + r_j1)/2.0, weight );
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
                    if(maximum < sinograms.GetBinContent(j,k))
                      {
                        // information (value, bin_theta, bin_R) about peak in given segment
                        maximum = sinograms.GetBinContent(j,k);
                        peak_Theta_bin = j;
                        peak_R_bin = k;
                      }
                  }
              }
                        
            if( save_sinograms ) 
              {
                TCanvas c2("sinograms", "sinograms", 2000, 1600);
		c2.cd();
                sinograms.SetStats(0);
                sinograms.SetContour(100);
                sinograms.Draw("COLZ");
                c2.SaveAs(Form("Events_visu/sinograms-run-%d_event-%d_side-%d_iter-%d.png",
				_event_->get_run_number(),
				_event_->get_event_number(),
				side,
				iter));
                c2.Close();
              }
                        
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
            double distance = std::fabs( hitH->get_xy('x')*std::sin(phi) - hitH->get_xy('y')*std::cos(phi) - r );
            distance = std::fabs( distance - hitH->get_r() );
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
        double quality = std::exp( -0.5*chi_squared );
                
        double norm = std::pow( 2.0*M_PI, -no_hits/2.0 ); 
        for(auto i = 0u; i < track->get_associated_tr_hits().size(); i++)
          {
            norm = norm / track->get_associated_tr_hits()[i]->get_sigma_R();
          }
        double likelihood = norm * std::exp( -0.5*chi_squared*no_hits );
                
        track->set_chi_squared_R( chi_squared );        
        track->set_quality_R( quality );        
        track->set_likelihood_R( likelihood );  

        track->reconstruct_vertical_least_square();
        //track->reconstruct_vertical_MLM();
        _event_->get_tracks().push_back(track);                                
      }
    return;
  }

  void TKalgos::reconstruct_full()
  {
    bool save_sinograms = _config_.save_sinograms;
    const double chi_square_threshold = _config_.chi_square_threshold;
    for(int side = 0; side < 2; side++)
      {
        DT_LOG_DEBUG(_config_.verbosity, "side: " << side);
        std::vector<TKtrhitHdl> hits = TKEvent::filter_side(_event_->get_tr_hits(), side); 
        hits = TKEvent::filter_usable( hits );
        hits = TKEvent::filter_distant( hits );
        auto no_hits_before = 2034u;
        while( hits.size() > 2 && no_hits_before > hits.size() )
          {
            DT_LOG_DEBUG(_config_.verbosity, "iteration start: ");
            no_hits_before = hits.size();
            bool failed = true;
            TKclusterHdl cluster = find_cluster(hits);                  
            if( cluster.get() != nullptr )
              {
                //if(debug_mode) std::clog << "find_cluster succes" << endl; 
                cluster->reconstruct_MLM( save_sinograms, _event_->get_run_number(), _event_->get_event_number() );   
                if( cluster->get_track()->get_chi_squared_R() < chi_square_threshold
		    && cluster->get_track()->get_associated_tr_hits().size() > 3 )
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
                    _event_->get_clusters().push_back( cluster );
                  }
                // else
                //   {
                //     //if(debug_mode) std::clog << "chi_squared bad" << endl; 
                //     delete cluster;
                //   }
              }
            if( failed )
              {
                TKclusterHdl cluster2 = find_cluster_legendre(hits);
                if( cluster2.get() != nullptr )
                  {
                    //if(debug_mode) std::clog << "find_cluster_legendre succes" << endl; 
                    cluster2->reconstruct_MLM( save_sinograms,
					       _event_->get_run_number(),
					       _event_->get_event_number() );
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
                        _event_->get_clusters().push_back( cluster2 );
                      } 
                    // else
                    //   {
                    //  //if(debug_mode) std::clog << "chi_squared bad" << endl; 
                    //  delete cluster;
                    //   }
                  }
              }
            hits = TKEvent::filter_unassociated( hits );
            hits = TKEvent::filter_distant( hits );
            //hits = TKEvent::filter_unclustered( hits );
          }
      }
    this->build_trajectories();
    //this->extrapolate_trajectories();
    return;
  }

  void TKalgos::reconstruct_simple()
  {
    bool save_sinograms = _config_.save_sinograms;
    const double chi_square_threshold = _config_.chi_square_threshold;
    for(int side = 0; side < 2; side++)
      {
        //if(debug_mode) std::clog << "side: " << side << endl;
        std::vector<TKtrhitHdl> hits = TKEvent::filter_side(_event_->get_tr_hits(), side); 
        hits = TKEvent::filter_usable( hits );
        hits = TKEvent::filter_distant( hits );
        if( hits.size() < 3 ) continue; 
        bool failed = true;
        TKclusterHdl cluster = find_cluster(hits);                      
        if( cluster.get() != nullptr )
          {
            //if(debug_mode) std::clog << "find_cluster succes" << endl; 
            cluster->reconstruct_MLM( save_sinograms,
				      _event_->get_run_number(), _event_->get_event_number() );
	    if( cluster->get_track()->get_chi_squared_R() < chi_square_threshold
		&& cluster->get_track()->get_associated_tr_hits().size() > 2 )
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
                _event_->get_clusters().push_back( cluster );
              }
            // else
            //   {
            //  //if(debug_mode) std::clog << "chi_squared bad" << endl; 
            //  delete cluster;
            //   }
          }
        if( failed )
          {
            TKclusterHdl cluster2 = find_cluster_legendre(hits);
            if( cluster2.get() != nullptr )
              {
                //if(debug_mode) std::clog << "find_cluster_legendre succes" << endl; 
                cluster2->reconstruct_MLM( save_sinograms,
					   _event_->get_run_number(),
					   _event_->get_event_number() );
                if( cluster2->get_track()->get_chi_squared_R() < chi_square_threshold
		    && cluster2->get_track()->get_associated_tr_hits().size() > 2 )
                  {
                    //if(debug_mode) std::clog << "chi_squared good" << endl; 
                    cluster2->detect_ambiguity_type();
                    cluster2->reconstruct_ambiguity();
                    cluster2->get_track()->calculate_tr_hit_points();
                    if(cluster2->get_track()->has_mirror_image())
                      { 
                        cluster2->get_track()->get_mirror_image()->calculate_tr_hit_points();
                      }
                    _event_->get_clusters().push_back( cluster2 );
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
    return;
  }

  void TKalgos::reconstruct_ML()
  {
    bool save_sinograms = _config_.save_sinograms;
    for(int side = 0; side < 2; side++)
      {
        std::vector<TKtrhitHdl> hits = TKEvent::filter_side(_event_->get_tr_hits(), side); 
        hits = TKEvent::filter_usable( hits );
        hits = TKEvent::filter_distant( hits );
        if( hits.size() < 3 ) continue;
                
        TKclusterHdl cluster = find_cluster(hits);
        if( cluster.get() != nullptr )
          {
            _event_->get_clusters().push_back( cluster );
            cluster->reconstruct_MLM( save_sinograms, _event_->get_run_number(), _event_->get_event_number() );
            cluster->detect_ambiguity_type();
            cluster->reconstruct_ambiguity();
          }
      }
    return;
  }


  void TKalgos::reconstruct_ML_3D()
  {
    bool save_sinograms = _config_.save_sinograms;
    for(int side = 0; side < 2; side++)
      {
        std::vector<TKtrhitHdl> hits = TKEvent::filter_side(_event_->get_tr_hits(), side); 
        hits = TKEvent::filter_usable( hits );
        hits = TKEvent::filter_distant( hits );
        if( hits.size() < 3 ) continue;
                
        TKclusterHdl cluster = find_cluster(hits);
        if( cluster.get() != nullptr )
          {
            _event_->get_clusters().push_back( cluster );
            cluster->reconstruct_MLM_3D( save_sinograms, _event_->get_run_number(), _event_->get_event_number() );
            cluster->detect_ambiguity_type();
            cluster->reconstruct_ambiguity();
          }
      }
    return;
  }

  TKclusterHdl TKalgos::find_cluster(const std::vector<TKtrhitHdl> & hits)
  {
    TKclusterHdl foundClusterHdl;
    //number of different values of phi among which the cluster is being searched for
    const int bins_phi = 100;
    const auto sideId = hits.front()->get_SRL('s');
    for(auto i = 0u; i < hits.size(); i++)
      {
        if( sideId != hits[i]->get_SRL('s'))
          {
            DT_LOG_DEBUG(_config_.verbosity, "cluster not found - hits from both sides included");
            return foundClusterHdl;
          }
      }

    if( hits.size() < 3 ) 
      {
        DT_LOG_DEBUG(_config_.verbosity, "cluster not found - too few usable hits");
        return foundClusterHdl;
      }
        
    int max_count[bins_phi] = {0};
    double argmax_R[bins_phi];
    int global_max = 0;
    for(int i = 0; i < bins_phi; i++)
      {
        std::vector<double> boundaries;
        std::vector<int> hit_count;
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
        
    std::vector<TKtrhitHdl> cluster_hits;
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
    cluster_hits = TKEvent::filter_distant(cluster_hits);//TODO is this good? 
    if(cluster_hits.size() < 3)
      {
        return foundClusterHdl;
      }
    foundClusterHdl = std::make_shared<TKcluster>(cluster_hits, phi_min, phi_max);
    return foundClusterHdl;
  }

  TKclusterHdl TKalgos::find_cluster_legendre(const std::vector<TKtrhitHdl> & hits)
  {
    TKclusterHdl clusterPtr;
    bool save_sinograms = _config_.save_sinograms;
    gROOT->SetBatch(kTRUE);

    if( hits.size() < 3 ) 
      {
        DT_LOG_DEBUG(_config_.verbosity, "cluster not found - too few usable hits");
        return clusterPtr;
      }

    const double distance_limit = _config_.legendre_distance_limit;
    const double limit_angle = _config_.legendre_limit_angle;

    // resolution of both dimensions of every 2D histogram      
    auto resolution = _config_.legendre_resolution; 
        
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
        TH2F sinograms("sinograms", "sinograms; phi; r",
		       resolution,
		       phi_min + offset,
		       phi_max + offset,
		       resolution,
		       r_min,
		       r_max);
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
                        weight = ( std::erf( (r_j2 - mu)/(std::sqrt(2.0)*sigma) ) - std::erf( (r_j1 - mu)/(std::sqrt(2.0)*sigma) ) ) / (2.0 * delta_R / double(resolution));
                                                
                        // result is 2D histogram of several sinusoid functions f(phi) in convolution with gauss with respect to R
                        sinograms.Fill( phi, (r_j2 + r_j1)/2.0, weight );
                        r_j1 = r_j2;                    
                      }                                                         
                  }                     
              } 
          }                                                                             

        // Get bin number of maximum value
        int maxBin = sinograms.GetMaximumBin();

        // Get X and Y values corresponding to the maximum bin
        int bin_phi, bin_R, bin_Z;
        sinograms.GetBinXYZ(maxBin, bin_phi, bin_R, bin_Z);
        peak_phi = sinograms.GetXaxis()->GetBinCenter(bin_phi);
        peak_R = sinograms.GetYaxis()->GetBinCenter(bin_R);
        delta_phi = delta_phi*0.1;
        delta_R = delta_R*0.1;

        if( save_sinograms ) 
          {
            TCanvas c2("sinograms", "sinograms", 2000, 1600);
	    c2.cd();
            sinograms.SetStats(0);
            sinograms.SetContour(100);
            sinograms.Draw("COLZ");
            c2.SaveAs(Form("Events_visu/clustering-run-%d_event-%d_iter-%d.png",
                            _event_->get_run_number(),
                            _event_->get_event_number(),
			    iter));
            c2.Close();
          }
      }
        
    std::vector<TKtrhitHdl> cluster_candidate = TKEvent::filter_close_hits(hits,
									   peak_phi,
									   peak_R,
									   distance_limit);
        
    cluster_candidate = TKEvent::filter_distant(cluster_candidate); //TODO is this good?
    if(cluster_candidate.size() >= 3)
      {
        clusterPtr = std::make_shared<TKcluster>(cluster_candidate,
                                                 peak_phi - limit_angle,
                                                 peak_phi + limit_angle);
      }
    return clusterPtr;
  }

  void TKalgos::build_trajectories()
  {
    DT_LOG_DEBUG(_config_.verbosity, "building trajectories...");
    std::vector<TKtrackHdl> all_tracks = _event_->get_all_tracks();
    DT_LOG_DEBUG(_config_.verbosity, "nb  tracks = " << all_tracks.size() << " track(s) available (all)");
        
    for(int side = 0; side < 2; side++)
      {
	DT_LOG_DEBUG(_config_.verbosity, "building side " << side << " ...");
        std::vector<TKtrackHdl> all_tracks_from_side;
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
	DT_LOG_DEBUG(_config_.verbosity, "nb tracks on side " << side << " = " << no_tracks << " track(s) available");
	// arrays stores information about candidates from each segment
        typedef boost::multi_array<bool, 2> array_type;
        typedef array_type::index index;
        array_type connections(boost::extents[no_tracks][no_tracks]);
        // Assign values to the elements:
        for(index ia = 0; ia != (int) no_tracks; ++ia) {
          for(index ja = 0; ja != (int) no_tracks; ++ja) {
            connections[ia][ja] = false;
	  }
	}
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
        std::vector<int> connection_counter(no_tracks, 0);
        for(auto i = 0u; i < no_tracks; i++)
          {
            for(auto j = 0u; j < no_tracks; j++)
              {
                if(connections[j][i])
                  {
                    connection_counter[i]++;
                  }
              }
          }

        std::vector<bool> trajectorized(no_tracks, false);
	DT_LOG_DEBUG(_config_.verbosity, "number of connections for each segment: ");
        if (datatools::logger::is_debug(_config_.verbosity)) {
	  for(auto i = 0u; i < no_tracks; i++) {
	    DT_LOG_DEBUG(_config_.verbosity, "connection_counter[" << i << "] = " << connection_counter[i]); 
	  }
	}
	DT_LOG_DEBUG(_config_.verbosity, "==> connection matrix:");
        if (datatools::logger::is_debug(_config_.verbosity)) {
          for(auto i = 0u; i < no_tracks; i++) {
	    std::cerr << "[debug] ";
	    for(auto j = 0u; j < no_tracks; j++) {
	      std::cerr << connections[i][j] << " ";        
	    }
	    std::cerr << std::endl;
	  }
 	}
	DT_LOG_DEBUG(_config_.verbosity, "<== connection matrix.");
	
        for(auto i = 0u; i < no_tracks; i++)
          {
	    DT_LOG_DEBUG(_config_.verbosity, "connecting track number #" << i << " ..."); 
            if(connection_counter[i] == 0)
              {
                //if(all_tracks_from_side[i]->get_associated_tr_hits().size() > 1) // in ideal case should be unnecessary
                _event_->get_trajectories().push_back(std::make_shared<TKtrajectory>(all_tracks_from_side[i]));
                trajectorized[i] = true;
		DT_LOG_DEBUG(_config_.verbosity, "  track #" << i << " is trajectorized as an unique segment"); 
              }
            else if(connection_counter[i] == 1 && not trajectorized[i])
              {
                std::vector<TKtrackHdl> composite_track;
                composite_track.push_back(all_tracks_from_side[i]);
                trajectorized[i] = true;
		DT_LOG_DEBUG(_config_.verbosity, "  track #" << i << " is trajectorized as a multiple segment"); 
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
			DT_LOG_DEBUG(_config_.verbosity, "    track #" << next_index << " is trajectorized"); 
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
			    DT_LOG_DEBUG(_config_.verbosity, "track #" << next_index << " is trajectorized"); 
                            break;
                          }
                      }
                  }
		DT_LOG_DEBUG(_config_.verbosity, "composite track size = " << composite_track.size()); 
                if(composite_track.size() > 1)
                  {
		    DT_LOG_DEBUG(_config_.verbosity, "several segments"); 
                    // 000
                    _event_->get_trajectories().push_back(std::make_shared<TKtrajectory>(composite_track));
                  }
                else if(composite_track.size() == 1)
                  {
		    DT_LOG_DEBUG(_config_.verbosity, "only one segment"); 
                    //if(composite_track[0]->get_associated_tr_hits().size() > 1) // in ideal case should be unnecessary
                    _event_->get_trajectories().push_back(std::make_shared<TKtrajectory>(composite_track.front()));
                  }
              }
          }
      }
    DT_LOG_DEBUG(_config_.verbosity, "===> trajectories:");
    if (datatools::logger::is_debug(_config_.verbosity)) {
      for(auto i = 0u; i < _event_->get_trajectories().size(); i++) {
	std::cerr << "[debug] trajectory #" << i << " :\n";
	_event_->get_trajectories()[i]->print(std::cerr);
      }
    }
    DT_LOG_DEBUG(_config_.verbosity, "<=== trajectories.");
    
    return;
  }

  void TKalgos::extrapolate_trajectories()
  {
    for(auto & trajectoryHdl : _event_->get_trajectories())
      {
        trajectoryHdl->extrapolate();
      }
    return;
  }

  void TKalgos::calculate_tr_hit_points()
  {
    std::vector<TKtrackHdl> all_tracks = _event_->get_all_tracks();
    for(auto & trackHdl : all_tracks)
      {
        trackHdl->calculate_tr_hit_points();
      }
    return;
  }
  
} //  end of namespace tkrec

