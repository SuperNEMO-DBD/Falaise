# - CMakeLists.txt for FalaiseEventBrowserPlugin/src subdir

#-----------------------------------------------------------------------
# function list_prepend(<list> <value>)
#          Prepend value to each element in list, in place
function(list_prepend _list _value)
  set(_tmp_list)
  foreach(_elem ${${_list}})
    list(APPEND _tmp_list "${_value}${_elem}")
  endforeach()
  set(${_list} ${_tmp_list} PARENT_SCOPE)
endfunction()

#-----------------------------------------------------------------------
# Explicitly list the sources for the library, separating into sources,
# public headers, private headers and resources.
#-----------------------------------------------------------------------
# PUBLIC HEADERS
# Because we're in the awkward position of separate headers and sources,
# use a simple listing and then prepend the actual source dir root

# - Add config header
configure_file(
  ${PROJECT_SOURCE_DIR}/EventBrowser/event_browser_version.h.in
  ${PROJECT_BINARY_DIR}/EventBrowser/event_browser_version.h
  )
configure_file(
  ${PROJECT_SOURCE_DIR}/EventBrowser/event_browser_config.h.in
  ${PROJECT_BINARY_DIR}/EventBrowser/event_browser_config.h
  )
set(FalaiseEventBrowserPlugin_PUBLIC_HEADERS
  ${PROJECT_BINARY_DIR}/EventBrowser/event_browser_version.h
  ${PROJECT_BINARY_DIR}/EventBrowser/event_browser_config.h
  )

#-----------------------------------------------------------------------
# PRIVATE HEADERS

set(FalaiseEventBrowserPlugin_PUBLIC_HEADERS
  log.h
  detector/i_volume.h
  detector/i_root_volume.h
  detector/volume_model.h
  detector/box_volume.h
  detector/composite_volume.h
  detector/cylinder_volume.h
  detector/detector_manager.h
  detector/polycone_volume.h
  detector/special_volume.h
  detector/sphere_volume.h
  detector/tube_volume.h
  # detector/tessellated_volume.h # Not ready for ROOT 6.16/00
  io/boost_access.h
  io/brio_access.h
  io/data_model.h
  io/event_server.h
  io/i_data_access.h
  utils/root_utilities.h
  utils/singleton.h
  # inc/EventBrowser/view/bipo_draw_manager.h
  view/utils.h
  view/base_renderer.h
  view/browser_icons.h
  view/browser_tracks.h
  view/calorimeter_hit_renderer.h
  view/display_2d.h
  view/display_3d.h
  view/display_options.h
  view/event_browser_ctrl.h
  view/event_browser.h
  view/event_browser_linkdef.h
  view/event_browser_menu.h
  view/event_browser_module.h
  view/event_browser_signals.h
  view/event_display.h
  view/event_selection.h
  view/i_draw_manager.h
  view/i_embedded_viewer.h
  view/opengl_embedded_viewer.h
  view/opengl_event_handler.h
  view/opengl_pad.h
  view/opengl_scene.h
  view/options_manager.h
  view/pad_embedded_viewer.h
  view/progress_bar.h
  view/signal_handling.h
  view/snemo_draw_manager.h
  view/default_draw_manager.h
  view/status_bar.h
  view/style_manager.h
  # inc/EventBrowser/view/tiny_viewer.h
  view/tracker_hit_renderer.h
  view/view_models.h
  view/visual_track_renderer.h
  )

# - Generate ROOT headers
set(FalaiseEventBrowserPlugin_DICT_HEADERS
  view/event_browser_signals.h
  view/view_models.h
  view/event_browser.h
  view/display_3d.h
  view/display_2d.h
  view/display_options.h
  view/browser_tracks.h
  view/event_selection.h
  view/progress_bar.h
  view/status_bar.h
  )


# - Generate ROOT headers
set(__EVENTBROWSER_MODULE_ARG MODULE Falaise_EventBrowser)

# Have to haxor CMAKE_INSTALL_LIBDIR and Output Dir because we can't disable
# install of pcm/rootmap files...)
set(CMAKE_INSTALL_LIBDIR_OLD "${CMAKE_INSTALL_LIBDIR}")
set(CMAKE_INSTALL_LIBDIR "${CMAKE_INSTALL_LIBDIR_OLD}/Falaise/modules")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/Falaise/modules")

