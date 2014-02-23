# - CMake directives to compile CellularAutomatonTracker source code:

include_directories(CellularAutomatonTracker/CellularAutomatonTracker)

# - Prepare CAT & Co
set(CellularAutomatonTracker_VERSION "0.3.4")

option ( CAT_MINIMAL_BUILD "Minimal build for CAT" ON)
set ( CAT_WITH_DEVEL_UTILS      0 )
set ( CAT_WITH_DEVEL_DISPLAY    0 )
set ( CAT_WITH_DEVEL_HISTOGRAMS 0 )
set ( CAT_WITH_DEVEL_ROOT       0 )
if (NOT CAT_MINIMAL_BUILD)
  set ( CAT_WITH_DEVEL_UTILS      1 )
  set ( CAT_WITH_DEVEL_DISPLAY    1 )
  set ( CAT_WITH_DEVEL_HISTOGRAMS 1 )
  set ( CAT_WITH_DEVEL_ROOT       1 )
endif ()

option ( SULTAN_MINIMAL_BUILD "Minimal build for SULTAN" ON)
set ( SULTAN_WITH_DEVEL_UTILS      0 )
set ( SULTAN_WITH_DEVEL_DISPLAY    0 )
set ( SULTAN_WITH_DEVEL_HISTOGRAMS 0 )
set ( SULTAN_WITH_DEVEL_ROOT       0 )
if (NOT SULTAN_MINIMAL_BUILD)
  set ( SULTAN_WITH_DEVEL_UTILS      1 )
  set ( SULTAN_WITH_DEVEL_DISPLAY    1 )
  set ( SULTAN_WITH_DEVEL_HISTOGRAMS 1 )
  set ( SULTAN_WITH_DEVEL_ROOT       1 )
endif ()

