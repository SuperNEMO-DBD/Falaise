/* browser_tracks.cc
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

// Ourselves:
#include <falaise/snemo/view/browser_tracks.h>
// This project
#include <falaise/snemo/view/browser_icons.h>
#include <falaise/snemo/view/event_browser.h>
#include <falaise/snemo/view/options_manager.h>
#include <falaise/snemo/view/style_manager.h>

#include <falaise/snemo/io/data_model.h>
#include <falaise/snemo/io/event_server.h>

#include <falaise/snemo/utils/root_utilities.h>

// Standard libraries:
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdexcept>

// Third party
// - ROOT:
#include <TColor.h>
#include <TGCanvas.h>
#include <TGFrame.h>
#include <TGListTree.h>
#include <TGPicture.h>
#include <TGResourcePool.h>
#include <TMath.h>
#include <TSystem.h>

// - Bayeux/mctools:
#include <mctools/utils.h>
// - Bayeux/geomtools:
#include <geomtools/base_hit.h>
// - Falaise:
#include <falaise/snemo/datamodels/helix_trajectory_pattern.h>
#include <falaise/snemo/datamodels/line_trajectory_pattern.h>

// Need trailing ; to satisfy clang-format, but leads to -pedantic error, ignore
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
ClassImp(snemo::visualization::view::browser_tracks);
#pragma GCC diagnostic pop

namespace snemo {
namespace visualization {

namespace view {

class TGListTreeItemStdPlus : public TGListTreeItemStd {
 public:
  TGListTreeItemStdPlus(const char *name_ = 0, browser_tracks *bt_ = 0,
                        const TGPicture *open_pic_ = 0, const TGPicture *close_pic_ = 0,
                        const bool checked_ = false);
  virtual ~TGListTreeItemStdPlus();

  virtual void CheckChildren(TGListTreeItem *item, Bool_t state);

 private:
  browser_tracks *fBrowser;
};

TGListTreeItemStdPlus::TGListTreeItemStdPlus(const char *name_, browser_tracks *bt_,
                                             const TGPicture *open_pic_,
                                             const TGPicture *close_pic_, const bool checked_)
    : TGListTreeItemStd(gClient, name_, open_pic_, close_pic_, checked_), fBrowser(bt_) {
  return;
}

TGListTreeItemStdPlus::~TGListTreeItemStdPlus() { return; }

void TGListTreeItemStdPlus::CheckChildren(TGListTreeItem *item, Bool_t state) {
  // Code a bit hacked from default TGListTreeItemStd in order
  // to only check 'default' trajectory and not everything
  if (!item) return;

  while (item) {
    if (state) {
      if (!item->IsChecked()) {
        const int id = (intptr_t)item->GetUserData();
        // Check item only if 'default' key is active
        if (id < 0 && fBrowser) {
          // Search in which dictionnary the item comes from:
          datatools::properties *properties = fBrowser->get_item_properties(id);

          if (properties->has_flag("default"))
            item->CheckItem(true);
          else
            properties->update(browser_tracks::CHECKED_FLAG, false);
        } else {
          item->CheckItem();
        }
      }
    } else {
      if (item->IsChecked()) item->Toggle();
    }
    if (item->GetFirstChild()) {
      CheckChildren(item->GetFirstChild(), state);
    }
    item->UpdateState();
    item = item->GetNextSibling();
  }

  return;
}

//______________________________________________________________________________

const std::string browser_tracks::CHECKED_FLAG = "snvisu.check_flag";
const std::string browser_tracks::COLOR_FLAG = "snvisu.color_flag";
const std::string browser_tracks::HIGHLIGHT_FLAG = "snvisu.highlight_flag";

bool browser_tracks::is_initialized() const { return _initialized_; }

// ctor:
browser_tracks::browser_tracks(TGCompositeFrame *main_, io::event_server *server_) {
  _initialized_ = false;
  _server_ = server_;
  _browser_ = 0;
  _main_ = 0;
  this->_at_init_(main_);
  return;
}

// dtor:
browser_tracks::~browser_tracks() {
  this->reset();

  for (std::map<std::string, const TGPicture *>::iterator i_pict = _icons_.begin();
       i_pict != _icons_.end(); ++i_pict)
    delete _icons_[i_pict->first];

  delete _tracks_list_box_;

  _main_->Cleanup();
  return;
}

void browser_tracks::initialize(TGCompositeFrame *main_) {
  this->_at_init_(main_);
  _initialized_ = true;
  return;
}

void browser_tracks::_at_init_(TGCompositeFrame *main_) {
  // Keep track of main frame in order to regenerate it for
  // different detector setup
  _main_ = main_;
  _main_->SetCleanup(kDeepCleanup);

  // event browser frame : this sucks but this is needed to
  // connect button with action. In order to instantiate
  // properly the number of parent, one has to count how many
  // frames have been instantiated until here
  TGCompositeFrame *parent = (TGCompositeFrame *)_main_->GetParent()
                                 ->GetParent()
                                 ->GetParent()
                                 ->GetParent()
                                 ->GetParent()
                                 ->GetParent();

  _browser_ = dynamic_cast<event_browser *>(parent);
  DT_THROW_IF(!_browser_, std::logic_error, "Event_browser can't be cast from frame!");

  this->_at_construct_();
  return;
}

void browser_tracks::_at_construct_() {
  const int width = _main_->GetWidth();
  const int height = _main_->GetHeight();

  TGCanvas *canvas = new TGCanvas(_main_, width, height);
  _tracks_list_box_ = new TGListTree(canvas, kHorizontalFrame);

  _main_->AddFrame(canvas, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY | kLHintsBottom));

  _tracks_list_box_->Connect("Checked(TObject *,Bool_t)",
                             "snemo::visualization::view::browser_tracks", this,
                             "item_checked(TObject *,bool)");

  _tracks_list_box_->Connect("DoubleClicked(TGListTreeItem *,Int_t)",
                             "snemo::visualization::view::browser_tracks", this,
                             "item_selected(TGListTreeItem *,int)");

  return;
}

void browser_tracks::clear() {
  for (std::vector<TGListTreeItem *>::iterator item = _item_list_.begin();
       item != _item_list_.end(); ++item)
    _tracks_list_box_->DeleteItem(*item);

  _item_list_.clear();

  _item_id_ = 0;
  _properties_dictionnary_.clear();
  _base_hit_dictionnary_.clear();

  return;
}

void browser_tracks::reset() {
  this->clear();
  return;
}

void browser_tracks::update() {
  this->clear();

  if (!_server_->is_initialized()) {
    DT_LOG_WARNING(options_manager::get_instance().get_logging_priority(),
                   "Event server has not been initialized !");
    return;
  }

  this->_update_event_header();
  this->_update_simulated_data();
  this->_update_calibrated_data();
  this->_update_tracker_clustering_data();
  this->_update_tracker_trajectory_data();
  this->_update_particle_track_data();

  _tracks_list_box_->MapSubwindows();
  _main_->GetClient()->NeedRedraw(_tracks_list_box_);

  return;
}

void browser_tracks::_update_event_header() {
  // Grab event and other resources. Here nothing is constant
  // since properties will be modified here and used later
  // through 'checking' and 'double_clicking' actions:
  const io::event_record &event = _server_->grab_event();

  std::ostringstream label;
  std::ostringstream tip_text;

  // 'event_header' availability:
  if (!event.has(io::EH_LABEL)) {
    label << "Event #" << _server_->get_current_event_number();
  } else {
    const snemo::datamodel::event_header &eh =
        event.get<snemo::datamodel::event_header>(io::EH_LABEL);

    label << "Run #" << eh.get_id().get_run_number() << " - "
          << "Event #" << eh.get_id().get_event_number();
    if (options_manager::get_instance().get_option_flag(DUMP_INTO_TOOLTIP)) {
      // Here we use the event_server dump method since it
      // gives much more info on the event record
      eh.tree_dump(tip_text);
      //_server_->dump_event(tip_text);
    }
  }

  // Add 'event_header' information as top item:
  _top_item_ = _tracks_list_box_->AddItem(0, label.str().c_str(), _get_colored_icon_("ofolder"),
                                          _get_colored_icon_("folder"));
  _item_list_.push_back(_top_item_);
  _top_item_->SetCheckBox(false);
  _top_item_->SetUserData((void *)(intptr_t)++_item_id_);
  if (!tip_text.str().empty()) {
    _top_item_->SetTipText(tip_text.str().c_str());
  } else {
    _top_item_->SetTipText("Double click to expand event record");
  }
  _tracks_list_box_->OpenItem(_top_item_);
  return;
}

void browser_tracks::_update_simulated_data() {
  const options_manager &options_mgr = options_manager::get_instance();

  // Grab event and other resources. Here nothing is constant
  // since properties will be modified here and used later
  // through 'checking' and 'double_clicking' actions:
  io::event_record &event = _server_->grab_event();

  // 'simulated_data' availability:
  if (!event.has(io::SD_LABEL)) return;

  mctools::simulated_data &sd = event.grab<mctools::simulated_data>(io::SD_LABEL);

  if (!options_mgr.get_option_flag(SHOW_MC_VERTEX) && !options_mgr.get_option_flag(SHOW_MC_HITS))
    return;

  // Add 'simulated_data' folder as sub item:
  const std::string data_bank_name = "Simulated data (" + io::SD_LABEL + ")";
  TGListTreeItem *item_simulated_data =
      _tracks_list_box_->AddItem(_top_item_, data_bank_name.c_str(), _get_colored_icon_("ofolder"),
                                 _get_colored_icon_("folder"));
  _tracks_list_box_->OpenItem(item_simulated_data);
  item_simulated_data->SetCheckBox(false);
  item_simulated_data->SetUserData((void *)(intptr_t)++_item_id_);

  if (options_mgr.get_option_flag(DUMP_INTO_TOOLTIP)) {
    std::ostringstream tip_text;
    sd.tree_dump(tip_text);
    item_simulated_data->SetTipText(tip_text.str().c_str());
  } else {
    item_simulated_data->SetTipText("Double click to expand simulated data");
  }

  // Item id for 'properties' dictionnary:
  int &icheck_id = _item_id_;

  // Add a folder relative to primary event info.
  if (options_mgr.get_option_flag(SHOW_MC_VERTEX)) {
    mctools::simulated_data::primary_event_type &pevent = sd.grab_primary_event();
    const std::string &a_label = pevent.get_label();
    TGListTreeItem *item_primary_event = _tracks_list_box_->AddItem(
        item_simulated_data,
        std::string("Primary particles" + (!a_label.empty() ? " - " + a_label : "")).c_str(),
        _get_colored_icon_("ofolder"), _get_colored_icon_("folder"));
    _tracks_list_box_->OpenItem(item_primary_event);
    item_primary_event->SetCheckBox(false);
    item_primary_event->SetUserData((void *)(intptr_t)++icheck_id);

    genbb::primary_event::particles_col_type &particles = pevent.grab_particles();

    for (genbb::primary_event::particles_col_type::iterator ip = particles.begin();
         ip != particles.end(); ++ip) {
      std::ostringstream label;
      label.precision(3);
      label.setf(std::ios::fixed, std::ios::floatfield);
      label << ip->get_particle_label() << " particle: E = ";
      utils::root_utilities::get_prettified_energy(label, ip->get_kinetic_energy());
      label << ", t = ";
      utils::root_utilities::get_prettified_time(label, ip->get_time());
      label << " - (px, py, pz) = " << ip->get_momentum() / CLHEP::MeV << " MeV, "
            << "(x, y, z) = ";
      if (sd.has_vertex()) {
        label << sd.get_vertex() / CLHEP::mm << " mm";
      } else {
        label << "no vertex position";
      }

      const std::string hex_str = utils::root_utilities::get_hex_color(kViolet);
      TGListTreeItem *item_particle = _tracks_list_box_->AddItem(
          item_primary_event, label.str().c_str(), _get_colored_icon_("vertex", hex_str, true),
          _get_colored_icon_("vertex", hex_str));
      item_particle->SetCheckBox(ip->has_generation_id());
      item_particle->SetUserData((void *)(intptr_t) - (++icheck_id));
      // _properties_dictionnary_[-icheck_id] = &(pevent.grab_auxiliaries());
      _properties_dictionnary_[-icheck_id] = &(ip->grab_auxiliaries());
      std::ostringstream tip_text;
      if (options_mgr.get_option_flag(DUMP_INTO_TOOLTIP)) {
        ip->tree_dump(tip_text);
      } else {
        tip_text << "Double click to highlight track "
                 << "and to dump info on terminal";
      }
      item_particle->SetTipText(tip_text.str().c_str());
    }
  }

  // Add MC step hits information:
  if (options_mgr.get_option_flag(SHOW_MC_TRACKS)) {
    // Get hit categories related to visual track
    std::vector<std::string> visual_categories;
    sd.get_step_hits_categories(visual_categories,
                                mctools::simulated_data::HIT_CATEGORY_TYPE_PREFIX, "__visu.tracks");
    typedef std::pair<size_t, TGListTreeItem *> entry_type;
    std::map<int, entry_type> item_tracks;
    if (!visual_categories.empty()) {
      TGListTreeItem *item_mc_tracks =
          _tracks_list_box_->AddItem(item_simulated_data, "Simulated tracks",
                                     _get_colored_icon_("ofolder"), _get_colored_icon_("folder"));
      item_mc_tracks->SetCheckBox(false);
      item_mc_tracks->SetUserData((void *)(intptr_t)++icheck_id);

      TGListTreeItem *item_mc_highlight_step_hits =
          _tracks_list_box_->AddItem(item_simulated_data, "Highlighted simulated step hits",
                                     _get_colored_icon_("ofolder"), _get_colored_icon_("folder"));
      item_mc_highlight_step_hits->SetCheckBox(false);
      item_mc_highlight_step_hits->SetUserData((void *)(intptr_t)++icheck_id);
      for (size_t icat = 0; icat < visual_categories.size(); ++icat) {
        const std::string &category = visual_categories[icat];
        if (sd.has_step_hits(category)) {
          mctools::simulated_data::hit_handle_collection_type &hit_collection =
              sd.grab_step_hits(category);
          for (mctools::simulated_data::hit_handle_collection_type::iterator it_hit =
                   hit_collection.begin();
               it_hit != hit_collection.end(); ++it_hit) {
            mctools::base_step_hit &a_step = it_hit->grab();
            datatools::properties &a_auxiliaries = a_step.grab_auxiliaries();

            std::string name = a_step.get_particle_name();
            if (name == "e-") name = "electron";
            if (name == "e+") name = "positron";
            const size_t color = style_manager::get_instance().get_particle_color(name);
            const std::string hex_str = utils::root_utilities::get_hex_color(color);
            TGListTreeItem *item = 0;
            if (a_auxiliaries.has_flag(mctools::hit_utils::HIT_VISU_HIGHLIGHTED_KEY)) {
              item = item_mc_highlight_step_hits;
            } else {
              const int a_track_id = a_step.get_track_id();
              if (!item_tracks.count(a_track_id)) {
                std::ostringstream track_label;
                track_label.precision(3);
                track_label.setf(std::ios::fixed, std::ios::floatfield);
                track_label << a_step.get_particle_name() << " track";
                if (a_step.is_primary_particle()) track_label << " (primary)";
                item = _tracks_list_box_->AddItem(item_mc_tracks, track_label.str().c_str(),
                                                  _get_colored_icon_("track", hex_str),
                                                  _get_colored_icon_("track", hex_str));
                entry_type a_pair = std::make_pair(1, item);
                item_tracks.insert(std::make_pair(a_track_id, a_pair));
                item->SetUserData((void *)(intptr_t) - (++icheck_id));
                _base_hit_dictionnary_[-icheck_id] = &(a_step);
                _properties_dictionnary_[-icheck_id] = &(a_auxiliaries);
              } else {
                entry_type &a_entry = item_tracks[a_track_id];
                item = a_entry.second;
                a_entry.first++;
              }
              item->SetCheckBox(true);
            }

            // Add subsubitem:
            std::ostringstream label_hit;
            label_hit.precision(3);
            label_hit.setf(std::ios::fixed, std::ios::floatfield);
            label_hit << "Step #" << a_step.get_hit_id() << " - "
                      << "particle " << a_step.get_particle_name();
            if (a_step.get_energy_deposit() > 0.0) {
              label_hit << " / energy deposit = ";
              utils::root_utilities::get_prettified_energy(label_hit, a_step.get_energy_deposit());
            }
            TGListTreeItem *item_hit = _tracks_list_box_->AddItem(item, label_hit.str().c_str());
            item_hit->SetUserData((void *)(intptr_t) - (++icheck_id));
            _base_hit_dictionnary_[-icheck_id] = &(a_step);
            item_hit->SetPictures(_get_colored_icon_("step", hex_str, true),
                                  _get_colored_icon_("step", hex_str));

            std::ostringstream tip_text;
            if (options_mgr.get_option_flag(DUMP_INTO_TOOLTIP)) {
              a_step.tree_dump(tip_text);
            } else {
              tip_text << "Double click to highlight step hit "
                       << "and to dump info on terminal";
            }
            item_hit->SetTipText(tip_text.str().c_str());
          }
        }
      }
      // Update item text following the number of hits found
      for (std::map<int, entry_type>::iterator i = item_tracks.begin(); i != item_tracks.end();
           ++i) {
        entry_type a_entry = i->second;
        const size_t count = a_entry.first;
        TGListTreeItem *item = a_entry.second;
        std::ostringstream oss;
        oss << item->GetText();
        if (count == 0) {
          oss << " - no hits";
        } else {
          oss << " - " << count << " hit";
          if (count > 1) oss << "s";
        }
        item->SetText(oss.str().c_str());
      }
      // Update number of tracks
      std::ostringstream oss;
      oss << item_mc_tracks->GetText();
      const size_t count = item_tracks.size();
      if (count == 0) {
        oss << " - no tracks";
      } else {
        oss << " - " << count << " track";
        if (count > 1) oss << "s";
      }
      item_mc_tracks->SetText(oss.str().c_str());
    }
  }

  // Add MC hits information:
  if (options_mgr.get_option_flag(SHOW_MC_HITS)) {
    if (options_mgr.get_option_flag(SHOW_MC_CALORIMETER_HITS)) {
      TGListTreeItem *item_mc_calorimeter =
          _tracks_list_box_->AddItem(item_simulated_data, "Simulated calorimeter hits",
                                     _get_colored_icon_("ofolder"), _get_colored_icon_("folder"));
      item_mc_calorimeter->SetCheckBox(false);
      item_mc_calorimeter->SetUserData((void *)(intptr_t)++icheck_id);

      // Retrieve all category names except private category such as '__visual.tracks'
      std::vector<std::string> calo_categories;
      sd.get_step_hits_categories(calo_categories,
                                  mctools::simulated_data::HIT_CATEGORY_TYPE_PUBLIC);

      size_t ihit = 0;
      for (std::vector<std::string>::const_iterator iname = calo_categories.begin();
           iname != calo_categories.end(); ++iname) {
        const std::string &a_calo_name = *iname;

        // 2015/07/07 XG: Remove Geiger energy deposit since it is treated
        // in a particular way
        if (a_calo_name == "gg") continue;

        if (!sd.has_step_hits(a_calo_name)) continue;

        mctools::simulated_data::hit_handle_collection_type &hit_collection =
            sd.grab_step_hits(a_calo_name);
        ihit += hit_collection.size();

        for (mctools::simulated_data::hit_handle_collection_type::iterator it_hit =
                 hit_collection.begin();
             it_hit != hit_collection.end(); ++it_hit) {
          mctools::base_step_hit &a_step = it_hit->grab();

          std::string hex_str;
          if (a_step.get_auxiliaries().has_key(COLOR_FLAG)) {
            a_step.get_auxiliaries().fetch(COLOR_FLAG, hex_str);
            // TColor::GetColor (hex_str.c_str());
          }
          // Add subsubitem:
          std::ostringstream label_hit;
          label_hit.precision(3);
          label_hit.setf(std::ios::fixed, std::ios::floatfield);
          label_hit << a_calo_name << " hit #" << a_step.get_hit_id() << " - "
                    << "Energy deposit = ";
          utils::root_utilities::get_prettified_energy(label_hit, a_step.get_energy_deposit());
          label_hit << " - geom_id = " << a_step.get_geom_id();

          TGListTreeItem *item_hit =
              _tracks_list_box_->AddItem(item_mc_calorimeter, label_hit.str().c_str());
          item_hit->SetUserData((void *)(intptr_t) - (++icheck_id));
          _base_hit_dictionnary_[-icheck_id] = &(a_step);
          item_hit->SetPictures(_get_colored_icon_("calorimeter", "", true),
                                _get_colored_icon_("calorimeter"));

          std::ostringstream tip_text;
          if (options_mgr.get_option_flag(DUMP_INTO_TOOLTIP)) {
            a_step.tree_dump(tip_text);
          } else {
            tip_text << "Double click to highlight calorimeter hit "
                     << "and to dump info on terminal";
          }
          item_hit->SetTipText(tip_text.str().c_str());
        }
      }
      // Update item text following the number of hits found
      std::ostringstream oss;
      oss << item_mc_calorimeter->GetText();
      ihit == 0 ? oss << " - no hits" : oss << " - " << ihit << " hits";
      item_mc_calorimeter->SetText(oss.str().c_str());
    }  // end of SHOW_MC_CALORIMETER_HITS

    if (options_mgr.get_option_flag(SHOW_MC_TRACKER_HITS)) {
      if (sd.has_step_hits("gg")) {
        TGListTreeItem *item_mc_tracker =
            _tracks_list_box_->AddItem(item_simulated_data, "Simulated tracker hits",
                                       _get_colored_icon_("ofolder"), _get_colored_icon_("folder"));
        item_mc_tracker->SetCheckBox(false);
        item_mc_tracker->SetUserData((void *)(intptr_t)++icheck_id);

        mctools::simulated_data::hit_handle_collection_type &hit_collection =
            sd.grab_step_hits("gg");

        for (mctools::simulated_data::hit_handle_collection_type::iterator it_hit =
                 hit_collection.begin();
             it_hit != hit_collection.end(); ++it_hit) {
          mctools::base_step_hit &a_step = it_hit->grab();

          // If color is available, add a color box close to the item:
          std::string hex_str;
          if (a_step.get_auxiliaries().has_key(COLOR_FLAG)) {
            a_step.get_auxiliaries().fetch(COLOR_FLAG, hex_str);
            // TColor::GetColor(hex_str.c_str());
          }
          // Add subsubitem:
          std::ostringstream label_hit;
          label_hit << "Geiger hit #" << std::setw(2) << std::setfill('0') << a_step.get_hit_id()
                    << " - "
                    << "geom_id = " << a_step.get_geom_id();

          TGListTreeItem *item_hit =
              _tracks_list_box_->AddItem(item_mc_tracker, label_hit.str().c_str());
          item_hit->SetUserData((void *)(intptr_t) - (++icheck_id));
          _base_hit_dictionnary_[-icheck_id] = &(a_step);
          item_hit->SetPictures(_get_colored_icon_("geiger", hex_str, true),
                                _get_colored_icon_("geiger", hex_str));

          std::ostringstream tip_text;
          if (options_mgr.get_option_flag(DUMP_INTO_TOOLTIP)) {
            a_step.tree_dump(tip_text);
          } else {
            tip_text << "Double click to highlight Geiger hit "
                     << "and to dump info on terminal";
          }
          item_hit->SetTipText(tip_text.str().c_str());
        }
        // Update item text following the number of hits found
        const size_t ihit = hit_collection.size();
        std::ostringstream oss;
        oss << item_mc_tracker->GetText();
        ihit == 0 ? oss << " - no hits" : oss << " - " << ihit << " hits";
        item_mc_tracker->SetText(oss.str().c_str());
      }
    }  // end of SHOW_MC_TRACKER_HITS
  }    // end of SHOW_MC_HITS

  return;
}

void browser_tracks::_update_calibrated_data() {
  // Grab event and other resources. Here nothing is constant
  // since properties will be modified here and used later
  // through 'checking' and 'double_clicking' actions:
  io::event_record &event = _server_->grab_event();

  // 'calibrated_data' availability:
  if (!event.has(io::CD_LABEL)) return;

  snemo::datamodel::calibrated_data &cd =
      event.grab<snemo::datamodel::calibrated_data>(io::CD_LABEL);

  const options_manager &options_mgr = options_manager::get_instance();
  if (!options_mgr.get_option_flag(SHOW_CALIBRATED_HITS)) return;

  // Add 'calibrated_data' folder as sub item:
  const std::string data_bank_name = "Calibrated data (" + io::CD_LABEL + ")";
  TGListTreeItem *item_calibrated_data =
      _tracks_list_box_->AddItem(_top_item_, data_bank_name.c_str(), _get_colored_icon_("ofolder"),
                                 _get_colored_icon_("folder"));
  _tracks_list_box_->OpenItem(item_calibrated_data);
  item_calibrated_data->SetCheckBox(false);
  item_calibrated_data->SetUserData((void *)(intptr_t)++_item_id_);
  if (options_manager::get_instance().get_option_flag(DUMP_INTO_TOOLTIP)) {
    std::ostringstream tip_text;
    cd.tree_dump(tip_text);
    item_calibrated_data->SetTipText(tip_text.str().c_str());
  } else {
    item_calibrated_data->SetTipText("Double click to expand calibrated data");
  }

  int &icheck_id = _item_id_;

  // Add calibrated hits information:
  // Start with calorimeter info:
  {
    TGListTreeItem *item_calorimeter =
        _tracks_list_box_->AddItem(item_calibrated_data, "Calibrated calorimeter hits",
                                   _get_colored_icon_("ofolder"), _get_colored_icon_("folder"));
    item_calorimeter->SetCheckBox(false);
    item_calorimeter->SetUserData((void *)(intptr_t)++icheck_id);

    snemo::datamodel::calibrated_data::calorimeter_hit_collection_type &cc_collection =
        cd.calibrated_calorimeter_hits();

    for (snemo::datamodel::calibrated_data::calorimeter_hit_collection_type::iterator it_hit =
             cc_collection.begin();
         it_hit != cc_collection.end(); ++it_hit) {
      snemo::datamodel::calibrated_calorimeter_hit &a_hit = it_hit->grab();

      std::string hex_str;
      if (a_hit.get_auxiliaries().has_key(COLOR_FLAG)) {
        a_hit.get_auxiliaries().fetch(COLOR_FLAG, hex_str);
        // TColor::GetColor(hex_str.c_str());
      }

      // Add subsubitem:
      std::ostringstream label_hit;
      label_hit.precision(3);
      label_hit.setf(std::ios::fixed, std::ios::floatfield);
      if (a_hit.get_auxiliaries().has_key("category"))
        label_hit << a_hit.get_auxiliaries().fetch_string("category") << " ";
      label_hit << "hit #" << a_hit.get_hit_id() << " - E = ";
      utils::root_utilities::get_prettified_energy(label_hit, a_hit.get_energy(),
                                                   a_hit.get_sigma_energy());
      label_hit << " - t = ";
      utils::root_utilities::get_prettified_time(label_hit, a_hit.get_time(),
                                                 a_hit.get_sigma_time());

      TGListTreeItem *item_hit =
          _tracks_list_box_->AddItem(item_calorimeter, label_hit.str().c_str());
      item_hit->SetUserData((void *)(intptr_t) - (++icheck_id));
      _base_hit_dictionnary_[-icheck_id] = &(a_hit);
      item_hit->SetPictures(_get_colored_icon_("calorimeter", "", true),
                            _get_colored_icon_("calorimeter"));

      std::ostringstream tip_text;
      if (options_mgr.get_option_flag(DUMP_INTO_TOOLTIP)) {
        a_hit.tree_dump(tip_text);
      } else {
        tip_text << "Double click to highlight calorimeter hit "
                 << "and to dump info on terminal";
      }
      item_hit->SetTipText(tip_text.str().c_str());
    }
    // Update item text following the number of hits found
    const size_t ihit = cc_collection.size();
    std::ostringstream oss;
    oss << item_calorimeter->GetText();
    ihit == 0 ? oss << " - no hits" : oss << " - " << ihit << " hits";
    item_calorimeter->SetText(oss.str().c_str());
  }  // end of calorimeter info

  // Continue with tracker info:
  {
    TGListTreeItem *item_tracker =
        _tracks_list_box_->AddItem(item_calibrated_data, "Calibrated tracker hits",
                                   _get_colored_icon_("ofolder"), _get_colored_icon_("folder"));
    item_tracker->SetCheckBox(false);
    item_tracker->SetUserData((void *)(intptr_t)++icheck_id);

    snemo::datamodel::calibrated_data::tracker_hit_collection_type &ct_collection =
        cd.calibrated_tracker_hits();

    for (snemo::datamodel::calibrated_data::tracker_hit_collection_type::iterator it_hit =
             ct_collection.begin();
         it_hit != ct_collection.end(); ++it_hit) {
      snemo::datamodel::calibrated_tracker_hit &a_hit = it_hit->grab();

      // Add subsubitem:
      std::ostringstream label_hit;
      label_hit.precision(3);
      label_hit.setf(std::ios::fixed, std::ios::floatfield);
      label_hit << "Geiger hit #" << std::setw(2) << std::setfill('0') << a_hit.get_id()
                << " (r, z) = (" << a_hit.get_r() / CLHEP::cm << ", " << a_hit.get_z() / CLHEP::cm
                << ") cm";

      TGListTreeItem *item_hit = _tracks_list_box_->AddItem(item_tracker, label_hit.str().c_str(),
                                                            _get_colored_icon_("geiger", "", true),
                                                            _get_colored_icon_("geiger"));
      item_hit->SetUserData((void *)(intptr_t) - (++icheck_id));
      _base_hit_dictionnary_[-icheck_id] = &(a_hit);

      std::ostringstream tip_text;
      if (options_mgr.get_option_flag(DUMP_INTO_TOOLTIP)) {
        a_hit.tree_dump(tip_text);
      } else {
        tip_text << "Double click to highlight Geiger hit "
                 << "and to dump info on terminal";
      }
      item_hit->SetTipText(tip_text.str().c_str());
    }
    // Update item text following the number of hits found
    const size_t ihit = ct_collection.size();
    std::ostringstream oss;
    oss << item_tracker->GetText();
    ihit == 0 ? oss << " - no hits" : oss << " - " << ihit << " hits";
    item_tracker->SetText(oss.str().c_str());
  }  // end of tracker info

  return;
}

void browser_tracks::_update_tracker_clustering_data() {
  // Grab event and other resources. Here nothing is constant
  // since properties will be modified here and used later
  // through 'checking' and 'double_clicking' actions:
  io::event_record &event = _server_->grab_event();

  // 'tracker_clustering_data' availability:
  if (!event.has(io::TCD_LABEL)) return;

  snemo::datamodel::tracker_clustering_data &tcd =
      event.grab<snemo::datamodel::tracker_clustering_data>(io::TCD_LABEL);

  const options_manager &options_mgr = options_manager::get_instance();
  if (!options_mgr.get_option_flag(SHOW_TRACKER_CLUSTERED_HITS)) return;

  // Identifiant for item dictionnary:
  int &icheck_id = _item_id_;

  // Add 'tracker_clustering_data' item:
  const std::string data_bank_name = "Tracker clustering data (" + io::TCD_LABEL + ")";
  TGListTreeItem *item_tracker_cluster =
      _tracks_list_box_->AddItem(_top_item_, data_bank_name.c_str(), _get_colored_icon_("ofolder"),
                                 _get_colored_icon_("folder"));
  item_tracker_cluster->SetCheckBox(false);
  item_tracker_cluster->SetUserData((void *)(intptr_t)++icheck_id);

  _tracks_list_box_->SetCheckMode(TGListTree::kRecursive);
  //        _tracks_list_box_->SetCheckBox(item_tracker_cluster);
  _tracks_list_box_->OpenItem(item_tracker_cluster);

  {
    std::ostringstream tip_text;
    if (options_mgr.get_option_flag(DUMP_INTO_TOOLTIP)) {
      tcd.tree_dump(tip_text);
    } else {
      tip_text << "Double click to expand tracker clustering data";
    }
    item_tracker_cluster->SetTipText(tip_text.str().c_str());
  }

  snemo::datamodel::tracker_clustering_data::solution_col_type &cluster_solutions =
      tcd.grab_solutions();
  for (snemo::datamodel::tracker_clustering_data::solution_col_type::iterator isolution =
           cluster_solutions.begin();
       isolution != cluster_solutions.end(); ++isolution) {
    // Get current tracker solution:
    snemo::datamodel::tracker_clustering_solution &a_solution = isolution->grab();

    // Add item:
    std::ostringstream label_solution;
    label_solution << "Solution #" << a_solution.get_solution_id() << " - "
                   << a_solution.get_unclustered_hits().size() << " unclustered hit"
                   << (a_solution.get_unclustered_hits().size() > 1 ? "s" : "");
    if (tcd.has_default_solution()) {
      if (&a_solution == &(tcd.get_default_solution())) {
        label_solution << " - default";
      }
    }

    TGListTreeItem *item_solution =
        _tracks_list_box_->AddItem(item_tracker_cluster, label_solution.str().c_str(),
                                   _get_colored_icon_("ofolder"), _get_colored_icon_("folder"));
    _tracks_list_box_->OpenItem(item_solution);
    item_solution->SetUserData((void *)(intptr_t)++icheck_id);
    item_solution->SetCheckBox(true);
    {
      // Set tooltip text
      std::ostringstream tip_text;
      a_solution.tree_dump(tip_text);
      item_solution->SetTipText(tip_text.str().c_str());
    }
    // Get solution auxiliaries:
    datatools::properties &a_auxiliaries = a_solution.grab_auxiliaries();

    if (a_auxiliaries.has_key(browser_tracks::CHECKED_FLAG))
      item_solution->CheckItem(a_auxiliaries.has_flag(browser_tracks::CHECKED_FLAG));

    // Update properties dictionnary:
    _properties_dictionnary_[icheck_id] = &(a_auxiliaries);

    // Get clusters stored in the current tracker solution:
    snemo::datamodel::tracker_clustering_solution::cluster_col_type &clusters =
        a_solution.grab_clusters();
    for (snemo::datamodel::tracker_clustering_solution::cluster_col_type::iterator icluster =
             clusters.begin();
         icluster != clusters.end(); ++icluster) {
      // Get current tracker cluster:
      snemo::datamodel::tracker_cluster &a_cluster = icluster->grab();

      // Add subitem:
      std::ostringstream label_cluster;
      label_cluster << "Cluster #" << a_cluster.get_cluster_id() << " "
                    << "(" << a_cluster.get_hits().size() << " hits) ";
      if (a_cluster.is_prompt()) {
        label_cluster << "- prompt";
      } else {
        label_cluster << "- delayed";
      }

      TGListTreeItem *item_cluster =
          _tracks_list_box_->AddItem(item_solution, label_cluster.str().c_str());
      item_cluster->SetUserData((void *)(intptr_t)++icheck_id);
      item_cluster->SetCheckBox(true);
      {
        // Set tooltip text
        std::ostringstream tip_text;
        a_cluster.tree_dump(tip_text);
        item_cluster->SetTipText(tip_text.str().c_str());
      }
      // Get cluster auxiliaries:
      datatools::properties &aa_auxiliaries = a_cluster.grab_auxiliaries();

      if (aa_auxiliaries.has_key(browser_tracks::CHECKED_FLAG))
        item_cluster->CheckItem(aa_auxiliaries.has_flag(browser_tracks::CHECKED_FLAG));

      // Update base hit dictionnary:
      _base_hit_dictionnary_[icheck_id] = &(a_cluster);

      // If color is available, add a color box close to the
      // item (obsolete since xpm icon can be colorized):
      std::string hex_str;
      if (aa_auxiliaries.has_key(browser_tracks::COLOR_FLAG)) {
        aa_auxiliaries.fetch(browser_tracks::COLOR_FLAG, hex_str);
        // const size_t color = TColor::GetColor(hex_str.c_str());
        // item_cluster->SetColor(color);
      }
      item_cluster->SetPictures(_get_colored_icon_("cluster", hex_str),
                                _get_colored_icon_("cluster", hex_str));

      // Get tracker hits stored in the current tracker cluster:
      snemo::datamodel::calibrated_tracker_hit::collection_type &hits = a_cluster.grab_hits();
      for (snemo::datamodel::calibrated_tracker_hit::collection_type::iterator igg = hits.begin();
           igg != hits.end(); ++igg) {
        snemo::datamodel::calibrated_tracker_hit &a_gg_hit = igg->grab();
        // Add subsubitem:
        std::ostringstream label_hit;
        label_hit.precision(3);
        label_hit.setf(std::ios::fixed, std::ios::floatfield);
        label_hit << "Geiger hit #" << std::setw(2) << std::setfill('0') << a_gg_hit.get_id()
                  << " (r, z) = (" << a_gg_hit.get_r() / CLHEP::cm << ", "
                  << a_gg_hit.get_z() / CLHEP::cm << ") cm";

        TGListTreeItem *item_hit = _tracks_list_box_->AddItem(
            item_cluster, label_hit.str().c_str(), _get_colored_icon_("geiger", hex_str, true),
            _get_colored_icon_("geiger", hex_str));
        item_hit->SetUserData((void *)(intptr_t) - (++icheck_id));
        _base_hit_dictionnary_[-icheck_id] = &(a_gg_hit);

        std::ostringstream tip_text;
        if (options_mgr.get_option_flag(DUMP_INTO_TOOLTIP)) {
          a_gg_hit.tree_dump(tip_text);
        } else {
          tip_text << "Double click to highlight Geiger hit "
                   << "and to dump info on terminal";
        }
        item_hit->SetTipText(tip_text.str().c_str());
      }
    }  // end of cluster loop
  }    // end of solution loop

  return;
}

void browser_tracks::_update_tracker_trajectory_data() {
  // Grab event and other resources. Here nothing is constant
  // since properties will be modified here and used later
  // through 'checking' and 'double_clicking' actions:
  io::event_record &event = _server_->grab_event();

  // 'tracker_trajectory_data' availability:
  if (!event.has(io::TTD_LABEL)) return;

  snemo::datamodel::tracker_trajectory_data &ttd =
      event.grab<snemo::datamodel::tracker_trajectory_data>(io::TTD_LABEL);

  const options_manager &options_mgr = options_manager::get_instance();
  if (!options_mgr.get_option_flag(SHOW_TRACKER_TRAJECTORIES)) return;

  // Identifiant for item dictionnary:
  int &icheck_id = _item_id_;

  // Add 'tracker_clustering_data' item:
  const std::string data_bank_name = "Tracker trajectory data (" + io::TTD_LABEL + ")";
  TGListTreeItem *item_tracker_trajectory =
      _tracks_list_box_->AddItem(_top_item_, data_bank_name.c_str(), _get_colored_icon_("ofolder"),
                                 _get_colored_icon_("folder"));
  item_tracker_trajectory->SetCheckBox(false);
  item_tracker_trajectory->SetUserData((void *)(intptr_t)++icheck_id);

  //        _tracks_list_box_->SetCheckBox(item_tracker_cluster);
  _tracks_list_box_->OpenItem(item_tracker_trajectory);

  std::ostringstream tip_text;
  if (options_mgr.get_option_flag(DUMP_INTO_TOOLTIP)) {
    ttd.tree_dump(tip_text);
  } else {
    tip_text << "Double click to expand tracker trajectory data";
  }
  item_tracker_trajectory->SetTipText(tip_text.str().c_str());

  if (!ttd.has_solutions()) return;

  snemo::datamodel::tracker_trajectory_data::solution_col_type &trajectory_solutions =
      ttd.grab_solutions();
  for (snemo::datamodel::tracker_trajectory_data::solution_col_type::iterator isolution =
           trajectory_solutions.begin();
       isolution != trajectory_solutions.end(); ++isolution) {
    // Get current tracker solution:
    snemo::datamodel::tracker_trajectory_solution &a_solution = isolution->grab();

    // Add item:
    std::ostringstream label_solution;
    label_solution << "Solution #" << a_solution.get_solution_id() << " - "
                   << a_solution.get_trajectories().size() << " trajector"
                   << (a_solution.get_trajectories().size() > 1 ? "ies" : "y") << ", "
                   << a_solution.get_unfitted_clusters().size() << " unfitted cluster"
                   << (a_solution.get_unfitted_clusters().size() > 1 ? "s" : "");
    if (ttd.has_default_solution()) {
      if (&a_solution == &(ttd.get_default_solution())) {
        label_solution << " - default";
      }
    }

    TGListTreeItemStdPlus *item_solution =
        new TGListTreeItemStdPlus(label_solution.str().c_str(), this, _get_colored_icon_("ofolder"),
                                  _get_colored_icon_("folder"),
                                  /*check=*/true);
    _tracks_list_box_->AddItem(item_tracker_trajectory, item_solution);
    // TGListTreeItem * item_solution
    //   = _tracks_list_box_->AddItem(item_tracker_trajectory,
    //                                 label_solution.str().c_str());
    _tracks_list_box_->OpenItem(item_solution);
    item_solution->SetUserData((void *)(intptr_t)++icheck_id);

    // Get solution auxiliaries:
    datatools::properties &a_auxiliaries = a_solution.grab_auxiliaries();

    if (a_auxiliaries.has_key(browser_tracks::CHECKED_FLAG))
      item_solution->CheckItem(a_auxiliaries.has_flag(browser_tracks::CHECKED_FLAG));

    // Update properties dictionnary:
    _properties_dictionnary_[icheck_id] = &(a_auxiliaries);

    // Prepare folder for helix pattern:
    TGListTreeItem *item_helix_solution = 0;

    // Prepare folder for line pattern:
    TGListTreeItem *item_line_solution = 0;

    // Get trajectories stored in the current tracker trajectory solution:
    snemo::datamodel::tracker_trajectory_solution::trajectory_col_type &trajectories =
        a_solution.grab_trajectories();
    for (snemo::datamodel::tracker_trajectory_solution::trajectory_col_type::iterator itrajectory =
             trajectories.begin();
         itrajectory != trajectories.end(); ++itrajectory) {
      // Get current tracker trajectory:
      snemo::datamodel::tracker_trajectory &a_trajectory = itrajectory->grab();

      // Determine trajectory color by getting cluster color:
      std::string hex_str;
      if (a_trajectory.has_cluster()) {
        const snemo::datamodel::tracker_cluster &a_cluster = a_trajectory.get_cluster();
        const datatools::properties &prop = a_cluster.get_auxiliaries();
        if (prop.has_key(browser_tracks::COLOR_FLAG)) {
          prop.fetch(browser_tracks::COLOR_FLAG, hex_str);
        }
      }

      // Add subitem:
      std::ostringstream label_trajectory;
      //                label_trajectory.setf(ios::fixed, ios::floatfield);
      datatools::properties &properties = a_trajectory.grab_auxiliaries();
      if (properties.has_key("chi2") && properties.has_key("ndof")) {
        const double chi2 = properties.fetch_real("chi2");
        const size_t ndof = properties.fetch_integer("ndof");
        label_trajectory << " - chi2/ndf = " << std::setprecision(2) << std::fixed << chi2 << "/"
                         << std::setprecision(0) << ndof << std::setprecision(2) << std::fixed
                         << ", p = " << TMath::Prob(chi2, ndof);
      }
      if (properties.has_key("t0")) {
        const double t0 = properties.fetch_real("t0");
        label_trajectory << std::setprecision(0) << std::fixed << ", t0 = ";
        utils::root_utilities::get_prettified_time(label_trajectory, t0);
      }
      if (properties.has_key("guess")) {
        label_trajectory << " (" << properties.fetch_string("guess") << " guess)";
      }
      bool is_default = false;
      if (properties.has_flag("default")) {
        label_trajectory << " - default";
        is_default = true;
      }

      TGListTreeItem *item_trajectory = 0;
      if (a_trajectory.get_pattern().get_pattern_id() ==
          snemo::datamodel::helix_trajectory_pattern::pattern_id()) {
        // First time instantiate it
        if (!item_helix_solution) {
          item_helix_solution =
              new TGListTreeItemStdPlus("Helix trajectories", this, _get_colored_icon_("ofolder"),
                                        _get_colored_icon_("folder"),
                                        /*check=*/true);
          _tracks_list_box_->AddItem(item_solution, item_helix_solution);
          // _tracks_list_box_->OpenItem(item_helix_solution);
          item_helix_solution->SetUserData((void *)(intptr_t)++icheck_id);
          if (a_auxiliaries.has_key(browser_tracks::CHECKED_FLAG))
            item_helix_solution->CheckItem(a_auxiliaries.has_flag(browser_tracks::CHECKED_FLAG));
          _properties_dictionnary_[icheck_id] = &(a_auxiliaries);
        }

        item_trajectory = _tracks_list_box_->AddItem(
            item_helix_solution, std::string("helix" + label_trajectory.str()).c_str(),
            _get_colored_icon_("helix", hex_str, true), _get_colored_icon_("helix", hex_str));
      } else if (a_trajectory.get_pattern().get_pattern_id() ==
                 snemo::datamodel::line_trajectory_pattern::pattern_id()) {
        // First time instantiate it
        if (!item_line_solution) {
          const bool checked = true;
          item_line_solution =
              new TGListTreeItemStdPlus("Line trajectories", this, _get_colored_icon_("ofolder"),
                                        _get_colored_icon_("folder"), checked);
          _tracks_list_box_->AddItem(item_solution, item_line_solution);
          // _tracks_list_box_->OpenItem(item_line_solution);
          item_line_solution->SetUserData((void *)(intptr_t)++icheck_id);
          if (a_auxiliaries.has_key(browser_tracks::CHECKED_FLAG))
            item_line_solution->CheckItem(a_auxiliaries.has_flag(browser_tracks::CHECKED_FLAG));
          _properties_dictionnary_[icheck_id] = &(a_auxiliaries);
        }

        item_trajectory = _tracks_list_box_->AddItem(
            item_line_solution, std::string("line" + label_trajectory.str()).c_str(),
            _get_colored_icon_("line", hex_str, true), _get_colored_icon_("line", hex_str));
      }

      if (!item_trajectory) continue;

      item_trajectory->SetCheckBox(true);
      if (is_default) {
        a_trajectory.grab_auxiliaries().update(CHECKED_FLAG, true);
        _tracks_list_box_->CheckItem(item_trajectory, true);
      } else {
        a_trajectory.grab_auxiliaries().update(CHECKED_FLAG, false);
        _tracks_list_box_->CheckItem(item_trajectory, false);
      }
      item_trajectory->SetUserData((void *)(intptr_t) - (++icheck_id));
      _base_hit_dictionnary_[-icheck_id] = &(a_trajectory);

      if (options_mgr.get_option_flag(DUMP_INTO_TOOLTIP)) {
        std::ostringstream oss;
        a_trajectory.tree_dump(oss);
        item_trajectory->SetTipText(oss.str().c_str());
      } else {
        std::ostringstream message;
        message << "Double click to highlight trajectory "
                << "and to dump info on terminal";
        item_trajectory->SetTipText(message.str().c_str());
      }
    }  // end of trajectory loop
  }    // end of solution loop

  return;
}