# Have to explicitly include the header path
include_directories("${PROJECT_SOURCE_DIR}")
root_generate_dictionary(event_browser_dict
  ${__EVENTBROWSER_MODULE_ARG}
  LINKDEF "${PROJECT_SOURCE_DIR}/EventBrowser/view/event_browser_linkdef.h"
  OPTIONS "-noIncludePaths"
  )
set(_event_browser_root_dict ${CMAKE_CURRENT_BINARY_DIR}/event_browser_dict.cxx)

# Publish the dictionary headers next to the module - needed for
# cling parsing in R6. NB: this is connected with the setting of
# ROOT_INCLUDE_PATH in the main flvisualize application.
foreach(_dictheader ${FalaiseEventBrowserPlugin_DICT_HEADERS})
  get_filename_component(_dictheader_file "${_dictheader}" NAME)
  configure_file(${_dictheader} ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/EventBrowser/view/${_dictheader_file})
endforeach()

# pop the true libinstalldir
set(CMAKE_INSTALL_LIBDIR "${CMAKE_INSTALL_LIBDIR_OLD}")


# ROOT dictionaries will refine accessor keywords for reflection purposes
# This is expected, but Clang will warn on ROOT5, so disable it.
# Done as a compile flag because we cant edit the code directly.
if(CMAKE_CXX_COMPILER_ID MATCHES "(Apple)+Clang")
  set_property(SOURCE ${_event_browser_root_dict}
    PROPERTY COMPILE_FLAGS "-Wno-keyword-macro"
    )
endif()


#-----------------------------------------------------------------------
# SOURCE FILES
# Pretty basic!
set(FalaiseEventBrowserPlugin_SOURCES
  ${_event_browser_root_dict}
  i_volume.cc
  i_root_volume.cc
  box_volume.cc
  composite_volume.cc
  cylinder_volume.cc
  detector_manager.cc
  polycone_volume.cc
  special_volume.cc
  sphere_volume.cc
  tube_volume.cc
  # tessellated_volume.cc # Not ready for ROOT 6.16/00
  boost_access.cc
  brio_access.cc
  event_server.cc
  i_data_access.cc
  root_utilities.cc
  base_renderer.cc
  # bipo_draw_manager.cc
  browser_tracks.cc
  calorimeter_hit_renderer.cc
  display_2d.cc
  display_3d.cc
  display_options.cc
  event_browser_ctrl.cc
  event_browser.cc
  event_browser_menu.cc
  event_browser_module.cc
  event_display.cc
  event_selection.cc
  i_draw_manager.cc
  i_embedded_viewer.cc
  # opengl_embedded_viewer.cc
  # opengl_event_handler.cc
  # opengl_pad.cc
  # opengl_scene.cc
  options_manager.cc
  pad_embedded_viewer.cc
  progress_bar.cc
  signal_handling.cc
  snemo_draw_manager.cc
  default_draw_manager.cc
  status_bar.cc
  style_manager.cc
  #default_draw_manager.cc
  # tiny_viewer.cc
  tracker_hit_renderer.cc
  visual_track_renderer.cc
  version.cc
  )

#-----------------------------------------------------------------------
# BUILD THE EVENTBROWSER LIBRARY
#
add_library(Falaise_EventBrowser SHARED
  ${FalaiseEventBrowserPlugin_PUBLIC_HEADERS}
  ${FalaiseEventBrowserPlugin_SOURCES}
  )

target_include_directories(Falaise_EventBrowser PUBLIC
  ${ROOT_INCLUDE_DIRS}
  ${PROJECT_BINARY_DIR}
  ${PROJECT_SOURCE_DIR}
  )

target_link_libraries(Falaise_EventBrowser
  Falaise
  ${Boost_LIBRARIES}
  ${ROOT_LIBRARIES}
  )
# Because it's loaded, make sure its RPATH can find dependencies
set_target_properties(Falaise_EventBrowser PROPERTIES INSTALL_RPATH_USE_LINK_PATH 1)

if(APPLE)
  set_target_properties(Falaise_EventBrowser
    PROPERTIES LINK_FLAGS "-undefined dynamic_lookup"
    )
endif()

# Install it:
install(TARGETS Falaise_EventBrowser DESTINATION ${CMAKE_INSTALL_LIBDIR}/Falaise/modules)
# Also have to install the headers for the dictionary so that Cling can use them
# at Runtime.
install(FILES ${FalaiseEventBrowserPlugin_DICT_HEADERS}
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/Falaise/modules/EventBrowser/view
  )

# - end of CMakeLists.txt for EventBrowser/src subdir
