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
  ${PROJECT_SOURCE_DIR}/include/falaise/snemo/event_browser_version.h.in
  ${PROJECT_BINARY_DIR}/include/falaise/snemo/event_browser_version.h
  )
configure_file(
  ${PROJECT_SOURCE_DIR}/include/falaise/snemo/event_browser_config.h.in
  ${PROJECT_BINARY_DIR}/include/falaise/snemo/event_browser_config.h
  )
set(FalaiseEventBrowserPlugin_PUBLIC_HEADERS
  ${PROJECT_BINARY_DIR}/include/falaise/snemo/event_browser_version.h
  ${PROJECT_BINARY_DIR}/include/falaise/snemo/event_browser_config.h
  )

#-----------------------------------------------------------------------
# PRIVATE HEADERS

set(FalaiseEventBrowserPlugin_DETECTOR_HEADERS
  include/falaise/snemo/detector/i_volume.h
  include/falaise/snemo/detector/i_root_volume.h
  include/falaise/snemo/detector/volume_model.h
  include/falaise/snemo/detector/box_volume.h
  include/falaise/snemo/detector/composite_volume.h
  include/falaise/snemo/detector/cylinder_volume.h
  include/falaise/snemo/detector/detector_manager.h
  include/falaise/snemo/detector/polycone_volume.h
  include/falaise/snemo/detector/special_volume.h
  include/falaise/snemo/detector/sphere_volume.h
  include/falaise/snemo/detector/tube_volume.h
 )
list_prepend(FalaiseEventBrowserPlugin_DETECTOR_HEADERS "${PROJECT_SOURCE_DIR}/")

set(FalaiseEventBrowserPlugin_IO_HEADERS
  include/falaise/snemo/io/boost_access.h
  include/falaise/snemo/io/brio_access.h
  include/falaise/snemo/io/data_model.h
  include/falaise/snemo/io/event_server.h
  include/falaise/snemo/io/i_data_access.h
  )
list_prepend(FalaiseEventBrowserPlugin_IO_HEADERS "${PROJECT_SOURCE_DIR}/")

set(FalaiseEventBrowserPlugin_UTILS_HEADERS
  include/falaise/snemo/utils/root_utilities.h
  include/falaise/snemo/utils/singleton.h
  )
list_prepend(FalaiseEventBrowserPlugin_UTILS_HEADERS "${PROJECT_SOURCE_DIR}/")

set(FalaiseEventBrowserPlugin_VIEW_HEADERS
  # include/falaise/snemo/view/bipo_draw_manager.h
  include/falaise/snemo/view/base_renderer.h
  include/falaise/snemo/view/browser_icons.h
  include/falaise/snemo/view/browser_tracks.h
  include/falaise/snemo/view/calorimeter_hit_renderer.h
  include/falaise/snemo/view/display_2d.h
  include/falaise/snemo/view/display_3d.h
  include/falaise/snemo/view/display_options.h
  include/falaise/snemo/view/event_browser_ctrl.h
  include/falaise/snemo/view/event_browser.h
  include/falaise/snemo/view/event_browser_linkdef.h
  include/falaise/snemo/view/event_browser_menu.h
  include/falaise/snemo/view/event_browser_module.h
  include/falaise/snemo/view/event_browser_signals.h
  include/falaise/snemo/view/event_display.h
  include/falaise/snemo/view/event_selection.h
  include/falaise/snemo/view/i_draw_manager.h
  include/falaise/snemo/view/i_embedded_viewer.h
  include/falaise/snemo/view/opengl_embedded_viewer.h
  include/falaise/snemo/view/opengl_event_handler.h
  include/falaise/snemo/view/opengl_pad.h
  include/falaise/snemo/view/opengl_scene.h
  include/falaise/snemo/view/options_manager.h
  include/falaise/snemo/view/pad_embedded_viewer.h
  include/falaise/snemo/view/progress_bar.h
  include/falaise/snemo/view/signal_handling.h
  include/falaise/snemo/view/snemo_draw_manager.h
  include/falaise/snemo/view/default_draw_manager.h
  include/falaise/snemo/view/status_bar.h
  include/falaise/snemo/view/style_manager.h
  # include/falaise/snemo/view/tiny_viewer.h
  include/falaise/snemo/view/tracker_hit_renderer.h
  include/falaise/snemo/view/view_models.h
  include/falaise/snemo/view/visual_track_renderer.h
  )
list_prepend(FalaiseEventBrowserPlugin_VIEW_HEADERS "${PROJECT_SOURCE_DIR}/")

# - Generate ROOT headers
set(FalaiseEventBrowserPlugin_DICT_HEADERS
  include/falaise/snemo/view/event_browser.h
  include/falaise/snemo/view/display_3d.h
  include/falaise/snemo/view/display_2d.h
  include/falaise/snemo/view/display_options.h
  include/falaise/snemo/view/browser_tracks.h
  include/falaise/snemo/view/event_selection.h
  include/falaise/snemo/view/progress_bar.h
  )
list_prepend(FalaiseEventBrowserPlugin_DICT_HEADERS "${PROJECT_SOURCE_DIR}/")
root_generate_dictionary("${FalaiseEventBrowserPlugin_DICT_HEADERS}"
  "${PROJECT_SOURCE_DIR}/include/falaise/snemo/view/event_browser_linkdef.h"
  "${PROJECT_BINARY_DIR}/event_browser_dict.cc"
  "${PROJECT_SOURCE_DIR}/include"
  )

#-----------------------------------------------------------------------
# SOURCE FILES
# Pretty basic!
set(FalaiseEventBrowserPlugin_SOURCES
  ${PROJECT_BINARY_DIR}/event_browser_dict.cc
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
  # ${FalaiseEventBrowserPlugin_IO_HEADERS}
  # ${FalaiseEventBrowserPlugin_DETECTOR_HEADERS}
  # ${FalaiseEventBrowserPlugin_UTILS_HEADERS}
  ${FalaiseEventBrowserPlugin_SOURCES}
  )

target_link_libraries(Falaise_EventBrowser
  ${Boost_LIBRARIES}
  ${ROOT_LIBRARIES}
  Falaise)

# Install it:
install(TARGETS Falaise_EventBrowser DESTINATION ${CMAKE_INSTALL_LIBDIR}/Falaise/modules)

# - end of CMakeLists.txt for EventBrowser/src subdir