void browser_tracks::_update_particle_track_data() {
  // Grab event and other resources. Here nothing is constant
  // since properties will be modified here and used later
  // through 'checking' and 'double_clicking' actions:
  io::event_record &event = _server_->grab_event();

  // 'tracker_trajectory_data' availability:
  if (!event.has(io::PTD_LABEL)) return;

  snemo::datamodel::particle_track_data &ptd =
      event.grab<snemo::datamodel::particle_track_data>(io::PTD_LABEL);

  const options_manager &options_mgr = options_manager::get_instance();
  if (!options_mgr.get_option_flag(SHOW_PARTICLE_TRACKS)) return;

  // Identifiant for item dictionnary:
  int &icheck_id = _item_id_;

  // Add 'tracker_clustering_data' item:
  const std::string data_bank_name = "Particle track data (" + io::PTD_LABEL + ")";
  TGListTreeItem *item_particle_track_data =
      _tracks_list_box_->AddItem(_top_item_, data_bank_name.c_str(), _get_colored_icon_("ofolder"),
                                 _get_colored_icon_("folder"));
  item_particle_track_data->SetCheckBox(false);
  item_particle_track_data->SetUserData((void *)(intptr_t)++icheck_id);
  _tracks_list_box_->OpenItem(item_particle_track_data);
  {
    std::ostringstream tip_text;
    if (options_mgr.get_option_flag(DUMP_INTO_TOOLTIP)) {
      ptd.tree_dump(tip_text);
    } else {
      tip_text << "Double click to expand particle track data";
    }
    item_particle_track_data->SetTipText(tip_text.str().c_str());
  }
  // Add unassociated calorimeter hits info
  if (ptd.has_non_associated_calorimeters()) {
    snemo::datamodel::calibrated_data::calorimeter_hit_collection_type &cc_collection =
        ptd.grab_non_associated_calorimeters();

    for (snemo::datamodel::calibrated_data::calorimeter_hit_collection_type::iterator it_hit =
             cc_collection.begin();
         it_hit != cc_collection.end(); ++it_hit) {
      snemo::datamodel::calibrated_calorimeter_hit &a_hit = it_hit->grab();

      // Add subsubitem:
      std::ostringstream label_hit;
      label_hit.precision(3);
      label_hit.setf(std::ios::fixed, std::ios::floatfield);
      label_hit << "Unassociated ";
      if (a_hit.get_auxiliaries().has_key("category"))
        label_hit << a_hit.get_auxiliaries().fetch_string("category") << " block ";
      label_hit << "hit #" << a_hit.get_hit_id() << " - E = ";
      utils::root_utilities::get_prettified_energy(label_hit, a_hit.get_energy(),
                                                   a_hit.get_sigma_energy());
      label_hit << " - t = ";
      utils::root_utilities::get_prettified_time(label_hit, a_hit.get_time(),
                                                 a_hit.get_sigma_time());

      TGListTreeItem *item_hit =
          _tracks_list_box_->AddItem(item_particle_track_data, label_hit.str().c_str());
      item_hit->SetUserData((void *)(intptr_t) - (++icheck_id));
      _base_hit_dictionnary_[-icheck_id] = &(a_hit);
      item_hit->SetPictures(_get_colored_icon_("calorimeter", "", true),
                            _get_colored_icon_("calorimeter"));

      {
        std::ostringstream tip_text;
        if (options_mgr.get_option_flag(DUMP_INTO_TOOLTIP)) {
          a_hit.tree_dump(tip_text);
        } else {
          tip_text << "Double click to highlight calorimeter hit "
                   << "and to dump info on terminal";
        }
        item_hit->SetTipText(tip_text.str().c_str());
      }
    }
  }

  if (!ptd.has_particles()) return;

  snemo::datamodel::particle_track_data::particle_collection_type &particles = ptd.grab_particles();
  for (snemo::datamodel::particle_track_data::particle_collection_type::iterator iparticle =
           particles.begin();
       iparticle != particles.end(); ++iparticle) {
    // Get current particle track:
    snemo::datamodel::particle_track &a_particle = iparticle->grab();

    // Add item:
    size_t item_color = 0;
    std::ostringstream label_particle;
    label_particle << "Particle #" << a_particle.get_track_id();
    if (a_particle.get_charge() == snemo::datamodel::particle_track::neutral) {
      label_particle << " - neutral charged particle";
      item_color = style_manager::get_instance().get_particle_color("gamma");
    } else if (a_particle.get_charge() == snemo::datamodel::particle_track::negative) {
      label_particle << " - negative charged particle";
      item_color = style_manager::get_instance().get_particle_color("electron");
    } else if (a_particle.get_charge() == snemo::datamodel::particle_track::positive) {
      label_particle << " - positive charged particle";
      item_color = style_manager::get_instance().get_particle_color("positron");
    } else if (a_particle.get_charge() == snemo::datamodel::particle_track::undefined) {
      label_particle << " - undefined charged particle";
      item_color = style_manager::get_instance().get_particle_color("alpha");
    }
    // Get particle auxiliaries:
    datatools::properties &a_auxiliaries = a_particle.grab_auxiliaries();
    // item_color = utils::root_utilities::get_fade_color_from(item_color, 0.5);
    std::string hex_str = utils::root_utilities::get_hex_color(item_color);
    if (a_auxiliaries.has_key(browser_tracks::COLOR_FLAG)) {
      a_auxiliaries.fetch(browser_tracks::COLOR_FLAG, hex_str);
    }

    TGListTreeItem *item_particle =
        _tracks_list_box_->AddItem(item_particle_track_data, label_particle.str().c_str());
    item_particle->SetPictures(_get_colored_icon_("track", hex_str),
                               _get_colored_icon_("track", hex_str));
    item_particle->SetUserData((void *)(intptr_t) - (++icheck_id));
    item_particle->SetCheckBox(true);
    item_particle->SetOpen(true);
    {
      // Set tooltip text
      std::ostringstream tip_text;
      a_particle.tree_dump(tip_text);
      item_particle->SetTipText(tip_text.str().c_str());
    }
    if (a_auxiliaries.has_key(browser_tracks::CHECKED_FLAG))
      item_particle->CheckItem(a_auxiliaries.has_flag(browser_tracks::CHECKED_FLAG));

    // Update base hit dictionnary:
    _base_hit_dictionnary_[-icheck_id] = &(a_particle);

    // Update property dictionnary:
    _properties_dictionnary_[-icheck_id] = &(a_auxiliaries);

    // Get associated vertices
    if (a_particle.has_vertices()) {
      snemo::datamodel::particle_track::vertex_collection_type &vts = a_particle.get_vertices();
      for (snemo::datamodel::particle_track::vertex_collection_type::iterator ivtx = vts.begin();
           ivtx != vts.end(); ++ivtx) {
        geomtools::blur_spot &a_vertex = ivtx->grab();

        std::ostringstream label;
        label << "Vertex on ";
        if (snemo::datamodel::particle_track::vertex_is_on_source_foil(a_vertex)) {
          label << "source foil - ";
        } else if (snemo::datamodel::particle_track::vertex_is_on_main_calorimeter(a_vertex)) {
          label << "main calorimeter wall - ";
        } else if (snemo::datamodel::particle_track::vertex_is_on_x_calorimeter(a_vertex)) {
          label << "X-calorimeter wall - ";
        } else if (snemo::datamodel::particle_track::vertex_is_on_gamma_veto(a_vertex)) {
          label << "gamma veto - ";
        } else if (snemo::datamodel::particle_track::vertex_is_on_wire(a_vertex)) {
          label << "wire - ";
        } else {
          label << "unkown part of the detector -";
        }
        label << "(x, y, z) = ";
        label.precision(3);
        label.setf(std::ios::fixed, std::ios::floatfield);
        label << a_vertex.get_position() / CLHEP::mm << " mm";
        TGListTreeItem *item_vertex = _tracks_list_box_->AddItem(
            item_particle, label.str().c_str(), _get_colored_icon_("vertex", hex_str, true),
            _get_colored_icon_("vertex", hex_str));
        item_vertex->SetCheckBox(false);
        item_vertex->SetUserData((void *)(intptr_t) - (++icheck_id));
        // Update properties dictionnary:
        _properties_dictionnary_[-icheck_id] = &(a_vertex.grab_auxiliaries());
        {
          std::ostringstream tip_text;
          if (options_mgr.get_option_flag(DUMP_INTO_TOOLTIP)) {
            a_vertex.tree_dump(tip_text);
          } else {
            tip_text << "Double click to highlight vertex position "
                     << "and to dump info on terminal";
          }
          item_vertex->SetTipText(tip_text.str().c_str());
        }
      }  // end of vertex list
    }    // end of vertex check

    // Get associated calorimeter
    if (a_particle.has_associated_calorimeter_hits()) {
      snemo::datamodel::calibrated_data::calorimeter_hit_collection_type &cc_collection =
          a_particle.get_associated_calorimeter_hits();

      for (snemo::datamodel::calibrated_data::calorimeter_hit_collection_type::iterator it_hit =
               cc_collection.begin();
           it_hit != cc_collection.end(); ++it_hit) {
        snemo::datamodel::calibrated_calorimeter_hit &a_hit = it_hit->grab();

        // Add subsubitem:
        std::ostringstream label_hit;
        label_hit.precision(3);
        label_hit.setf(std::ios::fixed, std::ios::floatfield);
        if (a_hit.get_auxiliaries().has_key("category"))
          label_hit << a_hit.get_auxiliaries().fetch_string("category") << " ";
        label_hit << "hit #" << a_hit.get_hit_id() << " - E = ";
        utils::root_utilities::get_prettified_energy(label_hit, a_hit.get_energy(),
                                                     a_hit.get_sigma_energy());
        label_hit << " - t = ";
        utils::root_utilities::get_prettified_time(label_hit, a_hit.get_time(),
                                                   a_hit.get_sigma_time());

        TGListTreeItem *item_hit =
            _tracks_list_box_->AddItem(item_particle, label_hit.str().c_str(),
                                       _get_colored_icon_("calorimeter", hex_str, true),
                                       _get_colored_icon_("calorimeter", hex_str));
        item_hit->SetUserData((void *)(intptr_t) - (++icheck_id));
        _base_hit_dictionnary_[-icheck_id] = &(a_hit);

        std::ostringstream tip_text;
        if (options_mgr.get_option_flag(DUMP_INTO_TOOLTIP)) {
          a_hit.tree_dump(tip_text);
        } else {
          tip_text << "Double click to highlight calorimeter hit "
                   << "and to dump info on terminal";
        }
        item_hit->SetTipText(tip_text.str().c_str());
      }  // end of associated calorimeter hits
    }    // end of calorimeter hits check
  }      // end of particle loop
  return;
}

