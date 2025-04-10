// TK headers
#include "tkrec/TKvisu.h"

#include <filesystem>

// ROOT headers
#include "TCanvas.h"
#include "TColor.h"
#include "TEllipse.h"
#include "TH2D.h"
#include "TAttLine.h"
#include "TGLViewer.h"
#include "TGeoManager.h"
#include "TGeoVolume.h"
#include "TFile.h"
#include "TROOT.h"
#include "TPolyLine3D.h"
#include "TPolyLine.h"
#include "TBox.h"
#include "TLatex.h"
#include "TLine.h"
#include "TPoint.h"
#include <TStyle.h>
#include <TF1.h>

namespace tkrec {

  TKvisu::TKvisu(const TKgeom & geom_, const std::string & visu_dir_)
    : _geom_(geom_)
  {
    if (not visu_dir_.empty()) {
      _visu_dir_ = visu_dir_;
    } else {
      _visu_dir_ = "./Events_visu";
    }
    std::filesystem::path visuDirPath(_visu_dir_);
    if (not std::filesystem::exists(visuDirPath)) {
      DT_LOG_DEBUG(verbosity, "Creating visu directory '" + _visu_dir_ + "'...");
      std::filesystem::create_directories(visuDirPath);
    }
    return;
  }

  bool TKvisu::has_event() const
  {
    return _event_ != nullptr;
  }

  void TKvisu::set_event(const TKEvent & event_)
  {
    _event_ = &event_;
    return;
  }
                
