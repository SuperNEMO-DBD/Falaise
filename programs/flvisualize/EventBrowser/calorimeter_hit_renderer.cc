/* calorimeter_hit_renderer.cc
 *
 * Copyright (C) 2011 Xavier Garrido <garrido@lal.in2p3.fr>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 */

#include <EventBrowser/view/browser_tracks.h>
#include <EventBrowser/view/calorimeter_hit_renderer.h>
#include <EventBrowser/view/options_manager.h>
#include <EventBrowser/view/style_manager.h>
#include <EventBrowser/detector/detector_manager.h>
#include <EventBrowser/detector/i_root_volume.h>
#include <EventBrowser/io/event_server.h>
#include <EventBrowser/utils/root_utilities.h>
#include <EventBrowser/log.h>

#include <geomtools/id_mgr.h>

#include <falaise/snemo/datamodels/geomid_utils.h>

#include <TColor.h>
#include <TMarker3DBox.h>
// #include <TObjArray.h>
#include <TPolyMarker3D.h>

#include <TROOT.h>
#include <TH1F.h>
#include <TCanvas.h>

namespace snemo {

  namespace visualization {

    namespace view {

      // ctor:
      calorimeter_hit_renderer::calorimeter_hit_renderer() = default;

      // dtor:
      calorimeter_hit_renderer::~calorimeter_hit_renderer() = default;

      void calorimeter_hit_renderer::push_simulated_hits(const std::string& hit_category_) {
	FL_LOG_DEVEL("Entering...");
	FL_LOG_DEVEL("Hit category = " << hit_category_);
	const io::event_record& event = _server->get_event();
	const auto& sim_data = event.get<mctools::simulated_data>(io::SD_LABEL);

	if (!sim_data.has_step_hits(hit_category_)) {
	  DT_LOG_DEBUG(options_manager::get_instance().get_logging_priority(),
		       "Event has no '" << hit_category_ << "' calorimeter hits");
	  FL_LOG_DEVEL("Event has no '" << hit_category_ << "' calorimeter hits");
	  FL_LOG_DEVEL("Exiting...");
	  return;
	}

	const mctools::simulated_data::hit_handle_collection_type& hit_collection =
	  sim_data.get_step_hits(hit_category_);
	if (hit_collection.empty()) {
	  DT_LOG_DEBUG(options_manager::get_instance().get_logging_priority(),
		       "No simulated calorimeter hits");
	  FL_LOG_DEVEL("No simulated calorimeter hits");
	  FL_LOG_DEVEL("Exiting...");
	  return;
	}

	for (const auto& it_hit : hit_collection) {
	  const mctools::base_step_hit& a_hit = it_hit.get();

	  const geomtools::vector_3d& pstart = a_hit.get_position_start();
	  const geomtools::vector_3d& pstop = a_hit.get_position_stop();

	  const double dx = std::abs(0.5 * (pstart.x() - pstop.x()));
	  const double dy = std::abs(0.5 * (pstart.y() - pstop.y()));
	  const double dz = std::abs(0.5 * (pstart.z() - pstop.z()));

	  const geomtools::vector_3d pos = 0.5 * (pstart + pstop);

	  auto* step_3d = new TMarker3DBox;
	  _objects->Add(step_3d);
	  step_3d->SetPosition(pos.x(), pos.y(), pos.z());
	  step_3d->SetSize(dx, dy, dz);
	  step_3d->SetLineColor(kRed);

	  // // Store this value into cluster properties:
	  // io::step_hit_type * mutable_hit = const_cast<io::step_hit_type*>(&(a_hit));
	  // datatools::utils::properties & hit_properties = mutable_hit->grab_auxiliaries();
	  // const long pixel = TColor::Number2Pixel(kRed);
	  // const string hex_str = TColor::PixelAsHexString(pixel);
	  // hit_properties.update(browser_tracks::COLOR_FLAG, hex_str);

	  // Retrieve line width from properties if 'hit' is highlighted:
	  size_t line_width = style_manager::get_instance().get_mc_line_width();
	  if (a_hit.get_auxiliaries().has_flag(browser_tracks::HIGHLIGHT_FLAG)) {
	    line_width = 3;
	    auto* mark1 = new TPolyMarker3D;
	    _objects->Add(mark1);
	    mark1->SetMarkerColor(kRed);
	    mark1->SetMarkerStyle(kCircle);
	    mark1->SetPoint(0, pstart.x(), pstart.y(), pstart.z());
	    auto* mark2 = new TPolyMarker3D;
	    _objects->Add(mark2);
	    mark2->SetMarkerColor(kRed);
	    mark2->SetMarkerStyle(kCircle);
	    mark2->SetPoint(0, pstop.x(), pstop.y(), pstop.z());
	  }
	  // hit_properties.update(browser_tracks::HIGHLIGHT_FLAG, false);
	  step_3d->SetLineWidth(line_width);

	  this->highlight_geom_id(a_hit.get_geom_id(), kRed);
	}  // end of step collection
	FL_LOG_DEVEL("Exiting...");
      }

