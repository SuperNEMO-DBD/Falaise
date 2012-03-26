/* lt_utils.h
 * Author (s) : François Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2011-06-27
 * Last modified: 2011-06-27
 * 
 * License: 
 * 
 * Description: 
 * 
 * History: 
 * 
 */

#ifndef __CATAlgorithm__model__lt_utils_h
#define __CATAlgorithm__model__lt_utils_h 1

#include <vector>

#include <CATAlgorithm/handle.h>
#include <CATAlgorithm/cell.h>
#include <CATAlgorithm/cluster.h>
#include <CATAlgorithm/sequence.h>

namespace CAT {
  //class point;
  //class track;
  //class scenario;

  //typedef ::snemo::core::model::calibrated_data::tracker_hit_handle_type hit_handle_type;

  //typedef handle<scenario> scenario_handle_type;  
  //typedef std::vector<scenario_handle_type>  scenario_handle_collection_type;  

  //typedef std::vector<point> point_collection_type;      

  typedef std::vector<topology::cell> cell_collection_type;
  typedef handle<topology::cell> cell_handle_type;  
  typedef std::vector<cell_handle_type>  cell_handle_collection_type;  

  typedef std::vector<topology::cluster> cluster_collection_type;
  typedef handle<topology::cluster> cluster_handle_type;  
  typedef std::vector<cluster_handle_type>  cluster_handle_collection_type;  

  typedef std::vector<topology::sequence> sequence_collection_type;
  typedef handle<topology::sequence> sequence_handle_type;  
  typedef std::vector<sequence_handle_type>  sequence_handle_collection_type;  

  //typedef datatools::utils::handle<track> track_handle_type;  
  //typedef std::vector<track_handle_type>  track_handle_collection_type;  

}
#endif // __snlocal_tracking__model__lt_utils_h

// end of lt_scenario.h
/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/

