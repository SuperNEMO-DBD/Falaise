/* event_browser_menu.cc
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

#include <EventBrowser/view/event_browser_menu.h>
#include <EventBrowser/view/options_manager.h>
#include <EventBrowser/detector/detector_manager.h>
#include <EventBrowser/io/event_server.h>
#include <EventBrowser/log.h>

#include <TTimer.h>
#include <TVirtualX.h>

#include <iostream>
#include <set>

namespace snemo {

	namespace visualization {

		namespace view {

			void TGMenuEntryPlus::SetLabel(TGHotString *label_) { fLabel = label_; }
			void TGMenuEntryPlus::SetPicture(const TGPicture *pic_) { fPic = pic_; }
			void TGMenuEntryPlus::SetType(const EMenuEntryType type_) { fType = type_; }
			void TGMenuEntryPlus::SetEntryId(const int id_) { fEntryId = id_; }
			void TGMenuEntryPlus::SetPopup(TGPopupMenu *popup_) { fPopup = popup_; }
			void TGMenuEntryPlus::SetStatus(const int status_) { fStatus = status_; }
			void TGMenuEntryPlus::SetEx(const unsigned int ex_) { fEx = ex_; }
			void TGMenuEntryPlus::SetEy(const unsigned int ey_) { fEy = ey_; }
			void TGMenuEntryPlus::SetEh(const unsigned int eh_) { fEh = eh_; }
			void TGMenuEntryPlus::SetEw(const unsigned int ew_) { fEw = ew_; }
			void TGMenuEntryPlus::SetUserData(void *user_data_) { fUserData = user_data_; }
			//________________________________________________________________________

			TGPopupMenuPlus::TGPopupMenuPlus(const TGWindow *p) : TGPopupMenu(p) {}

			TGPopupMenuPlus::~TGPopupMenuPlus() = default;

			void TGPopupMenuPlus::AddPopupPlus(TGHotString *s, TGPopupMenu *popup, Int_t id,
																				 TGMenuEntry *before, const TGPicture *p) {
				// Add a (cascading) popup menu to a popup menu. The hotstring is adopted
				// by the menu (actually by the TGMenuEntry) and deleted when possible.
				// If before is not 0, the entry will be added before it.

				auto *nw = new TGMenuEntryPlus;

				nw->SetLabel(s);
				nw->SetPicture(p);
				nw->SetType(kMenuPopup);
				nw->SetEntryId(id);
				nw->SetPopup(popup);
				nw->SetStatus(kMenuEnableMask);
				nw->SetEx(2);
				nw->SetEy(fMenuHeight - 2);
				nw->SetUserData(nullptr);

				if (before != nullptr) {
					fEntryList->AddBefore(before, nw);
				} else {
					fEntryList->Add(nw);
				}

				UInt_t tw = gVirtualX->TextWidth(fHifontStruct, s->GetString(), s->GetLength());

				UInt_t ph = 0, pw = 8;
				if (p != nullptr) {
					ph = p->GetHeight();
					pw = p->GetWidth();
					if (pw + 12 > fXl) {
						fMenuWidth += pw + 12 - fXl;
						fXl = pw + 12;
					}
				}
				Int_t max_ascent, max_descent;
				nw->SetEw(tw + pw + 18 + 12);
				fMenuWidth = TMath::Max(fMenuWidth, nw->GetEw());
				gVirtualX->GetFontProperties(fHifontStruct, max_ascent, max_descent);
				nw->SetEh(max_ascent + max_descent + fEntrySep);
				if (nw->GetEh() < ph + fEntrySep) {
					nw->SetEh(ph + fEntrySep);
				}
				fMenuHeight += nw->GetEh();

				if (before != nullptr) {
					Reposition();
				} else {
					Resize(fMenuWidth, fMenuHeight);
				}
			}

			void TGPopupMenuPlus::AddPopupPlus(const char *s, TGPopupMenu *popup, Int_t id, TGMenuEntry *before,
																				 const TGPicture *p) {
				// Add a (cascading) popup menu to a popup menu. The string s is not
				// adopted. If before is not 0, the entry will be added before it.

				AddPopupPlus(new TGHotString(s), popup, id, before, p);
			}

			void TGPopupMenuPlus::PlaceMenu(Int_t x, Int_t y, Bool_t stick_mode, Bool_t grab_pointer) {
				// Reset fPreviousId value
				fPreviousId = 0;
				TGPopupMenu::PlaceMenu(x, y, stick_mode, grab_pointer);
			}

			Int_t TGPopupMenuPlus::EndMenu(void *&userData) {
				// Close menu and return ID of selected menu item.
				// In case of cascading menus, recursively close all menus.

				Int_t id;

				if (fDelay != nullptr) {
					fDelay->Remove();
				}

				// destroy any cascaded childs and get any ID
				auto *current = (TGMenuEntryPlus *)fCurrent;

				if (current != nullptr) {
					// deactivate the entry
					int status = current->GetStatus();
					status &= ~kMenuActiveMask;
					current->SetStatus(status);

					if (current->GetType() == kMenuPopup && (current->GetPopup() != nullptr)) {
						id = current->GetPopup()->EndMenu(userData);

						if (fPreviousId == 0) {
							fPreviousId = current->GetEntryId();
						}

						if (id == -1) {
							id = fPreviousId;
							userData = nullptr;
							// SendMessage(fMsgWindow, MK_MSG(kC_COMMAND, kCM_MENU),
							//             g_test,(Long_t)userData);
						}
						// fCurrent = 0;
					} else {
						// return the ID if the entry is enabled, otherwise -1
						if ((status & kMenuEnableMask) != 0) {
							id = current->GetEntryId();
							userData = current->GetUserData();
						} else {
							id = -1;
							userData = nullptr;
						}
					}
				} else {
					// if no entry selected...
					id = -1;
					userData = nullptr;
				}

				// then unmap itself
				UnmapWindow();

				gClient->UnregisterPopup(this);
				//        if (fMenuBar) fMenuBar->BindKeys(kFALSE);

				if (fPoppedUp) {
					fPoppedUp = kFALSE;
					PoppedDown();
				}

				return id;
			}

			void TGPopupMenuPlus::UpdateDictionnary(std::map<button_signals_type, TGPopupMenuPlus *> &dict_) {
				TGMenuEntry *ptr;
				TIter next(fEntryList);

				while ((ptr = (TGMenuEntry *)next()) != nullptr) {
					const auto id = (button_signals_type)ptr->GetEntryId();
					if (id > 0) {
						dict_[id] = this;
					}
				}
			}

			//________________________________________________________________________

			// ctor:
			event_browser_menu::event_browser_menu(TGCompositeFrame *main_) {
				// for the global menu
				auto *menu_file = new TGPopupMenuPlus(gClient->GetRoot());
				menu_file->AddEntry("Open file...", FILE_OPEN);
				menu_file->AddEntry("Connect to DAQ server...", CONNECT_TO_DAQ);
				menu_file->AddEntry("Reload", FILE_RELOAD);
				menu_file->AddSeparator();
				menu_file->AddEntry("Save event as xml", SAVE_EVENT_TO_XML);
				menu_file->AddEntry("Save event as...", SAVE_EVENT_AS);
				menu_file->AddSeparator();
				menu_file->AddEntry("Load detector config.", LOAD_DETECTOR);
				menu_file->AddEntry("Load style file", LOAD_STYLE);
				menu_file->AddSeparator();
				menu_file->AddEntry("Quit", FILE_QUIT);

				menu_file->DisableEntry(CONNECT_TO_DAQ);
				menu_file->DisableEntry(LOAD_DETECTOR);

				// save options for cascaded menu
				auto *save_submenu = new TGPopupMenuPlus(gClient->GetRoot());
				save_submenu->AddEntry("Save 2D view as eps", PRINT_2D_AS_EPS);
				save_submenu->AddEntry("Save 2D view as...", PRINT_2D);
				save_submenu->AddSeparator();
				save_submenu->AddEntry("Save 3D view as eps", PRINT_3D_AS_EPS);
				save_submenu->AddEntry("Save 3D view as...", PRINT_3D);

				// view options
				auto *menu_view = new TGPopupMenuPlus(gClient->GetRoot());
				menu_view->AddEntry("Full 2D view", FULL_2D_VIEW);
				menu_view->UpdateDictionnary(_popup_dict_);
				menu_view->AddSeparator();
				menu_view->AddEntry("X3D view", VIEW_X3D);
				menu_view->AddEntry("Open GL view", VIEW_OGL);
				menu_view->AddSeparator();
				menu_view->AddPopupPlus("Save view as...", save_submenu);

				// Option menu
				auto *menu_options = new TGPopupMenuPlus(gClient->GetRoot());
				menu_options->AddEntry("Focus on ROI", FOCUS_ROI);
				menu_options->AddSeparator();
				menu_options->AddEntry("Show simu. vertex", SHOW_MC_VERTEX);
				menu_options->AddEntry("Show simu. tracks", SHOW_MC_TRACKS);

				// Submenu for simulated data
				auto *tracker_submenu = new TGPopupMenuPlus(gClient->GetRoot());
				tracker_submenu->AddEntry("Show Geiger drift circle", SHOW_GG_CIRCLE);
				tracker_submenu->AddEntry("Show Geiger hits with time gradient", SHOW_GG_TIME_GRADIENT);
				tracker_submenu->UpdateDictionnary(_popup_dict_);
				auto *sim_hits_submenu = new TGPopupMenuPlus(gClient->GetRoot());
				sim_hits_submenu->AddEntry("Show calorimeter hits", SHOW_MC_CALORIMETER_HITS);
				sim_hits_submenu->AddPopupPlus("Show tracker hits", tracker_submenu, SHOW_MC_TRACKER_HITS);
				sim_hits_submenu->UpdateDictionnary(_popup_dict_);
				menu_options->AddPopupPlus("Show simu. hits", sim_hits_submenu, SHOW_MC_HITS);
				menu_options->AddSeparator();

				menu_options->AddEntry("Show calib. hits", SHOW_CALIBRATED_HITS);
				menu_options->AddEntry("Show calib. info", SHOW_CALIBRATED_INFO);
				menu_options->AddSeparator();

				auto *cluster_submenu = new TGPopupMenuPlus(gClient->GetRoot());
				cluster_submenu->AddEntry("Show tracker hit as box", SHOW_TRACKER_CLUSTERED_BOX);
				cluster_submenu->AddEntry("Show tracker hit as circle", SHOW_TRACKER_CLUSTERED_CIRCLE);
				cluster_submenu->UpdateDictionnary(_popup_dict_);
				menu_options->AddPopupPlus("Show tracker clustered hits", cluster_submenu,
																	 SHOW_TRACKER_CLUSTERED_HITS);
				auto *trajectory_submenu = new TGPopupMenuPlus(gClient->GetRoot());
				trajectory_submenu->AddEntry("Show recalibrated tracker hits (only for delayed tracks)",
																		 SHOW_RECALIBRATED_TRACKER_HITS);
				trajectory_submenu->UpdateDictionnary(_popup_dict_);
				menu_options->AddPopupPlus("Show tracker trajectories", trajectory_submenu,
																	 SHOW_TRACKER_TRAJECTORIES);
				menu_options->AddSeparator();

				menu_options->AddEntry("Show particle tracks", SHOW_PARTICLE_TRACKS);
				menu_options->AddSeparator();

				auto *dump_submenu = new TGPopupMenuPlus(gClient->GetRoot());
				dump_submenu->AddEntry("Dump into tooltip", DUMP_INTO_TOOLTIP);
				dump_submenu->AddEntry("Dump into terminal", DUMP_INTO_TERMINAL);
				dump_submenu->AddEntry("Dump into a new window", DUMP_INTO_WINDOW);
				dump_submenu->UpdateDictionnary(_popup_dict_);
				menu_options->AddPopupPlus("Dump event info", dump_submenu, -DUMP_EVENT);
				menu_options->UpdateDictionnary(_popup_dict_);

				// miscellaneous menu
				auto *menu_misc_simu = new TGPopupMenuPlus(gClient->GetRoot());
				menu_misc_simu->AddEntry("Show simu. tracks", SHOW_ALL_MC_TRACKS);
				menu_misc_simu->AddEntry("Show simu. hits", SHOW_ALL_MC_HITS);
				menu_misc_simu->AddEntry("Show simu. tracks and hits", SHOW_ALL_MC_TRACKS_AND_HITS);
				auto *menu_misc = new TGPopupMenuPlus(gClient->GetRoot());
				menu_misc->AddEntry("Show all vertices", SHOW_ALL_VERTICES);
				menu_misc->AddPopup("Show all simulated data", menu_misc_simu);
				menu_misc->AddSeparator();
				menu_misc->AddEntry("Show all calib. hits", SHOW_ALL_CALIBRATED_HITS);

				// for some help
				auto *menu_help = new TGPopupMenuPlus(gClient->GetRoot());
				menu_help->AddEntry("Documentation ...", DOCUMENTATION);
				menu_help->AddSeparator();
				menu_help->AddEntry("About", ABOUT);

				menu_file->Associate(main_);
				menu_view->Associate(main_);
				menu_options->Associate(main_);
				menu_misc->Associate(main_);
				menu_help->Associate(main_);

				auto *menu_bar = new TGMenuBar(main_);
				menu_bar->AddPopup("File", menu_file, new TGLayoutHints(kLHintsTop | kLHintsLeft, 0, 4, 0, 0));
				menu_bar->AddPopup("View", menu_view, new TGLayoutHints(kLHintsTop | kLHintsLeft, 0, 4, 0, 0));
				menu_bar->AddPopup("Options", menu_options,
													 new TGLayoutHints(kLHintsTop | kLHintsLeft, 0, 4, 0, 0));
				menu_bar->AddPopup("Misc.", menu_misc, new TGLayoutHints(kLHintsTop | kLHintsLeft, 0, 4, 0, 0));
				menu_bar->AddPopup("Help", menu_help, new TGLayoutHints(kLHintsTop | kLHintsRight));
				menu_bar->DrawBorder();

				main_->AddFrame(menu_bar,
												new TGLayoutHints(kLHintsTop | kLHintsLeft | kLHintsExpandX, 0, 0, 1, 1));
			}

			// dtor:
			event_browser_menu::~event_browser_menu() { _popup_dict_.clear(); }

			void event_browser_menu::update(const button_signals_type signal_) {
				FL_LOG_DEVEL("Entering...");
				if (!has_option(signal_)) {
					FL_LOG_DEVEL("Exiting...");
					return;
				}

				TGPopupMenuPlus *ptr = _popup_dict_[signal_];

				if (!ptr->IsEntryEnabled(signal_) || ptr->IsEntryHidden(signal_)) {
					FL_LOG_DEVEL("Exiting...");
					return;
				}

				const bool flag = ptr->IsEntryChecked(signal_);

				if (flag) {
					uncheck_option(signal_);
				} else {
					check_option(signal_);
				}

				options_manager &options_mgr = options_manager::get_instance();
				options_mgr.set_option_flag(signal_, !flag);
				FL_LOG_DEVEL("Exiting...");
			}

			void event_browser_menu::set_default_option(const io::event_server &server_) {
				FL_LOG_DEVEL("Entering...");
				const options_manager &options_mgr = options_manager::get_instance();
				const std::map<button_signals_type, bool> &option_dict = options_mgr.get_options_dictionnary();
				for (auto i : option_dict) {
					const button_signals_type id = i.first;
					const bool flag = i.second;

					if (flag) {
						check_option(id);
					} else {
						uncheck_option(id);
					}
				}

				// Disable entries following detector setup
				const detector::detector_manager &detector_mgr = detector::detector_manager::get_instance();
				const std::string &setup_label = detector_mgr.get_setup_label_name();

				if (setup_label.find("bipo") != std::string::npos ||
						setup_label.find("test_bench") != std::string::npos) {
					disable_option(SHOW_MC_TRACKER_HITS);
					// disable_option(SHOW_GG_CIRCLE);
					// disable_option(SHOW_GG_TIME_GRADIENT);
					hide_option(SHOW_TRACKER_CLUSTERED_HITS);
					hide_option(SHOW_TRACKER_TRAJECTORIES);
				}

				disable_option(DUMP_INTO_WINDOW);

				if (!server_.is_initialized()) {
					disable_option(SHOW_MC_VERTEX);
					disable_option(SHOW_MC_HITS);
					disable_option(SHOW_MC_TRACKS);
					disable_option(SHOW_CALIBRATED_HITS);
					disable_option(SHOW_CALIBRATED_INFO);
					disable_option(SHOW_TRACKER_CLUSTERED_HITS);
					disable_option(SHOW_PARTICLE_TRACKS);
				} else {
					if (!server_.get_event().has(io::SD_LABEL)) {
						disable_option(SHOW_MC_VERTEX);
						disable_option(SHOW_MC_HITS);
						disable_option(SHOW_MC_TRACKS);
					}

					if (!server_.get_event().has(io::CD_LABEL)) {
						disable_option(SHOW_CALIBRATED_HITS);
						disable_option(SHOW_CALIBRATED_INFO);
					}

					if (!server_.get_event().has(io::TCD_LABEL)) {
						disable_option(SHOW_TRACKER_CLUSTERED_HITS);
					}

					if (!server_.get_event().has(io::TTD_LABEL)) {
						disable_option(SHOW_TRACKER_TRAJECTORIES);
					}

					if (!server_.get_event().has(io::PTD_LABEL)) {
						disable_option(SHOW_PARTICLE_TRACKS);
					}
				}
				FL_LOG_DEVEL("Exiting...");
			}

			bool event_browser_menu::has_option(const button_signals_type signal_) const {
				auto found = _popup_dict_.find(signal_);

				if (found == _popup_dict_.end()) {
					DT_LOG_WARNING(datatools::logger::PRIO_WARNING,
												 "No signal with " << signal_ << " id has been found !");
					return false;
				}
				return true;
			}

			void event_browser_menu::check_option(const button_signals_type signal_) {
				change_option(signal_, CHECKED);
			}

			void event_browser_menu::uncheck_option(const button_signals_type signal_) {
				change_option(signal_, UNCHECKED);
			}

			void event_browser_menu::enable_option(const button_signals_type signal_) {
				change_option(signal_, ENABLED);
			}

			void event_browser_menu::disable_option(const button_signals_type signal_) {
				change_option(signal_, DISABLED);
			}

			void event_browser_menu::hide_option(const button_signals_type signal_) {
				change_option(signal_, HIDDEN);
			}

			bool event_browser_menu::rcheck_option(const button_signals_type signal_, const int status_) {
				std::list<std::pair<button_signals_type, button_signals_type> > signal_intervals;
				signal_intervals.emplace_back(DUMP_INTO_TOOLTIP, DUMP_INTO_WINDOW);
				signal_intervals.emplace_back(SHOW_TRACKER_CLUSTERED_BOX, SHOW_TRACKER_CLUSTERED_CIRCLE);

				for (std::list<std::pair<button_signals_type, button_signals_type> >::const_iterator i =
							 signal_intervals.begin();
						 i != signal_intervals.end(); ++i) {
					if (signal_ >= i->first && signal_ <= i->second) {
						TGPopupMenuPlus *ptr = _popup_dict_[signal_];
						ptr->RCheckEntry(signal_, i->first, i->second);

						options_manager &options_mgr = options_manager::get_instance();
						for (int j = i->first; j <= i->second; j++) {
							const auto bst = (button_signals_type)j;

							if (bst == signal_) {
								options_mgr.set_option_flag(bst, status_ == CHECKED);
							} else {
								options_mgr.set_option_flag(bst, status_ != CHECKED);
							}
						}
						return true;
					}
				}
				return false;
			}

			void event_browser_menu::change_option(const button_signals_type signal_, const int status_) {
				if (!has_option(signal_)) {
					return;
				}

				if (status_ == DISABLED) {
					options_manager::get_instance().set_option_flag(signal_, false);
				}

				// Add exception for cascaded menu with radio button:
				std::set<button_signals_type> button_exception;
				button_exception.insert(SHOW_MC_HITS);
				button_exception.insert(SHOW_TRACKER_CLUSTERED_HITS);

				// Take care of radio check:
				if (status_ == CHECKED && rcheck_option(signal_, status_)) {
					return;
				}

				TGPopupMenuPlus *ptr = _popup_dict_[signal_];

				TGMenuEntryPlus *ntr;
				TIter next(ptr->GetListOfEntries());

				while ((ntr = (TGMenuEntryPlus *)next()) != nullptr) {
					const int id = (int)signal_;

					if (ntr->GetEntryId() != id) {
						continue;
					}

					// Othewise disable/enable, unhide/hide recursively all
					// related cascaded menu
					if (ntr->GetPopup() != nullptr) {
						// Do not (un)check recursively
						if (status_ == UNCHECKED || status_ == DISABLED) {
							ntr->SetType(kMenuEntry);
						}
						if (status_ == CHECKED || status_ == ENABLED) {
							ntr->SetType(kMenuPopup);
						}

						// Do not go into other menus since the job is done by the two previous comand
						// TGPopupMenuPlus * pptr = dynamic_cast<TGPopupMenuPlus*>(ntr->GetPopup());

						// TGMenuEntryPlus *nntr;
						// TIter next(pptr->GetListOfEntries());

						// while ((nntr = (TGMenuEntryPlus *) next()))
						//   {
						//     const button_signals_type id = (button_signals_type)nntr->GetEntryId();
						//     if (button_exception.find(signal_) == button_exception.end())
						//       {
						//         options_manager & options_mgr = options_manager::get_instance();
						//         if (!options_mgr.get_option_flag(signal_) && status_ == CHECKED)
						//           change_option(id, status_);
						//       }
						//   }
					}

					int status = ntr->GetStatus();
					switch (status_) {
					case DISABLED:
						status &= ~kMenuEnableMask;
						status &= ~kMenuCheckedMask;
						break;
					case ENABLED:
						status |= kMenuEnableMask;
						if ((status & kMenuHideMask) != 0) {
							status &= ~kMenuHideMask;
						}
						break;
					case UNCHECKED:
						status &= ~kMenuCheckedMask;
						break;
					case CHECKED:
						status |= kMenuCheckedMask;
						status |= kMenuEnableMask;
						break;
					case HIDDEN:
						status |= kMenuHideMask;
						status &= ~kMenuEnableMask;
						break;
					}
					ntr->SetStatus(status);
					break;
				}
			}

		} // end of namespace view

	} // end of namespace visualization

} // end of namespace snemo