############################################################################################
# - mybhep
set(_mybhep_HEADERS
  CellularAutomatonTracker/CellularAutomatonTracker/mybhep/store.h
  CellularAutomatonTracker/CellularAutomatonTracker/mybhep/particle_definition.h
  CellularAutomatonTracker/CellularAutomatonTracker/mybhep/container_algorithm.h
  CellularAutomatonTracker/CellularAutomatonTracker/mybhep/sparticle.h
  CellularAutomatonTracker/CellularAutomatonTracker/mybhep/hit_cvt.h
  CellularAutomatonTracker/CellularAutomatonTracker/mybhep/mybhep_svc.h
  # CellularAutomatonTracker/CellularAutomatonTracker/mybhep/control_panel.h
  CellularAutomatonTracker/CellularAutomatonTracker/mybhep/particle.h
  CellularAutomatonTracker/CellularAutomatonTracker/mybhep/utilities.h
  CellularAutomatonTracker/CellularAutomatonTracker/mybhep/sparticle_cvt.h
  CellularAutomatonTracker/CellularAutomatonTracker/mybhep/error.h
  CellularAutomatonTracker/CellularAutomatonTracker/mybhep/base_reader.h
  CellularAutomatonTracker/CellularAutomatonTracker/mybhep/dvector.h
  CellularAutomatonTracker/CellularAutomatonTracker/mybhep/writer_hdf5.h
  CellularAutomatonTracker/CellularAutomatonTracker/mybhep/system_of_units.h
  CellularAutomatonTracker/CellularAutomatonTracker/mybhep/reader_txt.h
  CellularAutomatonTracker/CellularAutomatonTracker/mybhep/point.h
  CellularAutomatonTracker/CellularAutomatonTracker/mybhep/sequential_writer.h
  CellularAutomatonTracker/CellularAutomatonTracker/mybhep/iconverter.h
  # CellularAutomatonTracker/CellularAutomatonTracker/mybhep/sreader.h
  CellularAutomatonTracker/CellularAutomatonTracker/mybhep/ibconverter.h
  CellularAutomatonTracker/CellularAutomatonTracker/mybhep/hit.h
  CellularAutomatonTracker/CellularAutomatonTracker/mybhep/event.h
  CellularAutomatonTracker/CellularAutomatonTracker/mybhep/tstore.h
  CellularAutomatonTracker/CellularAutomatonTracker/mybhep/brw.h
  CellularAutomatonTracker/CellularAutomatonTracker/mybhep/reader_hdf5.h
  CellularAutomatonTracker/CellularAutomatonTracker/mybhep/EventManager2.h
  CellularAutomatonTracker/CellularAutomatonTracker/mybhep/mparticle.h
  # CellularAutomatonTracker/CellularAutomatonTracker/mybhep/axis.h
  # CellularAutomatonTracker/CellularAutomatonTracker/mybhep/histogram.h
  CellularAutomatonTracker/CellularAutomatonTracker/mybhep/clhep.h
  CellularAutomatonTracker/CellularAutomatonTracker/mybhep/random_reader.h
  CellularAutomatonTracker/CellularAutomatonTracker/mybhep/random_writer.h
  CellularAutomatonTracker/CellularAutomatonTracker/mybhep/ray.h
  CellularAutomatonTracker/CellularAutomatonTracker/mybhep/converter_svc.h
  CellularAutomatonTracker/CellularAutomatonTracker/mybhep/track.h
  CellularAutomatonTracker/CellularAutomatonTracker/mybhep/gstore.h
  CellularAutomatonTracker/CellularAutomatonTracker/mybhep/bprint.h
  CellularAutomatonTracker/CellularAutomatonTracker/mybhep/writer_gz.h
  # CellularAutomatonTracker/CellularAutomatonTracker/mybhep/event_svc.h
  CellularAutomatonTracker/CellularAutomatonTracker/mybhep/bproperties.h
  CellularAutomatonTracker/CellularAutomatonTracker/mybhep/particle_cvt.h
  CellularAutomatonTracker/CellularAutomatonTracker/mybhep/messenger.h
  CellularAutomatonTracker/CellularAutomatonTracker/mybhep/track_cvt.h
  CellularAutomatonTracker/CellularAutomatonTracker/mybhep/material.h
  CellularAutomatonTracker/CellularAutomatonTracker/mybhep/writer_txt.h
  CellularAutomatonTracker/CellularAutomatonTracker/mybhep/event_cvt.h
  # CellularAutomatonTracker/CellularAutomatonTracker/mybhep/engine.h
  CellularAutomatonTracker/CellularAutomatonTracker/mybhep/sequential_reader.h
  CellularAutomatonTracker/CellularAutomatonTracker/mybhep/mparticle_cvt.h
  CellularAutomatonTracker/CellularAutomatonTracker/mybhep/reader_gz.h
  CellularAutomatonTracker/CellularAutomatonTracker/mybhep/EventManager.h
  )

