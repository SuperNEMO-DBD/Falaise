# TrackerPreClustering specific classes:

list(APPEND FalaiseLibrary_HEADERS
  TrackerPreClustering/interface.h
  TrackerPreClustering/interface.tpp
  TrackerPreClustering/pre_clusterizer.h
  TrackerPreClustering/pre_clusterizer.tpp
 )


list(APPEND FalaiseLibrary_SOURCES
  TrackerPreClustering/interface.cc
  TrackerPreClustering/pre_clusterizer.cc
  TrackerPreClustering/testing/gg_hit.h
  TrackerPreClustering/testing/gg_hit.cc
  TrackerPreClustering/testing/event_generator.h
  TrackerPreClustering/testing/event_display.h
  TrackerPreClustering/testing/event_display.tpp
  TrackerPreClustering/testing/event_generator.cc
  TrackerPreClustering/testing/event_display.cc
  )

list(APPEND FalaiseLibrary_TESTS
  TrackerPreClustering/testing/test_trackerpreclustering.cxx
  )
