# - Publish headers

configure_file(fecom/version.hpp.in ${CMAKE_CURRENT_BINARY_DIR}/fecom/version.hpp @ONLY)

add_definitions(-std=c++11)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(FeCom_HEADERS
  ${CMAKE_CURRENT_BINARY_DIR}/fecom/version.hpp
  fecom/utils.hpp
  fecom/base_hit.hpp
  fecom/calo_constants.hpp
  fecom/calo_utils.hpp
  fecom/calo_hit.hpp
  fecom/calo_pedestal_calib.hpp
  fecom/calo_calibration.hpp
  fecom/run_header.hpp
  )

set(FeCom_PRIVATE_HEADERS
  )

set(FeCom_SOURCES
  fecom/utils.cpp
  fecom/base_hit.cpp
  fecom/calo_constants.cpp
  fecom/calo_utils.cpp
  fecom/calo_hit.cpp
  fecom/calo_pedestal_calib.cpp
  fecom/calo_calibration.cpp
  fecom/run_header.cpp
  )

set(FeCom_TESTS
  testing/test-libfecom-version.cxx
  testing/test-libfecom-calo_constants.cxx
  testing/test-libfecom-base_hit.cxx
  testing/test-libfecom-calo_hit.cxx
  testing/test-libfecom-calo_pedestal_calib.cxx
  )

find_package(Bayeux 3.0.0
  REQUIRED
  )
# message( STATUS "FeCom: Bayeux found at '${Bayeux}'")
# message( STATUS "FeCom: Bayeux version = '${Bayeux}'")

find_package(Boost 1.60.0
  REQUIRED
  date_time
  )

message( STATUS "FeCom: Boost found at '${Boost_INCLUDE_DIRS}'")
message( STATUS "FeCom: Boost version = '${Boost_VERSION}'")
include_directories(SYSTEM ${Boost_INCLUDE_DIRS})

add_library(fecom SHARED
  ${FeCom_HEADERS}
  ${FeCom_HEADERS}
  ${FeCom_PRIVATE_HEADERS}
  ${FeCom_SOURCES}
  )

target_link_libraries(fecom Bayeux::Bayeux)
target_link_libraries(fecom ${Boost_LIBRARIES})

# Apple linker requires dynamic lookup of symbols, so we
# add link flags on this platform
if(APPLE)
  set_target_properties(fecom
    PROPERTIES
    LINK_FLAGS "-undefined dynamic_lookup"
    INSTALL_RPATH "@loader_path"
    )
endif()

install( FILES
  ${FeCom_HEADERS}
  DESTINATION ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_INCLUDEDIR}/fecom
  PERMISSIONS OWNER_READ OWNER_WRITE
  GROUP_READ
  WORLD_READ
  )

install( FILES
  ${PROJECT_BINARY_DIR}/libfecom${CMAKE_SHARED_LIBRARY_SUFFIX}
  DESTINATION ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}
  PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE
  GROUP_READ GROUP_EXECUTE
  WORLD_READ WORLD_EXECUTE
  )

# install( FILES
#   ${PROJECT_SOURCE_DIR}/LICENSE.txt
#   DESTINATION ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_DATAROOTDIR}/${FECOM_TAG}
#   PERMISSIONS OWNER_READ OWNER_WRITE
#   GROUP_READ
#   WORLD_READ
#   )

option(FECOM_ENABLE_TESTING "Build unit testing system for libfecom" ON)
if(FECOM_ENABLE_TESTING)
  enable_testing()
endif()

if(FECOM_ENABLE_TESTING)
  foreach(_testsource ${FeCom_TESTS})
    set(_fecom_TEST_ENVIRONMENT
      "FECOM_RESOURCE_FILES_DIR=${PROJECT_SOURCE_DIR}/resources"
      )
    get_filename_component(_testname "${_testsource}" NAME_WE)
    set(_testname "fecom-${_testname}")
    add_executable(${_testname}
      ${_testsource}
      )
    target_link_libraries(${_testname} fecom
      # ${FeCom_OTHER_LIBS}
      )
    # - On Apple, ensure dynamic_lookup of undefined symbols
    if(APPLE)
      set_target_properties(${_testname} PROPERTIES LINK_FLAGS "-undefined dynamic_lookup")
    endif()
    add_test(NAME ${_testname} COMMAND ${_testname})
    set_property(TEST ${_testname}
      APPEND PROPERTY ENVIRONMENT ${_fecom_TEST_ENVIRONMENT}
      )
    # - For now, dump them into the testing output directory
    set_target_properties(${_testname}
      PROPERTIES
      RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/fecom_tests
      ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/fecom_tests
      )
  endforeach()
endif()

# - end of CMakeLists.txt