set(_mybhep_SOURCES
  CellularAutomatonTracker/CellularAutomatonTracker/mybhep/brw.cpp
  CellularAutomatonTracker/CellularAutomatonTracker/mybhep/sparticle.cpp
  CellularAutomatonTracker/CellularAutomatonTracker/mybhep/sequential_reader.cpp
  CellularAutomatonTracker/CellularAutomatonTracker/mybhep/track.cpp
  CellularAutomatonTracker/CellularAutomatonTracker/mybhep/random_writer.cpp
  CellularAutomatonTracker/CellularAutomatonTracker/mybhep/mparticle_cvt.cpp
  CellularAutomatonTracker/CellularAutomatonTracker/mybhep/sequential_writer.cpp
  CellularAutomatonTracker/CellularAutomatonTracker/mybhep/event.cpp
  CellularAutomatonTracker/CellularAutomatonTracker/mybhep/event_cvt.cpp
  CellularAutomatonTracker/CellularAutomatonTracker/mybhep/particle_cvt.cpp
  CellularAutomatonTracker/CellularAutomatonTracker/mybhep/reader_gz.cpp
  CellularAutomatonTracker/CellularAutomatonTracker/mybhep/particle_definition.cpp
  # CellularAutomatonTracker/CellularAutomatonTracker/mybhep/histogram.cpp
  CellularAutomatonTracker/CellularAutomatonTracker/mybhep/writer_txt.cpp
  CellularAutomatonTracker/CellularAutomatonTracker/mybhep/hit.cpp
  CellularAutomatonTracker/CellularAutomatonTracker/mybhep/ray.cpp
  CellularAutomatonTracker/CellularAutomatonTracker/mybhep/sparticle_cvt.cpp
  CellularAutomatonTracker/CellularAutomatonTracker/mybhep/track_cvt.cpp
  CellularAutomatonTracker/CellularAutomatonTracker/mybhep/reader_txt.cpp
  CellularAutomatonTracker/CellularAutomatonTracker/mybhep/EventManager2.cpp
  CellularAutomatonTracker/CellularAutomatonTracker/mybhep/material.cpp
  # CellularAutomatonTracker/CellularAutomatonTracker/mybhep/event_svc.cpp
  CellularAutomatonTracker/CellularAutomatonTracker/mybhep/converter_svc.cpp
  CellularAutomatonTracker/CellularAutomatonTracker/mybhep/reader_hdf5.cpp
  CellularAutomatonTracker/CellularAutomatonTracker/mybhep/base_reader.cpp
  CellularAutomatonTracker/CellularAutomatonTracker/mybhep/writer_gz.cpp
  CellularAutomatonTracker/CellularAutomatonTracker/mybhep/mybhep_svc.cpp
  CellularAutomatonTracker/CellularAutomatonTracker/mybhep/particle.cpp
  CellularAutomatonTracker/CellularAutomatonTracker/mybhep/hit_cvt.cpp
  CellularAutomatonTracker/CellularAutomatonTracker/mybhep/random_reader.cpp
  # CellularAutomatonTracker/CellularAutomatonTracker/mybhep/axis.cpp
  CellularAutomatonTracker/CellularAutomatonTracker/mybhep/writer_hdf5.cpp
  CellularAutomatonTracker/CellularAutomatonTracker/mybhep/EventManager.cpp
  )

list(APPEND FalaiseCATPlugin_HEADERS ${_mybhep_HEADERS})
list(APPEND FalaiseCATPlugin_SOURCES ${_mybhep_SOURCES})


if(CAT_WITH_DEVEL_UTILS)
  ############################################################################################
  # - CATUtils
  set(_CATUtils_HEADERS
    CellularAutomatonTracker/CellularAutomatonTracker/CATUtils/clockable.h
    CellularAutomatonTracker/CellularAutomatonTracker/CATUtils/ntupler.h
    CellularAutomatonTracker/CellularAutomatonTracker/CATUtils/Clock.h
    CellularAutomatonTracker/CellularAutomatonTracker/CATUtils/NHistogramFunctions2.h
    CellularAutomatonTracker/CellularAutomatonTracker/CATUtils/EventDisplay.h
    CellularAutomatonTracker/CellularAutomatonTracker/CATUtils/ROOTbase.h
    CellularAutomatonTracker/CellularAutomatonTracker/CATUtils/NHistoManager2.h
    )

  set(_CATUtils_SOURCES
    CellularAutomatonTracker/CellularAutomatonTracker/CATUtils/NHistogramFunctions2.cpp
    CellularAutomatonTracker/CellularAutomatonTracker/CATUtils/ntupler.cpp
    CellularAutomatonTracker/CellularAutomatonTracker/CATUtils/NHistoManager2.cpp
    CellularAutomatonTracker/CellularAutomatonTracker/CATUtils/EventDisplay.cpp
    )

  list(APPEND FalaiseCATPlugin_HEADERS ${_CATUtils_HEADERS})
  list(APPEND FalaiseCATPlugin_SOURCES ${_CATUtils_SOURCES})

endif()


