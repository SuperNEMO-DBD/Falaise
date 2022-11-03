/// \file falaise/snemo/reconstruction/sngeometry.cc

// Ourselves:
#include <lttc/sngeometry.hh>

// Third party:
// - Bayeux/datatools :
#include <datatools/exception.h>
#include <datatools/logger.h>

// This project:
#include <falaise/geometry/line2.hh>

namespace lttc {

  using falaise::geometry::point2;
  using falaise::geometry::vector2;
  using falaise::geometry::rectangle;
  using falaise::geometry::line2;
    
  cell_id::cell_id(int side_, int layer_, int row_)
    : _side_(side_)
    , _layer_(layer_)
    , _row_(row_)
  {
    return;
  }

  int cell_id::side() const
  {
    return _side_;
  }

  int cell_id::layer() const
  {
    return _layer_;
  }

  int cell_id::row() const
  {
    return _row_;
  }
  
  bool cell_id::is_valid() const
  {
    if (_side_ < 0 or _side_ > 1) return false;
    if (_layer_ < 0 or _layer_ > 8) return false;
    if (_row_ < 0 or _row_ > 112) return false;
    return true;
  }

  int cell_id::distance(const cell_id & other_) const
  {
    if (!is_valid()) return -1;
    if (!other_.is_valid()) return -1;
    if (_side_ != other_._side_) return -1;
    return std::abs(_layer_ - other_._layer_) + std::abs(_row_ - other_._row_);
  }
  
  bool cell_id::operator<(const cell_id & id_) const
  {
    if (_side_ < id_._side_) return true;
    if (_side_ > id_._side_) return false;
    if (_layer_ < id_._layer_) return true;
    if (_layer_ > id_._layer_) return false;
    if (_row_ < id_._row_) return true;
    return false;
  }
 
  bool cell_id::operator==(const cell_id & id_) const
  {
    if (_side_ == -1) return false;
    if (_layer_ == -1) return false;
    if (_row_ == -1) return false;
    if (id_._side_ == -1) return false;
    if (id_._layer_ == -1) return false;
    if (id_._row_ == -1) return false;
    return (_side_ == id_._side_) && (_layer_ == id_._layer_) && (_row_ == id_._row_);
  }

  std::ostream & operator<<(std::ostream & out_, const cell_id & cid_)
  {
    out_ << '[' << cid_._side_ << '.' << cid_._layer_ << '.' << cid_._row_ << ']';
    return out_;
  }

  bool tracker_conditions::has_dead_cell(const cell_id & id_) const
  {
    return _dead_cells_.count(id_);
  }

  void tracker_conditions::add_dead_cell(const cell_id & id_)
  {
    _dead_cells_.insert(id_);
    return;
  }
 
  void tracker_conditions::remove_dead_cell(const cell_id & id_)
  {
    _dead_cells_.erase(id_);
    return;
  }
 
  const std::set<cell_id> & tracker_conditions::get_dead_cells() const
  {
    return _dead_cells_;
  }
 
  void tracker_conditions::clear_dead_cells()
  {
    _dead_cells_.clear();
    return;
  }
 
  tracker::tracker(const snemo::geometry::gg_locator & locator_)
  {
    _locator_ = &locator_;
    nsides = _locator_->numberOfSides();
    nlayers = _locator_->numberOfLayers(0);
    nrows = _locator_->numberOfRows(0);
    rcell = _locator_->cellRadius();
    dcell = _locator_->cellDiameter();
    hcell = _locator_->cellLength();
    xmin = _locator_->getXCoordOfLayer(1, 0) - rcell;
    xmax = _locator_->getXCoordOfLayer(1, nlayers - 1) + rcell;
    ymax = _locator_->getYCoordOfRow(1, nrows - 1) + rcell;
    ymin = -ymax;
    //std::cerr << "lttc::tracker::ctor: building rects...\n";
    halfCells[0] = rectangle(point2(-xmax, ymin), point2(-xmin, ymax));
    //std::cerr << "lttc::tracker::ctor: rect 0 OK \n";
    halfCells[1] = rectangle(point2(xmin, ymin), point2(xmax, ymax));
    // std::cerr << "lttc::tracker::ctor: rect 1 OK\n";
    return;
  }

  tracker::tracker(const snemo::geometry::gg_locator & locator_,
                   const tracker_conditions & trackconds_)
    : tracker(locator_)
  {
    set_tracker_conditions(trackconds_);
  }
 
