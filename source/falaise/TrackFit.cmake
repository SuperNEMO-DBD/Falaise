# TrackFit specific classes:

list(APPEND FalaiseLibrary_HEADERS
  TrackFit/gg_hit.h
  TrackFit/drawing.h
  TrackFit/i_drift_time_calibration.h
  TrackFit/fit_utils.h
  TrackFit/line_fit_mgr.h
  TrackFit/helix_fit_mgr.h
  )

list(APPEND FalaiseLibrary_SOURCES
  TrackFit/drawing.cc
  TrackFit/fit_utils.cc
  TrackFit/gg_hit.cc
  TrackFit/helix_fit_mgr.cc
  TrackFit/i_drift_time_calibration.cc
  TrackFit/line_fit_mgr.cc
  )

list(APPEND FalaiseLibrary_TESTS
  TrackFit/testing/test_trackfit_gg_hit.cxx
  TrackFit/testing/test_trackfit_drawing.cxx
  TrackFit/testing/test_trackfit_drift_time_calibration.cxx
  TrackFit/testing/test_trackfit_helix_fit_mgr.cxx
  TrackFit/testing/test_trackfit_line_fit_mgr.cxx
  )