############################################################################################
# - CATAlgorithm
set(_CATAlgorithm_HEADERS
  CellularAutomatonTracker/CellularAutomatonTracker/CATAlgorithm/circle.h
  CellularAutomatonTracker/CellularAutomatonTracker/CATAlgorithm/sequence.h
  CellularAutomatonTracker/CellularAutomatonTracker/CATAlgorithm/calorimeter_hit.h
  CellularAutomatonTracker/CellularAutomatonTracker/CATAlgorithm/clockable.h
  CellularAutomatonTracker/CellularAutomatonTracker/CATAlgorithm/sequentiator.h
  CellularAutomatonTracker/CellularAutomatonTracker/CATAlgorithm/Detector.h
  CellularAutomatonTracker/CellularAutomatonTracker/CATAlgorithm/cell.h
  CellularAutomatonTracker/CellularAutomatonTracker/CATAlgorithm/lt_utils.h
  CellularAutomatonTracker/CellularAutomatonTracker/CATAlgorithm/Sequence.h
  CellularAutomatonTracker/CellularAutomatonTracker/CATAlgorithm/Sultan.h
  CellularAutomatonTracker/CellularAutomatonTracker/CATAlgorithm/CAT_interface.h
  CellularAutomatonTracker/CellularAutomatonTracker/CATAlgorithm/node.h
  CellularAutomatonTracker/CellularAutomatonTracker/CATAlgorithm/experimental_double.h
  CellularAutomatonTracker/CellularAutomatonTracker/CATAlgorithm/i_predicate.h
  CellularAutomatonTracker/CellularAutomatonTracker/CATAlgorithm/scenario.h
  CellularAutomatonTracker/CellularAutomatonTracker/CATAlgorithm/Clock.h
  CellularAutomatonTracker/CellularAutomatonTracker/CATAlgorithm/clusterizer.h
  CellularAutomatonTracker/CellularAutomatonTracker/CATAlgorithm/cell_triplet.h
  CellularAutomatonTracker/CellularAutomatonTracker/CATAlgorithm/cluster.h
  CellularAutomatonTracker/CellularAutomatonTracker/CATAlgorithm/SULTAN_interface.h
  CellularAutomatonTracker/CellularAutomatonTracker/CATAlgorithm/helix.h
  CellularAutomatonTracker/CellularAutomatonTracker/CATAlgorithm/LinearRegression.h
  CellularAutomatonTracker/CellularAutomatonTracker/CATAlgorithm/cylinder.h
  CellularAutomatonTracker/CellularAutomatonTracker/CATAlgorithm/joint.h
  CellularAutomatonTracker/CellularAutomatonTracker/CATAlgorithm/printable.h
  CellularAutomatonTracker/CellularAutomatonTracker/CATAlgorithm/experimental_vector.h
  CellularAutomatonTracker/CellularAutomatonTracker/CATAlgorithm/tracked_data.h
  CellularAutomatonTracker/CellularAutomatonTracker/CATAlgorithm/logic_sequence.h
  CellularAutomatonTracker/CellularAutomatonTracker/CATAlgorithm/Circle.h
  CellularAutomatonTracker/CellularAutomatonTracker/CATAlgorithm/logic_cell.h
  CellularAutomatonTracker/CellularAutomatonTracker/CATAlgorithm/handle.h
  CellularAutomatonTracker/CellularAutomatonTracker/CATAlgorithm/cell_couplet.h
  CellularAutomatonTracker/CellularAutomatonTracker/CATAlgorithm/tracking_object.h
  CellularAutomatonTracker/CellularAutomatonTracker/CATAlgorithm/logic_scenario.h
  CellularAutomatonTracker/CellularAutomatonTracker/CATAlgorithm/Legendre_function.h
  CellularAutomatonTracker/CellularAutomatonTracker/CATAlgorithm/plane.h
  CellularAutomatonTracker/CellularAutomatonTracker/CATAlgorithm/experimental_point.h
  CellularAutomatonTracker/CellularAutomatonTracker/CATAlgorithm/line.h
  CellularAutomatonTracker/CellularAutomatonTracker/CATAlgorithm/Cell.h
  CellularAutomatonTracker/CellularAutomatonTracker/CATAlgorithm/CircleRegression.h
  CellularAutomatonTracker/CellularAutomatonTracker/CATAlgorithm/Tracked_data.h
  )

