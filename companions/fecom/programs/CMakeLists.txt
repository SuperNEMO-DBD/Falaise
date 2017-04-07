# - Dependencies

find_package(Boost 1.60.0
  REQUIRED
  date_time
  serialization
  )

find_package(Bayeux 3.0.0
  REQUIRED
  )

include_directories(SYSTEM ${Boost_INCLUDE_DIRS})

# - Publish headers

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${PROJECT_BINARY_DIR}/src/libs/libfecom)
include_directories(${PROJECT_SOURCE_DIR}/src/libs/libfecom)

set(FeCom_PROGRAMS
  decode_sort_serialize.cxx
  hc_event_builder.cxx
  hc_raw_data_to_calibrated_data.cxx
  #main_decoder_serializer.cxx
  #main_histogram_builder.cxx
  )

option(FECOM_ENABLE_PROGRAMS "Build programs associated to libfecom" ON)

if(FECOM_ENABLE_PROGRAMS)
  foreach(_programsource ${FeCom_PROGRAMS})
    get_filename_component(_programname "${_programsource}" NAME_WE)
    set(_programname "fecom-${_programname}")
    add_executable(${_programname}
      ${_programsource}
      )
    #target_link_libraries(${_programname} ${Boost_LIBRARIES})
    #target_link_libraries(${_programname} Bayeux::Bayeux)
    target_link_libraries(${_programname} fecom
      # ${FeCom_OTHER_LIBS}
      )

    # - Ensure link to internal and external deps
    set_target_properties(${_programname} PROPERTIES INSTALL_RPATH_USE_LINK_PATH 1)

    if(UNIX AND NOT APPLE)
      set_target_properties(${_programname}
        PROPERTIES INSTALL_RPATH "\$ORIGIN/../${CMAKE_INSTALL_LIBDIR}"
        )
    elseif(APPLE)
      # Temporary setting - needs testing
      set_target_properties(${_programname}
        PROPERTIES
        INSTALL_RPATH "@loader_path/../${CMAKE_INSTALL_LIBDIR}"
        )
    endif()

    install(TARGETS ${_programname}
      # EXPORT FalaiseTargets
      RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
      )
  endforeach()
endif()