      void calorimeter_hit_renderer::push_precalibrated_hits() {
	FL_LOG_DEVEL("Entering...");
	const io::event_record& event = _server->get_event();
	const auto& precalib_data = event.get<snemo::datamodel::precalibrated_data>(io::pCD_LABEL);

	const snemo::datamodel::PreCalibratedCalorimeterHitHdlCollection& pcc_collection =
	  precalib_data.calorimeter_hits();

	if (pcc_collection.empty()) {
	  DT_LOG_DEBUG(options_manager::get_instance().get_logging_priority(),
		       "No calibrated calorimeter hits");
	  FL_LOG_DEVEL("Exiting...");
	  return;
	}

	for (const auto& it_hit : pcc_collection) {
	  const snemo::datamodel::precalibrated_calorimeter_hit& a_hit = it_hit.get();

	  this->highlight_geom_id(a_hit.get_geom_id(), style_manager::get_instance().get_precalibrated_data_color());

	  // if (options_manager::get_instance().get_option_flag(SHOW_PRECALIBRATED_INFO)) {
	  // }

	  // Draw UDD waveform
	  if (a_hit.get_auxiliaries().has_flag(browser_tracks::HIGHLIGHT_FLAG)) {

	    const std::string UDD_parent_key = "UDD.parent";
	    const datatools::properties & calo_hit_properties = a_hit.get_auxiliaries();

	    if (calo_hit_properties.has_key(UDD_parent_key)) {

	      // retrieve parent UDD calorimeter hit index
	      const int UDD_parent_index = calo_hit_properties.fetch_integer(UDD_parent_key);

	      // retrieve UDD calorimeter hit waveform
	      const auto & udd = event.get<snemo::datamodel::unified_digitized_data>(io::UDD_LABEL);
	      const auto & udd_hit = udd.get_calorimeter_hits().at(UDD_parent_index);
	      const std::vector<int16_t> & waveform = udd_hit->get_waveform();

	      const std::string waveform_histo_name (Form("udd_waveform_hit_%02d", a_hit.get_hit_id()));
	      const std::string waveform_histo_title (snemo::datamodel::om_label(a_hit.get_geom_id()));

	      TH1F *waveform_histo = new TH1F (waveform_histo_name.c_str(), waveform_histo_title.c_str(), 1024, 0, 1024);
	      waveform_histo->GetXaxis()->SetTitle("Sample");
	      waveform_histo->GetYaxis()->SetTitle("ADC");
	      // _objects->Add(waveform_histo);

	      for (size_t s=0; s<waveform.size(); s++)
		waveform_histo->SetBinContent(1+s, waveform[s]);

	      TCanvas *udd_waveform_canvas = (TCanvas*)(gROOT->FindObject("udd_waveform_canvas"));

	      if (udd_waveform_canvas == nullptr) {
		udd_waveform_canvas = new TCanvas ("udd_waveform_canvas", "UDD waveform");
		_objects->Add(udd_waveform_canvas);
		waveform_histo->Draw();
	      } else {
		udd_waveform_canvas->cd();
		waveform_histo->Draw("same");
	      }

	      // if (options_manager::get_instance().get_option_flag(SHOW_PRECALIBRATED_INFO)) {
	      // annotate UDD firmware measurement ?!
	      // }

	    } else {
	      // "UDD.parent" properties missing
	    }

	  } // if (HIGHLIGHT_FLAG)

	}
	FL_LOG_DEVEL("Exiting...");
      }