set(_CATAlgorithm_SOURCES
  CellularAutomatonTracker/CellularAutomatonTracker/CATAlgorithm/cluster.cpp
  CellularAutomatonTracker/CellularAutomatonTracker/CATAlgorithm/Legendre_function.cpp
  CellularAutomatonTracker/CellularAutomatonTracker/CATAlgorithm/line.cpp
  CellularAutomatonTracker/CellularAutomatonTracker/CATAlgorithm/experimental_point.cpp
  CellularAutomatonTracker/CellularAutomatonTracker/CATAlgorithm/sequentiator.cpp
  CellularAutomatonTracker/CellularAutomatonTracker/CATAlgorithm/Detector.cpp
  CellularAutomatonTracker/CellularAutomatonTracker/CATAlgorithm/clusterizer.cpp
  CellularAutomatonTracker/CellularAutomatonTracker/CATAlgorithm/Cell.cpp
  CellularAutomatonTracker/CellularAutomatonTracker/CATAlgorithm/sequence.cpp
  CellularAutomatonTracker/CellularAutomatonTracker/CATAlgorithm/node.cpp
  CellularAutomatonTracker/CellularAutomatonTracker/CATAlgorithm/joint.cpp
  CellularAutomatonTracker/CellularAutomatonTracker/CATAlgorithm/LinearRegression.cpp
  CellularAutomatonTracker/CellularAutomatonTracker/CATAlgorithm/circle.cpp
  CellularAutomatonTracker/CellularAutomatonTracker/CATAlgorithm/cell_triplet.cpp
  CellularAutomatonTracker/CellularAutomatonTracker/CATAlgorithm/calorimeter_hit.cpp
  CellularAutomatonTracker/CellularAutomatonTracker/CATAlgorithm/mparticle.cpp
  CellularAutomatonTracker/CellularAutomatonTracker/CATAlgorithm/experimental_double.cpp
  CellularAutomatonTracker/CellularAutomatonTracker/CATAlgorithm/CAT_interface.cpp
  CellularAutomatonTracker/CellularAutomatonTracker/CATAlgorithm/tracking_object.cpp
  CellularAutomatonTracker/CellularAutomatonTracker/CATAlgorithm/Circle.cpp
  CellularAutomatonTracker/CellularAutomatonTracker/CATAlgorithm/SULTAN_interface.cpp
  CellularAutomatonTracker/CellularAutomatonTracker/CATAlgorithm/experimental_vector.cpp
  CellularAutomatonTracker/CellularAutomatonTracker/CATAlgorithm/cell_couplet.cpp
  CellularAutomatonTracker/CellularAutomatonTracker/CATAlgorithm/CircleRegression.cpp
  CellularAutomatonTracker/CellularAutomatonTracker/CATAlgorithm/clockable.cpp
  CellularAutomatonTracker/CellularAutomatonTracker/CATAlgorithm/Sultan.cpp
  CellularAutomatonTracker/CellularAutomatonTracker/CATAlgorithm/printable.cpp
  CellularAutomatonTracker/CellularAutomatonTracker/CATAlgorithm/plane.cpp
  CellularAutomatonTracker/CellularAutomatonTracker/CATAlgorithm/scenario.cpp
  CellularAutomatonTracker/CellularAutomatonTracker/CATAlgorithm/Clock.cpp
  CellularAutomatonTracker/CellularAutomatonTracker/CATAlgorithm/cell.cpp
  )
list(APPEND FalaiseCATPlugin_HEADERS ${_CATAlgorithm_HEADERS})
list(APPEND FalaiseCATPlugin_SOURCES ${_CATAlgorithm_SOURCES})

configure_file(CellularAutomatonTracker/cmake/Templates/CAT_config.h.in
  ${CMAKE_CURRENT_BINARY_DIR}/CATAlgorithm/CAT_config.h
  @ONLY)

configure_file(source/falaise/CATAlgorithm/SULTAN_config.h.in
  ${CMAKE_CURRENT_BINARY_DIR}/CATAlgorithm/SULTAN_config.h
  @ONLY)

configure_file(CellularAutomatonTracker/cmake/Templates/SULTAN_config.h.in
  ${CMAKE_CURRENT_BINARY_DIR}/sultan/SULTAN_config.h
  @ONLY)