  bool tracker::has_tracker_conditions() const
  {
    return _trackconds_ != nullptr;
  }
  
  void tracker::set_tracker_conditions(const tracker_conditions & trackconds_)
  {
    _trackconds_ = &trackconds_;
  }
  
  const tracker_conditions & tracker::get_tracker_conditions() const
  {
    DT_THROW_IF(_trackconds_ == nullptr, std::logic_error, "No tracker conditions!");
    return *_trackconds_;
  }
 
  bool tracker::contains(const point2 & p_) const
  {
    if (p_.x() <= -xmax) return false;
    if (p_.x() >= xmax) return false;
    if (p_.y() <= ymin) return false;
    if (p_.y() >= ymax) return false;
    return true;
  }

  bool tracker::locate(const point2 & p_, cell_id & id_) const
  {
    int s, l, r;
    bool success = locate(p_, s, l, r);
    id_ = cell_id(s, l, r);
    return success;
  }
 
  bool tracker::locate(const point2 & p_, int & side_, int & layer_, int & row_) const
  {
    return locate(p_.x(), p_.y(), side_, layer_, row_);
  }

  rectangle tracker::cell_rectangle(const cell_id & id_) const
  {
    point2 cellCenter = cell_position(id_);
    return rectangle(point2(cellCenter.x() - rcell, cellCenter.y() - rcell),
                     point2(cellCenter.x() + rcell, cellCenter.y() + rcell));                
  }

  bool tracker::cell_contains(const cell_id & cid_, const point2 & p_, double tolerance_) const
  {
    rectangle cellRect = cell_rectangle(cid_);
    return cellRect.contains(p_, tolerance_);
  }

  point2 tracker::cell_position(int side_, int layer_, int row_) const
  {
    auto pos = _locator_->getCellPosition(side_, layer_, row_);
    return point2(pos.x(), pos.y());
  }

  point2 tracker::cell_position(const cell_id & id_) const
  {
    return cell_position(id_.side(), id_.layer(), id_.row());
  }
 
  bool tracker::locate(double x_, double y_,
                       int & side_, int & layer_, int & row_) const
  {
    geomtools::vector_3d pos(x_, y_, 0.0);
    geomtools::geom_id gid;
    double tolerance = 0.1 * CLHEP::mm;
    if (_locator_->findCellGID(pos, gid, tolerance)) { 
      side_  = _locator_->getSideAddress(gid);
      layer_ = _locator_->getLayerAddress(gid);
      row_   = _locator_->getRowAddress(gid);
      return true;
    }
    return false;
  }

  tracker_conditions_drawer::tracker_conditions_drawer(const tracker_conditions & trkcond_,
                                                       const tracker & trk_)
    : _trkcond_(trkcond_)
    , _trk_(trk_)
  {
  }
      
  void tracker_conditions_drawer::draw(std::ostream & out_, uint32_t /* flags_ */)
  {
    int counter = 0;
    for (const auto & cid : _trkcond_.get_dead_cells()) {
      point2 cellPos = _trk_.cell_position(cid);
      out_ << "#@tracker-conditions=dead-cell-" << counter << '\n';
      out_ << cellPos.x() - _trk_.rcell << ' ' <<  cellPos.y() - _trk_.rcell << '\n';
      out_ << cellPos.x() - _trk_.rcell << ' ' <<  cellPos.y() + _trk_.rcell << '\n';
      out_ << cellPos.x() + _trk_.rcell << ' ' <<  cellPos.y() + _trk_.rcell << '\n';
      out_ << cellPos.x() + _trk_.rcell << ' ' <<  cellPos.y() - _trk_.rcell << '\n';
      out_ << cellPos.x() - _trk_.rcell << ' ' <<  cellPos.y() - _trk_.rcell << '\n';
      out_ << '\n';
      out_ << cellPos.x() - _trk_.rcell << ' ' <<  cellPos.y() - _trk_.rcell << '\n';
      out_ << cellPos.x() + _trk_.rcell << ' ' <<  cellPos.y() + _trk_.rcell << '\n';
      out_ << '\n';
      out_ << cellPos.x() - _trk_.rcell << ' ' <<  cellPos.y() + _trk_.rcell << '\n';
      out_ << cellPos.x() + _trk_.rcell << ' ' <<  cellPos.y() - _trk_.rcell << '\n';
      out_ << '\n';
      counter++;
    }
    return;
  }

