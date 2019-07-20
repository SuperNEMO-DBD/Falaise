# - CMake directives to compile CellularAutomatonTracker source code:

include_directories(CAT/CellularAutomatonTracker)

# - Prepare CAT & Co
set(CellularAutomatonTracker_VERSION "0.3.4")

option(CAT_MINIMAL_BUILD "Minimal build for CAT" ON)
set(CAT_WITH_DEVEL_UTILS      0)
set(CAT_WITH_DEVEL_DISPLAY    0)
set(CAT_WITH_DEVEL_HISTOGRAMS 0)
set(CAT_WITH_DEVEL_ROOT       0)
if (NOT CAT_MINIMAL_BUILD)
  set(CAT_WITH_DEVEL_UTILS      1)
  set(CAT_WITH_DEVEL_DISPLAY    1)
  set(CAT_WITH_DEVEL_HISTOGRAMS 1)
  set(CAT_WITH_DEVEL_ROOT       1)
endif()

option(SULTAN_MINIMAL_BUILD "Minimal build for SULTAN" ON)
set(SULTAN_WITH_DEVEL_UTILS      0)
set(SULTAN_WITH_DEVEL_DISPLAY    0)
set(SULTAN_WITH_DEVEL_HISTOGRAMS 0)
set(SULTAN_WITH_DEVEL_ROOT       0)
if(NOT SULTAN_MINIMAL_BUILD)
  set(SULTAN_WITH_DEVEL_UTILS      1)
  set(SULTAN_WITH_DEVEL_DISPLAY    1)
  set(SULTAN_WITH_DEVEL_HISTOGRAMS 1)
  set(SULTAN_WITH_DEVEL_ROOT       1)
endif()

############################################################################################
# - mybhep
set(_mybhep_HEADERS
  CAT/CellularAutomatonTracker/mybhep/store.h
  CAT/CellularAutomatonTracker/mybhep/particle_definition.h
  CAT/CellularAutomatonTracker/mybhep/container_algorithm.h
  CAT/CellularAutomatonTracker/mybhep/sparticle.h
  CAT/CellularAutomatonTracker/mybhep/hit_cvt.h
  CAT/CellularAutomatonTracker/mybhep/mybhep_svc.h
  # CellularAutomatonTracker/CellularAutomatonTracker/mybhep/control_panel.h
  CAT/CellularAutomatonTracker/mybhep/particle.h
  CAT/CellularAutomatonTracker/mybhep/utilities.h
  CAT/CellularAutomatonTracker/mybhep/sparticle_cvt.h
  CAT/CellularAutomatonTracker/mybhep/error.h
  CAT/CellularAutomatonTracker/mybhep/base_reader.h
  CAT/CellularAutomatonTracker/mybhep/dvector.h
  CAT/CellularAutomatonTracker/mybhep/writer_hdf5.h
  CAT/CellularAutomatonTracker/mybhep/system_of_units.h
  CAT/CellularAutomatonTracker/mybhep/reader_txt.h
  CAT/CellularAutomatonTracker/mybhep/point.h
  CAT/CellularAutomatonTracker/mybhep/sequential_writer.h
  CAT/CellularAutomatonTracker/mybhep/iconverter.h
  # CAT/CellularAutomatonTracker/mybhep/sreader.h
  CAT/CellularAutomatonTracker/mybhep/ibconverter.h
  CAT/CellularAutomatonTracker/mybhep/hit.h
  CAT/CellularAutomatonTracker/mybhep/event.h
  CAT/CellularAutomatonTracker/mybhep/tstore.h
  CAT/CellularAutomatonTracker/mybhep/brw.h
  CAT/CellularAutomatonTracker/mybhep/reader_hdf5.h
  CAT/CellularAutomatonTracker/mybhep/EventManager2.h
  CAT/CellularAutomatonTracker/mybhep/mparticle.h
  # CAT/CellularAutomatonTracker/mybhep/axis.h
  # CAT/CellularAutomatonTracker/mybhep/histogram.h
  CAT/CellularAutomatonTracker/mybhep/clhep.h
  CAT/CellularAutomatonTracker/mybhep/random_reader.h
  CAT/CellularAutomatonTracker/mybhep/random_writer.h
  CAT/CellularAutomatonTracker/mybhep/ray.h
  CAT/CellularAutomatonTracker/mybhep/converter_svc.h
  CAT/CellularAutomatonTracker/mybhep/track.h
  CAT/CellularAutomatonTracker/mybhep/gstore.h
  CAT/CellularAutomatonTracker/mybhep/bprint.h
  CAT/CellularAutomatonTracker/mybhep/writer_gz.h
  # CAT/CellularAutomatonTracker/mybhep/event_svc.h
  CAT/CellularAutomatonTracker/mybhep/bproperties.h
  CAT/CellularAutomatonTracker/mybhep/particle_cvt.h
  CAT/CellularAutomatonTracker/mybhep/messenger.h
  CAT/CellularAutomatonTracker/mybhep/track_cvt.h
  CAT/CellularAutomatonTracker/mybhep/material.h
  CAT/CellularAutomatonTracker/mybhep/writer_txt.h
  CAT/CellularAutomatonTracker/mybhep/event_cvt.h
  # CAT/CellularAutomatonTracker/mybhep/engine.h
  CAT/CellularAutomatonTracker/mybhep/sequential_reader.h
  CAT/CellularAutomatonTracker/mybhep/mparticle_cvt.h
  CAT/CellularAutomatonTracker/mybhep/reader_gz.h
  CAT/CellularAutomatonTracker/mybhep/EventManager.h
  )

