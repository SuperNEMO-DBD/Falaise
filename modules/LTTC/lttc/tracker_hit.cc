/// \file lttc/tracker_hit.cc

// Ourselves:
#include <lttc/tracker_hit.hh>

// Standard library:                                                            
#include <algorithm>

// Third party:
// - Bayeux/datatools :
#include <datatools/exception.h>
#include <datatools/utils.h>

// This project:
#include <lttc/circle.hh>

namespace lttc {

  tracker_hit::tracker_hit(int id_,
                           int side_id_,
                           int layer_id_,
                           int row_id_,
                           double drift_radius_,
                           double drift_radius_err_,
                           double z_,
                           double z_err_,
                           bool delayed_)
  {
    DT_THROW_IF(id_ < -1, std::logic_error, "Invalid tracker hit ID value!");
    DT_THROW_IF(side_id_ < -1 or side_id_ > 1, std::logic_error, "Invalid tracker side ID value!");
    DT_THROW_IF(row_id_ < -1 or row_id_ > 112, std::logic_error, "Invalid tracker row ID value!");
    DT_THROW_IF(layer_id_ < -1 or layer_id_ > 8, std::logic_error, "Invalid tracker layer ID value!");
    id = id_;
    side_id = side_id_;
    layer_id = layer_id_;
    row_id = row_id_;
    drift_radius = drift_radius_;
    drift_radius_err = drift_radius_err_;
    z = z_;
    z_err = z_err_;
    delayed = delayed_;
  }
 
  void tracker_hit::reset()
  {
    id = -1;
    side_id = -1;
    layer_id = -1;
    row_id = -1;
    drift_radius = std::numeric_limits<double>::quiet_NaN();
    drift_radius_err = std::numeric_limits<double>::quiet_NaN();
    z = std::numeric_limits<double>::quiet_NaN();
    z_err = std::numeric_limits<double>::quiet_NaN();
    delayed = false;
  }

  bool tracker_hit::check() const
  {
    if (id < 0) return false;
    if (side_id < 0) return false;
    if (layer_id < 0) return false;
    if (row_id < 0) return false;
    if (datatools::is_valid(drift_radius)) return false;
    if (datatools::is_valid(drift_radius_err)) return false;
    if (datatools::is_valid(z)) return false;
    if (datatools::is_valid(z_err)) return false;
    return true;
  }

  void tracker_hit::print(std::ostream & out_) const
  {
    out_ << "|-- " << "ID = " << id << '\n';
    out_ << "|-- " << "Side = " << side_id << '\n';
    out_ << "|-- " << "Layer = " << layer_id << '\n';
    out_ << "|-- " << "Row = " << row_id << '\n';
    out_ << "|-- " << "Drift radius = " << drift_radius / CLHEP::mm << " mm" << '\n';
    out_ << "|-- " << "Drift radius error = " << drift_radius_err / CLHEP::mm << " mm" << '\n';
    out_ << "|-- " << "Z = " << z / CLHEP::mm << " mm" << '\n';
    out_ << "|-- " << "Z error = " << z_err / CLHEP::mm << " mm" << '\n';
    out_ << "`-- " << "Delayed = " << std::boolalpha << delayed << '\n';
    return;
  }

  cell_id tracker_hit::get_cell_id() const
  {
    return cell_id(side_id, layer_id, row_id); 
  }

  bool tracker_hit_collection::has_hit_with_id(int id_) const
  {
    return std::find_if(this->begin(), this->end(), [&](const tracker_hit & hit_) { return hit_.id == id_; }) != this->end();
  }

  bool tracker_hit_collection::has_hit_with_cell_id(const cell_id & cell_id_) const
  {
    return std::find_if(this->begin(), this->end(), [&](const tracker_hit & hit_) { return hit_.get_cell_id() == cell_id_; }) != this->end();
  }

  int tracker_hit_collection::max_hit_id() const
  {
    int maxId = -1;
    for (const auto & hit : *this) {
      if (hit.id > maxId) maxId = hit.id;
    }
    return maxId;
  }

  bool tracker_hit_collection::add_hit(const tracker_hit & hit_)
  {
    DT_THROW_IF(has_hit_with_id(hit_.id), std::logic_error, "Hit with ID=" << hit_.id << " already exists!");
    iterator found = std::find_if(this->begin(), this->end(), [&](const tracker_hit & hit2_) { return hit2_.get_cell_id() == hit_.get_cell_id(); });
    bool newHit = true;
    bool success = true;
    if (found != this->end()) {
      tracker_hit & foundHit = *found;
      if (foundHit.drift_radius > hit_.drift_radius) {
        // override the previous hit at this cell
        // std::cerr << "[devel] tracker_hit_collection::add_hit: Overriding an existing hit!\n";
        foundHit = hit_;
      } else {
        success = false;
      }
      newHit = false;
    }
    if (newHit) {
      // std::cerr << "[devel] tracker_hit_collection::add_hit: New hit #" << hit_.id << "!\n";
      this->push_back(hit_);
    }
    return success;
  }

  void tracker_hit_collection::print(std::ostream & out_) const
  {
    out_ << "Tracker hit collection: " << size() << " hit(s)\n";
    for (int i  = 0; i < (int) size(); i++) {
      out_ << " == Hit[" << i << "] with ID=" << at(i).id << " @ cell=" << at(i).get_cell_id() << '\n';
    }
    return;
  }
  
  tracker_hit_drawer::tracker_hit_drawer(const tracker_hit & hit_,
                                         const tracker & trk_)
    : _hit_(hit_)
    , _trk_(trk_)
  {
  }
  
  void tracker_hit_drawer::draw(std::ostream & out_, uint32_t /* flags_ */)
  {
    cell_id cid(_hit_.side_id, _hit_.layer_id, _hit_.row_id);
    double z = _hit_.z;
    double z_err = _hit_.z_err;
    point2 cellPos = _trk_.cell_position(cid);
    point3 hitPos(cellPos.x(), cellPos.y(), z);
    out_ << "#@tracker-hit-" << _hit_.id << '\n';
    circle hitDriftRing1(cellPos, _hit_.drift_radius - _hit_.drift_radius_err);
    circle hitDriftRing2(cellPos, _hit_.drift_radius + _hit_.drift_radius_err);
    polyline2 hitDriftRing1Pl;
    polyline2 hitDriftRing2Pl;
    hitDriftRing1.generate_samples(0.0, hitDriftRing1.r() * M_PI * 2, hitDriftRing1Pl);
    hitDriftRing2.generate_samples(0.0, hitDriftRing2.r() * M_PI * 2, hitDriftRing2Pl);
    polyline3 hitDriftRing1Pl3;
    polyline3 hitDriftRing2Pl3;
    hitDriftRing1Pl3.from_2d(hitDriftRing1Pl, z);           
    hitDriftRing2Pl3.from_2d(hitDriftRing2Pl, z);           
    draw_polyline(out_, hitDriftRing1Pl3);
    draw_polyline(out_, hitDriftRing2Pl3);
    point3 botHitPos(cellPos.x(), cellPos.y(), z - z_err);
    point3 topHitPos(cellPos.x(), cellPos.y(), z + z_err);
    draw_point(out_, botHitPos);
    draw_point(out_, topHitPos);
    out_ << std::endl;
    return;
  }

} // end of namespace lttc
