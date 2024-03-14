// -*- mode: c++ ; -*-
/// \file  falaise/snemo/datamodels/udd_utils.h
/* Author(s) :    François Mauger <mauger@lpccaen.in2p3.fr>
 *                Guillaume Oliviero <oliviero@cenbg.in2p3.fr>
 *                Emmanuel Chauveau <chauveau@cenbg.in2p3.fr>
 * Creation date: 2022-06-07
 * Last modified: 2022-08-10
 *
 * Description:
 *
 *   SuperNEMO Falaise Unified Digitized Data utils
 *
 */

#ifndef FALAISE_SNEMO_DATAMODELS_UDD_UTILS_H
#define FALAISE_SNEMO_DATAMODELS_UDD_UTILS_H 1

namespace snemo {

  namespace datamodel {

    static const int32_t INVALID_RUN_ID     = -1; /// Value for an invalid run ID
    static const int32_t INVALID_TRIGGER_ID = -1; /// Value for an invalid trigger ID
    static const int32_t MAX_TRIGGER_ID     = 16777215; /// Value for the maximum value of the trigger ID
    static const int32_t INVALID_EVENT_ID   = -1; /// Value for an invalid event ID
    static const int32_t INVALID_HIT_ID     = -1; /// Value for an invalid run ID
    static const int64_t INVALID_TIME_TICKS = std::numeric_limits<int64_t>::min(); /// Value for an invalid time tick or timestamp

    /// \brief Comparator of trigger IDs
    ///
    /// The comparison takes into account the range of valid trigger IDs (24-bit integer)
    /// and possible overflow values above max trigger ID (0xC00000).
    struct trigger_id_comparator_type
    {
      static const int32_t LOWER_SAFE_TID =  4194304;
      static const int32_t UPPER_SAFE_TID = 12582912;

      bool operator() (const int32_t lhs_,
                       const int32_t rhs_) const
      {

        if ((lhs_ > UPPER_SAFE_TID) && (rhs_ < LOWER_SAFE_TID)) {
          // 0----------------max 0----------------max
          // [   ][   ][   ][LHS] [RHS][   ][   ][   ]
          // <low            >up  <low            >up
          return true;
        }
        if ((lhs_ < LOWER_SAFE_TID) && (rhs_ > UPPER_SAFE_TID)) {
          // 0----------------max 0----------------max
          // [   ][   ][   ][RHS] [LHS][   ][   ][   ]
          // <low            >up  <low            >up
          return false;
        }
        return (lhs_ < rhs_);
      }
      
    };

  } // namespace datamodel

} // namespace snemo

#endif // FALAISE_SNEMO_DATAMODELS_UDD_UTILS_H