set(_mybhep_SOURCES
  CAT/CellularAutomatonTracker/mybhep/brw.cpp
  CAT/CellularAutomatonTracker/mybhep/sparticle.cpp
  CAT/CellularAutomatonTracker/mybhep/sequential_reader.cpp
  CAT/CellularAutomatonTracker/mybhep/track.cpp
  CAT/CellularAutomatonTracker/mybhep/random_writer.cpp
  CAT/CellularAutomatonTracker/mybhep/mparticle_cvt.cpp
  CAT/CellularAutomatonTracker/mybhep/sequential_writer.cpp
  CAT/CellularAutomatonTracker/mybhep/event.cpp
  CAT/CellularAutomatonTracker/mybhep/event_cvt.cpp
  CAT/CellularAutomatonTracker/mybhep/particle_cvt.cpp
  CAT/CellularAutomatonTracker/mybhep/reader_gz.cpp
  CAT/CellularAutomatonTracker/mybhep/particle_definition.cpp
  # CAT/CellularAutomatonTracker/mybhep/histogram.cpp
  CAT/CellularAutomatonTracker/mybhep/writer_txt.cpp
  CAT/CellularAutomatonTracker/mybhep/hit.cpp
  CAT/CellularAutomatonTracker/mybhep/ray.cpp
  CAT/CellularAutomatonTracker/mybhep/sparticle_cvt.cpp
  CAT/CellularAutomatonTracker/mybhep/track_cvt.cpp
  CAT/CellularAutomatonTracker/mybhep/reader_txt.cpp
  CAT/CellularAutomatonTracker/mybhep/EventManager2.cpp
  CAT/CellularAutomatonTracker/mybhep/material.cpp
  # CAT/CellularAutomatonTracker/mybhep/event_svc.cpp
  CAT/CellularAutomatonTracker/mybhep/converter_svc.cpp
  CAT/CellularAutomatonTracker/mybhep/reader_hdf5.cpp
  CAT/CellularAutomatonTracker/mybhep/base_reader.cpp
  CAT/CellularAutomatonTracker/mybhep/writer_gz.cpp
  CAT/CellularAutomatonTracker/mybhep/mybhep_svc.cpp
  CAT/CellularAutomatonTracker/mybhep/particle.cpp
  CAT/CellularAutomatonTracker/mybhep/hit_cvt.cpp
  CAT/CellularAutomatonTracker/mybhep/random_reader.cpp
  # CAT/CellularAutomatonTracker/mybhep/axis.cpp
  CAT/CellularAutomatonTracker/mybhep/writer_hdf5.cpp
  CAT/CellularAutomatonTracker/mybhep/EventManager.cpp
  )

list(APPEND FalaiseCATPlugin_HEADERS ${_mybhep_HEADERS})
list(APPEND FalaiseCATPlugin_SOURCES ${_mybhep_SOURCES})


