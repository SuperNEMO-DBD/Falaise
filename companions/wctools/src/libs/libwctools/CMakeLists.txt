# - Publish headers

configure_file(wctools/version.h.in ${CMAKE_CURRENT_BINARY_DIR}/wctools/version.h @ONLY)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(WcTools_HEADERS
  wctools/utils.hpp
  wctools/raw_event.hpp
  wctools/run_header.hpp
  wctools/raw_reader.hpp
  )

set(WcTools_PRIVATE_HEADERS
  )

set(WcTools_SOURCES
  wctools/utils.cpp
  wctools/raw_event.cpp
  wctools/raw_reader.cpp
  )

set(WcTools_TESTS
  testing/test-libwctools-raw_reader.cxx
  )


add_library(wctools SHARED
  ${WcTools_HEADERS}
  ${WcTools_HEADERS}
  ${WcTools_PRIVATE_HEADERS}
  ${WcTools_SOURCES}
  )
target_link_libraries(wctools Bayeux::Bayeux)
# Apple linker requires dynamic lookup of symbols, so we
# add link flags on this platform
if(APPLE)
  set_target_properties(wctools
    PROPERTIES
    LINK_FLAGS "-undefined dynamic_lookup"
    INSTALL_RPATH "@loader_path"
    )
endif()
target_clang_format(wctools)

install( FILES
  ${WcTools_HEADERS}
  DESTINATION ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_INCLUDEDIR}/wctools
  PERMISSIONS OWNER_READ OWNER_WRITE
  GROUP_READ
  WORLD_READ
  )

install( FILES
  ${PROJECT_BUILD_PREFIX}/${CMAKE_INSTALL_LIBDIR}/libwctools${CMAKE_SHARED_LIBRARY_SUFFIX}
  DESTINATION ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}
  PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE
  GROUP_READ GROUP_EXECUTE
  WORLD_READ WORLD_EXECUTE
  )

option(WCTOOLS_ENABLE_TESTING "Build unit testing system for libwctools" ON)
if(WCTOOLS_ENABLE_TESTING)
  enable_testing()
endif()

if(WCTOOLS_ENABLE_TESTING)
  foreach(_testsource ${WcTools_TESTS})
    set(_wctools_TEST_ENVIRONMENT
      "WCTOOLS_RESOURCE_FILES_DIR=${PROJECT_SOURCE_DIR}/resources"
      )
    get_filename_component(_testname "${_testsource}" NAME_WE)
    set(_testname "wctools-${_testname}")
    add_executable(${_testname}
      ${_testsource}
      )
    target_link_libraries(${_testname} wctools)
    # - On Apple, ensure dynamic_lookup of undefined symbols
    if(APPLE)
      set_target_properties(${_testname} PROPERTIES LINK_FLAGS "-undefined dynamic_lookup")
    endif()
    target_clang_format(${_testname})

    add_test(NAME ${_testname} COMMAND ${_testname})
    set_property(TEST ${_testname}
      APPEND PROPERTY ENVIRONMENT ${_wctools_TEST_ENVIRONMENT}
      )
    # - For now, dump them into the testing output directory
    set_target_properties(${_testname}
      PROPERTIES
      RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/wctools_tests
      ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/wctools_tests
      )
  endforeach()
endif()

# - end of CMakeLists.txt
