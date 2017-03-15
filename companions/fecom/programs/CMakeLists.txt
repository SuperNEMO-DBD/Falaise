# - Publish headers

add_definitions(-std=c++11)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${PROJECT_BINARY_DIR}/src/libs/libfecom)
include_directories(${PROJECT_SOURCE_DIR}/src/libs/libfecom)

set(FeCom_PROGRAMS
  main_decoder_serializer.cxx
  main_histogram_builder.cxx
  )

find_package(Bayeux 3.0.0
  REQUIRED
  )

find_package(Boost 1.60.0
  REQUIRED
  date_time
  )

message( STATUS "FeCom: Boost found at '${Boost_INCLUDE_DIRS}'")
message( STATUS "FeCom: Boost version = '${Boost_VERSION}'")
include_directories(SYSTEM ${Boost_INCLUDE_DIRS})

option(FECOM_ENABLE_PROGRAMS "Build unit testing system for libfecom" ON)

if(FECOM_ENABLE_PROGRAMS)
  foreach(_programsource ${FeCom_PROGRAMS})
    get_filename_component(_programname "${_programsource}" NAME_WE)
    set(_programname "fecom-${_programname}")
    add_executable(${_programname}
      ${_programsource}
      )
    target_link_libraries(${_programname} fecom
      # ${FeCom_OTHER_LIBS}
      )
    # - On Apple, ensure dynamic_lookup of undefined symbols
    if(APPLE)
      set_target_properties(${_programname} PROPERTIES LINK_FLAGS "-undefined dynamic_lookup")
    endif()
    # add_test(NAME ${_programname} COMMAND ${_programname})
    # set_property(TEST ${_programname}
    #   APPEND PROPERTY ENVIRONMENT ${_fecom_PROGRAM_ENVIRONMENT}
    #   )
    # - For now, dump them into the programs output directory
    set_target_properties(${_programname}
      PROPERTIES
      RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/fecom_programs
      ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/fecom_programs
      )
  endforeach()
endif()