if(CAT_WITH_DEVEL_UTILS)
  ############################################################################################
  # - CATUtils
  set(_CATUtils_HEADERS
    CAT/CellularAutomatonTracker/CATUtils/clockable.h
    CAT/CellularAutomatonTracker/CATUtils/ntupler.h
    CAT/CellularAutomatonTracker/CATUtils/Clock.h
    CAT/CellularAutomatonTracker/CATUtils/NHistogramFunctions2.h
    CAT/CellularAutomatonTracker/CATUtils/EventDisplay.h
    CAT/CellularAutomatonTracker/CATUtils/ROOTbase.h
    CAT/CellularAutomatonTracker/CATUtils/NHistoManager2.h
    )

  set(_CATUtils_SOURCES
    CAT/CellularAutomatonTracker/CATUtils/NHistogramFunctions2.cpp
    CAT/CellularAutomatonTracker/CATUtils/ntupler.cpp
    CAT/CellularAutomatonTracker/CATUtils/NHistoManager2.cpp
    CAT/CellularAutomatonTracker/CATUtils/EventDisplay.cpp
    )

  list(APPEND FalaiseCATPlugin_HEADERS ${_CATUtils_HEADERS})
  list(APPEND FalaiseCATPlugin_SOURCES ${_CATUtils_SOURCES})

endif()


############################################################################################
# - CATAlgorithm
set(_CATAlgorithm_HEADERS
  CAT/CellularAutomatonTracker/CATAlgorithm/circle_base.h
  CAT/CellularAutomatonTracker/CATAlgorithm/sequence_base.h
  CAT/CellularAutomatonTracker/CATAlgorithm/calorimeter_hit.h
  CAT/CellularAutomatonTracker/CATAlgorithm/clockable.h
  CAT/CellularAutomatonTracker/CATAlgorithm/sequentiator.h
  CAT/CellularAutomatonTracker/CATAlgorithm/Detector.h
  CAT/CellularAutomatonTracker/CATAlgorithm/cell_base.h
  CAT/CellularAutomatonTracker/CATAlgorithm/lt_utils.h
  CAT/CellularAutomatonTracker/CATAlgorithm/Sequence.h
  CAT/CellularAutomatonTracker/CATAlgorithm/Sultan.h
  CAT/CellularAutomatonTracker/CATAlgorithm/CAT_interface.h
  CAT/CellularAutomatonTracker/CATAlgorithm/node.h
  CAT/CellularAutomatonTracker/CATAlgorithm/experimental_double.h
  CAT/CellularAutomatonTracker/CATAlgorithm/i_predicate.h
  CAT/CellularAutomatonTracker/CATAlgorithm/scenario.h
  CAT/CellularAutomatonTracker/CATAlgorithm/Clock.h
  CAT/CellularAutomatonTracker/CATAlgorithm/clusterizer.h
  CAT/CellularAutomatonTracker/CATAlgorithm/cell_triplet.h
  CAT/CellularAutomatonTracker/CATAlgorithm/cluster.h
  CAT/CellularAutomatonTracker/CATAlgorithm/SULTAN_interface.h
  CAT/CellularAutomatonTracker/CATAlgorithm/helix.h
  CAT/CellularAutomatonTracker/CATAlgorithm/LinearRegression.h
  CAT/CellularAutomatonTracker/CATAlgorithm/cylinder.h
  CAT/CellularAutomatonTracker/CATAlgorithm/joint.h
  CAT/CellularAutomatonTracker/CATAlgorithm/printable.h
  CAT/CellularAutomatonTracker/CATAlgorithm/experimental_vector.h
  CAT/CellularAutomatonTracker/CATAlgorithm/tracked_data_base.h
  CAT/CellularAutomatonTracker/CATAlgorithm/logic_sequence.h
  CAT/CellularAutomatonTracker/CATAlgorithm/Circle.h
  CAT/CellularAutomatonTracker/CATAlgorithm/logic_cell.h
  CAT/CellularAutomatonTracker/CATAlgorithm/handle.h
  CAT/CellularAutomatonTracker/CATAlgorithm/cell_couplet.h
  CAT/CellularAutomatonTracker/CATAlgorithm/tracking_object.h
  CAT/CellularAutomatonTracker/CATAlgorithm/logic_scenario.h
  CAT/CellularAutomatonTracker/CATAlgorithm/Legendre_function.h
  CAT/CellularAutomatonTracker/CATAlgorithm/plane.h
  CAT/CellularAutomatonTracker/CATAlgorithm/experimental_point.h
  CAT/CellularAutomatonTracker/CATAlgorithm/line.h
  CAT/CellularAutomatonTracker/CATAlgorithm/Cell.h
  CAT/CellularAutomatonTracker/CATAlgorithm/CircleRegression.h
  CAT/CellularAutomatonTracker/CATAlgorithm/Tracked_data.h
  CAT/CellularAutomatonTracker/CATAlgorithm/broken_line.h
  )

