/* visual_track_renderer.cc
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
#include <EventBrowser/view/options_manager.h>
#include <EventBrowser/view/style_manager.h>
#include <EventBrowser/view/visual_track_renderer.h>
#include <EventBrowser/detector/detector_manager.h>
#include <EventBrowser/detector/i_root_volume.h>
#include <EventBrowser/io/event_server.h>
#include <EventBrowser/utils/root_utilities.h>

#include <falaise/snemo/datamodels/helix_trajectory_pattern.h>
#include <falaise/snemo/datamodels/line_trajectory_pattern.h>

#include <mctools/utils.h>

#include <TLatex.h>
#include <TObjArray.h>
#include <TPolyLine3D.h>
#include <TPolyMarker3D.h>

namespace snemo {

	namespace visualization {

		namespace view {

			// ctor:
			visual_track_renderer::visual_track_renderer() = default;

			// dtor:
			visual_track_renderer::~visual_track_renderer() = default;

			void visual_track_renderer::push_mc_tracks() {
				const io::event_record &event = _server->get_event();
				const auto &sim_data = event.get<mctools::simulated_data>(io::SD_LABEL);

				// Get hit categories related to visual track
				std::vector<std::string> visual_categories;
				sim_data.get_step_hits_categories(
																					visual_categories, mctools::simulated_data::HIT_CATEGORY_TYPE_PREFIX, "__visu.tracks");
				if (visual_categories.empty()) {
					DT_LOG_INFORMATION(options_manager::get_instance().get_logging_priority(),
														 "Event has no __visu.tracks* hits");
					return;
				}

				// Enable/disable track hits
				std::set<int> enable_tracks;
				// Highlight track from primary particles
				const mctools::simulated_data::primary_event_type &pevent = sim_data.get_primary_event();
				const genbb::primary_event::particles_col_type &particles = pevent.get_particles();

				for (const auto &a_primary : particles) {
					if (!a_primary.has_generation_id()) {
						continue;
					}
					const datatools::properties &particle_aux = a_primary.get_auxiliaries();
					const int track_id = a_primary.get_generation_id() + 1;
					if (particle_aux.has_key(browser_tracks::CHECKED_FLAG) &&
							!particle_aux.has_flag(browser_tracks::CHECKED_FLAG)) {
						enable_tracks.insert(track_id);
					}
				}

				for (const auto &visual_categorie : visual_categories) {
					const mctools::simulated_data::hit_handle_collection_type &hit_collection =
						sim_data.get_step_hits(visual_categorie);
					if (hit_collection.empty()) {
						DT_LOG_INFORMATION(options_manager::get_instance().get_logging_priority(), "No MC hits");
						continue;
					}

					for (const auto &it_hit : hit_collection) {
						const mctools::base_step_hit &a_hit = it_hit.get();
						std::string particle_name = a_hit.get_particle_name();

						const std::string delta_ray_from_alpha_flag = mctools::track_utils::DELTA_RAY_FROM_ALPHA_FLAG;
						const bool is_delta_ray_from_alpha =
							a_hit.get_auxiliaries().has_flag(delta_ray_from_alpha_flag);

						if (is_delta_ray_from_alpha) {
							particle_name = "delta_ray_from_alpha";
						} else if (particle_name == "e+") {
							particle_name = "positron";
						} else if (particle_name == "e-") {
							particle_name = "electron";
						} else if (particle_name == "mu+") {
							particle_name = "muon_plus";
						} else if (particle_name == "mu-") {
							particle_name = "muon_minus";
						}

						style_manager &style_mgr = style_manager::get_instance();
						if (!style_mgr.has_particle_properties(particle_name)) {
							if (!style_mgr.add_particle_properties(particle_name)) {
								DT_LOG_INFORMATION(options_manager::get_instance().get_logging_priority(),
																	 "Particle '" << particle_name << "' has no properties set !");
							}
						}

						if (!style_mgr.get_particle_visibility(particle_name)) {
							continue;
						}

						size_t line_color = style_mgr.get_particle_color(particle_name);
						size_t line_width = style_mgr.get_mc_line_width();
						size_t line_style = style_mgr.get_mc_line_style();

						const datatools::properties &hit_aux = a_hit.get_auxiliaries();
						const int track_id = a_hit.get_track_id();
						if (hit_aux.has_key(browser_tracks::CHECKED_FLAG) &&
								!hit_aux.has_flag(browser_tracks::CHECKED_FLAG)) {
							enable_tracks.insert(track_id);
						}
						if (enable_tracks.count(track_id) != 0u) {
							continue;
						}

						if (hit_aux.has_flag(browser_tracks::HIGHLIGHT_FLAG)) {  //  &&
							// a_hit.get_auxiliaries().has_flag(mctools::hit_utils::HIT_VISU_HIGHLIGHTED_KEY)) {
							line_width += 3;
							TPolyMarker3D *mark1 = base_renderer::make_polymarker(a_hit.get_position_start());
							_objects->Add(mark1);
							mark1->SetMarkerColor(kRed);
							// mark1->SetMarkerStyle(kPlus);
							mark1->SetMarkerStyle(kOpenCrossX);
							TPolyMarker3D *mark2 = base_renderer::make_polymarker(a_hit.get_position_stop());
							_objects->Add(mark2);
							mark2->SetMarkerColor(kRed);
							mark2->SetMarkerStyle(kCircle);
						}
						geomtools::polyline_type points;
						points.push_back(a_hit.get_position_start());
						points.push_back(a_hit.get_position_stop());
						TPolyLine3D *mc_path = base_renderer::make_polyline(points);
						_objects->Add(mc_path);
						mc_path->SetLineColor(line_color);
						mc_path->SetLineWidth(line_width);
						mc_path->SetLineStyle(line_style);
					}
				}  // end of category list
			}

			void visual_track_renderer::push_mc_legend() {
				const style_manager &style_mgr = style_manager::get_instance();
				const std::map<std::string, style_manager::particle_properties> &particles =
					style_mgr.get_particles_properties();

				double x = 1.00;
				double y = 0.97;
				const double dx = 0.05;

				for (const auto &particle : particles) {
					const std::string particle_name = particle.first;
					const style_manager::particle_properties particle_properties = particle.second;

					if (!particle_properties._visibility_) {
						continue;
					}

					auto *legend = new TLatex;
					_objects->Add(legend);
					legend->SetNDC();
					legend->SetTextAlign(31);
					legend->SetTextSize(0.04);
					legend->SetTextFont(42);
					legend->SetTextColor(particle_properties._color_);

					const std::string latex = particle_properties._latex_name_;
					legend->SetText(x -= dx, y, latex.c_str());
				}

				// Add a latest legend text for particles not in the previous list
				auto *legend = new TLatex;
				_objects->Add(legend);
				legend->SetNDC();
				legend->SetTextAlign(31);
				legend->SetTextSize(0.04);
				legend->SetTextFont(42);
				legend->SetTextColor(style_mgr.get_particle_color("others"));
				legend->SetText(x -= dx, y, "others");
			}

			void visual_track_renderer::push_reconstructed_tracks() {
				const io::event_record &event = _server->get_event();
				const auto &pt_data = event.get<snemo::datamodel::particle_track_data>(io::PTD_LABEL);

				if (!pt_data.hasParticles()) {
					DT_LOG_DEBUG(options_manager::get_instance().get_logging_priority(),
											 "Event has no reconstructed particles");
					return;
				}

				// Show non-associated calorimeters
				if (pt_data.hasIsolatedCalorimeters()) {
					const snemo::datamodel::CalorimeterHitHdlCollection &calos = pt_data.isolatedCalorimeters();
					for (const auto &calo : calos) {
						const snemo::datamodel::calibrated_calorimeter_hit &a_calo = calo.get();
						const geomtools::geom_id &a_calo_gid = a_calo.get_geom_id();
						this->highlight_geom_id(a_calo_gid,
																		style_manager::get_instance().get_calibrated_data_color());
						if (!options_manager::get_instance().get_option_flag(SHOW_CALIBRATED_INFO)) {
							const double energy = a_calo.get_energy() / CLHEP::MeV;
							const double sigma_e = a_calo.get_sigma_energy() / CLHEP::MeV;
							const double time = a_calo.get_time() / CLHEP::ns;
							const double sigma_t = a_calo.get_sigma_time() / CLHEP::ns;

							// Save z position inside text and then parse it
							std::ostringstream text_to_parse;
							text_to_parse.precision(2);
							text_to_parse << std::fixed << "#splitline"
														<< "{E = " << energy << " #pm " << sigma_e << " MeV}"
														<< "{t  = " << time << " #pm " << sigma_t << " ns}";
							this->highlight_geom_id(a_calo_gid,
																			style_manager::get_instance().get_calibrated_data_color(),
																			text_to_parse.str());
						}
					}  // end of calorimeter list
				} else {
					DT_LOG_DEBUG(options_manager::get_instance().get_logging_priority(),
											 "No calorimeter hits unassociated to particle track");
				}

				for (const auto &particle : pt_data.particles()) {
					const snemo::datamodel::particle_track &a_particle = particle.get();

					if (a_particle.get_auxiliaries().has_key(browser_tracks::CHECKED_FLAG) &&
							!a_particle.get_auxiliaries().has_flag(browser_tracks::CHECKED_FLAG)) {
						continue;
					}

					// Get color from charge
					size_t color = 0;
					if (a_particle.get_charge() == snemo::datamodel::particle_track::CHARGE_NEUTRAL) {
						color = style_manager::get_instance().get_particle_color("gamma");
					} else if (a_particle.get_charge() == snemo::datamodel::particle_track::CHARGE_NEGATIVE) {
						color = style_manager::get_instance().get_particle_color("electron");
					} else if (a_particle.get_charge() == snemo::datamodel::particle_track::CHARGE_POSITIVE) {
						color = style_manager::get_instance().get_particle_color("positron");
					} else if (a_particle.get_charge() == snemo::datamodel::particle_track::CHARGE_UNDEFINED) {
						color = style_manager::get_instance().get_particle_color("alpha");
					}

					// Show reconstructed vertices
					if (a_particle.has_vertices()) {
						const snemo::datamodel::VertexHdlCollection & vertices =
							a_particle.get_vertices();
						for (const auto & a_vertex : vertices) {
							const geomtools::blur_spot & a_spot = a_vertex->get_spot();
							const geomtools::vector_3d & a_position = a_spot.get_position();
							{
								TPolyMarker3D * mark = base_renderer::make_polymarker(a_position);
								_objects->Add(mark);
								mark->SetMarkerColor(color);
								// mark->SetMarkerStyle(kStar);
								mark->SetMarkerStyle(kOpenCrossX);
							}
							if (a_vertex->get_auxiliaries().has_flag(browser_tracks::HIGHLIGHT_FLAG)) {
								TPolyMarker3D *mark = base_renderer::make_polymarker(a_position);
								_objects->Add(mark);
								mark->SetMarkerColor(color);
								mark->SetMarkerStyle(kCircle);
							}
						}  // end of vertex list

						// Gamma tracks
						if (a_particle.get_charge() == snemo::datamodel::particle_track::CHARGE_NEUTRAL) {
							geomtools::polyline_type polyVertices;
							for (const auto & a_vertex : vertices) {
								polyVertices.push_back(a_vertex->get_spot().get_position());
							}
							TPolyLine3D * track = base_renderer::make_polyline(polyVertices);
							_objects->Add(track);
							track->SetLineColor(color);
							if (a_particle.get_auxiliaries().has_flag("__gamma_from_annihilation")) {
								track->SetLineStyle(kDashDotted);
							} else {
								track->SetLineStyle(kDashed);
							}
							// track->SetLineWidth(line_width);
						}

					} else {
						DT_LOG_DEBUG(options_manager::get_instance().get_logging_priority(),
												 "No vertex associated to particle track");
					}

					// Show associated calorimeters
					if (a_particle.has_associated_calorimeter_hits()) {
						const snemo::datamodel::CalorimeterHitHdlCollection & calos =
							a_particle.get_associated_calorimeter_hits();
						for (const auto & calo : calos) {
							const snemo::datamodel::calibrated_calorimeter_hit &a_calo = calo.get();
							const geomtools::geom_id &a_calo_gid = a_calo.get_geom_id();
							this->highlight_geom_id(a_calo_gid, color);
							const double energy = a_calo.get_energy();
							const double sigma_e = a_calo.get_sigma_energy();
							const double time = a_calo.get_time();
							const double sigma_t = a_calo.get_sigma_time();

							// Save z position inside text and then parse it
							std::ostringstream oss;
							oss.precision(2);
							oss << std::fixed << "#splitline{E = ";
							utils::root_utilities::get_prettified_energy(oss, energy, sigma_e, true);
							oss << "}{t  = ";
							utils::root_utilities::get_prettified_time(oss, time, sigma_t, true);
							oss << "}";
							this->highlight_geom_id(a_calo_gid, color, oss.str());
						}  // end of calorimeter list
					} else {
						DT_LOG_DEBUG(options_manager::get_instance().get_logging_priority(),
												 "No calorimeter hits associated to particle track");
					}

					// Show attached fit
					if (a_particle.has_trajectory()) {
						const snemo::datamodel::tracker_trajectory &a_trajectory = a_particle.get_trajectory();
						const snemo::datamodel::base_trajectory_pattern &a_pattern = a_trajectory.get_pattern();
						const auto &iw3dr =
							dynamic_cast<const geomtools::i_wires_3d_rendering &>(a_pattern.get_shape());
						TPolyLine3D *track = base_renderer::make_track(iw3dr);
						_objects->Add(track);
						track->SetLineColor(color);
					}
				}
			}
	
		} // end of namespace view

	} // end of namespace visualization

} // end of namespace snemo