  void TKvisu::make_top_projection(const uint32_t hits_option_,
				   const uint32_t tracking_option_) const
  {
    gROOT->SetBatch(true);
    TCanvas *canvas = new TCanvas("canvas","", 5800, 1600);     
    canvas->Range(-2900.0, -700.0, 2900.0, 900.0);
    TLatex* title = new TLatex(-2600.0, 700.0,
                               Form("Run %d | Event %d",
                                    _event_->run_number,
                                    _event_->event_number));
    title->SetTextSize(0.07);
    title->Draw();

    // Drawing mainwall and mainwall calo hits
    std::vector<TBox*> calorimeter_blocks;
    std::vector<TLatex*> calorimeter_block_titles;
    for(int om_side = 0; om_side < 2; om_side++) 
      {
        for(int om_column = 0; om_column < 20; om_column++) 
          {
            int swcr[4] = {om_side, -1, om_column, 0};
            auto ohit = std::make_shared<TKOMhit>(swcr);        
            TBox *calo = new TBox(ohit->get_xyz('y') - _geom_.mw_sizey/2.0, 
                                  -(ohit->get_xyz('x') + _geom_.mw_sizex/2.0), 
                                  ohit->get_xyz('y') + _geom_.mw_sizey/2.0, 
                                  -(ohit->get_xyz('x') - _geom_.mw_sizex/2.0));
                        
            TLatex *tex = new TLatex(ohit->get_xyz('y') - _geom_.mw_sizey/4.0,
                                     -ohit->get_xyz('x'),
                                     Form("%d.%d.%d.-",
                                          ohit->get_SWCR('s'),
                                          ohit->get_SWCR('w'),
                                          ohit->get_SWCR('c')));
            tex->SetTextSize(0.035);
                        
            calorimeter_blocks.push_back(calo);
            calorimeter_block_titles.push_back(tex);
                        
            bool is_hit = false;
            for(auto hit = 0u; hit < _event_->OM_hits.size(); hit++)
              {
                const auto & omHit = _event_->OM_hits[hit];
                if(om_side   == omHit->get_SWCR('s') && 
                   -1   == omHit->get_SWCR('w') && 
                   om_column == omHit->get_SWCR('c') ) 
                  {
                    is_hit = true;
                  }
              }
            if(is_hit)
              {
                calo->SetFillColor(kRed);       
              }
            else
              {
                //calo->SetLineColor(kGray);    
              }
            calo->SetLineColor(kBlack); 
            calo->SetLineWidth(2);
            /*
              if(om_side == 0 && om_column == 0) calo->Draw();
              else calo->Draw("same");
            */
            calo->Draw("same");
            if(is_hit)
              {
                //tex->Draw("same");
              }
          }
      }
        
    // Drawing Xwall and Xwall calo hits
    for(int om_side = 0; om_side < 2; om_side++) 
      {
        for(int om_wall = 0; om_wall < 2; om_wall++) 
          {
            for(int om_column = 0; om_column < 2; om_column++) 
              {
                int swcr[4] = {om_side, om_wall, om_column, 0};
                auto ohit = std::make_shared<TKOMhit>(swcr); 
                                                                
                TBox *calo = new TBox(ohit->get_xyz('y') - _geom_.xw_sizey/2.0, 
                                      -(ohit->get_xyz('x') + _geom_.xw_sizex/2.0), 
                                      ohit->get_xyz('y') + _geom_.xw_sizey/2.0, 
                                      -(ohit->get_xyz('x') - _geom_.xw_sizex/2.0));
                calorimeter_blocks.push_back(calo);
                                
                bool is_hit = false;
                for(auto hit = 0u; hit < _event_->OM_hits.size(); hit++)
                  {
                    const auto & omHit = _event_->OM_hits[hit];
                    if(om_side   == omHit->get_SWCR('s') && 
                       om_wall   == omHit->get_SWCR('w') && 
                       om_column == omHit->get_SWCR('c') )  
                      {
                        is_hit = true;
                      }
                  }
                if(is_hit)
                  {
                    calo->SetFillColor(kRed);   
                  }
                else
                  {
                    //calo->SetLineColor(kGray);        
                  }
                calo->SetLineColor(kBlack);     
                calo->SetLineWidth(2);
                calo->Draw("same");     
              } 
          }
      }
        
    // Drawing tracker and tracker hits
    std::vector<TEllipse*> tracker_hits;
    for(int cell_num = 0; cell_num < 2034; cell_num++)
      {
        auto thit = std::make_shared<TKtrhit>(cell_num); 
                
        double radius = _geom_.tc_radius;
        double sigma = std::numeric_limits<double>::quiet_NaN();
        bool is_hit = false;
        bool is_broken = false;
        bool is_associated = false;
        bool has_height = false;
                
        for(const auto& hit : _event_->tr_hits)
          {
            if(cell_num == hit->get_cell_num())
              {
                is_hit = true;
                if( hit->get_r() > 35.0 || hit->get_r() == -1.0 )
                  {
                    is_broken = true;
                  }
                else
                  {
                    radius = hit->get_r();
                    sigma = hit->get_sigma_R();
                  }     
                                
                if( hit->has_associated_track() )
                  {
                    is_associated = true;
                  }
                                
                if( hit->get_h() != 0.0 )
                  {
                    has_height = true;
                  }             
                break;
              }
          }
                
        TEllipse* tracker_cell = nullptr;
        if(is_hit)
          {
            if(is_broken)
              {
                tracker_cell = new TEllipse(thit->get_xy('y'),
                                            -thit->get_xy('x'), radius, radius);
                tracker_hits.push_back(tracker_cell);
                tracker_cell->SetLineWidth(1);
                switch(hits_option_)
                  {
                  case 0:
                    break;
                  case 1:
                    tracker_cell->SetFillColor(kOrange);
                    tracker_cell->Draw("same"); 
                    break;
                  case 2:
                    tracker_cell->SetFillColor(kOrange);
                    tracker_cell->Draw("same");                 
                    break;
                  case 3:
                    tracker_cell->SetFillColor(kOrange);
                    tracker_cell->Draw("same");                 
                    break;              
                  }
                //delete tracker_cell;
              }
            else
              {
                tracker_cell = new TEllipse(thit->get_xy('y'),
                                            -thit->get_xy('x'),
                                            radius + sigma,
                                            radius + sigma);
                tracker_hits.push_back(tracker_cell);
                tracker_cell->SetLineWidth(0);  
                                
                if(is_associated)
                  {
                    switch(hits_option_)
                      {
                      case 0:
                        tracker_cell->SetFillColor(kRed);
                        break;
                      case 1:
                        tracker_cell->SetFillColor(kRed);
                        break;
                      case 2:
                        tracker_cell->SetFillColor(kGreen);                     
                        break;
                      case 3:
                        if(has_height)
                          {
                            tracker_cell->SetFillColor(kGreen);                 
                          }
                        else
                          {                                                     
                            tracker_cell->SetFillColor(kTeal);
                          }
                        break;          
                      }
                  } 
                else
                  {
                    switch(hits_option_)
                      {
                      case 3:
                        if(has_height)
                          {
                            tracker_cell->SetFillColor(kRed);
                          }
                        else
                          {                                                     
                            tracker_cell->SetFillColor(kMagenta);
                          }
                        break;
                                                
                      default:
                        tracker_cell->SetFillColor(kRed);
                        break;
                      }
                  }
                                
                tracker_cell->Draw("same");
                if( radius - sigma > 0.0 )
                  {
                    TEllipse* tracker_cell_in
                      = new TEllipse(thit->get_xy('y'),
                                     -thit->get_xy('x'),
                                     radius - sigma,
                                     radius - sigma);   
                    tracker_hits.push_back(tracker_cell_in);
                    tracker_cell_in->SetLineWidth(0);
                    tracker_cell_in->Draw("same");
                  }
              }
          }
        else
          {
            tracker_cell = new TEllipse(thit->get_xy('y'),
                                        -thit->get_xy('x'),
                                        radius,
                                        radius);
            tracker_hits.push_back(tracker_cell);
            tracker_cell->SetLineWidth(1);
            tracker_cell->Draw("same");
          }
      }
        
    // Drawing Bi sources
    std::vector<TBox*> sources;
    for(int column = 0; column < 6; column++)
      {
        TBox *Bi_source = new TBox((column-2.5)*_geom_.Bi_source_dist_y - _geom_.Bi_source_y/2.0,
                                   -(-_geom_.Bi_source_x/2.0),
                                   (column-2.5)*_geom_.Bi_source_dist_y + _geom_.Bi_source_y/2.0,
                                   -(_geom_.Bi_source_x/2.0));
        sources.push_back(Bi_source);
                
        Bi_source->SetFillColor(kBlue); 
        
        Bi_source->SetLineWidth(2);
        Bi_source->Draw("same");
      }
        
    // Drawing tracks
    std::vector<TLine*> lines;
    if(tracking_option_ & show_tracking_tracks)
      {
        std::vector<ConstTKtrackHdl> all_tracks = _event_->get_all_tracks();
        for (auto i = 0u; i < all_tracks.size(); i++)
          {
            const auto & trackPtr = all_tracks[i];
            TLine* trackLine = nullptr;
            double x, y;
                        
            x = 435.0;
            if( trackPtr->get_side() == 0) 
              {
                x = -x;
              }         
            y = trackPtr->get_a()*x + trackPtr->get_b();
                        
            if( y > 2505.5 )
              {
                x = (2505.5-trackPtr->get_b())/trackPtr->get_a();
                y = trackPtr->get_a()*x + trackPtr->get_b();
                                
              }
            else if( y < -2505.5 )
              {
                x = (-2505.5-trackPtr->get_b())/trackPtr->get_a();
                y = trackPtr->get_a()*x + trackPtr->get_b();
              }
                        
            trackLine = new TLine(trackPtr->get_b(), 0.0, y, -x); // XXX memory leak with root???       
            trackLine->SetLineColor(kBlue);
            trackLine->SetLineWidth(2);
            trackLine->Draw("same");
            lines.push_back(trackLine);         
          }
      }
        
    // Drawing trajectories
    std::vector<TPolyLine*> polylines;
    if(tracking_option_ & show_tracking_trajectories)
      {
        for (auto i = 0u; i < _event_->trajectories.size(); i++)
          {
            TPolyLine* trajectory = new TPolyLine();
            polylines.push_back(trajectory);
            trajectory->SetLineColor(kOrange - 3);
            trajectory->SetLineWidth(4);
            const auto & evTraj = _event_->trajectories[i];
            for (auto j = 0u; j < evTraj->get_track_points().size(); j++)
              {
                const ConstTKpointHdl & point = evTraj->get_track_points()[j];
                trajectory->SetPoint(j, point->get_y(), -point->get_x());
              }
            trajectory->Draw("same");
          }
      }
        
    // Drawing avalanche origin points
    std::vector<TGraph*> avalanche_origins;
    std::vector<ConstTKtrackHdl> all_tracks = _event_->get_all_tracks();
    if(tracking_option_ & show_tracking_hit_avalanches)
      {
        for(const auto & track : all_tracks)
          {
            if(track->get_associated_tr_hit_points().size() > 0)
              {
                TGraph *graph = new TGraph();
                avalanche_origins.push_back(graph);
                for (auto j = 0u; j < track->get_associated_tr_hit_points().size(); j++)
                  {     
                    const ConstTKpointHdl & point = track->get_associated_tr_hit_points()[j];
                    graph->SetPoint(j, point->get_y(), -point->get_x());
                  }
                graph->SetMarkerColor(kRed);
                graph->SetMarkerStyle(kFullCircle);
                graph->SetMarkerSize(1);
                graph->Draw("sameP");
              }
          }
      }

    canvas->SaveAs(Form("%s/Run-%d_event-%d_2D.png",
			_visu_dir_.c_str(),
                        _event_->get_run_number(),
                        _event_->get_event_number()));
    delete canvas;
        
    for (auto& box : calorimeter_blocks) delete box;
    for (auto& aTitle : calorimeter_block_titles) delete aTitle;
    for (auto& tracker_cell : tracker_hits) delete tracker_cell;
    for (auto& Bi_source : sources) delete Bi_source;
    for (auto& track : lines) delete track;
    for (auto& trajetory : polylines) delete trajetory;
    for (auto& avalanche_origin : avalanche_origins) delete avalanche_origin;
    return;
  }
                