set(_CATAlgorithm_SOURCES
  CAT/CellularAutomatonTracker/CATAlgorithm/cluster.cpp
  CAT/CellularAutomatonTracker/CATAlgorithm/Legendre_function.cpp
  CAT/CellularAutomatonTracker/CATAlgorithm/line.cpp
  CAT/CellularAutomatonTracker/CATAlgorithm/experimental_point.cpp
  CAT/CellularAutomatonTracker/CATAlgorithm/sequentiator.cpp
  CAT/CellularAutomatonTracker/CATAlgorithm/Detector.cpp
  CAT/CellularAutomatonTracker/CATAlgorithm/clusterizer.cpp
  CAT/CellularAutomatonTracker/CATAlgorithm/Cell.cpp
  CAT/CellularAutomatonTracker/CATAlgorithm/sequence_base.cpp
  CAT/CellularAutomatonTracker/CATAlgorithm/node.cpp
  CAT/CellularAutomatonTracker/CATAlgorithm/joint.cpp
  CAT/CellularAutomatonTracker/CATAlgorithm/LinearRegression.cpp
  CAT/CellularAutomatonTracker/CATAlgorithm/circle_base.cpp
  CAT/CellularAutomatonTracker/CATAlgorithm/cell_triplet.cpp
  CAT/CellularAutomatonTracker/CATAlgorithm/calorimeter_hit.cpp
  CAT/CellularAutomatonTracker/CATAlgorithm/mparticle.cpp
  CAT/CellularAutomatonTracker/CATAlgorithm/experimental_double.cpp
  CAT/CellularAutomatonTracker/CATAlgorithm/CAT_interface.cpp
  CAT/CellularAutomatonTracker/CATAlgorithm/tracking_object.cpp
  CAT/CellularAutomatonTracker/CATAlgorithm/Circle.cpp
  CAT/CellularAutomatonTracker/CATAlgorithm/SULTAN_interface.cpp
  CAT/CellularAutomatonTracker/CATAlgorithm/experimental_vector.cpp
  CAT/CellularAutomatonTracker/CATAlgorithm/cell_couplet.cpp
  CAT/CellularAutomatonTracker/CATAlgorithm/CircleRegression.cpp
  CAT/CellularAutomatonTracker/CATAlgorithm/clockable.cpp
  CAT/CellularAutomatonTracker/CATAlgorithm/Sultan.cpp
  CAT/CellularAutomatonTracker/CATAlgorithm/printable.cpp
  CAT/CellularAutomatonTracker/CATAlgorithm/plane.cpp
  CAT/CellularAutomatonTracker/CATAlgorithm/scenario.cpp
  CAT/CellularAutomatonTracker/CATAlgorithm/Clock.cpp
  CAT/CellularAutomatonTracker/CATAlgorithm/cell_base.cpp
  CAT/CellularAutomatonTracker/CATAlgorithm/broken_line.cpp
  )
list(APPEND FalaiseCATPlugin_HEADERS ${_CATAlgorithm_HEADERS})
list(APPEND FalaiseCATPlugin_SOURCES ${_CATAlgorithm_SOURCES})

configure_file(CAT/CellularAutomatonTracker/CATAlgorithm/CAT_config.h.in
  ${CMAKE_CURRENT_BINARY_DIR}/CATAlgorithm/CAT_config.h
  @ONLY)

configure_file(CAT/CATAlgorithm/SULTAN_config.h.in
  ${CMAKE_CURRENT_BINARY_DIR}/CATAlgorithm/SULTAN_config.h
  @ONLY)