  tracker_drawer::tracker_drawer(const tracker & trk_)
    : _trk_(trk_)
  {
  }
      
  void tracker_drawer::draw(std::ostream & out_, uint32_t /* flags_ */)
  {
    out_ << "#@what=tracker-frame-side0\n";
    out_ << -_trk_.xmin << ' ' << _trk_.ymin << '\n';
    out_ << -_trk_.xmin << ' ' << _trk_.ymax << '\n';
    out_ << -_trk_.xmax << ' ' << _trk_.ymax << '\n';
    out_ << -_trk_.xmax << ' ' << _trk_.ymin << '\n';
    out_ << -_trk_.xmin << ' ' << _trk_.ymin << '\n';
    out_ << '\n';
    
    out_ << "#@what=tracker-frame-side1\n";
    out_ << _trk_.xmin << ' ' << _trk_.ymin << '\n';
    out_ << _trk_.xmin << ' ' << _trk_.ymax << '\n';
    out_ << _trk_.xmax << ' ' << _trk_.ymax << '\n';
    out_ << _trk_.xmax << ' ' << _trk_.ymin << '\n';
    out_ << _trk_.xmin << ' ' << _trk_.ymin << '\n';
    out_ << '\n';

    out_ << "#@what=cells\n";
    for (size_t iside = 0; iside < _trk_.nsides; iside++) {
      for (size_t irow = 0; irow < _trk_.nrows; irow++) {
        for (size_t ilayer = 0; ilayer < _trk_.nlayers; ilayer++) {
          cell_id cid(iside, ilayer, irow);
          point2 cellPos = _trk_.cell_position(cid);
          out_ << "#@tracker-cells=" << cid << '\n';
          out_ << cellPos.x() - _trk_.rcell << ' ' <<  cellPos.y() - _trk_.rcell << '\n';
          out_ << cellPos.x() - _trk_.rcell << ' ' <<  cellPos.y() + _trk_.rcell << '\n';
          out_ << cellPos.x() + _trk_.rcell << ' ' <<  cellPos.y() + _trk_.rcell << '\n';
          out_ << cellPos.x() + _trk_.rcell << ' ' <<  cellPos.y() - _trk_.rcell << '\n';
          out_ << cellPos.x() - _trk_.rcell << ' ' <<  cellPos.y() - _trk_.rcell << '\n';
          out_ << '\n';
          out_ << cellPos.x() << ' ' <<  cellPos.y() - 0.1 * _trk_.rcell << '\n';
          out_ << cellPos.x() << ' ' <<  cellPos.y() + 0.1 * _trk_.rcell << '\n';
          out_ << '\n';
          out_ << cellPos.x() - 0.1 * _trk_.rcell << ' ' <<  cellPos.y() << '\n';
          out_ << cellPos.x() + 0.1 * _trk_.rcell << ' ' <<  cellPos.y() << '\n';
          out_ << '\n';
        }
      }
    }
    /*
    out_ << "#@what=cells-horizontal\n";
    for (size_t iside = 0; iside <= _trk_.nsides; iside++) {
      int xsgn = 1;
      if (iside == 0) xsgn = -1;
      for (size_t irow = 0; irow <= _trk_.nrows; irow++) {
        double y1 = _trk_.ymin + _trk_.xcalo_yskip + irow * 2 * _trk_.rcell;
        double x1 = _trk_.xmin + _trk_.source_xskip;
        double x2 = _trk_.xmax - _trk_.calo_xskip;
        out_ << xsgn * x1 << ' ' << y1 << '\n';
        if (irow==0) {
          out_ << xsgn * (0.5*(x1+x2)) << ' ' << y1 << '\n';
        }
        out_ << xsgn * x2 << ' ' << y1 << '\n';
        out_ << '\n';
      }
    
      out_ << "#@what=cells-vertical\n";
      for (size_t ilayer = 0; ilayer <= _trk_.nlayers; ilayer++) {
        double x1 = xsgn * (_trk_.xmin + _trk_.source_xskip + ilayer * 2 * _trk_.rcell);
        double y1 = _trk_.ymin + _trk_.xcalo_yskip;
        double y2 = _trk_.ymax - _trk_.xcalo_yskip;
        out_ << x1  << ' ' << y1 << '\n';
        if (ilayer==0) {
          out_ << x1  << ' ' << (0.5*(y1+y2)) << '\n';
        }
        out_ << x1 << ' ' << y2 << '\n';
        out_ << '\n';
      }
    }
    */
    // out_ << '\n';
    return;
  }

