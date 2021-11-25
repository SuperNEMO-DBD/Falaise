/// \file falaise/snemo/reconstruction/sngeometry.cc

// Ourselves:
#include <lttc/sngeometry.hh>

// Third party:
// - Bayeux/datatools :
#include <datatools/exception.h>

namespace lttc {

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
 
  tracker::tracker()
  {
    xmin = 0.0;
    xmax = source_xskip + nlayers * 2 * rcell + calo_xskip;
    double dy =  2 * xcalo_yskip + nrows * 2 * rcell;
    ymin = -dy / 2;
    ymax = +dy / 2;   
    return;
  }
  
  tracker::tracker(const tracker_conditions & trackconds_)
    : tracker()
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
 
  bool tracker::contains(const point & p_) const
  {
    if (p_.x() <= -xmax) return false;
    if (p_.x() >= xmax) return false;
    if (p_.y() <= ymin) return false;
    if (p_.y() >= ymax) return false;
    return true;
  }

  bool tracker::locate(const point & p_, cell_id & id_) const
  {
    int s, l, r;
    bool success = locate(p_, s, l, r);
    id_ = cell_id(s, l, r);
    return success;
  }
 
  bool tracker::locate(const point & p_, int & side_, int & layer_, int & row_) const
  {
    return locate(p_.x(), p_.y(), side_, layer_, row_);
  }

  point tracker::cell_position(int side_, int layer_, int row_) const
  {
    double x, y;
    x = xmin + source_xskip + rcell * ( 1 + 2 * layer_);
    if (side_ == 0) {
      x *= -1;
    }
    y = ymin + xcalo_yskip  + rcell * ( 1 + 2 * row_);
    return point(x, y);
  }

  point tracker::cell_position(const cell_id & id_) const
  {
    return cell_position(id_.side(), id_.layer(), id_.row());
  }
 
  bool tracker::locate(double x_, double y_,
                       int & side_, int & layer_, int & row_) const
  {
    side_ = -1;
    layer_ = -1;
    row_ = -1;
    if (x_ < 0.0) side_ = 0;
    else side_ = 1;
    double absx = std::abs(x_);
    if (absx < xmin + source_xskip) return false;
    if (absx > xmax - calo_xskip) return false;
    if (y_ < ymin + xcalo_yskip) return false;
    if (y_ > ymax - xcalo_yskip) return false;
    layer_ = (int) ((absx - xmin - source_xskip) / (2 * rcell));
    row_   = (int) ((y_ - ymin - xcalo_yskip)  / (2 * rcell));
    return true;
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
      point cellPos = _trk_.cell_position(cid);
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
    
    out_ << "#@what=cells-horizontal\n";
    for (size_t iside = 0; iside <= _trk_.nsides; iside++) {
      int xsgn = 1;
      if (iside == 0) xsgn = -1;
      for (size_t irow = 0; irow <= _trk_.nrows; irow++) {
        double y1 = _trk_.ymin + _trk_.xcalo_yskip + irow * 2 * _trk_.rcell;
        out_ << xsgn * (_trk_.xmin + _trk_.source_xskip) << ' ' << y1 << '\n';
        out_ << xsgn * (_trk_.xmax - _trk_.calo_xskip) << ' ' << y1 << '\n';
        out_ << '\n';
      }
    
      out_ << "#@what=cells-vertical\n";
      for (size_t ilayer = 0; ilayer <= _trk_.nlayers; ilayer++) {
        double x1 = xsgn * (_trk_.xmin + _trk_.source_xskip + ilayer * 2 * _trk_.rcell);
        out_ << x1  << ' ' << _trk_.ymin + _trk_.xcalo_yskip << '\n';
        out_ << x1 << ' ' << _trk_.ymax - _trk_.xcalo_yskip << '\n';
        out_ << '\n';
      }
     }
    out_ << '\n';
    return;
  }

} // end of namespace lttc