configure_file(CAT/CellularAutomatonTracker/sultan/SULTAN_config.h.in
  ${CMAKE_CURRENT_BINARY_DIR}/sultan/SULTAN_config.h
  @ONLY)

############################################################################################
# - Sultan
set(_sultan_HEADERS
  CAT/CellularAutomatonTracker/sultan/sequence.h
  CAT/CellularAutomatonTracker/sultan/clockable.h
  CAT/CellularAutomatonTracker/sultan/cell.h
  CAT/CellularAutomatonTracker/sultan/node.h
  CAT/CellularAutomatonTracker/sultan/experimental_double.h
  CAT/CellularAutomatonTracker/sultan/scenario.h
  CAT/CellularAutomatonTracker/sultan/Clock.h
  CAT/CellularAutomatonTracker/sultan/clusterizer.h
  CAT/CellularAutomatonTracker/sultan/cell_triplet.h
  CAT/CellularAutomatonTracker/sultan/cluster.h
  CAT/CellularAutomatonTracker/sultan/SULTAN_interface.h
  CAT/CellularAutomatonTracker/sultan/cluster_of_experimental_helices.h
  CAT/CellularAutomatonTracker/sultan/printable.h
  CAT/CellularAutomatonTracker/sultan/experimental_vector.h
  CAT/CellularAutomatonTracker/sultan/tracked_data.h
  CAT/CellularAutomatonTracker/sultan/sultan.h
  CAT/CellularAutomatonTracker/sultan/experimental_helix.h
  CAT/CellularAutomatonTracker/sultan/cell_couplet.h
  CAT/CellularAutomatonTracker/sultan/experimental_legendre_vector.h
  CAT/CellularAutomatonTracker/sultan/tracking_object.h
  CAT/CellularAutomatonTracker/sultan/experimental_point.h
  CAT/CellularAutomatonTracker/sultan/calorimeter_hit.h
  CAT/CellularAutomatonTracker/sultan/experimental_line.h
  CAT/CellularAutomatonTracker/sultan/plane.h
  CAT/CellularAutomatonTracker/sultan/LinearRegression.h
  )

set(_sultan_SOURCES
  CAT/CellularAutomatonTracker/sultan/cluster.cpp
  CAT/CellularAutomatonTracker/sultan/experimental_point.cpp
  CAT/CellularAutomatonTracker/sultan/experimental_legendre_vector.cpp
  CAT/CellularAutomatonTracker/sultan/clusterizer.cpp
  CAT/CellularAutomatonTracker/sultan/sultan.cpp
  CAT/CellularAutomatonTracker/sultan/sequence.cpp
  CAT/CellularAutomatonTracker/sultan/node.cpp
  CAT/CellularAutomatonTracker/sultan/experimental_helix.cpp
  CAT/CellularAutomatonTracker/sultan/cell_triplet.cpp
  CAT/CellularAutomatonTracker/sultan/experimental_double.cpp
  CAT/CellularAutomatonTracker/sultan/tracking_object.cpp
  CAT/CellularAutomatonTracker/sultan/SULTAN_interface.cpp
  CAT/CellularAutomatonTracker/sultan/experimental_vector.cpp
  CAT/CellularAutomatonTracker/sultan/cell_couplet.cpp
  CAT/CellularAutomatonTracker/sultan/cluster_of_experimental_helices.cpp
  CAT/CellularAutomatonTracker/sultan/clockable.cpp
  CAT/CellularAutomatonTracker/sultan/printable.cpp
  CAT/CellularAutomatonTracker/sultan/scenario.cpp
  CAT/CellularAutomatonTracker/sultan/Clock.cpp
  CAT/CellularAutomatonTracker/sultan/cell.cpp
  CAT/CellularAutomatonTracker/sultan/calorimeter_hit.cpp
  CAT/CellularAutomatonTracker/sultan/experimental_line.cpp
  CAT/CellularAutomatonTracker/sultan/plane.cpp
  CAT/CellularAutomatonTracker/sultan/LinearRegression.cpp
  )
list(APPEND FalaiseCATPlugin_HEADERS ${_sultan_HEADERS})
list(APPEND FalaiseCATPlugin_SOURCES ${_sultan_SOURCES})

#