  bool tracker::intersect_segment(const point2 & p1_,
                                  const point2 & p2_,
                                  std::set<cell_id> & cids_) const
  {
    datatools::logger::priority logging = datatools::logger::PRIO_FATAL;
    // logging = datatools::logger::PRIO_DEBUG;
    DT_LOG_DEBUG(logging, "Entering...");
    cids_.clear();
    double tolerance = 0.1 * CLHEP::mm;
    line2 line = line2::make_from_start_stop(p1_, p2_);
    DT_LOG_DEBUG(logging, "Line to be intersected :");
    if (datatools::logger::is_debug(logging)) {
      line.print(std::cerr, "[debug] ");
    }
    // Search in both sides of the source plane:
    for (int iSide = 0; iSide < 2; iSide++) {
      DT_LOG_DEBUG(logging, "Side " << iSide);
      point2 first = p1_;
      point2 last = p2_;
      if (! this->halfCells[iSide].contains(first, tolerance)) {
        DT_LOG_DEBUG(logging, "First is not in this half!");
        vector2 dir = p2_ - p1_;
        point2 best;
        geomtools::invalidate(best);
        double bestDist;
        datatools::plus_infinity(bestDist);
        point2 root;
        if (this->halfCells[iSide].unique_intersection(first, dir, root, tolerance)) {
          double dist = (root - first).mag();
          if (dist < bestDist) {
            bestDist = dist;
            best = root;
          }
        }
        if (not geomtools::is_valid(best)) {
          continue;
        }
        first = best;
      }
      if (! this->halfCells[iSide].contains(last, tolerance)) {
        DT_LOG_DEBUG(logging, "Last is not in this half!");
        vector2 dir = p1_ - p2_;
        point2 best;
        geomtools::invalidate(best);
        double bestDist;
        datatools::plus_infinity(bestDist);
        point2 root;
        if (this->halfCells[iSide].unique_intersection(last, dir, root, tolerance)) {
          double dist = (root - last).mag();
          if (dist < bestDist) {
            bestDist = dist;
            best = root;
          }
        }
        if (not geomtools::is_valid(best)) {
          continue;
        }
        last = best;
      }
      cell_id firstCid;
      cell_id lastCid;
      this->locate(first, firstCid);
      this->locate(last, lastCid);
      int minLayer = std::min(firstCid.layer(), lastCid.layer());
      int maxLayer = std::max(firstCid.layer(), lastCid.layer());
      int minRow = std::min(firstCid.row(), lastCid.row());
      int maxRow = std::max(firstCid.row(), lastCid.row());
      if (datatools::logger::is_debug(logging)) {
        std::cerr << "[debug] ltts::tracker::intersect_segment: first =" << first << '\n';
        std::cerr << "[debug] ltts::tracker::intersect_segment: last  =" << last << '\n';
        std::cerr << "[debug] ltts::tracker::intersect_segment: firstCid =" << firstCid << '\n';
        std::cerr << "[debug] ltts::tracker::intersect_segment: lastCid  =" << lastCid << '\n';
        std::cerr << "[debug] ltts::tracker::intersect_segment: iSide=" << iSide << '\n';
        std::cerr << "[debug] ltts::tracker::intersect_segment:   minLayer=" << minLayer << '\n';
        std::cerr << "[debug] ltts::tracker::intersect_segment:   maxLayer=" << maxLayer << '\n';
        std::cerr << "[debug] ltts::tracker::intersect_segment:   minRow=" << minRow << '\n';
        std::cerr << "[debug] ltts::tracker::intersect_segment:   maxRow=" << maxRow << '\n';
      }
      // Check for cells in a fiducial rectangle between the ending hits
      for (int iLayer = minLayer; iLayer <= maxLayer; iLayer++) {
        for (int iRow = minRow; iRow <= maxRow; iRow++) {
          cell_id cid(iSide, iLayer, iRow);
          point2 cellCenter = this->cell_position(cid);
          point2 nearestPointOnLine = line.orthogonal_projection(cellCenter);
          if (this->cell_contains(cid, nearestPointOnLine, tolerance)) {
            cids_.insert(cid);
          }
        }
      }
    }
    DT_LOG_DEBUG(logging, "Exiting.");
    return cids_.size() > 0;
  }

} // end of namespace lttc
