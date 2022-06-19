// \file missing_hits_finder.cc

// Ourselves:
#include <lttc/missing_hits_finder.hh>
#include <lttc/lttc_algo.hh>

namespace lttc {

  using falaise::geometry::point2;
  using falaise::geometry::line2;

  missing_hits_finder::missing_hits_finder(const lttc_algo & lttc_,
                                           const tracker_hit_collection & hits_,
                                           const hit_cells_map & hit_cells_,
                                           const std::set<int> & owned_hits_)
    : _lttc_(lttc_)
    , _hits_(hits_)
    , _hit_cells_(hit_cells_)
    , _owned_hits_(owned_hits_)
  {
  }

  bool missing_hits_finder::find(const point2 & first_,
                                 const point2 & last_,
                                 std::vector<cluster_missing_hit_data> & missing_hits_) const
  {
    DT_LOG_DEBUG(logging, "Entering...");
    missing_hits_.clear();
    std::set<cell_id> crossedCells;
    if (not _lttc_.get_sntracker().intersect_segment(first_, last_, crossedCells)) {
      return false;
    }
    double length = (last_ - first_).mag();
    DT_LOG_DEBUG(logging, "#crossed cells = " << crossedCells.size());
    line2 line = line2::make_from_start_stop(first_, last_);
    for (const cell_id & cid : crossedCells) {
      DT_LOG_DEBUG(logging, "  checking cell " << cid << "...");
      point2 cellCenter = _lttc_.get_sntracker().cell_position(cid);
      point2 nearestPointOnSegment = line.orthogonal_projection(cellCenter);
      double lengthToNearest = (nearestPointOnSegment - first_).mag();
      if (lengthToNearest > length) {
        continue;
      }
      double expectedDrift = (nearestPointOnSegment - cellCenter).mag();
      if (expectedDrift > _lttc_.get_sntracker().rcell) {
        DT_LOG_DEBUG(logging, "     -> no hit is expected in this cell");
        continue;
      }
      // This candidate cell should contains a hit:
      bool deadCell = false;
      if (! _lttc_.validate_cell(cid)) {
        // The cell is dead/off and is missing because of that !
        deadCell = true;
      }
      if (deadCell) {
        cluster_missing_hit_data cmh;
        cmh.cid = cid;
        cmh.why = MCR_DEAD_CELL;
        missing_hits_.push_back(cmh);
        continue;
      }
      // Now we search if there is effectively a tracker hit in this cell:
      if (_hit_cells_.count(cid) > 0) {
        // We find the hit associated to this cell :
        int iHitWithThisCell = _hit_cells_.find(cid)->second;
        const tracker_hit & hitWithThisCell = _hits_[iHitWithThisCell];
        double hitWithThisCellDriftRadius = hitWithThisCell.drift_radius;
        double hitWithThisCellDriftRadiusErr = hitWithThisCell.drift_radius_err;
        if (_owned_hits_.count(iHitWithThisCell) == 0) {
          // but it is not part of the owned hits :
          cluster_missing_hit_data cmh;
          cmh.cid = cid;
          if (expectedDrift > (hitWithThisCellDriftRadius - 2.0 * hitWithThisCellDriftRadiusErr)) {
            // The missing hit is due to another hit with a shorter drift radius:
            cmh.why = MCR_OTHER_TRACK;
          } else {
            // The missing hit is due to the lack of a Geiger avalanche (no trigger):
            cmh.why = MCR_NO_TRIGGER;
          }
          missing_hits_.push_back(cmh);        
        } else {
          // the cell is hit by one of the owned hits : so it is not missing !
        }
      } else {
        // No hit has been found in this cell: it is missing
        cluster_missing_hit_data cmh;
        cmh.cid = cid;
        cmh.why = MCR_NO_TRIGGER;
        missing_hits_.push_back(cmh);          
      }
    } // end of cell ID loop
    DT_LOG_DEBUG(logging, "Exiting.");
    return missing_hits_.size() > 0;
  }
    
} // namespace lttc 

