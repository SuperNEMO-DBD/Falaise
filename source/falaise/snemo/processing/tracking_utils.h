/// \file falaise/snemo/processing/tracking_utils.h
/* Author(s)     : Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date : 2024-03-15
 * Last modified : 2024-03-15
 */
 
#ifndef FALAISE_SNEMO_PROCESSING_TRACKING_UTILS_H
#define FALAISE_SNEMO_PROCESSING_TRACKING_UTILS_H 1

// Standard library:
#include <iostream>

// Falaise:
#include "falaise/snemo/datamodels/calibrated_tracker_hit.h"

namespace snemo {

  namespace processing {

    /// Tracking precluster of calibrated Geiger hits after space/time partitionning
    ///
    /// A tracking precluster can be used as a region of interest where to search for
    /// charged particle track clusters. Particularly, a precluster may contains several
    /// tracks. It is the task of a tracking algorithm/driver to extract track clusters
    /// from a precluster.
    ///
    /// - a precluster is located on one side of the SuperNEMO tracking chamber.
    /// - a precluster is tagged as a 'prompt' one ot a 'delayed' one, depending on the time coincidence
    ///   window used by the preclusterizer algorithm.
    /// - a precluster contains a collection of Geiger hits.
    ///
    /// In principle, after the preclusterizer algorithm has made is job, one should obtain no
    /// more than two 'prompt' preclusters (one per side) and up to N 'delayed' clusters, depending
    /// on the ability of the preclusterizer to disantangle several delayed time windows.
    ///
    struct tracking_precluster
    {
      tracking_precluster(const int side_, const bool delayed_);
      tracking_precluster(const int side_, const bool delayed_,
                          const snemo::datamodel::TrackerHitHdlCollection & hits_);
      int side() const;
      bool is_delayed() const;
      const snemo::datamodel::TrackerHitHdlCollection & hits() const;  
      snemo::datamodel::TrackerHitHdlCollection & hits();
      void print(std::ostream & out_,
                 const std::string & indent_,
                 const uint32_t flags_ = 0) const;
    private:
      
      int _side_ = -1;
      bool _delayed_ = false;
      snemo::datamodel::TrackerHitHdlCollection _hits_;
      
    };

    typedef std::vector<tracking_precluster> tracking_precluster_collection_type;
    
  } // end of namespace processing

} // end of namespace snemo

#endif // FALAISE_SNEMO_PROCESSING_TRACKING_UTILS_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
