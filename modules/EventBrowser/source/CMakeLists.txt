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
  ${PROJECT_SOURCE_DIR}/source/inc/falaise/snemo/event_browser_version.h.in
  ${PROJECT_BINARY_DIR}/source/inc/falaise/snemo/event_browser_version.h
  )
configure_file(
  ${PROJECT_SOURCE_DIR}/source/inc/falaise/snemo/event_browser_config.h.in
  ${PROJECT_BINARY_DIR}/source/inc/falaise/snemo/event_browser_config.h
  )
set(FalaiseEventBrowserPlugin_PUBLIC_HEADERS
  ${PROJECT_BINARY_DIR}/source/inc/falaise/snemo/event_browser_version.h
  ${PROJECT_BINARY_DIR}/source/inc/falaise/snemo/event_browser_config.h
  )

#-----------------------------------------------------------------------
# PRIVATE HEADERS

set(FalaiseEventBrowserPlugin_PUBLIC_HEADERS
  inc/falaise/snemo/detector/i_volume.h
  inc/falaise/snemo/detector/i_root_volume.h
  inc/falaise/snemo/detector/volume_model.h
  inc/falaise/snemo/detector/box_volume.h
  inc/falaise/snemo/detector/composite_volume.h
  inc/falaise/snemo/detector/cylinder_volume.h
  inc/falaise/snemo/detector/detector_manager.h
  inc/falaise/snemo/detector/polycone_volume.h
  inc/falaise/snemo/detector/special_volume.h
  inc/falaise/snemo/detector/sphere_volume.h
  inc/falaise/snemo/detector/tube_volume.h
  inc/falaise/snemo/io/boost_access.h
  inc/falaise/snemo/io/brio_access.h
  inc/falaise/snemo/io/data_model.h
  inc/falaise/snemo/io/event_server.h
  inc/falaise/snemo/io/i_data_access.h
  inc/falaise/snemo/utils/root_utilities.h
  inc/falaise/snemo/utils/singleton.h
  inc/falaise/snemo/view/base_renderer.h
  inc/falaise/snemo/view/browser_icons.h
  inc/falaise/snemo/view/browser_tracks.h
  inc/falaise/snemo/view/calorimeter_hit_renderer.h
  inc/falaise/snemo/view/display_2d.h
  inc/falaise/snemo/view/display_3d.h
  inc/falaise/snemo/view/display_options.h
  inc/falaise/snemo/view/event_browser_ctrl.h
  inc/falaise/snemo/view/event_browser.h
  inc/falaise/snemo/view/event_browser_linkdef.h
  inc/falaise/snemo/view/event_browser_menu.h
  inc/falaise/snemo/view/event_browser_module.h
  inc/falaise/snemo/view/event_browser_signals.h
  inc/falaise/snemo/view/event_display.h
  inc/falaise/snemo/view/event_selection.h
  inc/falaise/snemo/view/i_draw_manager.h
  inc/falaise/snemo/view/i_embedded_viewer.h
  inc/falaise/snemo/view/opengl_embedded_viewer.h
  inc/falaise/snemo/view/opengl_event_handler.h
  inc/falaise/snemo/view/opengl_pad.h
  inc/falaise/snemo/view/opengl_scene.h
  inc/falaise/snemo/view/options_manager.h
  inc/falaise/snemo/view/pad_embedded_viewer.h
  inc/falaise/snemo/view/progress_bar.h
  inc/falaise/snemo/view/signal_handling.h
  inc/falaise/snemo/view/snemo_draw_manager.h
  inc/falaise/snemo/view/default_draw_manager.h
  inc/falaise/snemo/view/status_bar.h
  inc/falaise/snemo/view/style_manager.h
  # inc/falaise/snemo/view/tiny_viewer.h
  inc/falaise/snemo/view/tracker_hit_renderer.h
  inc/falaise/snemo/view/view_models.h
  inc/falaise/snemo/view/visual_track_renderer.h
  )

# - Generate ROOT headers
set(FalaiseEventBrowserPlugin_DICT_HEADERS
  inc/falaise/snemo/view/event_browser_signals.h
  inc/falaise/snemo/view/view_models.h
  inc/falaise/snemo/view/event_browser.h
  inc/falaise/snemo/view/display_3d.h
  inc/falaise/snemo/view/display_2d.h
  inc/falaise/snemo/view/display_options.h
  inc/falaise/snemo/view/browser_tracks.h
  inc/falaise/snemo/view/event_selection.h
  inc/falaise/snemo/view/progress_bar.h
  inc/falaise/snemo/view/status_bar.h
  )

set(_event_browser_root_dict "")
# - Generate ROOT headers
if(FALAISE_USE_LEGACY_ROOT)
  root_generate_dictionary("${FalaiseEventBrowserPlugin_DICT_HEADERS}"
    "${PROJECT_SOURCE_DIR}/source/inc/falaise/snemo/view/event_browser_linkdef.h"
    "${PROJECT_BINARY_DIR}/event_browser_dict.cc"
    "${PROJECT_SOURCE_DIR}/include"
    )
  set(_event_browser_root_dict ${PROJECT_BINARY_DIR}/event_browser_dict.cc)
else()
  if(ROOT_VERSION VERSION_GREATER 5.99)
    set(__EVENTBROWSER_MODULE_ARG MODULE Falaise_EventBrowser)
    # Also have to haxor CMAKE_INSTALL_LIBDIR because we can't disable
    # install of pcm/rootmap files...)
    set(CMAKE_INSTALL_LIBDIR_OLD "${CMAKE_INSTALL_LIBDIR}")
    set(CMAKE_INSTALL_LIBDIR "${CMAKE_INSTALL_LIBDIR_OLD}/Falaise/modules")
  endif()
  # Root 6.12 requires additional include path
  include_directories("${CMAKE_CURRENT_SOURCE_DIR}")
  root_generate_dictionary(event_browser_dict
    "${FalaiseEventBrowserPlugin_DICT_HEADERS}"
    ${__EVENTBROWSER_MODULE_ARG}
    LINKDEF "${PROJECT_SOURCE_DIR}/source/inc/falaise/snemo/view/event_browser_linkdef.h"
    OPTIONS "-noIncludePaths"
    )
  # Publish the dictionary headers next to the module - needed for
  # cling parsing in R6. NB: this is connected with the setting of
  # ROOT_INCLUDE_PATH in the main flvisualize application.
  foreach(_dictheader ${FalaiseEventBrowserPlugin_DICT_HEADERS})
    get_filename_component(_dictheader_file "${_dictheader}" NAME)
    configure_file(${_dictheader} ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/falaise/snemo/view/${_dictheader_file})
  endforeach()


  if(ROOT_VERSION VERSION_GREATER 5.99)
    # pop the true libinstalldir
    set(CMAKE_INSTALL_LIBDIR "${CMAKE_INSTALL_LIBDIR_OLD}")
  endif()

  set(_event_browser_root_dict ${CMAKE_CURRENT_BINARY_DIR}/event_browser_dict.cxx)

endif()

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
  boost_access.cc
  brio_access.cc
  event_server.cc
  i_data_access.cc
  root_utilities.cc
  base_renderer.cc
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
  ${CMAKE_CURRENT_BINARY_DIR}/inc
  ${CMAKE_CURRENT_SOURCE_DIR}/inc
  )

target_link_libraries(Falaise_EventBrowser
  Falaise
  ${Boost_LIBRARIES}
  ${ROOT_LIBRARIES}
  )

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
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/Falaise/modules/falaise/snemo/view
  )

# - end of CMakeLists.txt for EventBrowser/src subdir
