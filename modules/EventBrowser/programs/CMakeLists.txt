
set(_app_name flvisualize)
add_executable(${_app_name} snemo_event_browser.cxx)

set_target_properties(${_app_name} PROPERTIES COMPILE_DEFINITIONS ENABLE_BINRELOC)
target_link_libraries(${_app_name} Falaise_EventBrowser)
set_target_properties(${_app_name} PROPERTIES INSTALL_RPATH_USE_LINK_PATH 1)

if(UNIX AND NOT APPLE)
  set_target_properties(${_app_name} PROPERTIES INSTALL_RPATH "\$ORIGIN/../${CMAKE_INSTALL_LIBDIR};\$ORIGIN/../${CMAKE_INSTALL_LIBDIR}/Falaise/modules")
elseif(APPLE)
  # Temporary setting - needs testing
  set_target_properties(${_app_name}
    PROPERTIES
    INSTALL_RPATH "@executable_path/../${CMAKE_INSTALL_LIBDIR};@executable_path/../${CMAKE_INSTALL_LIBDIR}/Falaise/modules"
    )
endif()

install(TARGETS ${_app_name} RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

# end of CMakeLists.txt
