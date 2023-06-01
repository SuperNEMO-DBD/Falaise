# Falaise geometry specific classes:

list(APPEND FalaiseLibrary_HEADERS
  geometry/point.hh
  geometry/i_curve2.hh
  geometry/line2.hh
  geometry/fitted_point2.hh
  geometry/fitted_line2.hh
  geometry/segment2.hh
  geometry/triplet2.hh
  geometry/rectangle.hh
  geometry/circle.hh
  geometry/translated_curve2.hh
  geometry/rotated_curve2.hh
  geometry/kinked_curve2.hh
  geometry/helix_utils.hh
  geometry/line_utils.hh
  )


list(APPEND FalaiseLibrary_SOURCES
  geometry/point.cc
  geometry/line2.cc
  geometry/fitted_point2.cc
  geometry/fitted_line2.cc
  geometry/segment2.cc
  geometry/triplet2.cc
  geometry/rectangle.cc
  geometry/circle.cc
  geometry/translated_curve2.cc
  geometry/rotated_curve2.cc
  geometry/kinked_curve2.cc
  geometry/helix_utils.cc
  geometry/line_utils.cc
  )

list(APPEND FalaiseLibrary_TESTS
  geometry/testing/test-point.cxx
  geometry/testing/test-rectangle.cxx
  geometry/testing/test-circle.cxx
  geometry/testing/test-line.cxx
  geometry/testing/test-line-2.cxx
  geometry/testing/test-line-3.cxx
  geometry/testing/test-triplet.cxx
  geometry/testing/test-kinked_curve.cxx
  )

option(FALAISE_GEOMETRY_ENABLE_PLOT_TESTING "Build interactive plot unit testing system for Falaise/geometry" OFF)

# - end