/** \file falaise/snemo/reconstruction/sultan_then_cat_driver.cc */

// Ourselves:
#include <CAT/sultan_then_cat_driver.h>

// Standard library:
#include <sstream>
#include <stdexcept>

// Third party:
// - Boost :
#include <boost/foreach.hpp>
// - Bayeux/datatools :
#include <datatools/exception.h>
#include <datatools/logger.h>
#include <datatools/properties.h>
#include <datatools/utils.h>
// - Bayeux/geomtools :
#include <geomtools/manager.h>

// This project :
#include <falaise/snemo/datamodels/tracker_clustering_data.h>
#include <falaise/snemo/geometry/calo_locator.h>
#include <falaise/snemo/geometry/gg_locator.h>
#include <falaise/snemo/geometry/gveto_locator.h>
#include <falaise/snemo/geometry/locator_plugin.h>
#include <falaise/snemo/geometry/xcalo_locator.h>

namespace snemo {

namespace reconstruction {

const std::string sultan_then_cat_driver::SULTAN_THEN_CAT_ID = "SULTAN_THEN_CAT";

void sultan_then_cat_driver::set_magfield(double value_) {
  DT_THROW_IF(is_initialized(), std::logic_error, "SULTAN driver is already initialized!");
  _magfield_ = value_;
}

double sultan_then_cat_driver::get_magfield() const { return _magfield_; }

/// Constructor
sultan_then_cat_driver::sultan_then_cat_driver()
    : ::snemo::processing::base_tracker_clusterizer(sultan_then_cat_driver::SULTAN_THEN_CAT_ID) {
  _set_defaults();
}

/// Destructor
sultan_then_cat_driver::~sultan_then_cat_driver() = default;

void sultan_then_cat_driver::_set_defaults() {
  _SULTAN_setup_.reset();
  _sigma_z_factor_ = 1.0;
  datatools::invalidate(_magfield_);
  _process_calo_hits_ = true;
  _calo_locator_ = nullptr;
  _xcalo_locator_ = nullptr;
  _gveto_locator_ = nullptr;
  this->base_tracker_clusterizer::_reset();
}

/// Reset the clusterizer
void sultan_then_cat_driver::reset() {
  _set_initialized(false);
  _SULTAN_clusterizer_.finalize();
  _SULTAN_sultan_.finalize();
  _set_defaults();
  _CAT_clusterizer_.finalize();
  _CAT_sequentiator_.finalize();
  _CAT_setup_.reset();
  _sigma_z_factor_ = 1.0;
  datatools::invalidate(_magfield_);
  this->base_tracker_clusterizer::_reset();
}

/// Initialize the clusterizer through configuration properties
void sultan_then_cat_driver::initialize(const datatools::properties& setup_) {
  DT_THROW_IF(is_initialized(), std::logic_error, "SULTAN THEN CAT driver is already initialized!");

  // Invoke initialization at parent level :
  this->snemo::processing::base_tracker_clusterizer::_initialize(setup_);

  _CAT_setup_.SuperNemo = true;
  _CAT_setup_.FoilRadius = 0.0;
  _SULTAN_setup_.SuperNemo = true;
  _SULTAN_setup_.foil_radius = 0.0;

  // Forcing magnetic field (a temporary trick)
  if (!datatools::is_valid(_magfield_)) {
    if (setup_.has_key("SULTAN.magnetic_field")) {
      const double default_magfield_unit = CLHEP::tesla;
      double magfield = setup_.fetch_real("SULTAN.magnetic_field");
      if (!setup_.has_explicit_unit("SULTAN.magnetic_field")) {
        magfield *= default_magfield_unit;
      }
      set_magfield(magfield);
    }
  }

  // Verbosity level
  if (setup_.has_key("CAT.level")) {
    _CAT_setup_.level = setup_.fetch_string("CAT.level");
  }

  // Maximum processing time in ms
  if (setup_.has_key("CAT.max_time")) {
    _CAT_setup_.MaxTime = setup_.fetch_real("CAT.max_time");
  }
  if (setup_.has_key("SULTAN.clusterizer_level")) {
    _SULTAN_setup_.clusterizer_level = setup_.fetch_string("SULTAN.clusterizer_level");
  }
  // Verbosity level
  if (setup_.has_key("SULTAN.sequentiator_level")) {
    _SULTAN_setup_.sequentiator_level = setup_.fetch_string("SULTAN.sequentiator_level");
  }

  // Max radius of cells to be not treated as points in mm
  if (setup_.has_key("CAT.small_radius")) {
    _CAT_setup_.SmallRadius = setup_.fetch_real("CAT.small_radius");
  }

  // Minimal probability away from the straight line
  if (setup_.has_key("CAT.probmin")) {
    _CAT_setup_.probmin = setup_.fetch_real("CAT.probmin");
  }

  // Number of cells which can be skipped (because the cell did not
  // work) and still the cluster is continuous
  if (setup_.has_key("CAT.nofflayers")) {
    _CAT_setup_.nofflayers = setup_.fetch_integer("CAT.nofflayers");
    if (_CAT_setup_.nofflayers < 0)
      DT_THROW_IF(true, std::logic_error,
                  "Invalid number of 'off' layers(" << _CAT_setup_.nofflayers << ") !");
  }

  // 1st event to be processed
  if (setup_.has_key("CAT.first_event")) {
    _CAT_setup_.first_event = setup_.fetch_integer("CAT.first_event");
  }

  // Ratio of 2nd best to best chi2 which is acceptable as 2nd solution
  if (setup_.has_key("CAT.ratio")) {
    _CAT_setup_.Ratio = setup_.fetch_real("CAT.ratio");
  }

  // Sigma Z factor
  if (setup_.has_key("CAT.driver.sigma_z_factor")) {
    _sigma_z_factor_ = setup_.fetch_real("CAT.driver.sigma_z_factor");
    DT_THROW_IF(_sigma_z_factor_ <= 0.0 || _sigma_z_factor_ >= 100.0, std::logic_error,
                "Invalid Sigma Z factor(" << _sigma_z_factor_ << ") !");
  }

  // Maximum processing time in ms
  if (setup_.has_key("SULTAN.max_time")) {
    _SULTAN_setup_.max_time = setup_.fetch_real("SULTAN.max_time");
  }

  // make an event display?
  if (setup_.has_key("SULTAN.print_event_display")) {
    _SULTAN_setup_.print_event_display = setup_.fetch_boolean("SULTAN.print_event_display");
  }

  // use clocks to time the software ?
  if (setup_.has_key("SULTAN.use_clocks")) {
    _SULTAN_setup_.use_clocks = setup_.fetch_boolean("SULTAN.use_clocks");
  }

  // use endpoints to clusterize ?
  if (setup_.has_key("SULTAN.use_endpoints")) {
    _SULTAN_setup_.use_endpoints = setup_.fetch_boolean("SULTAN.use_endpoints");
  }

  // use legendre transform to clusterize ?
  if (setup_.has_key("SULTAN.use_legendre")) {
    _SULTAN_setup_.use_legendre = setup_.fetch_boolean("SULTAN.use_legendre");
  }

  // clusterize endpoints with helix model ?
  if (setup_.has_key("SULTAN.clusterize_with_helix_model")) {
    _SULTAN_setup_.clusterize_with_helix_model =
        setup_.fetch_boolean("SULTAN.clusterize_with_helix_model");
  }

  // assign a helix to each cluster ?
  if (setup_.has_key("SULTAN.assign_helices_to_clusters")) {
    _SULTAN_setup_.assign_helices_to_clusters =
        setup_.fetch_boolean("SULTAN.assign_helices_to_clusters");
  }

  // Minimum energy of detected electron
  if (setup_.has_key("SULTAN.Emin")) {
    _SULTAN_setup_.Emin = setup_.fetch_real("SULTAN.Emin");
  }

  // Maximum energy of detected electron
  if (setup_.has_key("SULTAN.Emax")) {
    _SULTAN_setup_.Emax = setup_.fetch_real("SULTAN.Emax");
  }

  // Minimal probability away from the straight line
  if (setup_.has_key("SULTAN.probmin")) {
    _SULTAN_setup_.probmin = setup_.fetch_real("SULTAN.probmin");
  }

  // Number of sigmas
  if (setup_.has_key("SULTAN.nsigma_r")) {
    _SULTAN_setup_.nsigma_r = setup_.fetch_real("SULTAN.nsigma_r");
  }

  if (setup_.has_key("SULTAN.nsigma_z")) {
    _SULTAN_setup_.nsigma_z = setup_.fetch_real("SULTAN.nsigma_z");
  }

  // Number of cells which can be skipped (because the cell did not
  // work) and still the cluster is continuous
  if (setup_.has_key("SULTAN.nofflayers")) {
    _SULTAN_setup_.nofflayers = setup_.fetch_integer("SULTAN.nofflayers");
  }
  DT_THROW_IF(_SULTAN_setup_.nofflayers < 0, std::logic_error,
              "Invalid number of 'off' layers(" << _SULTAN_setup_.nofflayers << ") !");

  // 1st event to be processed
  if (setup_.has_key("SULTAN.first_event")) {
    _SULTAN_setup_.first_event = setup_.fetch_integer("SULTAN.first_event");
  }

  // Sigma Z factor
  if (setup_.has_key("SULTAN.driver.sigma_z_factor")) {
    _sigma_z_factor_ = setup_.fetch_real("SULTAN.driver.sigma_z_factor");
    DT_THROW_IF(_sigma_z_factor_ <= 0.0 || _sigma_z_factor_ >= 100.0, std::logic_error,
                "Invalid Sigma Z factor(" << _sigma_z_factor_ << ") !");
  }

  if (setup_.has_key("SULTAN.min_ncells_in_cluster")) {
    _SULTAN_setup_.min_ncells_in_cluster = setup_.fetch_integer("SULTAN.min_ncells_in_cluster");
  }

  if (setup_.has_key("SULTAN.min_layer_for_triplet")) {
    _SULTAN_setup_.min_layer_for_triplet = setup_.fetch_integer("SULTAN.min_layer_for_triplet");
  }

  if (setup_.has_key("SULTAN.ncells_between_triplet_min")) {
    _SULTAN_setup_.ncells_between_triplet_min =
        setup_.fetch_integer("SULTAN.ncells_between_triplet_min");
  }

  if (setup_.has_key("SULTAN.ncells_between_triplet_range")) {
    _SULTAN_setup_.ncells_between_triplet_range =
        setup_.fetch_integer("SULTAN.ncells_between_triplet_range");
  }

  if (setup_.has_key("SULTAN.nsigmas")) {
    _SULTAN_setup_.nsigmas = setup_.fetch_real("SULTAN.nsigmas");
  }

  // Geometry description :
  _CAT_setup_.num_blocks = 1;
  _CAT_setup_.planes_per_block.clear();
  _CAT_setup_.planes_per_block.push_back(_CAT_setup_.num_blocks);
  _CAT_setup_.planes_per_block.at(0) = get_gg_locator().numberOfLayers(0);
  _CAT_setup_.num_cells_per_plane = get_gg_locator().numberOfRows(0);
  _CAT_setup_.cell_size = get_gg_locator().cellDiameter();

  // hard-coded values of bfield and chamber size
  _CAT_setup_.bfield = 0.0025;
  _CAT_setup_.xsize = 2500. * CLHEP::mm;  // this is y in SnWare coordinates
  _CAT_setup_.ysize = 1350. * CLHEP::mm;  // this is z in SnWare coordinates
  _CAT_setup_.zsize = 450. * CLHEP::mm;   // this is x in SnWare coordinates

  // Get the calorimeter locators from a geometry plugin :
  const geomtools::manager& geo_mgr = get_geometry_manager();
  std::string locator_plugin_name;
  if (setup_.has_key("locator_plugin_name")) {
    locator_plugin_name = setup_.fetch_string("locator_plugin_name");
  }
  // If no locator plugin name is set, then search for the first one
  if (locator_plugin_name.empty()) {
    const geomtools::manager::plugins_dict_type& plugins = geo_mgr.get_plugins();
    for (const auto& plugin : plugins) {
      const std::string& plugin_name = plugin.first;
      if (geo_mgr.is_plugin_a<snemo::geometry::locator_plugin>(plugin_name)) {
        DT_LOG_DEBUG(get_logging_priority(), "Find locator plugin with name = " << plugin_name);
        locator_plugin_name = plugin_name;
        break;
      }
    }
  }
  // Access to a given plugin by name and type :
  if (geo_mgr.has_plugin(locator_plugin_name) &&
      geo_mgr.is_plugin_a<snemo::geometry::locator_plugin>(locator_plugin_name)) {
    DT_LOG_NOTICE(get_logging_priority(),
                  "Found locator plugin named '" << locator_plugin_name << "'");
    const auto& lp = geo_mgr.get_plugin<snemo::geometry::locator_plugin>(locator_plugin_name);
    // Set the calo cell locator :
    _calo_locator_ = &(lp.caloLocator());
    _xcalo_locator_ = &(lp.xcaloLocator());
    _gveto_locator_ = &(lp.gvetoLocator());
  }
  if (get_logging_priority() >= datatools::logger::PRIO_DEBUG) {
    DT_LOG_DEBUG(get_logging_priority(), "Calo locator :");
    _calo_locator_->tree_dump(std::clog, "", "[debug]: ");
    DT_LOG_DEBUG(get_logging_priority(), "X-calo locator :");
    _xcalo_locator_->tree_dump(std::clog, "", "[debug]: ");
    DT_LOG_DEBUG(get_logging_priority(), "G-veto locator :");
    _gveto_locator_->tree_dump(std::clog, "", "[debug]: ");
  }

  // Geometry description :
  _SULTAN_setup_.num_blocks = 1;
  _SULTAN_setup_.planes_per_block.clear();
  _SULTAN_setup_.planes_per_block.push_back(_SULTAN_setup_.num_blocks);
  const size_t nlayers = get_gg_locator().numberOfLayers(0);
  const size_t nrows = get_gg_locator().numberOfRows(0);
  const double cell_diameter = get_gg_locator().cellDiameter();
  _SULTAN_setup_.planes_per_block.at(0) = nlayers;
  _SULTAN_setup_.num_cells_per_plane = nrows;
  _SULTAN_setup_.cell_distance = cell_diameter;
  _SULTAN_setup_.n_layers = 9;
  _SULTAN_setup_.n_cell_columns = 56;

  // hard-coded values of bfield and chamber size
  if (!datatools::is_valid(_magfield_)) {
    set_magfield(0.0025 * CLHEP::tesla);
  }
  _SULTAN_setup_.bfield = _magfield_ / CLHEP::tesla;
  _SULTAN_setup_.xsize = 0.5 * nrows * cell_diameter;
  _SULTAN_setup_.ysize = 0.5 * nlayers * cell_diameter;
  _SULTAN_setup_.zsize = 0.5 * get_gg_locator().cellLength();

  // Check the validity of the CAT setup data :
  DT_THROW_IF(!_CAT_setup_.check(), std::logic_error,
              "Setup data for the CAT machine is not checked : " << _CAT_setup_.get_error_message()
                                                                 << " !");

  // Check the validity of the SULTAN setup data :
  DT_THROW_IF(!_SULTAN_setup_.check(), std::logic_error,
              "Setup data for the SULTAN machine is not checked : "
                  << _SULTAN_setup_.get_error_message() << " !");

  // Configure and initialize the CAT machine :
  CAT::clusterizer_configure(_CAT_clusterizer_, _CAT_setup_);
  CAT::sequentiator_configure(_CAT_sequentiator_, _CAT_setup_);

  _CAT_clusterizer_.initialize();
  _CAT_sequentiator_.initialize();

  // Configure and initialize the SULTAN machine :
  SULTAN::clusterizer_configure(_SULTAN_clusterizer_, _SULTAN_setup_);
  SULTAN::sultan_configure(_SULTAN_sultan_, _SULTAN_setup_);

  _SULTAN_clusterizer_.initialize();
  _SULTAN_sultan_.initialize();

  _set_initialized(true);
}

CAT::topology::cell sultan_then_cat_driver::fill_CAT_hit_from_SULTAN_hit(
    SULTAN::topology::cell sc) {
  ///   SULTAN coordinate system ----->   CAT coordinate system
  ///     x     ----->   z
  ///     y     ----->   x
  ///     z     ----->   y

  CAT::topology::experimental_double x(sc.ep().y().value(), sc.ep().y().error());
  CAT::topology::experimental_double y(sc.ep().z().value(), sc.ep().z().error());
  CAT::topology::experimental_double z(sc.ep().x().value(), sc.ep().x().error());
  CAT::topology::experimental_double r(sc.r().value(), sc.r().error());
  CAT::topology::experimental_point p(x, y, z);

  CAT::topology::cell c(p, r, sc.id(), sc.fast(), sc.probmin());

  c.set_layer(sc.layer());
  c.set_n3id(sc.n3id());
  c.set_iid(sc.iid());
  c.set_block(sc.block());
  c.set_print_level(_CAT_setup_.level);
  c.set_type(sc.type());
  c.set_small_radius(_CAT_setup_.SmallRadius);

  return c;
}

CAT::topology::calorimeter_hit sultan_then_cat_driver::fill_CAT_calo_hit_from_SULTAN_calo_hit(
    SULTAN::topology::calorimeter_hit sc) {
  ///   SULTAN coordinate system ----->   CAT coordinate system
  ///     x     ----->   z
  ///     y     ----->   x
  ///     z     ----->   y

  CAT::topology::experimental_vector cnorm(sc.pl().norm().y().value(), sc.pl().norm().z().value(),
                                           sc.pl().norm().x().value(), sc.pl().norm().y().error(),
                                           sc.pl().norm().z().error(), sc.pl().norm().x().error());
  CAT::topology::experimental_vector csizes(
      sc.pl().sizes().y().value(), sc.pl().sizes().z().value(), sc.pl().sizes().x().value(),
      sc.pl().sizes().y().error(), sc.pl().sizes().z().error(), sc.pl().sizes().x().error());
  CAT::topology::experimental_point ccenter(
      sc.pl().center().y().value(), sc.pl().center().z().value(), sc.pl().center().x().value(),
      sc.pl().center().y().error(), sc.pl().center().z().error(), sc.pl().center().x().error());
  CAT::topology::plane cpl(ccenter, csizes, cnorm);
  CAT::topology::experimental_double cenergy(sc.e().value(), sc.e().error());
  CAT::topology::experimental_double ctime(sc.t().value(), sc.t().error());
  cpl.set_probmin(_CAT_setup_.probmin);
  cpl.set_type("SuperNEMO");
  CAT::topology::calorimeter_hit cc(cpl, cenergy, ctime, sc.id(), sc.layer());
  cc.set_print_level(_CAT_setup_.level);
  cc.set_probmin(_CAT_setup_.probmin);

  return cc;
}

CAT::topology::cluster sultan_then_cat_driver::fill_CAT_cluster_from_SULTAN_sequence(
    SULTAN::topology::sequence ss) {
  std::vector<CAT::topology::node> nodes;

  std::vector<SULTAN::topology::node> snodes = ss.nodes();

  for (std::vector<SULTAN::topology::node>::const_iterator inode = snodes.begin();
       inode != snodes.end(); ++inode) {
    CAT::topology::cell cc = fill_CAT_hit_from_SULTAN_hit(inode->c());
    CAT::topology::node cn(cc);
    cn.set_print_level(_CAT_setup_.level);
    cn.set_probmin(ss.probmin());
    nodes.push_back(cn);
  }

  CAT::topology::cluster c(nodes);
  c.set_print_level(_CAT_setup_.level);
  c.set_probmin(ss.probmin());

  return c;
}

void sultan_then_cat_driver::convert_sultan_data_to_cat_data() {
  namespace ct = CAT::topology;
  namespace st = SULTAN::topology;
  namespace sdm = snemo::datamodel;

  for (std::vector<SULTAN::topology::cell>::const_iterator sc = _SULTAN_input_.cells.begin();
       sc != _SULTAN_input_.cells.end(); ++sc) {
    ct::cell& c = _CAT_input_.add_cell();
    c = fill_CAT_hit_from_SULTAN_hit(*sc);
  }

  for (std::vector<SULTAN::topology::calorimeter_hit>::const_iterator sc =
           _SULTAN_input_.calo_cells.begin();
       sc != _SULTAN_input_.calo_cells.end(); ++sc) {
    ct::calorimeter_hit& c = _CAT_input_.add_calo_cell();
    c = fill_CAT_calo_hit_from_SULTAN_calo_hit(*sc);
  }

  std::vector<SULTAN::topology::scenario> scenarios = _SULTAN_output_.tracked_data.get_scenarios();
  if (!scenarios.empty() != 0u) {
    std::vector<CAT::topology::cluster> cs;
    std::vector<SULTAN::topology::sequence> sequences = scenarios[0].sequences();
    for (std::vector<SULTAN::topology::sequence>::const_iterator iseq = sequences.begin();
         iseq != sequences.end(); ++iseq) {
      CAT::topology::cluster cl = fill_CAT_cluster_from_SULTAN_sequence(*iseq);
      cs.push_back(cl);
    }
    _CAT_output_.tracked_data.set_clusters(cs);
  }
}

SULTAN::topology::sequence sultan_then_cat_driver::fill_SULTAN_sequence_from_CAT_sequence(
    CAT::topology::sequence cs) {
  std::vector<SULTAN::topology::node> nodes;

  std::vector<CAT::topology::node> snodes = cs.nodes();

  for (std::vector<CAT::topology::node>::const_iterator inode = snodes.begin();
       inode != snodes.end(); ++inode) {
    SULTAN::topology::cell cc = fill_SULTAN_hit_from_CAT_hit(inode->c());
    SULTAN::topology::node cn(cc);
    CAT::topology::experimental_point cep = inode->ep();
    SULTAN::topology::experimental_point sep(cep.z().value(), cep.x().value(), cep.y().value(),
                                             cep.z().error(), cep.x().error(), cep.y().error());
    cn.set_ep(sep);
    cn.set_print_level(_SULTAN_setup_.sequentiator_level);
    cn.set_probmin(cs.probmin());
    nodes.push_back(cn);
  }

  SULTAN::topology::sequence c(nodes);
  c.set_print_level(_SULTAN_setup_.sequentiator_level);
  c.set_probmin(cs.probmin());

  return c;
}

SULTAN::topology::cell sultan_then_cat_driver::fill_SULTAN_hit_from_CAT_hit(
    CAT::topology::cell cc) {
  ///   CAT coordinate system ----->   SULTAN coordinate system
  ///     x     ----->   y
  ///     z     ----->   x
  ///     y     ----->   z

  SULTAN::topology::experimental_double x(cc.ep().z().value(), cc.ep().z().error());
  SULTAN::topology::experimental_double y(cc.ep().x().value(), cc.ep().x().error());
  SULTAN::topology::experimental_double z(cc.ep().y().value(), cc.ep().y().error());
  SULTAN::topology::experimental_double r(cc.r().value(), cc.r().error());
  SULTAN::topology::experimental_point p(x, y, z);

  SULTAN::topology::cell c(p, r, cc.id(), cc.fast(), cc.probmin());

  c.set_layer(cc.layer());
  c.set_n3id(cc.n3id());
  c.set_iid(cc.iid());
  c.set_block(cc.block());
  c.set_print_level(_SULTAN_setup_.sequentiator_level);
  c.set_type(cc.type());

  return c;
}

SULTAN::topology::calorimeter_hit sultan_then_cat_driver::fill_SULTAN_calo_hit_from_CAT_calo_hit(
    CAT::topology::calorimeter_hit cc) {
  ///   CAT coordinate system ----->   SULTAN coordinate system
  ///     x     ----->   y
  ///     y     ----->   z
  ///     z     ----->   x

  SULTAN::topology::experimental_double x(cc.pl().center().z().value(),
                                          cc.pl().center().z().error());
  SULTAN::topology::experimental_double y(cc.pl().center().x().value(),
                                          cc.pl().center().x().error());
  SULTAN::topology::experimental_double z(cc.pl().center().y().value(),
                                          cc.pl().center().y().error());

  SULTAN::topology::experimental_point center(x, y, z);

  SULTAN::topology::experimental_double nx(cc.pl().norm().z().value(), cc.pl().norm().z().error());
  SULTAN::topology::experimental_double ny(cc.pl().norm().x().value(), cc.pl().norm().x().error());
  SULTAN::topology::experimental_double nz(cc.pl().norm().y().value(), cc.pl().norm().y().error());

  SULTAN::topology::experimental_vector norm(nx, ny, nz);

  SULTAN::topology::experimental_double sx(cc.pl().sizes().z().value(),
                                           cc.pl().sizes().z().error());
  SULTAN::topology::experimental_double sy(cc.pl().sizes().x().value(),
                                           cc.pl().sizes().x().error());
  SULTAN::topology::experimental_double sz(cc.pl().sizes().y().value(),
                                           cc.pl().sizes().y().error());

  SULTAN::topology::experimental_vector sizes(sx, sy, sz);

  SULTAN::topology::plane pl(center, sizes, norm);
  pl.set_type("SuperNEMO");
  pl.set_print_level(_SULTAN_setup_.sequentiator_level);
  pl.set_probmin(cc.probmin());

  SULTAN::topology::experimental_double energy(cc.e().value(), cc.e().error());

  SULTAN::topology::experimental_double time(cc.t().value(), cc.t().error());

  SULTAN::topology::calorimeter_hit c(pl, energy, time, cc.id(), cc.layer());
  c.set_print_level(_SULTAN_setup_.sequentiator_level);
  c.set_probmin(cc.probmin());

  return c;
}

SULTAN::topology::scenario sultan_then_cat_driver::fill_SULTAN_scenario_from_CAT_scenario(
    CAT::topology::scenario cs) {
  std::vector<SULTAN::topology::sequence> sseqs;
  std::vector<CAT::topology::sequence> seqs = cs.sequences();
  for (std::vector<CAT::topology::sequence>::const_iterator iseq = seqs.begin(); iseq != seqs.end();
       ++iseq) {
    SULTAN::topology::sequence ss = fill_SULTAN_sequence_from_CAT_sequence(*iseq);
    sseqs.push_back(ss);
  }

  SULTAN::topology::scenario s(sseqs);
  s.set_print_level(_SULTAN_setup_.sequentiator_level);
  s.set_probmin(cs.probmin());

  return s;
}

SULTAN::topology::cluster sultan_then_cat_driver::fill_SULTAN_cluster_from_CAT_sequence(
    CAT::topology::sequence cs) {
  std::vector<SULTAN::topology::node> nodes;

  std::vector<CAT::topology::node> snodes = cs.nodes();

  for (std::vector<CAT::topology::node>::const_iterator inode = snodes.begin();
       inode != snodes.end(); ++inode) {
    SULTAN::topology::cell cc = fill_SULTAN_hit_from_CAT_hit(inode->c());
    SULTAN::topology::node cn(cc);
    cn.set_print_level(_SULTAN_setup_.sequentiator_level);
    cn.set_probmin(cs.probmin());
    nodes.push_back(cn);
  }

  SULTAN::topology::cluster c(nodes);
  c.set_print_level(_SULTAN_setup_.sequentiator_level);
  c.set_probmin(cs.probmin());

  return c;
}

void sultan_then_cat_driver::convert_cat_data_to_sultan_data() {
  namespace ct = CAT::topology;
  namespace st = SULTAN::topology;
  namespace sdm = snemo::datamodel;

  _SULTAN_output_.tracked_data.reset();

  for (std::vector<CAT::topology::cell>::const_iterator cc = _CAT_input_.cells.begin();
       cc != _CAT_input_.cells.end(); ++cc) {
    st::cell& c = _SULTAN_input_.add_cell();
    c = fill_SULTAN_hit_from_CAT_hit(*cc);
  }

  for (std::vector<CAT::topology::calorimeter_hit>::const_iterator cc =
           _CAT_input_.calo_cells.begin();
       cc != _CAT_input_.calo_cells.end(); ++cc) {
    st::calorimeter_hit& c = _SULTAN_input_.add_calo_cell();
    c = fill_SULTAN_calo_hit_from_CAT_calo_hit(*cc);
  }

  std::vector<CAT::topology::scenario> cat_scenarios = _CAT_output_.tracked_data.get_scenarios();
  std::vector<SULTAN::topology::scenario> sultan_scenarios;
  for (std::vector<CAT::topology::scenario>::const_iterator iscen = cat_scenarios.begin();
       iscen != cat_scenarios.end(); ++iscen) {
    SULTAN::topology::scenario ssc = fill_SULTAN_scenario_from_CAT_scenario(*iscen);

    std::vector<SULTAN::topology::cluster> cs;
    std::vector<CAT::topology::sequence> sequences = iscen->sequences();
    for (std::vector<CAT::topology::sequence>::const_iterator iseq = sequences.begin();
         iseq != sequences.end(); ++iseq) {
      SULTAN::topology::cluster cl = fill_SULTAN_cluster_from_CAT_sequence(*iseq);
      cs.push_back(cl);
    }
    _SULTAN_output_.tracked_data.set_clusters(cs);
    sultan_scenarios.push_back(ssc);
  }
  _SULTAN_output_.tracked_data.set_scenarios(sultan_scenarios);
}

/// Main clustering method
int sultan_then_cat_driver::_process_algo(
    const base_tracker_clusterizer::hit_collection_type& gg_hits_,
    const base_tracker_clusterizer::calo_hit_collection_type& calo_hits_,
    snemo::datamodel::tracker_clustering_data& clustering_) {
  namespace ct = CAT::topology;
  namespace st = SULTAN::topology;
  namespace sdm = snemo::datamodel;

  // input data model :
  _CAT_input_.cells.clear();
  _CAT_input_.calo_cells.clear();
  _SULTAN_input_.cells.clear();
  if (_SULTAN_input_.cells.capacity() < gg_hits_.size()) {
    _SULTAN_input_.cells.reserve(gg_hits_.size());
  }
  _SULTAN_input_.calo_cells.clear();
  if (_SULTAN_input_.calo_cells.capacity() < calo_hits_.size()) {
    _SULTAN_input_.calo_cells.reserve(calo_hits_.size());
  }
  _SULTAN_output_.tracked_data.reset();
  _CAT_output_.tracked_data.reset();
  size_t ihit = 0;

  // Hit accounting :
  std::map<int, sdm::TrackerHitHdl> gg_hits_mapping;

  // GG hit loop :
  BOOST_FOREACH (const sdm::TrackerHitHdl& gg_handle, gg_hits_) {
    // Skip NULL handle :
    if (!gg_handle.has_data()) {
      continue;
    }

    // Get a const reference on the calibrated Geiger hit :
    const sdm::calibrated_tracker_hit& snemo_gg_hit = gg_handle.get();

    // Check the geometry ID as a Geiger cell :
    const snemo::geometry::gg_locator& gg_locator = get_gg_locator();
    const geomtools::geom_id& gg_hit_gid = snemo_gg_hit.get_geom_id();
    DT_THROW_IF(!gg_locator.isGeigerCell(gg_hit_gid), std::logic_error,
                "Calibrated tracker hit can not be located inside detector !");

    if (!gg_locator.isGeigerCellInThisModule(gg_hit_gid)) {
      DT_LOG_DEBUG(get_logging_priority(), "Current Geiger cell is not in the module!");
      continue;
    }

    // Extract the numbering scheme of the cell from its geom ID :
    const int side = gg_locator.getSideAddress(gg_hit_gid);
    const int layer = gg_locator.getLayerAddress(gg_hit_gid);
    const int row = gg_locator.getRowAddress(gg_hit_gid);

    // Translate into the SULTAN's numbering scheme :
    // -1 : negative X side; +1 : positive X side
    int block_id = side == 0 ? -1 : +1;
    /* Later number:
     * negative X side: layer from -8, -7, -6, -5, -4, -3, -2, -1, 0
     * positive X side: layer from 0, 1, 2 , 3, 4, 5, 6, 7, 8
     */
    int layer_id = side == 0 ? -layer : layer;
    // Cell number :  0:113 is mapped to -56:+56
    int cell_id = row - (_SULTAN_setup_.num_cells_per_plane / 2);

    // X-Y position of the anode wire of the hit cell :
    st::experimental_double x;
    st::experimental_double y;

    // Center of the cell set in SULTAN's own reference frame:
    x.set_value(snemo_gg_hit.get_x());
    x.set_error(0.0);
    y.set_value(snemo_gg_hit.get_y());
    y.set_error(0.0);

    // Transverse Geiger drift distance :
    st::experimental_double z;
    // Plasma longitudinal origin along the anode wire :
    z.set_value(snemo_gg_hit.get_z());
    z.set_error(_sigma_z_factor_ * snemo_gg_hit.get_sigma_z());

    // Prompt/delayed trait of the hit :
    bool fast = snemo_gg_hit.is_prompt();

    // Transverse Geiger drift distance :
    double rdrift = snemo_gg_hit.get_r();
    double rdrift_err = snemo_gg_hit.get_sigma_r();

    // Build the Geiger hit position :
    st::experimental_point gg_hit_position(x, y, z);

    // Add a new hit cell in the SULTAN input data model :
    st::cell& c = _SULTAN_input_.add_cell();
    c.set_type("SN");
    c.set_id(ihit++);
    c.set_probmin(_SULTAN_setup_.probmin);
    c.set_p(gg_hit_position);
    c.set_r(rdrift);
    c.set_er(rdrift_err);
    c.set_layer(layer_id);
    c.set_block(block_id);
    c.set_iid(cell_id);
    c.set_fast(fast);

    // Store mapping info between both data models :
    gg_hits_mapping[c.id()] = gg_handle;

    DT_LOG_DEBUG(get_logging_priority(), "Geiger cell #"
                                             << snemo_gg_hit.get_id() << " has been added "
                                             << "to SULTAN input data with id number #" << c.id());
  }

  // Take into account calo hits:
  _SULTAN_input_.calo_cells.clear();
  // Calo hit accounting :
  std::map<int, sdm::CalorimeterHitHdl> calo_hits_mapping;
  std::map<int, int> gg_hits_status;
  bool conserve_clustering_from_removal_of_cells = true;
  if (_process_calo_hits_) {
    if (_SULTAN_input_.calo_cells.capacity() < calo_hits_.size()) {
      _SULTAN_input_.calo_cells.reserve(calo_hits_.size());
    }
    _SULTAN_output_.tracked_data.reset();
    size_t jhit = 0;

    // CALO hit loop :
    BOOST_FOREACH (const sdm::CalorimeterHitHdl& calo_handle, calo_hits_) {
      // Skip NULL handle :
      if (!calo_handle.has_data()) {
        continue;
      }

      // Get a const reference on the calibrated Calo hit :
      const sdm::calibrated_calorimeter_hit& sncore_calo_hit = calo_handle.get();

      // Get calibrated calo. geom_id
      const geomtools::geom_id& a_calo_hit_gid = sncore_calo_hit.get_geom_id();
      // Extract the numbering scheme of the calo_cell from its geom ID :
      int column = -1;
      int side = -1;
      double width = datatools::invalid_real();
      double height = datatools::invalid_real();
      double thickness = datatools::invalid_real();

      st::experimental_vector norm(0., 0., 0., 0., 0., 0.);
      geomtools::vector_3d block_position;
      // Extract the numbering scheme of the scin block from its geom ID :
      if (_calo_locator_->isCaloBlockInThisModule(a_calo_hit_gid)) {
        block_position = _calo_locator_->getBlockPosition(a_calo_hit_gid);
        width = _calo_locator_->blockWidth();
        height = _calo_locator_->blockHeight();
        thickness = _calo_locator_->blockThickness();
        column = _calo_locator_->getColumnAddress(a_calo_hit_gid);
        side = _calo_locator_->getSideAddress(a_calo_hit_gid);
        const int side_number = (side == snemo::geometry::side_t::BACK) ? 1 : -1;
        norm.set_x(st::experimental_double((double)side_number, 0.));
      } else if (_xcalo_locator_->isCaloBlockInThisModule(a_calo_hit_gid)) {
        block_position = _xcalo_locator_->getBlockPosition(a_calo_hit_gid);
        width = _xcalo_locator_->blockWidth();
        height = _xcalo_locator_->blockHeight();
        thickness = _xcalo_locator_->blockThickness();
        column = _xcalo_locator_->getColumnAddress(a_calo_hit_gid);
        side = _xcalo_locator_->getSideAddress(a_calo_hit_gid);
        const int side_number = (side == snemo::geometry::side_t::BACK) ? 1 : -1;
        norm.set_y(st::experimental_double((double)side_number, 0.));
      } else if (_gveto_locator_->isCaloBlockInThisModule(a_calo_hit_gid)) {
        block_position = _gveto_locator_->getBlockPosition(a_calo_hit_gid);
        width = _gveto_locator_->blockWidth();
        height = _gveto_locator_->blockHeight();
        thickness = _gveto_locator_->blockThickness();
        column = _gveto_locator_->getColumnAddress(a_calo_hit_gid);
        side = _gveto_locator_->getSideAddress(a_calo_hit_gid);
        const int side_number = (side == snemo::geometry::side_t::BACK) ? 1 : -1;
        norm.set_z(st::experimental_double((double)side_number, 0.));
      }

      st::experimental_double energy(sncore_calo_hit.get_energy(),
                                     sncore_calo_hit.get_sigma_energy());
      st::experimental_double time(sncore_calo_hit.get_time(), sncore_calo_hit.get_sigma_time());
      // size_t id = sncore_calo_hit.get_hit_id ();
      st::experimental_point center(block_position.x(), block_position.y(), block_position.z(), 0.,
                                    0., 0.);
      st::experimental_vector sizes(width, height, thickness, 0., 0., 0.);
      st::plane pl(center, sizes, norm);
      pl.set_probmin(_SULTAN_setup_.probmin);
      pl.set_type("SuperNEMO");

      // Build the Calo hit position :
      // Add a new hit calo_cell in the SULTAN input data model :
      st::calorimeter_hit& c = _SULTAN_input_.add_calo_cell();
      c.set_pl(pl);
      c.set_e(energy);
      c.set_t(time);
      c.set_probmin(_SULTAN_setup_.probmin);
      c.set_layer(column);
      c.set_id(jhit++);

      // Store mapping info between both data models :
      calo_hits_mapping[c.id()] = calo_handle;

      DT_LOG_DEBUG(get_logging_priority(),
                   "Calo_cell #" << sncore_calo_hit.get_hit_id() << " has been added "
                                 << "to SULTAN input data with id number #" << c.id());
    }
  }

  // Validate the input data :
  if (!_SULTAN_input_.check()) {
    DT_LOG_ERROR(get_logging_priority(), "Invalid SULTAN input data !");
    return 1;
  }

  // Install the input data model within the algorithm object :
  _SULTAN_clusterizer_.set_cells(_SULTAN_input_.cells);
  _SULTAN_clusterizer_.set_calorimeter_hits(_SULTAN_input_.calo_cells);

  // Prepare the output data model :
  _SULTAN_clusterizer_.prepare_event(_SULTAN_output_.tracked_data);

  // Run the Sultan algorithm :
  _SULTAN_clusterizer_.clusterize(_SULTAN_output_.tracked_data);
  _SULTAN_sultan_.sequentiate(_SULTAN_output_.tracked_data);

  convert_sultan_data_to_cat_data();

  // Validate the input data :
  if (!_CAT_input_.check()) {
    DT_LOG_ERROR(get_logging_priority(), "Invalid CAT input data !");
    return 1;
  }

  // Install the input data model within the algorithm object :
  _CAT_clusterizer_.set_cells(_CAT_input_.cells);
  _CAT_clusterizer_.set_calorimeter_hits(_CAT_input_.calo_cells);

  // Prepare the output data model :
  _CAT_clusterizer_.prepare_event(_CAT_output_.tracked_data);

  // Run the clusterizer algorithm :
  _CAT_clusterizer_.clusterize_after_sultan(_CAT_output_.tracked_data);

  // Run the sequentiator algorithm :
  _CAT_sequentiator_.sequentiate_after_sultan(_CAT_output_.tracked_data,
                                              conserve_clustering_from_removal_of_cells);

  convert_cat_data_to_sultan_data();
  _SULTAN_sultan_.sequentiate_after_cat(_SULTAN_output_.tracked_data);

  // Analyse the Sultan output: scenarios made of sequences
  const std::vector<ct::scenario>& tss = _CAT_output_.tracked_data.get_scenarios();
  DT_LOG_DEBUG(get_logging_priority(), "Number of scenarios = " << tss.size());

  std::vector<SULTAN::topology::sequence> sultan_sequences;
  std::vector<SULTAN::topology::scenario> sultan_scenarios =
      _SULTAN_output_.tracked_data.get_scenarios();
  if (!sultan_scenarios.empty() != 0u) {
    sultan_sequences = sultan_scenarios[0].sequences();
  }

  for (const auto& ts : tss) {
    // Add a new solution :
    sdm::TrackerClusteringSolutionHdl htcs(new sdm::tracker_clustering_solution);
    clustering_.add_solution(htcs, true);
    clustering_.get_default_solution().set_solution_id(clustering_.get_number_of_solutions() - 1);
    sdm::tracker_clustering_solution& clustering_solution = clustering_.get_default_solution();
    clustering_solution.get_auxiliaries().update_string(
        sdm::tracker_clustering_data::clusterizer_id_key(), SULTAN_THEN_CAT_ID);

    clustering_solution.get_auxiliaries().update_string("TRACKER", "CAT");

    // Analyse the sequentiator output :
    const std::vector<ct::sequence>& the_sequences = ts.sequences();

    if (the_sequences.size() != sultan_sequences.size()) {
      std::clog << " snreconstruction::sultan_then_cat_driver:: problem: cat_sequences.size() "
                << the_sequences.size() << " sultan_sequences.size() " << sultan_sequences.size()
                << std::endl;
      exit(0);
    }

    for (auto isequence = the_sequences.begin(); isequence != the_sequences.end(); ++isequence) {
      const ct::sequence& a_sequence = *isequence;
      const size_t seqsz = a_sequence.nodes().size();

      if (seqsz == 1) {
        // A CAT cluster with only one hit/cell (node) :
        int hit_id = a_sequence.nodes()[0].c().id();
        gg_hits_status[hit_id] = 1;
        clustering_solution.get_unclustered_hits().push_back(gg_hits_mapping[hit_id]);
      } else {
        // A CAT cluster with more than one hit/cell (node) :
        {
          // Append a new cluster :
          sdm::TrackerClusterHdl tch(new sdm::tracker_cluster);
          clustering_solution.get_clusters().push_back(tch);
        }
        sdm::TrackerClusterHdl& cluster_handle = clustering_solution.get_clusters().back();
        cluster_handle.grab().set_cluster_id(clustering_solution.get_clusters().size() - 1);
        const ct::helix& seq_helix = isequence->get_helix();

        size_t index_of_sultan_sequence = isequence - the_sequences.begin();
        const SULTAN::topology::experimental_helix& sultan_helix =
            sultan_sequences[index_of_sultan_sequence].get_helix();
        const SULTAN::topology::experimental_double& sultan_x0 = sultan_helix.x0();
        const SULTAN::topology::experimental_double& sultan_y0 = sultan_helix.y0();
        const SULTAN::topology::experimental_double& sultan_z0 = sultan_helix.z0();
        const SULTAN::topology::experimental_double& sultan_radius = sultan_helix.R();
        const SULTAN::topology::experimental_double& sultan_pitch = -sultan_helix.H();
        const SULTAN::topology::experimental_vector& CAT_sultan_helix_momentum =
            sultan_sequences[index_of_sultan_sequence].helix_momentum();

        // 2012/06/28 XG : Adding
        // - tangency points
        // - helix points
        // - track tangency vertex
        // - track helix vertex
        // from CAT algorithm. Since it is a none generic
        // information, this info will be added to
        // calibrated data cells as properties.
        // Be careful of the system coordinate :
        // xcat -> y_snemo
        // ycat -> z_snemo
        // zcat -> x_snemo
        const ct::experimental_point& helix_decay_vertex = a_sequence.decay_helix_vertex();
        const ct::experimental_point& helix_vertex = a_sequence.helix_vertex();

        const ct::experimental_point& tangent_decay_vertex = a_sequence.decay_tangent_vertex();
        const ct::experimental_point& tangent_vertex = a_sequence.tangent_vertex();

        const bool& has_momentum = a_sequence.has_momentum();
        const ct::experimental_vector& CAT_momentum = a_sequence.momentum();

        const bool& has_charge = a_sequence.has_charge();
        const double charge = a_sequence.charge().value();

        const bool& has_helix_charge = a_sequence.has_helix_charge();
        const double helix_charge = a_sequence.helix_charge().value();

        const bool& has_detailed_charge = a_sequence.has_detailed_charge();
        const double detailed_charge = a_sequence.detailed_charge().value();

        const bool& has_tangent_length = a_sequence.has_tangent_length();
        const double tangent_length = a_sequence.tangent_length().value();
        const double tangent_length_error = a_sequence.tangent_length().error();

        const bool& has_helix_length = a_sequence.has_helix_length();
        const double helix_length = a_sequence.helix_length().value();
        const double helix_length_error = a_sequence.helix_length().error();

        const double xhv = helix_vertex.z().value();
        const double yhv = helix_vertex.x().value();
        const double zhv = helix_vertex.y().value();
        const double xhverr = helix_vertex.z().error();
        const double yhverr = helix_vertex.x().error();
        const double zhverr = helix_vertex.y().error();
        const bool hashv = a_sequence.has_helix_vertex();

        const double xhdv = helix_decay_vertex.z().value();
        const double yhdv = helix_decay_vertex.x().value();
        const double zhdv = helix_decay_vertex.y().value();
        const double xhdverr = helix_decay_vertex.z().error();
        const double yhdverr = helix_decay_vertex.x().error();
        const double zhdverr = helix_decay_vertex.y().error();
        const bool hashdv = a_sequence.has_decay_helix_vertex();

        const double xtv = tangent_vertex.z().value();
        const double ytv = tangent_vertex.x().value();
        const double ztv = tangent_vertex.y().value();
        const double xtverr = tangent_vertex.z().error();
        const double ytverr = tangent_vertex.x().error();
        const double ztverr = tangent_vertex.y().error();
        const bool hastv = a_sequence.has_tangent_vertex();

        const double xtdv = tangent_decay_vertex.z().value();
        const double ytdv = tangent_decay_vertex.x().value();
        const double ztdv = tangent_decay_vertex.y().value();
        const double xtdverr = tangent_decay_vertex.z().error();
        const double ytdverr = tangent_decay_vertex.x().error();
        const double ztdverr = tangent_decay_vertex.y().error();
        const bool hastdv = a_sequence.has_decay_tangent_vertex();

        const std::vector<double> chi2s_all = a_sequence.chi2s_all();
        const std::vector<double> probs_all = a_sequence.probs_all();
        const std::vector<double> chi2s = a_sequence.chi2s();
        const std::vector<double> probs = a_sequence.probs();
        const std::vector<double> helix_chi2s = a_sequence.helix_chi2s();

        cluster_handle.grab().grab_auxiliaries().update("CAT_helix_x0",
                                                        seq_helix.center().z().value());
        cluster_handle.grab().grab_auxiliaries().update("CAT_helix_y0",
                                                        seq_helix.center().x().value());
        cluster_handle.grab().grab_auxiliaries().update("CAT_helix_z0",
                                                        seq_helix.center().y().value());
        cluster_handle.grab().grab_auxiliaries().update("CAT_helix_R", seq_helix.radius().value());
        cluster_handle.grab().grab_auxiliaries().update("CAT_helix_H", -seq_helix.pitch().value());

        cluster_handle.grab().grab_auxiliaries().update("CAT_has_momentum", has_momentum);
        if (has_momentum) {
          cluster_handle.grab().grab_auxiliaries().update("CAT_momentum_x",
                                                          CAT_momentum.z().value());
          cluster_handle.grab().grab_auxiliaries().update("CAT_momentum_y",
                                                          CAT_momentum.x().value());
          cluster_handle.grab().grab_auxiliaries().update("CAT_momentum_z",
                                                          CAT_momentum.y().value());
        }

        cluster_handle.grab().grab_auxiliaries().update("CAT_has_charge", has_charge);
        if (has_charge) {
          cluster_handle.grab().grab_auxiliaries().update("CAT_charge", charge);
        }

        cluster_handle.grab().grab_auxiliaries().update("CAT_has_helix_charge", has_helix_charge);
        if (has_helix_charge) {
          cluster_handle.grab().grab_auxiliaries().update("CAT_helix_charge", helix_charge);
        }

        cluster_handle.grab().grab_auxiliaries().update("CAT_has_detailed_charge",
                                                        has_detailed_charge);
        if (has_detailed_charge) {
          cluster_handle.grab().grab_auxiliaries().update("CAT_detailed_charge", detailed_charge);
        }

        cluster_handle.grab().grab_auxiliaries().update("CAT_has_tangent_length",
                                                        has_tangent_length);
        if (has_tangent_length) {
          cluster_handle.grab().grab_auxiliaries().update("CAT_tangent_length", tangent_length);
          cluster_handle.grab().grab_auxiliaries().update("CAT_tangent_length_error",
                                                          tangent_length_error);
        }

        cluster_handle.grab().grab_auxiliaries().update("CAT_has_helix_length", has_helix_length);
        if (has_helix_length) {
          cluster_handle.grab().grab_auxiliaries().update("CAT_helix_length", helix_length);
          cluster_handle.grab().grab_auxiliaries().update("CAT_helix_length_error",
                                                          helix_length_error);
        }

        cluster_handle.grab().grab_auxiliaries().update("CAT_has_helix_vertex", hashv);
        if (hashv) {
          cluster_handle.grab().grab_auxiliaries().update("CAT_helix_vertex_type",
                                                          a_sequence.helix_vertex_type());
          cluster_handle.grab().grab_auxiliaries().update("CAT_helix_vertex_x", xhv);
          cluster_handle.grab().grab_auxiliaries().update("CAT_helix_vertex_y", yhv);
          cluster_handle.grab().grab_auxiliaries().update("CAT_helix_vertex_z", zhv);
          cluster_handle.grab().grab_auxiliaries().update("CAT_helix_vertex_x_error", xhverr);
          cluster_handle.grab().grab_auxiliaries().update("CAT_helix_vertex_y_error", yhverr);
          cluster_handle.grab().grab_auxiliaries().update("CAT_helix_vertex_z_error", zhverr);
        }

        cluster_handle.grab().grab_auxiliaries().update("CAT_has_helix_decay_vertex", hashdv);
        if (hashdv) {
          cluster_handle.grab().grab_auxiliaries().update("CAT_helix_decay_vertex_type",
                                                          a_sequence.decay_helix_vertex_type());
          cluster_handle.grab().grab_auxiliaries().update("CAT_helix_decay_vertex_x", xhdv);
          cluster_handle.grab().grab_auxiliaries().update("CAT_helix_decay_vertex_y", yhdv);
          cluster_handle.grab().grab_auxiliaries().update("CAT_helix_decay_vertex_z", zhdv);
          cluster_handle.grab().grab_auxiliaries().update("CAT_helix_decay_vertex_x_error",
                                                          xhdverr);
          cluster_handle.grab().grab_auxiliaries().update("CAT_helix_decay_vertex_y_error",
                                                          yhdverr);
          cluster_handle.grab().grab_auxiliaries().update("CAT_helix_decay_vertex_z_error",
                                                          zhdverr);
        }

        cluster_handle.grab().grab_auxiliaries().update("sultan_center_of_curvature_x",
                                                        sultan_x0.value());
        cluster_handle.grab().grab_auxiliaries().update("sultan_center_of_curvature_y",
                                                        sultan_y0.value());
        cluster_handle.grab().grab_auxiliaries().update("sultan_center_of_curvature_z",
                                                        sultan_z0.value());
        cluster_handle.grab().grab_auxiliaries().update("sultan_radius", sultan_radius.value());
        cluster_handle.grab().grab_auxiliaries().update("sultan_pitch", sultan_pitch.value());
        cluster_handle.grab().grab_auxiliaries().update("sultan_helix_momentum_x",
                                                        CAT_sultan_helix_momentum.x().value());
        cluster_handle.grab().grab_auxiliaries().update("sultan_helix_momentum_y",
                                                        CAT_sultan_helix_momentum.y().value());
        cluster_handle.grab().grab_auxiliaries().update("sultan_helix_momentum_z",
                                                        CAT_sultan_helix_momentum.z().value());
        cluster_handle.grab().grab_auxiliaries().update("sultan_helix_momentum_x_error",
                                                        CAT_sultan_helix_momentum.x().error());
        cluster_handle.grab().grab_auxiliaries().update("sultan_helix_momentum_y_error",
                                                        CAT_sultan_helix_momentum.y().error());
        cluster_handle.grab().grab_auxiliaries().update("sultan_helix_momentum_z_error",
                                                        CAT_sultan_helix_momentum.z().error());

        cluster_handle.grab().grab_auxiliaries().update("CAT_has_tangent_vertex", hastv);
        if (hastv) {
          cluster_handle.grab().grab_auxiliaries().update("CAT_tangent_vertex_type",
                                                          a_sequence.tangent_vertex_type());
          cluster_handle.grab().grab_auxiliaries().update("CAT_tangent_vertex_x", xtv);
          cluster_handle.grab().grab_auxiliaries().update("CAT_tangent_vertex_y", ytv);
          cluster_handle.grab().grab_auxiliaries().update("CAT_tangent_vertex_z", ztv);
          cluster_handle.grab().grab_auxiliaries().update("CAT_tangent_vertex_x_error", xtverr);
          cluster_handle.grab().grab_auxiliaries().update("CAT_tangent_vertex_y_error", ytverr);
          cluster_handle.grab().grab_auxiliaries().update("CAT_tangent_vertex_z_error", ztverr);
        }

        cluster_handle.grab().grab_auxiliaries().update("CAT_has_tangent_decay_vertex", hastdv);
        if (hastdv) {
          cluster_handle.grab().grab_auxiliaries().update("CAT_tangent_decay_vertex_type",
                                                          a_sequence.decay_tangent_vertex_type());
          cluster_handle.grab().grab_auxiliaries().update("CAT_tangent_decay_vertex_x", xtdv);
          cluster_handle.grab().grab_auxiliaries().update("CAT_tangent_decay_vertex_y", ytdv);
          cluster_handle.grab().grab_auxiliaries().update("CAT_tangent_decay_vertex_z", ztdv);
          cluster_handle.grab().grab_auxiliaries().update("CAT_tangent_decay_vertex_x_error",
                                                          xtdverr);
          cluster_handle.grab().grab_auxiliaries().update("CAT_tangent_decay_vertex_y_error",
                                                          ytdverr);
          cluster_handle.grab().grab_auxiliaries().update("CAT_tangent_decay_vertex_z_error",
                                                          ztdverr);
        }

        cluster_handle.grab().grab_auxiliaries().update("CAT_chi2s_all", chi2s_all);
        cluster_handle.grab().grab_auxiliaries().update("CAT_probs_all", probs_all);
        cluster_handle.grab().grab_auxiliaries().update("CAT_chi2s", chi2s);
        cluster_handle.grab().grab_auxiliaries().update("CAT_probs", probs);
        cluster_handle.grab().grab_auxiliaries().update("CAT_helix_chi2s", helix_chi2s);

        ct::experimental_double phi(0., 0.);
        double phi_ref = 0.;

        // Loop on all hits within the sequence (nodes) :
        for (int i = 0; i < (int)seqsz; i++) {
          const ct::node& a_node = a_sequence.nodes()[i];
          int hit_id = a_node.c().id();
          cluster_handle->get_hits().push_back(gg_hits_mapping[hit_id]);
          gg_hits_status[hit_id] = 1;

          const double xt = a_node.ep().x().value();
          const double yt = a_node.ep().y().value();
          const double zt = a_node.ep().z().value();
          const double xterr = a_node.ep().x().error();
          const double yterr = a_node.ep().y().error();
          const double zterr = a_node.ep().z().error();

          phi_ref = phi.value();
          phi = seq_helix.phi_of_point(a_node.c().ep(), phi_ref);
          ct::experimental_vector hpos = seq_helix.position(phi);
          const double hx = hpos.x().value();
          const double hy = hpos.y().value();
          const double hz = hpos.z().value();
          const double hxerr = hpos.x().error();
          const double hyerr = hpos.y().error();
          const double hzerr = hpos.z().error();

          // Be careful of the system coordinate :
          // xcat -> y_snemo
          // ycat -> z_snemo
          // zcat -> x_snemo
          sdm::calibrated_tracker_hit& the_last_cell = gg_hits_mapping[hit_id].grab();
          the_last_cell.grab_auxiliaries().update("CAT_tangency_x", zt);
          the_last_cell.grab_auxiliaries().update("CAT_tangency_y", xt);
          the_last_cell.grab_auxiliaries().update("CAT_tangency_z", yt);
          the_last_cell.grab_auxiliaries().update("CAT_tangency_x_error", zterr);
          the_last_cell.grab_auxiliaries().update("CAT_tangency_y_error", xterr);
          the_last_cell.grab_auxiliaries().update("CAT_tangency_z_error", yterr);
          the_last_cell.grab_auxiliaries().update("CAT_helix_x", hz);
          the_last_cell.grab_auxiliaries().update("CAT_helix_y", hx);
          the_last_cell.grab_auxiliaries().update("CAT_helix_z", hy);
          the_last_cell.grab_auxiliaries().update("CAT_helix_x_error", hzerr);
          the_last_cell.grab_auxiliaries().update("CAT_helix_y_error", hxerr);
          the_last_cell.grab_auxiliaries().update("CAT_helix_z_error", hyerr);
        }
      }
    }  // for sequence

    // Search for remaining unclustered hits :
    for (std::map<int, int>::const_iterator ihs = gg_hits_status.begin();
         ihs != gg_hits_status.end(); ihs++) {
      bool hit_id = ihs->first != 0;
      if (ihs->second == 0) {
        clustering_solution.get_unclustered_hits().push_back(gg_hits_mapping[hit_id]);
      }
    }

  }  // for scenario

  return 0;
}

// static
void sultan_then_cat_driver::init_ocd(datatools::object_configuration_description& ocd_) {
  // Invoke OCD support from parent class :
  ::snemo::processing::base_tracker_clusterizer::ocd_support(ocd_);

  {
    // Description of the 'SULTAN.magnetic_field' configuration property :
    datatools::configuration_property_description& cpd = ocd_.add_property_info();
    cpd.set_name_pattern("SULTAN.magnetic_field")
        .set_from("snemo::reconstruction::sultan_then_cat_driver")
        .set_terse_description("Force the magnetic field value (vertical)")
        .set_traits(datatools::TYPE_REAL)
        .set_mandatory(false)
        // .set_long_description("Default value: 25 gauss")
        .set_default_value_real(25 * CLHEP::gauss, "gauss")
        .add_example(
            "Use no magnetic field::                    \n"
            "                                           \n"
            "  SULTAN.magnetic_field : real = 0.0 gauss \n"
            "                                           \n");
  }

  {
    // Description of the 'SULTAN.clusterizer_level' configuration property :
    datatools::configuration_property_description& cpd = ocd_.add_property_info();
    cpd.set_name_pattern("SULTAN.clusterizer_level")
        .set_from("snemo::reconstruction::sultan_then_cat_driver")
        .set_terse_description("SULTAN clusterizer's verbosity level")
        .set_traits(datatools::TYPE_STRING)
        .set_mandatory(false)
        // .set_long_description("Default value: \"normal\"")
        .set_default_value_string("normal")
        .add_example(
            "Use normal verbosity::                           \n"
            "                                                 \n"
            "  SULTAN.clusterizer_level : string = \"normal\" \n"
            "                                                 \n");
  }

  {
    // Description of the 'SULTAN.sequentiator_level' configuration property :
    datatools::configuration_property_description& cpd = ocd_.add_property_info();
    cpd.set_name_pattern("SULTAN.sequentiator_level")
        .set_from("snemo::reconstruction::sultan_then_cat_driver")
        .set_terse_description("SULTAN sequentiator's verbosity level")
        .set_traits(datatools::TYPE_STRING)
        .set_mandatory(false)
        // .set_long_description("Default value: \"normal\"")
        .set_default_value_string("normal")
        .add_example(
            "Use normal verbosity::                           \n"
            "                                                 \n"
            "  SULTAN.sequentiator_level : string = \"normal\" \n"
            "                                                 \n");
  }

  {
    // Description of the 'SULTAN.max_time' configuration property :
    datatools::configuration_property_description& cpd = ocd_.add_property_info();
    cpd.set_name_pattern("SULTAN.max_time")
        .set_from("snemo::reconstruction::sultan_then_cat_driver")
        .set_terse_description("Maximum processing time")
        .set_traits(datatools::TYPE_REAL)
        .set_mandatory(false)
        // .set_long_description("Default value: 5000 ms")
        .set_default_value_real(5000 * CLHEP::ms, "ms")
        .add_example(
            "Use default value::                \n"
            "                                   \n"
            "  SULTAN.max_time : real = 5000 ms \n"
            "                                   \n");
  }

  {
    // Description of the 'SULTAN.print_event_display' configuration property :
    datatools::configuration_property_description& cpd = ocd_.add_property_info();
    cpd.set_name_pattern("SULTAN.print_event_display")
        .set_from("snemo::reconstruction::sultan_then_cat_driver")
        .set_terse_description("Activate the SULTAN print event display")
        .set_traits(datatools::TYPE_BOOLEAN)
        .set_mandatory(false)
        .set_default_value_boolean(false)
        .add_example(
            "Use normal verbosity::                           \n"
            "                                                 \n"
            "  SULTAN.print_event_display : boolean = 0       \n"
            "                                                 \n");
  }

  {
    // Description of the 'SULTAN.use_clocks' configuration property :
    datatools::configuration_property_description& cpd = ocd_.add_property_info();
    cpd.set_name_pattern("SULTAN.use_clocks")
        .set_from("snemo::reconstruction::sultan_then_cat_driver")
        .set_terse_description("Activate the SULTAN clocks")
        .set_traits(datatools::TYPE_BOOLEAN)
        .set_mandatory(false)
        .set_default_value_boolean(false)
        .add_example(
            "Activate clocks::                                \n"
            "                                                 \n"
            "  SULTAN.use_clocks : boolean = 1                \n"
            "                                                 \n");
  }

  {
    // Description of the 'SULTAN.endpoints' configuration property :
    datatools::configuration_property_description& cpd = ocd_.add_property_info();
    cpd.set_name_pattern("SULTAN.endpoints")
        .set_from("snemo::reconstruction::sultan_then_cat_driver")
        .set_terse_description("Use end points to clusterize")
        .set_traits(datatools::TYPE_BOOLEAN)
        .set_mandatory(false)
        .set_default_value_boolean(false)
        .add_example(
            "Activate endpoints::                             \n"
            "                                                 \n"
            "  SULTAN.endpoints : boolean = 1                 \n"
            "                                                 \n");
  }

  {
    // Description of the 'SULTAN.legendre' configuration property :
    datatools::configuration_property_description& cpd = ocd_.add_property_info();
    cpd.set_name_pattern("SULTAN.legendre")
        .set_from("snemo::reconstruction::sultan_then_cat_driver")
        .set_terse_description("Use Legendre transform to clusterize")
        .set_traits(datatools::TYPE_BOOLEAN)
        .set_mandatory(false)
        .set_default_value_boolean(false)
        .add_example(
            "Activate Legendre transform::                    \n"
            "                                                 \n"
            "  SULTAN.legendre : boolean = 1                  \n"
            "                                                 \n");
  }

  {
    // Description of the 'SULTAN.clusterize_with_helix_model' configuration property :
    datatools::configuration_property_description& cpd = ocd_.add_property_info();
    cpd.set_name_pattern("SULTAN.clusterize_with_helix_model")
        .set_from("snemo::reconstruction::sultan_then_cat_driver")
        .set_terse_description("Use helix model to clusterize")
        .set_traits(datatools::TYPE_BOOLEAN)
        .set_mandatory(false)
        .set_default_value_boolean(false)
        .add_example(
            "Activate helix model::                             \n"
            "                                                   \n"
            "  SULTAN.clusterize_with_helix_model : boolean = 1 \n"
            "                                                   \n");
  }

  {
    // Description of the 'SULTAN.Emin' configuration property :
    datatools::configuration_property_description& cpd = ocd_.add_property_info();
    cpd.set_name_pattern("SULTAN.Emin")
        .set_from("snemo::reconstruction::sultan_then_cat_driver")
        .set_terse_description("Minimum energy of detected electron")
        .set_traits(datatools::TYPE_REAL)
        .set_mandatory(false)
        // .set_long_description("Default value: 0.2 MeV")
        .set_default_value_real(0.2 * CLHEP::MeV, "MeV")
        .add_example(
            "Use default value::                \n"
            "                                   \n"
            "  SULTAN.Emin : real = 0.2 MeV     \n"
            "                                   \n");
  }

  {
    // Description of the 'SULTAN.Emax' configuration property :
    datatools::configuration_property_description& cpd = ocd_.add_property_info();
    cpd.set_name_pattern("SULTAN.Emax")
        .set_from("snemo::reconstruction::sultan_then_cat_driver")
        .set_terse_description("Maximum energy of detected electron")
        .set_traits(datatools::TYPE_REAL)
        .set_mandatory(false)
        // .set_long_description("Default value: 7.0 MeV")
        .set_default_value_real(7.0 * CLHEP::MeV, "MeV")
        .add_example(
            "Use default value::                \n"
            "                                   \n"
            "  SULTAN.Emax : real = 7.0 MeV     \n"
            "                                   \n");
  }

  {
    // Description of the 'SULTAN.probmin' configuration property :
    datatools::configuration_property_description& cpd = ocd_.add_property_info();
    cpd.set_name_pattern("SULTAN.probmin")
        .set_from("snemo::reconstruction::sultan_then_cat_driver")
        .set_terse_description("Minimal probability away from the straight line")
        .set_traits(datatools::TYPE_REAL)
        .set_mandatory(false)
        // .set_long_description("Default value: 0.0")
        .set_default_value_real(0.0)
        .add_example(
            "Use default value::                \n"
            "                                   \n"
            "  SULTAN.probmin : real = 0.0      \n"
            "                                   \n");
  }

  {
    // Description of the 'SULTAN.nsigma_r' configuration property :
    datatools::configuration_property_description& cpd = ocd_.add_property_info();
    cpd.set_name_pattern("SULTAN.nsigma_r")
        .set_from("snemo::reconstruction::sultan_then_cat_driver")
        .set_terse_description("Number of sigmas for tolerance on the drift radius")
        .set_traits(datatools::TYPE_REAL)
        .set_mandatory(false)
        // .set_long_description("Default value: 5.0")
        .set_default_value_real(5.0)
        .add_example(
            "Use default value::                \n"
            "                                   \n"
            "  SULTAN.nsigma_r : real = 5.0     \n"
            "                                   \n");
  }

  {
    // Description of the 'SULTAN.nsigma_z' configuration property :
    datatools::configuration_property_description& cpd = ocd_.add_property_info();
    cpd.set_name_pattern("SULTAN.nsigma_z")
        .set_from("snemo::reconstruction::sultan_then_cat_driver")
        .set_terse_description("Number of sigmas for tolerance on the longitudinal position")
        .set_traits(datatools::TYPE_REAL)
        .set_mandatory(false)
        // .set_long_description("Default value: 3.0")
        .set_default_value_real(3.0)
        .add_example(
            "Use default value::                \n"
            "                                   \n"
            "  SULTAN.nsigma_z : real = 3.0     \n"
            "                                   \n");
  }

  {
    // Description of the 'SULTAN.nofflayers' configuration property :
    datatools::configuration_property_description& cpd = ocd_.add_property_info();
    cpd.set_name_pattern("SULTAN.nofflayers")
        .set_from("snemo::reconstruction::sultan_then_cat_driver")
        .set_terse_description(
            "Number of cells which can be skipped (because the cell did not work) and still the "
            "cluster is continuous")
        .set_traits(datatools::TYPE_INTEGER)
        .set_mandatory(false)
        // .set_long_description("Default value: 1")
        .set_default_value_integer(1)
        .add_example(
            "Use default value::               \n"
            "                                  \n"
            "  SULTAN.nofflayers : integer = 1 \n"
            "                                  \n");
  }

  {
    // Description of the 'SULTAN.first_event' configuration property :
    datatools::configuration_property_description& cpd = ocd_.add_property_info();
    cpd.set_name_pattern("SULTAN.first_event")
        .set_from("snemo::reconstruction::sultan_then_cat_driver")
        .set_terse_description("First event to be processed")
        .set_traits(datatools::TYPE_INTEGER)
        .set_mandatory(false)
        // .set_long_description("Default value: -1")
        .set_default_value_integer(-1)
        .add_example(
            "Do not specify any first event::     \n"
            "                                     \n"
            "  SULTAN.first_event : integer = -1  \n"
            "                                     \n");
  }

  {
    // Description of the 'SULTAN.sigma_z_factor' configuration property :
    datatools::configuration_property_description& cpd = ocd_.add_property_info();
    cpd.set_name_pattern("SULTAN.sigma_z_factor")
        .set_from("snemo::reconstruction::sultan_then_cat_driver")
        .set_terse_description("Sigma Z factor")
        .set_traits(datatools::TYPE_REAL)
        .set_mandatory(false)
        // .set_long_description("Default value: 1.0")
        .set_default_value_real(1.0)
        .add_example(
            "Use the default value::                     \n"
            "                                            \n"
            "  SULTAN.sigma_z_factor : real = 1.0 \n"
            "                                            \n");
  }

  {
    // Description of the 'SULTAN.min_ncells_in_cluster' configuration property :
    datatools::configuration_property_description& cpd = ocd_.add_property_info();
    cpd.set_name_pattern("SULTAN.min_ncells_in_cluster")
        .set_from("snemo::reconstruction::sultan_then_cat_driver")
        .set_terse_description("Minimum number of cells in a cluster")
        .set_traits(datatools::TYPE_INTEGER)
        .set_mandatory(false)
        // .set_long_description("Default value: -1")
        .set_default_value_integer(0)
        .add_example(
            "Do not specify any min value::                \n"
            "                                              \n"
            "  SULTAN.min_ncells_in_cluster : integer = 0  \n"
            "                                              \n");
  }

  {
    // Description of the 'SULTAN.ncells_between_triplet_min' configuration property :
    datatools::configuration_property_description& cpd = ocd_.add_property_info();
    cpd.set_name_pattern("SULTAN.ncells_between_triplet_min")
        .set_from("snemo::reconstruction::sultan_then_cat_driver")
        .set_terse_description("Minimum distance between cells in a triplet")
        .set_traits(datatools::TYPE_INTEGER)
        .set_mandatory(false)
        // .set_long_description("Default value: -1")
        .set_default_value_integer(0)
        .add_example(
            "Do not specify any min value::                \n"
            "                                              \n"
            "  SULTAN.ncells_between_triplet_min : integer = 0  \n"
            "                                              \n");
  }

  {
    // Description of the 'SULTAN.ncells_between_triplet_range' configuration property :
    datatools::configuration_property_description& cpd = ocd_.add_property_info();
    cpd.set_name_pattern("SULTAN.ncells_between_triplet_range")
        .set_from("snemo::reconstruction::sultan_then_cat_driver")
        .set_terse_description("Range distance between cells in a triplet")
        .set_traits(datatools::TYPE_INTEGER)
        .set_mandatory(false)
        // .set_long_description("Default value: -1")
        .set_default_value_integer(0)
        .add_example(
            "Do not specify any min value::                \n"
            "                                              \n"
            "  SULTAN.ncells_between_triplet_range : integer = 0  \n"
            "                                              \n");
  }

  {
    // Description of the 'CAT.magnetic_field' configuration property :
    datatools::configuration_property_description& cpd = ocd_.add_property_info();
    cpd.set_name_pattern("CAT.magnetic_field")
        .set_from("snemo::reconstruction::sultan_then_cat_driver")
        .set_terse_description("Force the magnetic field value (vertical)")
        .set_traits(datatools::TYPE_REAL)
        .set_mandatory(false)
        // .set_long_description("Default value: 25 gauss")
        .set_default_value_real(25 * CLHEP::gauss, "gauss")
        .add_example(
            "Use no magnetic field::               \n"
            "                                      \n"
            "  CAT.magnetic_field : real = 0 gauss \n"
            "                                      \n");
  }

  {
    // Description of the 'CAT.level' configuration property :
    datatools::configuration_property_description& cpd = ocd_.add_property_info();
    cpd.set_name_pattern("CAT.level")
        .set_from("snemo::reconstruction::sultan_then_cat_driver")
        .set_terse_description("Verbosity level")
        .set_traits(datatools::TYPE_STRING)
        .set_mandatory(false)
        // .set_long_description("Default value: \"normal\"")
        .set_default_value_string("normal")
        .add_example(
            "Use normal verbosity:: \n"
            "                                  \n"
            "  CAT.level : string = \"normal\" \n"
            "                                  \n");
  }

  {
    // Description of the 'CAT.max_time' configuration property :
    datatools::configuration_property_description& cpd = ocd_.add_property_info();
    cpd.set_name_pattern("CAT.max_time")
        .set_from("snemo::reconstruction::sultan_then_cat_driver")
        .set_terse_description("Maximum processing time")
        .set_traits(datatools::TYPE_REAL)
        .set_mandatory(false)
        // .set_long_description("Default value: 5000 ms")
        .set_default_value_real(5000 * CLHEP::ms, "ms")
        .add_example(
            "Use default value::               \n"
            "                                  \n"
            "  CAT.max_time : real = 5000 ms   \n"
            "                                  \n");
  }

  {
    // Description of the 'CAT.small_radius' configuration property :
    datatools::configuration_property_description& cpd = ocd_.add_property_info();
    cpd.set_name_pattern("CAT.small_radius")
        .set_from("snemo::reconstruction::sultan_then_cat_driver")
        .set_terse_description("Max radius of cells to be not treated as points in distance unit")
        .set_traits(datatools::TYPE_REAL)
        .set_mandatory(false)
        .set_long_description("Default value: 2.0 mm")
        .add_example(
            "Use default value::                \n"
            "                                   \n"
            "  CAT.small_radius : real = 2.0 mm \n"
            "                                   \n");
  }

  {
    // Description of the 'CAT.probmin' configuration property :
    datatools::configuration_property_description& cpd = ocd_.add_property_info();
    cpd.set_name_pattern("CAT.probmin")
        .set_from("snemo::reconstruction::sultan_then_cat_driver")
        .set_terse_description("Minimal probability away from the straight line")
        .set_traits(datatools::TYPE_REAL)
        .set_mandatory(false)
        .set_long_description("Default value: 0.0")
        .add_example(
            "Use default value::               \n"
            "                                  \n"
            "  CAT.probmin : real = 0.0        \n"
            "                                  \n");
  }

  {
    // Description of the 'CAT.nofflayers' configuration property :
    datatools::configuration_property_description& cpd = ocd_.add_property_info();
    cpd.set_name_pattern("CAT.nofflayers")
        .set_from("snemo::reconstruction::sultan_then_cat_driver")
        .set_terse_description(
            "Number of cells which can be skipped (because the cell did not work) and still the "
            "cluster is continuous")
        .set_traits(datatools::TYPE_INTEGER)
        .set_mandatory(false)
        .set_long_description("Default value: 1")
        .add_example(
            "Use default value::               \n"
            "                                  \n"
            "  CAT.nofflayers : integer = 1    \n"
            "                                  \n");
  }

  {
    // Description of the 'CAT.first_event' configuration property :
    datatools::configuration_property_description& cpd = ocd_.add_property_info();
    cpd.set_name_pattern("CAT.first_event")
        .set_from("snemo::reconstruction::sultan_then_cat_driver")
        .set_terse_description("First event to be processed")
        .set_traits(datatools::TYPE_INTEGER)
        .set_mandatory(false)
        .set_long_description("Default value: -1")
        .add_example(
            "Do not specify any first event::  \n"
            "                                  \n"
            "  CAT.first_event : integer = -1  \n"
            "                                  \n");
  }

  {
    // Description of the 'CAT.ratio' configuration property :
    datatools::configuration_property_description& cpd = ocd_.add_property_info();
    cpd.set_name_pattern("CAT.ratio")
        .set_from("snemo::reconstruction::sultan_then_cat_driver")
        .set_terse_description("Ratio of 2nd best to best chi2 which is acceptable as 2nd solution")
        .set_traits(datatools::TYPE_REAL)
        .set_mandatory(false)
        .set_long_description("Default value: 10000.0")
        .add_example(
            "Use the default value::          \n"
            "                                 \n"
            "  CAT.ratio : real = 10000.0     \n"
            "                                 \n");
  }

  {
    // Description of the 'CAT.driver.sigma_z_factor' configuration property :
    datatools::configuration_property_description& cpd = ocd_.add_property_info();
    cpd.set_name_pattern("CAT.sigma_z_factor")
        .set_from("snemo::reconstruction::sultan_then_cat_driver")
        .set_terse_description("Sigma Z factor")
        .set_traits(datatools::TYPE_REAL)
        .set_mandatory(false)
        .set_long_description("Default value: 1.0")
        .add_example(
            "Use the default value::                  \n"
            "                                         \n"
            "  CAT.sigma_z_factor : real = 1.0        \n"
            "                                         \n");
  }
}

}  // end of namespace reconstruction

}  // end of namespace snemo

/* OCD support */
#include <datatools/object_configuration_description.h>
DOCD_CLASS_IMPLEMENT_LOAD_BEGIN(snemo::reconstruction::sultan_then_cat_driver, ocd_) {
  ocd_.set_class_name("snemo::reconstruction::sultan_then_cat_driver");
  ocd_.set_class_description("A driver class for the SULTAN THEN CAT algorithm");
  ocd_.set_class_library("Falaise_CAT");
  ocd_.set_class_documentation("This driver manager for the SULTAN THEN CAT clustering algorithm.");

  // Invoke specific OCD support :
  ::snemo::reconstruction::sultan_then_cat_driver::init_ocd(ocd_);

  ocd_.set_validation_support(true);
  ocd_.lock();
  return;
}
DOCD_CLASS_IMPLEMENT_LOAD_END()  // Closing macro for implementation
DOCD_CLASS_SYSTEM_REGISTRATION(snemo::reconstruction::sultan_then_cat_driver,
                               "snemo::reconstruction::sultan_then_cat_driver")

/* end of sultan_then_cat_driver.cc */