############################################################################################
# - Sultan
set(_sultan_HEADERS
  CellularAutomatonTracker/CellularAutomatonTracker/sultan/sequence.h
  CellularAutomatonTracker/CellularAutomatonTracker/sultan/clockable.h
  CellularAutomatonTracker/CellularAutomatonTracker/sultan/cell.h
  CellularAutomatonTracker/CellularAutomatonTracker/sultan/node.h
  CellularAutomatonTracker/CellularAutomatonTracker/sultan/experimental_double.h
  CellularAutomatonTracker/CellularAutomatonTracker/sultan/scenario.h
  CellularAutomatonTracker/CellularAutomatonTracker/sultan/Clock.h
  CellularAutomatonTracker/CellularAutomatonTracker/sultan/clusterizer.h
  CellularAutomatonTracker/CellularAutomatonTracker/sultan/cell_triplet.h
  CellularAutomatonTracker/CellularAutomatonTracker/sultan/cluster.h
  CellularAutomatonTracker/CellularAutomatonTracker/sultan/SULTAN_interface.h
  CellularAutomatonTracker/CellularAutomatonTracker/sultan/cluster_of_experimental_helices.h
  CellularAutomatonTracker/CellularAutomatonTracker/sultan/printable.h
  CellularAutomatonTracker/CellularAutomatonTracker/sultan/experimental_vector.h
  CellularAutomatonTracker/CellularAutomatonTracker/sultan/tracked_data.h
  CellularAutomatonTracker/CellularAutomatonTracker/sultan/sultan.h
  CellularAutomatonTracker/CellularAutomatonTracker/sultan/experimental_helix.h
  CellularAutomatonTracker/CellularAutomatonTracker/sultan/cell_couplet.h
  CellularAutomatonTracker/CellularAutomatonTracker/sultan/experimental_legendre_vector.h
  CellularAutomatonTracker/CellularAutomatonTracker/sultan/tracking_object.h
  CellularAutomatonTracker/CellularAutomatonTracker/sultan/experimental_point.h
  )

set(_sultan_SOURCES
  CellularAutomatonTracker/CellularAutomatonTracker/sultan/cluster.cpp
  CellularAutomatonTracker/CellularAutomatonTracker/sultan/experimental_point.cpp
  CellularAutomatonTracker/CellularAutomatonTracker/sultan/experimental_legendre_vector.cpp
  CellularAutomatonTracker/CellularAutomatonTracker/sultan/clusterizer.cpp
  CellularAutomatonTracker/CellularAutomatonTracker/sultan/sultan.cpp
  CellularAutomatonTracker/CellularAutomatonTracker/sultan/sequence.cpp
  CellularAutomatonTracker/CellularAutomatonTracker/sultan/node.cpp
  CellularAutomatonTracker/CellularAutomatonTracker/sultan/experimental_helix.cpp
  CellularAutomatonTracker/CellularAutomatonTracker/sultan/cell_triplet.cpp
  CellularAutomatonTracker/CellularAutomatonTracker/sultan/experimental_double.cpp
  CellularAutomatonTracker/CellularAutomatonTracker/sultan/tracking_object.cpp
  CellularAutomatonTracker/CellularAutomatonTracker/sultan/SULTAN_interface.cpp
  CellularAutomatonTracker/CellularAutomatonTracker/sultan/experimental_vector.cpp
  CellularAutomatonTracker/CellularAutomatonTracker/sultan/cell_couplet.cpp
  CellularAutomatonTracker/CellularAutomatonTracker/sultan/cluster_of_experimental_helices.cpp
  CellularAutomatonTracker/CellularAutomatonTracker/sultan/clockable.cpp
  CellularAutomatonTracker/CellularAutomatonTracker/sultan/printable.cpp
  CellularAutomatonTracker/CellularAutomatonTracker/sultan/scenario.cpp
  CellularAutomatonTracker/CellularAutomatonTracker/sultan/Clock.cpp
  CellularAutomatonTracker/CellularAutomatonTracker/sultan/cell.cpp
  )
list(APPEND FalaiseCATPlugin_HEADERS ${_sultan_HEADERS})
list(APPEND FalaiseCATPlugin_SOURCES ${_sultan_SOURCES})

#