datatools::properties *browser_tracks::get_item_properties(const int id_) {
  // Search in which dictionnary the item comes from:
  datatools::properties *properties = 0;
  {
    // First looking in 'properties' dictionnary:
    std::map<int, datatools::properties *>::iterator found = _properties_dictionnary_.find(id_);

    if (found != _properties_dictionnary_.end()) {
      properties = found->second;
    }
  }

  {
    // Then looking in 'base_hit' dictionnary:
    std::map<int, geomtools::base_hit *>::iterator found = _base_hit_dictionnary_.find(id_);

    if (found != _base_hit_dictionnary_.end()) {
      properties = &(found->second->grab_auxiliaries());
    }
  }
  return properties;
}

geomtools::base_hit *browser_tracks::get_base_hit(const int id_) {
  // First looking in 'properties' dictionnary:
  std::map<int, geomtools::base_hit *>::iterator found = _base_hit_dictionnary_.find(id_);

  if (found != _base_hit_dictionnary_.end()) {
    return found->second;
  }

  return 0;
}

void browser_tracks::item_checked(TObject *object_, bool is_checked_) {
  const int i = (intptr_t)object_;

  // Search in which dictionnary the item comes from:
  datatools::properties *properties = get_item_properties(i);

  // Non referenced item id:
  if (!properties) return;

  properties->update(CHECKED_FLAG, is_checked_);

  _browser_->track_select();
  return;
}

