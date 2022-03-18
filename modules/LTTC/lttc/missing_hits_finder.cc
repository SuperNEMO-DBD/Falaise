// \file missing_hits_finder.cc

// Ourselves:
#include <lttc/missing_hits_finder.hh>

namespace lttc {

  missing_hits_finder::missing_hits_finder(const tracker & sntracker_,
                                           const tracker_hit_collection & hits_,
                                           const hit_cells_map & hit_cells_,
                                           const std::set<int> & owned_hits_)
    : _sntracker_(sntracker_)
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
    double length = (last_ - first_).mag();
    if (_sntracker_.intersect_segment(first_, last_, crossedCells)) {
      DT_LOG_DEBUG(logging, "# crossed cells = " << crossedCells.size());
      line2 line = line2::make_from_start_stop(first_, last_);
      for (const cell_id & cid : crossedCells) {
        DT_LOG_DEBUG(logging, "  checking cell " << cid << "...");
        point2 cellCenter = _sntracker_.cell_position(cid);
        point2 nearestPointOnSegment = line.orthogonal_projection(cellCenter);
        double lengthToNearest = (nearestPointOnSegment - first_).mag();
        if (lengthToNearest > length) {
          continue;
        }
        double expectedDrift = (nearestPointOnSegment - cellCenter).mag();
        if (expectedDrift > _sntracker_.rcell) continue;
        // This is a candidate cell:
        if (_sntracker_.has_tracker_conditions()) {
          const tracker_conditions & trkConds = _sntracker_.get_tracker_conditions();
          if (trkConds.has_dead_cell(cid)) {
            // The cell is dead and missing because of that !
            cluster_missing_hit_data cmh;
            cmh.cid = cid;
            cmh.why = MCR_DEAD_CELL;
            missing_hits_.push_back(cmh);
          } else if (_hit_cells_.count(cid) > 0) {
            // The cell is hit :
            int iHitWithThisCell = _hit_cells_.find(cid)->second;
            const tracker_hit & hitWithThisCell = _hits_[iHitWithThisCell];
            double hitWithThisCellDriftRadius = hitWithThisCell.drift_radius;
            double hitWithThisCellDriftRadiusErr = hitWithThisCell.drift_radius_err;
            if (_owned_hits_.count(iHitWithThisCell) == 0) {
              // but not by the owned hits :
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
              // the cell is hit by one of the owned hits 
            }
          } else if (_hit_cells_.count(cid) == 0) {
            cluster_missing_hit_data cmh;
            cmh.cid = cid;
            cmh.why = MCR_NO_TRIGGER;
            missing_hits_.push_back(cmh);          
          }
        }
      }
    } 
    DT_LOG_DEBUG(logging, "Exiting.");
    return missing_hits_.size() > 0;
  }
    
} // namespace lttc 

