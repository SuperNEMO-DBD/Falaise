# BiPo3 specific classes:

list(APPEND FalaiseLibrary_HEADERS
  bipo3/geometry/calo_tapered_scin_box_model.h
  bipo3/processing/calorimeter_s2c_module.h
  )

list(APPEND FalaiseLibrary_SOURCES
  bipo3/geometry/calo_tapered_scin_box_model.cc
  bipo3/processing/calorimeter_s2c_module.cc
  )

# list(APPEND FalaiseLibrary_TESTS
#  bipo3/testing/test_bipo3_processing_calorimeter_s2c_module.cxx
#  )