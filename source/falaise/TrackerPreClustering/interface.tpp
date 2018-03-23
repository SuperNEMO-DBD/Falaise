// -*- mode: c++ ; -*-
/** \file falaise/TrackerPreClustering/interface.tpp
 */

#ifndef FALAISE_TRACKERPRECLUSTERING_INTERFACE_TPP
#define FALAISE_TRACKERPRECLUSTERING_INTERFACE_TPP 1

namespace TrackerPreClustering {

template <class Hit>
const std::string& input_data<Hit>::get_last_error_message() const {
  return _last_error_message;
}

template <class Hit>
void input_data<Hit>::set_last_error_message(const std::string& message_) {
  _last_error_message = message_;
  return;
}

template <class Hit>
input_data<Hit>::input_data() {
  return;
}

template <class Hit>
void input_data<Hit>::reset() {
  hits.clear();
  _last_error_message.clear();
  return;
}

template <class Hit>
bool input_data<Hit>::check() const {
  input_data* mutable_this = const_cast<input_data*>(this);
  hit_collection_type tags;
  tags.reserve(hits.size());
  for (unsigned int i = 0; i < hits.size(); i++) {
    const hit_type* a_hit = hits.at(i);
    if (a_hit == 0) {
      std::ostringstream message;
      message << "TrackerPreClustering::input_data<>::check: "
              << "Null hit !";
      mutable_this->set_last_error_message(message.str());
      return false;
    }
    if (std::find(tags.begin(), tags.end(), a_hit) != tags.end()) {
      std::ostringstream message;
      message << "TrackerPreClustering::input_data<>::check: "
              << "Double referenced hit !";
      mutable_this->set_last_error_message(message.str());
      return false;
    }
    tags.push_back(a_hit);
    if (!a_hit->has_geom_id()) {
      std::ostringstream message;
      message << "TrackerPreClustering::input_data<>::check: "
              << "Missing GID !";
      mutable_this->set_last_error_message(message.str());
      return false;
    }
    if (!a_hit->has_xy()) {
      std::ostringstream message;
      message << "TrackerPreClustering::input_data<>::check: "
              << "Missing XY position !";
      mutable_this->set_last_error_message(message.str());
      return false;
    }
    if (a_hit->is_delayed() && !a_hit->has_delayed_time()) {
      std::ostringstream message;
      message << "TrackerPreClustering::input_data<>::check: "
              << "Missing delayed time !";
      mutable_this->set_last_error_message(message.str());
      return false;
    }
  }
  return true;
}

template <class Hit>
output_data<Hit>::output_data() {
  return;
}

template <class Hit>
void output_data<Hit>::reset() {
  ignored_hits.clear();
  prompt_clusters.clear();
  delayed_clusters.clear();
  return;
}

template <class Hit>
void output_data<Hit>::dump(std::ostream& out_) const {
  out_ << "TrackerPreClustering::output_data: " << std::endl;
  out_ << "|-- Ignored hits : " << ignored_hits.size() << std::endl;
  out_ << "|-- Prompt clusters: " << prompt_clusters.size() << std::endl;
  for (unsigned int i = 0; i < prompt_clusters.size(); i++) {
    if (i < prompt_clusters.size() - 1) {
      out_ << "|   |-- ";
    } else {
      out_ << "|   `-- ";
    }
    out_ << "Prompt cluster #" << i << "  size : " << prompt_clusters.at(i).size() << std::endl;
  }
  out_ << "`-- Delayed clusters: " << delayed_clusters.size() << std::endl;
  for (unsigned int i = 0; i < delayed_clusters.size(); i++) {
    if (i < delayed_clusters.size() - 1) {
      out_ << "    |-- ";
    } else {
      out_ << "    `-- ";
    }
    out_ << "Delayed cluster #" << i << "  size : " << delayed_clusters.at(i).size() << std::endl;
  }

  return;
}

}  // end of namespace TrackerPreClustering

#endif  // FALAISE_TRACKERPRECLUSTERING_INTERFACE_TPP
