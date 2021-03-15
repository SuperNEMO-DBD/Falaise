//! \file falaise/snemo/processing/black_hole_module.h
#ifndef FALAISE_SNEMO_PROCESSING_BLACK_HOLE_MODULE_H
#define FALAISE_SNEMO_PROCESSING_BLACK_HOLE_MODULE_H

#include "falaise/property_set.h"
#include "falaise/snemo/processing/module.h"

namespace falaise {
namespace processing {

//! \brief A DPP module to delete events from a pipeline
/*!
 * Filtering events using Falaise's @ref falaise::processing::filter module creates
 * branches in the processing pipeline, the choice of which branch to take being determined
 * by the filter. In some use cases, only one branch may be relevant to further processing,
 * the other simply representing a "dead end" (e.g. "fails cut"). `black_hole_module` implements
 * this use case, discarding any events it is passed from further processing and output to file.
 * For example, in the pipeline script:
 *
 * ```ini
 * [name="pipeline", type="MyFilter"]
 * on_accept : string = "FurtherProcessing"
 * on_reject : string = "BlackHole"
 * ... filter config ...
 *
 * [name="FurtherProcessing", type="FurtherProcessing"]
 * ...
 *
 * [name="BlackHole", type="falaise::processing::black_hole_module"]
 * ...
 * ```
 *
 * events accepted by the `MyFilter` filter would flow to the `FurtherProcessing` module and
 * eventually to the main output file. Rejected events flow to the `black_hole_module` and
 * are removed from the pipeline and hence output to file.
 *
 * one path of which events will flow
 * \sa falaise::processing::filter
 * \sa falaise::processing::module
 */

class black_hole_module {
 public:
  /// Default constructor
  black_hole_module() = default;

  //! Constructor from parameters interface required by falaise::processing::module
  black_hole_module(falaise::property_set const&, datatools::service_manager&){};

  //! Process the input data
  /*!
   *  \return falaise::processing::status::PROCESS_STOP
   */
  status process(datatools::things&) { return status::PROCESS_STOP; }
};

}  // namespace processing
}  // namespace falaise

#endif  // FALAISE_SNEMO_PROCESSING_BLACK_HOLE_MODULE_H