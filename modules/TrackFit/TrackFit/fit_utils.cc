/// \file falaise/TrackFit/fit_utils.cc

// Ourselves
#include <TrackFit/fit_utils.h>

// Third party:
// - Bayeux/datatools:
#include <datatools/ioutils.h>

namespace TrackFit {

  double fit_utils::default_guess_bt_factor() {
    static double factor = 1.;
    return factor;
  }

  double fit_utils::default_vicinity_factor() {
    static double factor = 3.;
    return factor;
  }

  int fit_utils::compute_guess_trust_barycenter(const gg_hit_info_dict_type & hits_infos_,
						const gg_hit & /* hit_ */,
						const geomtools::vector_3d & /* hit_pos_ */,
						const geomtools::vector_3d & /* bottom_pos_ */,
						const geomtools::vector_3d & /* top_pos_ */) {
    if (hits_infos_.empty()) {
      return INVALID_HYPOTHESIS;
    }
    return INVALID_HYPOTHESIS;
  }

  int fit_utils::compute_guess_trust_counter(const gg_hits_col & hits_,
					     const geomtools::vector_3d & hit_pos_,
					     const geomtools::vector_3d & bottom_pos_,
					     const geomtools::vector_3d & top_pos_) {
    if (hits_.empty()) {
      return INVALID_HYPOTHESIS;
    }
    const geomtools::vector_2d hit_pos(hit_pos_.x(), hit_pos_.y());
    const geomtools::vector_2d bottom_pos(bottom_pos_.x(), bottom_pos_.y());
    const geomtools::vector_2d top_pos(top_pos_.x(), top_pos_.y());

    double vicinity_factor = default_vicinity_factor();

    size_t near_bottom_counter = 0;
    size_t near_top_counter = 0;
    const double min_distance = vicinity_factor * hits_.begin()->get_rmax();

    for (const auto &hit : hits_) {
      const geomtools::vector_2d curr_hit_pos(hit.get_x(), hit.get_y());
      const double distance = (hit_pos - curr_hit_pos).mag();

      if (distance > hit.get_rmax() && distance < min_distance) {
	const double bottom_current_distance = (bottom_pos - curr_hit_pos).mag();
	const double top_current_distance = (top_pos - curr_hit_pos).mag();
	if (bottom_current_distance < top_current_distance) {
	  near_bottom_counter++;
	} else if (top_current_distance < bottom_current_distance) {
	  near_top_counter++;
	}
      }
    }

    if (near_bottom_counter > near_top_counter) {
      return BOTTOM_HYPOTHESIS;
    }
    if (near_bottom_counter < near_top_counter) {
      return TOP_HYPOTHESIS;
    }
    return INVALID_HYPOTHESIS;
  }

  bool fit_utils::is_debug() const { return _debug_; }

  void fit_utils::set_debug(bool debug_) { _debug_ = debug_; }

  double fit_utils::get_vicinity_factor() const { return _vicinity_factor_; }

  void fit_utils::set_vicinity_factor(double vicinity_factor_) {
    _vicinity_factor_ = vicinity_factor_;
  }

  void fit_utils::reset() {
    set_vicinity_factor(fit_utils::default_vicinity_factor());
    set_debug(false);
    _config_.clear();
  }

  fit_utils::fit_utils(bool debug_) {
    reset();
    set_debug(debug_);
  }

  fit_utils::~fit_utils() = default;

  gg_hit_info::gg_hit_info(size_t max_nb_closest_hits_) {
    hit = nullptr;
    max_nb_closest_hits = max_nb_closest_hits_;
  }

  void gg_hit_info::update_closest_hits(const gg_hit * other_hit_, double dist_) {
    // If no neighbour hits have been registered yet :
    closest_hits_map[dist_] = other_hit_;

    // If maximum capacity of the map is reached, remove trailing elements :
    if (max_nb_closest_hits > 0 && closest_hits_map.size() > max_nb_closest_hits) {
      auto erase_pos = closest_hits_map.begin();
      for (unsigned int count = 0; count < max_nb_closest_hits; count++) {
	erase_pos++;
      }
      closest_hits_map.erase(erase_pos, closest_hits_map.end());
    }
  }

  void gg_hit_info::dump(std::ostream &out_) const {
    if (hit == nullptr) {
      out_ << "trackfit::gg_hit_info::dump: "
	   << "No hit !" << std::endl;
      return;
    }
    out_ << "trackfit::gg_hit_info::dump: "
	 << "Hit #" << hit->get_id() << std::endl;
    out_ << "trackfit::gg_hit_info::dump: "
	 << "Closest hits [" << closest_hits_map.size() << "] : " << std::endl;
    for (auto i : closest_hits_map) {
      out_ << "trackfit::gg_hit_info::dump: "
	   << "  Hit #" << i.second->get_id() << " @ " << i.first << std::endl;
    }
  }

  void build_hit_info_map(const gg_hits_col & hits_,
			  gg_hit_info_dict_type & dict_,
			  size_t max_nb_closest_hits_) {
    // Loop on hits :
    int irank = 0;
    for (auto i = hits_.begin(); i != hits_.end(); ++i, ++irank) {
      const gg_hit &hit_1 = *i;
      if (dict_.find(hit_1.get_id()) == dict_.end()) {
	gg_hit_info dummy_1(max_nb_closest_hits_);
	dummy_1.rank = irank;
	dummy_1.hit = &hit_1;
	dict_[hit_1.get_id()] = dummy_1;
      }
      // Current hit's infos from the map :
      gg_hit_info &wrapper_1 = dict_[hit_1.get_id()];
      const geomtools::vector_3d pos1(hit_1.get_x(), hit_1.get_y(), hit_1.get_z());

      // Loop on the other hits :
      auto j_start = i;
      j_start++;
      int jrank = irank + 1;
      for (auto j = j_start; j != hits_.end(); ++j, ++jrank) {
	const gg_hit &hit_2 = *j;
	if (dict_.find(hit_2.get_id()) == dict_.end()) {
	  gg_hit_info dummy_2(max_nb_closest_hits_);
	  dummy_2.rank = jrank;
	  dummy_2.hit = &hit_2;
	  dict_[hit_2.get_id()] = dummy_2;
	}
	// Current hit's infos from the map :
	gg_hit_info &wrapper_2 = dict_[hit_2.get_id()];
	const geomtools::vector_3d pos2(hit_2.get_x(), hit_2.get_y(), hit_2.get_z());

	// Compute distance between two hits :
	const double d12 = (pos2 - pos1).mag();

	// Store informations about the closest hits :
	const double d12_bis = d12 - (hit_1.get_r() + hit_2.get_r());
	wrapper_1.update_closest_hits(&hit_2, d12_bis);
	wrapper_2.update_closest_hits(&hit_1, d12_bis);
      }
    }
  }

} // end of namespace TrackFit