      void calorimeter_hit_renderer::push_calibrated_hits() {
	FL_LOG_DEVEL("Entering...");
	const io::event_record& event = _server->get_event();
	const auto& calib_data = event.get<snemo::datamodel::calibrated_data>(io::CD_LABEL);

	const snemo::datamodel::CalorimeterHitHdlCollection& cc_collection =
	  calib_data.calorimeter_hits();

	if (cc_collection.empty()) {
	  DT_LOG_DEBUG(options_manager::get_instance().get_logging_priority(),
		       "No calibrated calorimeter hits");
	  FL_LOG_DEVEL("Exiting...");
	  return;
	}

	for (const auto& it_hit : cc_collection) {
	  const snemo::datamodel::calibrated_calorimeter_hit& a_hit = it_hit.get();

	  this->highlight_geom_id(a_hit.get_geom_id(),
				  style_manager::get_instance().get_calibrated_data_color());

	  if (options_manager::get_instance().get_option_flag(SHOW_CALIBRATED_INFO)) {
	    const double energy = a_hit.get_energy();
	    const double sigma_e = a_hit.get_sigma_energy();
	    const double time = a_hit.get_time();
	    const double sigma_t = a_hit.get_sigma_time();

	    // Save z position inside text and then parse it
	    std::ostringstream oss;
	    oss.precision(2);
	    oss << std::fixed << "#splitline{E = ";
	    utils::root_utilities::get_prettified_energy(oss, energy, sigma_e, true);
	    oss << "}{t  = ";
	    utils::root_utilities::get_prettified_time(oss, time, sigma_t, true);
	    oss << "}";
	    this->highlight_geom_id(a_hit.get_geom_id(),
				    style_manager::get_instance().get_calibrated_data_color(), oss.str());
	  }

	  // Draw calibrated UDD waveform
	  if (a_hit.get_auxiliaries().has_flag(browser_tracks::HIGHLIGHT_FLAG)) {

	    const std::string UDD_parent_key = "UDD.parent";
	    const std::string pCD_parent_key = "pCD.parent";

	    const datatools::properties & calo_hit_properties = a_hit.get_auxiliaries();

	    if (calo_hit_properties.has_key(UDD_parent_key)) {

	      // retrieve parent UDD and pCD calorimeter hit index
	      const int UDD_parent_index = calo_hit_properties.fetch_integer(UDD_parent_key);
	      const int pCD_parent_index = calo_hit_properties.fetch_integer(pCD_parent_key);

	      // retrieve UDD calorimeter hit waveform
	      const auto & udd = event.get<snemo::datamodel::unified_digitized_data>(io::UDD_LABEL);
	      const auto & udd_hit = udd.get_calorimeter_hits().at(UDD_parent_index);
	      const std::vector<int16_t> & waveform = udd_hit->get_waveform();

	      const auto & pcd = event.get<snemo::datamodel::precalibrated_data>(io::pCD_LABEL);
	      const auto & pcd_hit = pcd.calorimeter_hits().at(pCD_parent_index);
	      const double & pcd_baseline = pcd_hit->get_baseline()/(1E-3*CLHEP::volt);

	      const std::string waveform_histo_name (Form("pcd_waveform_hit_%02d", a_hit.get_hit_id()));
	      const std::string waveform_histo_title (snemo::datamodel::om_label(a_hit.get_geom_id()));

	      TH1F *waveform_histo = new TH1F (waveform_histo_name.c_str(), waveform_histo_title.c_str(), 1024, 0, 400);
	      waveform_histo->GetXaxis()->SetTitle("Time (ns)");
	      waveform_histo->GetYaxis()->SetTitle("Amplitude (mV)");
	      // _objects->Add(waveform_histo);

	      for (size_t s=0; s<waveform.size(); s++)
		waveform_histo->SetBinContent(1+s, 0.61035156*(waveform[s]-2048) - pcd_baseline);

	      TCanvas *pcd_waveform_canvas = (TCanvas*)(gROOT->FindObject("pcd_waveform_canvas"));

	      if (pcd_waveform_canvas == nullptr) {
		pcd_waveform_canvas = new TCanvas ("pcd_waveform_canvas", "pCD waveform");
		_objects->Add(pcd_waveform_canvas);
		waveform_histo->Draw();
	      } else {
		pcd_waveform_canvas->cd();
		waveform_histo->Draw("same");
	      }

	      // if (options_manager::get_instance().get_option_flag(SHOW_CALIBRATED_INFO)) {
	      // }

	    } else {
	      // "UDD.parent" properties missing
	    }

	  } // if (HIGHLIGHT_FLAG)

	}
  	FL_LOG_DEVEL("Exiting...");
      }

    } // end of namespace view

  } // end of namespace visualization

} // end of namespace snemo
