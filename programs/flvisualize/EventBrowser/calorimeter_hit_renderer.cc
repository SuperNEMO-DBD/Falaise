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

#include <TCanvas.h>
#include <TH1F.h>
#include <TLegend.h>
#include <TLegendEntry.h>
#include <TROOT.h>

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
	  if (not it_hit.has_data()) {
	    continue;
	  }
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

      void calorimeter_hit_renderer::push_digitized_hits() {
	const io::event_record& event = _server->get_event();
	const auto& digi_data = event.get<snemo::datamodel::unified_digitized_data>(io::UDD_LABEL);

	const snemo::datamodel::CalorimeterDigiHitHdlCollection& dc_collection = digi_data.get_calorimeter_hits();

	if (dc_collection.empty()) {
	  DT_LOG_DEBUG(options_manager::get_instance().get_logging_priority(),
		       "No digitized calorimeter hits");
	  return;
	}

	for (const auto& it_hit : dc_collection) {
	  if (not it_hit.has_data()) {
	    continue;
	  }
	  const snemo::datamodel::calorimeter_digitized_hit& a_hit = it_hit.get();

	  // Geom ID fix
	  geomtools::geom_id a_geom_id = a_hit.get_geom_id();
	  if (a_geom_id.get_type() == 1301) {
	    a_geom_id.set_type(1302);
	    a_geom_id.set_any(4);
	  } else if (a_geom_id.get_type() == 1231) {
	    a_geom_id.set_type(1232);
	  } else if (a_geom_id.get_type() == 1251) {
	    a_geom_id.set_type(1252);
	  }

	  this->highlight_geom_id(a_geom_id, style_manager::get_instance().get_digitized_data_color());
	  // this->highlight_geom_id(a_hit.get_geom_id(), style_manager::get_instance().get_digitized_data_color());

	  // if (options_manager::get_instance().get_option_flag(SHOW_DIGITIZED_INFO)) {
	  // }

	  // Highlight flag => Draw UDD waveform
	  if (a_hit.get_auxiliaries().has_flag(browser_tracks::HIGHLIGHT_FLAG)) {

	    const std::vector<int16_t> & waveform = it_hit->get_waveform();

	    const std::string udd_waveform_histo_name (Form("udd_waveform_hit_%02d", a_hit.get_hit_id()));
	    const std::string udd_waveform_histo_title (snemo::datamodel::om_label(a_hit.get_geom_id()));

	    TH1F *udd_waveform_histo = new TH1F (udd_waveform_histo_name.c_str(), udd_waveform_histo_title.c_str(), 1024, 0, 1024);
	    udd_waveform_histo->GetXaxis()->SetTitle("Sample");
	    udd_waveform_histo->GetYaxis()->SetTitle("ADC");
	    // _objects->Add(udd_waveform_histo);

	    int16_t adc_min = 4096;
	    int16_t adc_max = 0;

	    for (size_t s=0; s<waveform.size(); s++) {
	      const int16_t adc = waveform[s];
	      if (adc < adc_min) adc_min = adc;
	      if (adc > adc_max) adc_max = adc;
	      udd_waveform_histo->SetBinContent(1+s, adc);
	    }

	    TCanvas *udd_waveform_canvas = (TCanvas*)(gROOT->FindObject("udd_waveform_canvas"));

	    if (udd_waveform_canvas == nullptr) {

	      udd_waveform_canvas = new TCanvas ("udd_waveform_canvas", "UDD waveform");
	      _objects->Add(udd_waveform_canvas);

	      TH1F *udd_waveform_frame = new TH1F ("udd_waveform_frame", ";Sample;ADC", 1024, 0, 1024);
	      // _objects->Add(udd_waveform_frame);

	      // adjust Y range to [adc_min,adc_max] with 10 % additional margin
	      const double adc_range = adc_max - adc_min;
	      const double adc_margin = 0.1*adc_range;
	      const double adc_ymin = std::max(adc_min-adc_margin, 0.);
	      const double adc_ymax = std::min(adc_max+adc_margin, 4096.);
	      udd_waveform_frame->GetYaxis()->SetRangeUser(adc_ymin, adc_ymax);

	      udd_waveform_canvas->cd();
	      udd_waveform_frame->Draw("0");
	      udd_waveform_canvas->Update();

	      TLegend *udd_waveform_legend = new TLegend(0.78, 0.20, 1.0, 0.25);
	      udd_waveform_legend->SetName("udd_waveform_legend");
	      udd_waveform_legend->SetFillStyle(0);
	      udd_waveform_legend->SetLineWidth(0);
	      udd_waveform_legend->Draw();

	    } else {

	      TH1F *udd_waveform_frame = (TH1F*)(gROOT->FindObject("udd_waveform_frame"));

	      if (udd_waveform_frame != nullptr) {

		// retrieve [adc_min,adc_max] from the canvas
		const double canvas_range_min = udd_waveform_canvas->GetUymin();
		const double canvas_range_max = udd_waveform_canvas->GetUymax();
		const double canvas_margin = (canvas_range_max-canvas_range_min)*0.1/1.2;
		const double canvas_adc_min = canvas_range_min + canvas_margin;
		const double canvas_adc_max = canvas_range_max - canvas_margin;

		// update boundaries if needed
		adc_min = std::min((double)(adc_min), canvas_adc_min);
		adc_max = std::max((double)(adc_max), canvas_adc_max);

		// adjust Y range with 10 % additional margin
		const double adc_range = adc_max - adc_min;
		const double adc_margin = 0.1*adc_range;
		const double adc_ymin = std::max(adc_min-adc_margin, 0.);
		const double adc_ymax = std::min(adc_max+adc_margin, 4096.);
		udd_waveform_frame->GetYaxis()->SetRangeUser(adc_ymin, adc_ymax);

	      } else {
		std::cout << "WAVEFORM FRAME not found" << std::endl;
	      }
	    }

	    // home made palette
	    const Color_t waveform_color[8] = {kBlue+1, kGreen+1, kRed+1, kCyan+1, kYellow+1, kMagenta+1, kOrange-3, kGray+1};

	    // use the number of drawned object as color index !
	    const size_t color_index = udd_waveform_canvas->GetListOfPrimitives()->GetSize() - 3;
	    // udd_waveform_histo->SetLineColor(style_manager::get_instance().get_color(color_index));
	    udd_waveform_histo->SetLineColor(waveform_color[color_index%8]);

	    if (TLegend *udd_waveform_legend = (TLegend*)(gROOT->FindObject("udd_waveform_legend"))) {
	      const std::string l_label = snemo::datamodel::om_label(a_geom_id);
	      TLegendEntry *lentry = udd_waveform_legend->AddEntry(udd_waveform_histo, l_label.c_str(), "");
	      lentry->SetTextColor(waveform_color[color_index%8]);
	      lentry->SetTextFont(62); // helvetica bold
	      udd_waveform_legend->SetY2NDC(0.25 + 0.05*color_index);
	    }

	    udd_waveform_canvas->cd();
	    udd_waveform_histo->Draw("same");
	    udd_waveform_canvas->Update();

	  } // if (HIGHLIGHT_FLAG)
	}
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
	  if (not it_hit.has_data()) {
	    continue;
	  }
	  const snemo::datamodel::precalibrated_calorimeter_hit& a_hit = it_hit.get();

	  this->highlight_geom_id(a_hit.get_geom_id(), style_manager::get_instance().get_precalibrated_data_color());

	  // if (options_manager::get_instance().get_option_flag(SHOW_PRECALIBRATED_INFO)) {
	  // }

	  // Draw pCD waveform
	  if (a_hit.get_auxiliaries().has_flag(browser_tracks::HIGHLIGHT_FLAG)) {

	    const std::string UDD_parent_key = "UDD.parent";
	    const datatools::properties & calo_hit_properties = a_hit.get_auxiliaries();

	    if (calo_hit_properties.has_key(UDD_parent_key)) {

	      // retrieve UDD calorimeter hit waveform
	      const int UDD_parent_index = calo_hit_properties.fetch_integer(UDD_parent_key);
	      const auto & udd = event.get<snemo::datamodel::unified_digitized_data>(io::UDD_LABEL);
	      const auto & udd_hit = udd.get_calorimeter_hits().at(UDD_parent_index);
	      const std::vector<int16_t> & waveform = udd_hit->get_waveform();

	      const std::string pcd_waveform_histo_name (Form("pcd_waveform_hit_%02d", a_hit.get_hit_id()));
	      const std::string pcd_waveform_histo_title (snemo::datamodel::om_label(a_hit.get_geom_id()));

	      TH1F *pcd_waveform_histo = new TH1F (pcd_waveform_histo_name.c_str(), pcd_waveform_histo_title.c_str(), 1024, 0, 400);
	      pcd_waveform_histo->GetXaxis()->SetTitle("Time (ns)");
	      pcd_waveform_histo->GetYaxis()->SetTitle("Amplitude (mV)");
	      // _objects->Add(pcd_waveform_histo);

	      double amplitude_min = 2500.;
	      double amplitude_max = -2500.;

	      const double & pcd_baseline = a_hit.get_baseline()/(1E-3*CLHEP::volt);

	      for (size_t s=0; s<waveform.size(); s++) {
		const int16_t adc = waveform[s];
		const double amplitude = 0.61035156*(adc-2048.) - pcd_baseline;
		if (amplitude < amplitude_min) amplitude_min = amplitude;
		if (amplitude > amplitude_max) amplitude_max = amplitude;
		pcd_waveform_histo->SetBinContent(1+s, amplitude);
	      }

	      TCanvas *pcd_waveform_canvas = (TCanvas*)(gROOT->FindObject("pcd_waveform_canvas"));

	      if (pcd_waveform_canvas == nullptr) {

		pcd_waveform_canvas = new TCanvas ("pcd_waveform_canvas", "pCD waveform");
		_objects->Add(pcd_waveform_canvas);

		TH1F *pcd_waveform_frame = new TH1F ("pcd_waveform_frame", ";Time (ns);Amplitude (mV)", 1024, 0, 400);
		// _objects->Add(pcd_waveform_frame);

		const double amplitude_range = amplitude_max - amplitude_min;
		const double amplitude_margin = 0.1*amplitude_range;
		const double amplitude_ymin = amplitude_min - amplitude_margin;
		const double amplitude_ymax = amplitude_max + amplitude_margin;
		pcd_waveform_frame->GetYaxis()->SetRangeUser(amplitude_ymin, amplitude_ymax);

		pcd_waveform_canvas->cd();
		pcd_waveform_frame->Draw("0");
		pcd_waveform_canvas->Update();

		TLegend *pcd_waveform_legend = new TLegend(0.78, 0.20, 1.0, 0.25);
		pcd_waveform_legend->SetName("pcd_waveform_legend");
		pcd_waveform_legend->SetFillStyle(0);
		pcd_waveform_legend->SetLineWidth(0);
		pcd_waveform_legend->Draw();

	      } else {

		TH1F *pcd_waveform_frame = (TH1F*)(gROOT->FindObject("pcd_waveform_frame"));

		if (pcd_waveform_frame != nullptr) {

		  // retrieve [amplitude_min,amplitude_max] from the canvas
		  const double canvas_range_min = pcd_waveform_canvas->GetUymin();
		  const double canvas_range_max = pcd_waveform_canvas->GetUymax();
		  const double canvas_margin = (canvas_range_max-canvas_range_min)*0.1/1.2;
		  const double canvas_amplitude_min = canvas_range_min + canvas_margin;
		  const double canvas_amplitude_max = canvas_range_max - canvas_margin;

		  // update boundaries if needed
		  amplitude_min = std::min((double)(amplitude_min), canvas_amplitude_min);
		  amplitude_max = std::max((double)(amplitude_max), canvas_amplitude_max);

		  // adjust Y range with 10 % additional margin
		  const double amplitude_range = amplitude_max - amplitude_min;
		  const double amplitude_margin = 0.1*amplitude_range;
		  const double amplitude_ymin = amplitude_min - amplitude_margin;
		  const double amplitude_ymax = amplitude_max + amplitude_margin;
		  pcd_waveform_frame->GetYaxis()->SetRangeUser(amplitude_ymin, amplitude_ymax);

		} else {
		  std::cout << "WAVEFORM FRAME not found" << std::endl;
		}
	      }

	      // home made palette
	      const Color_t waveform_color[8] = {kBlue+1, kGreen+1, kRed+1, kCyan+1, kYellow+1, kMagenta+1, kOrange-3, kGray+1};

	      // use the number of drawned object as color index !
	      const size_t color_index = pcd_waveform_canvas->GetListOfPrimitives()->GetSize() - 3;
	      // pcd_waveform_histo->SetLineColor(style_manager::get_instance().get_color(color_index));
	      pcd_waveform_histo->SetLineColor(waveform_color[color_index%8]);

	      if (TLegend *pcd_waveform_legend = (TLegend*)(gROOT->FindObject("pcd_waveform_legend"))) {
		const std::string l_label = snemo::datamodel::om_label(a_hit.get_geom_id());
		TLegendEntry *lentry = pcd_waveform_legend->AddEntry(pcd_waveform_histo, l_label.c_str(), "");
		lentry->SetTextColor(waveform_color[color_index%8]);
		lentry->SetTextFont(62); // helvetica bold
		pcd_waveform_legend->SetY2NDC(0.25 + 0.05*color_index);
	      }

	      pcd_waveform_canvas->cd();
	      pcd_waveform_histo->Draw("same");
	      pcd_waveform_canvas->Update();

	      // if (options_manager::get_instance().get_option_flag(SHOW_PRECALIBRATED_INFO)) {
	      // // Annotate pCD measurement values on waveform ?
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
	  if (not it_hit.has_data()) {
	    continue;
	  }
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

	}
  	FL_LOG_DEVEL("Exiting...");
      }

    } // end of namespace view

  } // end of namespace visualization

} // end of namespace snemo
