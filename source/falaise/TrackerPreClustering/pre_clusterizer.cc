/// \file falaise/TrackerPreClustering/pre_clusterizer.cc

// Ourselves:
#include <TrackerPreClustering/pre_clusterizer.h>

// Standard library:
#include <limits>
#include <sstream>
#include <stdexcept>
#include <cstdlib>

// Third party:
// - CLHEP:
#include <CLHEP/Units/SystemOfUnits.h>
// - Bayeux/datatools:
#include <datatools/exception.h>

namespace TrackerPreClustering {

  const int pre_clusterizer::OK    = EXIT_SUCCESS;
  const int pre_clusterizer::ERROR = EXIT_FAILURE;

  bool pre_clusterizer::is_locked() const
  {
    return _locked_;
  }

  datatools::logger::priority pre_clusterizer::get_logging_priority() const
  {
    return _logging_;
  }

  void pre_clusterizer::set_logging_priority(datatools::logger::priority logging_)
  {
    _logging_ = logging_;
    return;
  }

  double pre_clusterizer::get_cell_size() const
  {
    return _cell_size_;
  }

  void pre_clusterizer::set_cell_size(double cell_size_)
  {
    DT_THROW_IF(is_locked(), std::logic_error, "Pre clusterizer is locked!");
    DT_THROW_IF(cell_size_ <= 1.0 * CLHEP::millimeter, std::logic_error,
                "Invalid cell size ("
                << cell_size_ / CLHEP::mm << " mm) !");
    _cell_size_ = cell_size_;
    return;
  }

  void pre_clusterizer::set_delayed_hit_cluster_time(double dhct_)
  {
    DT_THROW_IF(is_locked(), std::logic_error, "Pre clusterizer is locked!");
    DT_THROW_IF(dhct_ <= 1.0 * CLHEP::microsecond, std::logic_error,
                "Invalid delayed hit cluster time ("
                << dhct_ / CLHEP::microsecond << " microsec) !");
    _delayed_hit_cluster_time_ = dhct_;
    return;
  }

  double pre_clusterizer::get_delayed_hit_cluster_time() const
  {
    return _delayed_hit_cluster_time_;
  }

  bool pre_clusterizer::is_processing_prompt_hits() const
  {
    return _processing_prompt_hits_;
  }

  void pre_clusterizer::set_processing_prompt_hits(bool pph_)
  {
    DT_THROW_IF(is_locked(), std::logic_error, "Pre clusterizer is locked!");
    _processing_prompt_hits_ = pph_;
    return;
  }

  bool pre_clusterizer::is_processing_delayed_hits() const
  {
    return _processing_delayed_hits_;
  }

  void pre_clusterizer::set_processing_delayed_hits(bool pdh_)
  {
    DT_THROW_IF(is_locked(), std::logic_error, "Pre clusterizer is locked!");
    _processing_delayed_hits_ = pdh_;
    return;
  }

  bool pre_clusterizer::is_split_chamber() const
  {
    return _split_chamber_;
  }

  void pre_clusterizer::set_split_chamber(bool sc_)
  {
    DT_THROW_IF(is_locked(), std::logic_error, "Pre clusterizer is locked!");
    _split_chamber_ = sc_;
    return;
  }

  void pre_clusterizer::_set_defaults()
  {
    _cell_size_                = 44.0 * CLHEP::millimeter;
    _delayed_hit_cluster_time_ = 10.0 * CLHEP::microsecond;
    _processing_prompt_hits_   = true;
    _processing_delayed_hits_  = true;
    _split_chamber_            = true;
    return;
  }

  void pre_clusterizer::reset()
  {
    _locked_ = false;
    _set_defaults();
    return;
  }

  pre_clusterizer::pre_clusterizer()
  {
    _locked_ = false;
    _logging_ = datatools::logger::PRIO_WARNING;
    _set_defaults();
    return;
  }

  pre_clusterizer::~pre_clusterizer()
  {
    return;
  }

  int pre_clusterizer::initialize(const setup_data & setup_)
  {
    DT_THROW_IF(is_locked(), std::logic_error, "Pre clusterizer is locked!");
    set_logging_priority(setup_.logging);
    set_cell_size(setup_.cell_size);
    set_delayed_hit_cluster_time(setup_.delayed_hit_cluster_time);
    set_processing_prompt_hits(setup_.processing_prompt_hits);
    set_processing_delayed_hits(setup_.processing_delayed_hits);
    set_split_chamber(setup_.split_chamber);
    _locked_ = true;
    return OK;
  }

} // end of namespace TrackerPreClustering