  void TKvisu::build_event(const uint32_t tracking_option_) const
  {     
    gROOT->SetBatch(true);
        
    TFile *file = new TFile(Form("%s/Run-%d_event-%d_3D.root",
				 _visu_dir_.c_str(),
                                 _event_->get_run_number(),
                                 _event_->get_event_number()), "RECREATE");
        
    TGeoManager *geom = new TGeoManager();
    TGeoMaterial *matVacuum = new TGeoMaterial("matVacuum", 0, 0, 0);    
    TGeoMedium *Vacuum = new TGeoMedium("Vacuum",1, matVacuum);
    TGeoVolume *top = gGeoManager->MakeBox("top", Vacuum, 1500, 1750, 2500);
    geom->SetTopVolume(top);

    int object_counter = 0;

    // Drawing calorimeter
    for(int omnum = 0; omnum < 712; omnum++) 
      {
        // Skipping calo hits
        bool is_hit = false;
        for(auto hit = 0u; hit < _event_->OM_hits.size(); hit++)
          {
            if(omnum == _event_->OM_hits[hit]->get_OM_num()) {
              is_hit = true;
            }
          }
        if (is_hit) continue;
                
        TGeoVolume * calo = nullptr;
                
        auto ohit = std::make_shared<TKOMhit>(omnum); 
                
        if(omnum < 520) // Mainwall
          {
            calo = gGeoManager->MakeBox(Form("OM:%d.%d.%d.%d",
					     ohit->get_SWCR('s'),
					     ohit->get_SWCR('w'),
					     ohit->get_SWCR('c'),
					     ohit->get_SWCR('r')),
					Vacuum,
					_geom_.mw_sizex/2.0,
					_geom_.mw_sizey/2.0,
					_geom_.mw_sizez/2.0);
          }
        else if(omnum < 648) // Xwall
          {
            calo = gGeoManager->MakeBox(Form("OM:%d.%d.%d.%d",
					     ohit->get_SWCR('s'),
					     ohit->get_SWCR('w'),
					     ohit->get_SWCR('c'),
					     ohit->get_SWCR('r')),
					Vacuum,
					_geom_.xw_sizex/2.0,
					_geom_.xw_sizey/2.0,
					_geom_.xw_sizez/2.0);
          }
        else // Gveto
          {
            calo = gGeoManager->MakeBox(Form("OM:%d.%d.%d.%d",
					     ohit->get_SWCR('s'),
					     ohit->get_SWCR('w'),
					     ohit->get_SWCR('c'),
					     ohit->get_SWCR('r')),
					Vacuum,
					_geom_.gv_sizex/2.0,
					_geom_.gv_sizey/2.0,
					_geom_.gv_sizez/2.0);
          }     
                
        TGeoHMatrix *trans = new TGeoHMatrix("Trans");
                
        trans->SetDx(ohit->get_xyz('x'));
        trans->SetDy(ohit->get_xyz('y'));
        trans->SetDz(ohit->get_xyz('z'));
                
        calo->SetLineColor(kGray);      
                
        top->AddNode(calo, object_counter, trans);
        object_counter++;
      }

    // Drawing Bi calibration sources
    for(int row = 0; row < 7; row++)
      {    
        for(int column = 0; column < 6; column++)
          {
            TGeoVolume *Bi_source = gGeoManager->MakeBox("Bi_source",
							 Vacuum,
							 _geom_.Bi_source_x/2.0,
							 _geom_.Bi_source_y/2.0,
							 _geom_.Bi_source_z/2.0);
                
            TGeoHMatrix *trans = new TGeoHMatrix("Trans");
            trans->SetDy( (column - 2.5) * _geom_.Bi_source_dist_y );
            trans->SetDz( (row - 3.0) * _geom_.Bi_source_dist_z );
                        
            Bi_source->SetLineColor(kCyan);
            Bi_source->SetLineWidth(3);
                        
            top->AddNode(Bi_source, object_counter, trans);
            object_counter++;
          }
      }

    // Adding calo hits
    for(auto hit = 0u; hit < _event_->OM_hits.size(); hit++)
      {
        const auto & omHit = _event_->OM_hits[hit];
        TGeoVolume *box;
        if(omHit->get_OM_num() < 520) // Mainwall
          {
            box = gGeoManager->MakeBox(Form("OM:%d.%d.%d.%d",
					    omHit->get_SWCR('s'),
					    omHit->get_SWCR('w'),
					    omHit->get_SWCR('c'),
					    omHit->get_SWCR('r')),
				       Vacuum,
				       _geom_.mw_sizex/2.0,
				       _geom_.mw_sizey/2.0,
				       _geom_.mw_sizez/2.0);
          }
        else if(omHit->get_OM_num() < 648) // Xwall
          {
            box = gGeoManager->MakeBox(Form("OM:%d.%d.%d.%d",
					    omHit->get_SWCR('s'),
					    omHit->get_SWCR('w'),
					    omHit->get_SWCR('c'),
					    omHit->get_SWCR('r')),
				       Vacuum,
				       _geom_.xw_sizex/2.0,
				       _geom_.xw_sizey/2.0,
				       _geom_.xw_sizez/2.0);
          }
        else // Gveto
          {
            box = gGeoManager->MakeBox(Form("OM:%d.%d.%d.%d",
					    omHit->get_SWCR('s'),
					    omHit->get_SWCR('w'),
					    omHit->get_SWCR('c'),
					    omHit->get_SWCR('r')),
				       Vacuum,
				       _geom_.gv_sizex/2.0,
				       _geom_.gv_sizey/2.0,
				       _geom_.gv_sizez/2.0);
          }     
                
        TGeoHMatrix *trans = new TGeoHMatrix("Trans");
                
        trans->SetDx(omHit->get_xyz('x'));
        trans->SetDy(omHit->get_xyz('y'));
        trans->SetDz(omHit->get_xyz('z'));
                
        if(omHit->is_HT())
          {
            box->SetLineColor(kRed);
            box->SetLineWidth(3);                                               
          }
        else
          {
            box->SetLineColor(kOrange);
            box->SetLineWidth(2);                                               
          }

        top->AddNode(box, object_counter, trans);
        object_counter++;
      }

    // Adding tracker hits
    for(auto hit = 0u; hit < _event_->tr_hits.size(); hit++)
      {
        const auto & trkHit = _event_->tr_hits[hit];
        double radius = _geom_.tc_radius; // default value
        double sigma_R = 2.0; //default value
        double sigma_Z = 17.0; //trkHit->get_sigma_Z();
        bool is_broken = false;
                
        if( trkHit->get_r() > 35.0 || trkHit->get_r() == -1.0 ) 
          {
            is_broken = true;
          }
                
        if( !is_broken )
          {
            radius = trkHit->get_r();
            sigma_R = trkHit->get_sigma_R();
          }
                
        double radius_min = radius - sigma_R; 
        if( radius_min <= 0 )
          {
            radius_min = 0.0;
          }
        
        TGeoVolume *tracker_cell = geom->MakeTube(Form("cell:%d.%d.%d",
						       trkHit->get_SRL('s'),
						       trkHit->get_SRL('r'),
						       trkHit->get_SRL('l')),
						  Vacuum,
						  radius_min,
						  radius + sigma_R,
						  sigma_Z);
        TGeoHMatrix *trans = new TGeoHMatrix("Trans");
                        
        trans->SetDx( trkHit->get_xy('x') );
        trans->SetDy( trkHit->get_xy('y') );
        trans->SetDz( trkHit->get_h() );
                
        if( is_broken )
          {
            tracker_cell->SetLineColor(kOrange);
            tracker_cell->SetLineWidth(1);              
          }
        else if( trkHit->get_h() == 0.0 )
          {
            tracker_cell->SetLineColor(kMagenta);
            tracker_cell->SetLineWidth(1);
          }
        else
          {
            tracker_cell->SetLineColor(kRed);
            tracker_cell->SetLineWidth(1);
          }
                
        top->AddNode(tracker_cell, object_counter, trans);
        object_counter++;
      }
        
    // Close geometry and write to file
    geom->CloseGeometry(); 
    file->WriteObject(top, "demonstrator");

    if(tracking_option_ & show_tracking_tracks)
      {
        std::vector<ConstTKtrackHdl> all_tracks = _event_->get_all_tracks(); 
        for(auto i = 0u; i < all_tracks.size(); i++)
          {
            const auto & trackPtr = all_tracks[i];
            TPolyLine3D *track = new TPolyLine3D();
            track->SetPoint(0, 0.0, trackPtr->get_b(), trackPtr->get_d());
            double x,y,z;
                        
            x = 435.0;
            if( trackPtr->get_side() == 0 ) 
              {
                x = -x;
              }         
            y = trackPtr->get_a()*x + trackPtr->get_b();
                        
            if( y > 2505.5 )
              {
                x = (2505.5-trackPtr->get_b())/trackPtr->get_a();
                y = trackPtr->get_a()*x + trackPtr->get_b();
                                
              }
            else if( y < -2505.5 )
              {
                x = (-2505.5-trackPtr->get_b())/trackPtr->get_a();
                y = trackPtr->get_a()*x + trackPtr->get_b();
              }
            z = trackPtr->get_c()*x + trackPtr->get_d();
                        
            if( z > 1550.0 )
              {
                x = (1550.0-trackPtr->get_d())/trackPtr->get_c();
                y = trackPtr->get_a()*x + trackPtr->get_b();
                z = trackPtr->get_c()*x + trackPtr->get_d();
              }
            else if( z < -1550.0 )
              {
                x = (-1550.0-trackPtr->get_d())/trackPtr->get_c();
                y = trackPtr->get_a()*x + trackPtr->get_b();
                z = trackPtr->get_c()*x + trackPtr->get_d();
              }
            track->SetPoint(1, x, y, z);                        
            track->SetLineColor(kBlue);
            track->SetLineWidth(2);
            file->WriteObject(track, Form("track-%d", i));
            delete track;
          }
      }
        
    if(tracking_option_ & show_tracking_hit_avalanches)
      {
        for (auto i = 0u; i < _event_->trajectories.size(); i++)
          {
	    const auto & evTraj = _event_->trajectories[i];
            TPolyLine3D* trajectory = new TPolyLine3D();
            trajectory->SetLineColor(kOrange - 3);
            trajectory->SetLineWidth(4);
            for (auto j = 0u; j < evTraj->get_track_points().size(); j++)
              {
                const ConstTKpointHdl & point = evTraj->get_track_points()[j];
                trajectory->SetPoint(j, point->get_x(), point->get_y(), point->get_z());
              }
            file->WriteObject(trajectory, Form("trajectory-%d", i));
            delete trajectory;
          }     
      }

    // Close file and delete dynamically allocated objects
    file->Close();              
    delete file;        
    delete geom;        
    return;
  }

} // end of namespace tkrec
