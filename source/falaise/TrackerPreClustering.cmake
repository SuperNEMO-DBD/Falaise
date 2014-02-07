# TrackerPreClustering specific classes:

list(APPEND FalaiseLibrary_HEADERS
  TrackerPreClustering/interface.h
  TrackerPreClustering/interface.tpp
  TrackerPreClustering/pre_clusterizer.h
  TrackerPreClustering/pre_clusterizer.tpp
  TrackerPreClustering/gg_hit.h
  TrackerPreClustering/event_generator.h
  TrackerPreClustering/event_display.h
  TrackerPreClustering/event_display.tpp
  )


list(APPEND FalaiseLibrary_SOURCES
  TrackerPreClustering/interface.cc
  TrackerPreClustering/pre_clusterizer.cc
  TrackerPreClustering/gg_hit.cc
  TrackerPreClustering/event_generator.cc
  TrackerPreClustering/event_display.cc
  )

list(APPEND FalaiseLibrary_TESTS
  TrackerPreClustering/testing/test_trackerpreclustering.cxx
  )