void browser_tracks::item_selected(TGListTreeItem *item_, int /*button_*/) {
  const int i = (intptr_t)item_->GetUserData();

  // Positive id means nothing to do:
  if (i > 0) return;

  // Search in which dictionnary the item comes from:
  datatools::properties *properties = get_item_properties(i);
  // Non referenced item id:
  if (!properties) return;

  properties->update(HIGHLIGHT_FLAG, !properties->has_flag(HIGHLIGHT_FLAG));

  if (options_manager::get_instance().get_option_flag(DUMP_INTO_TERMINAL)) {
    // Dump info on terminal
    geomtools::base_hit *bh = get_base_hit(i);

    if (bh) bh->dump();
  }

  _browser_->track_select();
  return;
}

const TGPicture *browser_tracks::_get_colored_icon_(const std::string &icon_type_,
                                                    const std::string &hex_color_,
                                                    const bool reverse_color_) {
  // xpm image are defined in browser_icons.h file
  //
  // Build icon name and look inside icon dictionnary:
  const std::string icon_name = icon_type_ + hex_color_ + (reverse_color_ ? "reverse" : "");

  std::map<std::string, const TGPicture *>::const_iterator found = _icons_.find(icon_name);

  if (found != _icons_.end()) return found->second;

  // Otherwise create new entry:
  const char **xpm = 0;
  if (icon_type_ == "vertex")
    xpm = xpm_vertex;
  else if (icon_type_ == "calorimeter")
    xpm = xpm_calorimeter;
  else if (icon_type_ == "geiger")
    xpm = xpm_geiger;
  else if (icon_type_ == "cluster")
    xpm = xpm_cluster;
  else if (icon_type_ == "helix")
    xpm = xpm_helix;
  else if (icon_type_ == "line")
    xpm = xpm_line;
  else if (icon_type_ == "step")
    xpm = xpm_step;
  else if (icon_type_ == "track")
    xpm = xpm_track;
  else if (icon_type_ == "flag")
    xpm = xpm_flag;
  else if (icon_type_ == "folder")
    xpm = xpm_folder;
  else if (icon_type_ == "ofolder")
    xpm = xpm_ofolder;
  else
    return 0;

  // Change color:
  if (icon_type_ != "folder" && icon_type_ != "ofolder") {
    std::string color = "#303030";
    if (!hex_color_.empty()) color = hex_color_;
    if (reverse_color_) {
      const std::string bg = ". c " + color;
      xpm[1] = bg.c_str();
      xpm[2] = "# c None";
    } else {
      const std::string fg = "# c " + color;
      xpm[1] = ". c None";
      xpm[2] = fg.c_str();
    }
  }

  // Create a picture from the XPM data:
  TGPicturePool *picpool = gClient->GetResourcePool()->GetPicturePool();
  const TGPicture *iconpic = picpool->GetPicture(icon_name.c_str(), const_cast<char **>(xpm));

  // Add new picture to dictionnary:
  _icons_[icon_name] = iconpic;

  return iconpic;
}

}  // end of namespace view

}  // end of namespace visualization

}  // end of namespace snemo

// end of browser_tracks.cc
/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
