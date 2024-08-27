# Falaise math specific classes:

list(APPEND FalaiseLibrary_HEADERS
  math/double_gauss_pdf.hpp
  )


list(APPEND FalaiseLibrary_SOURCES
  math/double_gauss_pdf.cpp
 )

list(APPEND FalaiseLibrary_TESTS
  math/testing/test-double_gauss_pdf.cxx
  )

option(FALAISE_MATH_ENABLE_PLOT_TESTING "Build interactive plot unit testing system for Falaise/math" OFF)

# - end
