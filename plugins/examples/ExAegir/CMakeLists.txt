cmake_minimum_required(VERSION 3.3)
project(ExAegir VERSION "1.0.0")

set(ExAegirVersion "${PROJECT_VERSION}")
message(STATUS "ExAegirVersion='${ExAegirVersion}'")
include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

include(GNUInstallDirs)
message(STATUS "CMAKE_INSTALL_LIBDIR='${CMAKE_INSTALL_LIBDIR}'")
message(STATUS "CMAKE_INSTALL_BINDIR='${CMAKE_INSTALL_BINDIR}'")
message(STATUS "CMAKE_INSTALL_BINDIR='${CMAKE_INSTALL_BINDIR}'")
message(STATUS "CMAKE_INSTALL_DATAROOTDIR='${CMAKE_INSTALL_DATAROOTDIR}'")
message(STATUS "CMAKE_INSTALL_RPATH='${CMAKE_INSTALL_RPATH}'")
message(STATUS "CMAKE_SKIP_RPATH='${CMAKE_SKIP_RPATH}'")
message(STATUS "CMAKE_SKIP_BUILD_RPATH='${CMAKE_SKIP_BUILD_RPATH}'")
message(STATUS "CMAKE_BUILD_WITH_INSTALL_RPATH='${CMAKE_BUILD_WITH_INSTALL_RPATH}'")

find_package(Falaise REQUIRED)

set(ExAegirResourceDir "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_DATAROOTDIR}/exaegir-${ExAegirVersion}")
configure_file(exaegir/version.cc.in ${CMAKE_CURRENT_BINARY_DIR}/exaegir/version.cc @ONLY)
configure_file(exaegir/resources.cc.in ${CMAKE_CURRENT_BINARY_DIR}/exaegir/resources.cc @ONLY)
configure_file(exaegir-config.in ${CMAKE_CURRENT_BINARY_DIR}/exaegir-config @ONLY)

add_library(exaegir SHARED
  exaegir/version.h
  exaegir/resources.h
  exaegir/dummy_event_generator.h
  exaegir/dummy_event_generator.cc
  exaegir/dummy_event_generator2.h
  exaegir/dummy_event_generator2.cc
  ${CMAKE_CURRENT_BINARY_DIR}/exaegir/version.cc
  ${CMAKE_CURRENT_BINARY_DIR}/exaegir/resources.cc
  exaegir/init_fini.cc
  )

target_link_libraries(exaegir Falaise::Falaise)

set_target_properties(exaegir PROPERTIES INSTALL_RPATH_USE_LINK_PATH 1)
# if(UNIX AND NOT APPLE)
#   set_target_properties(exaegir
#     PROPERTIES INSTALL_RPATH "\$ORIGIN/../${CMAKE_INSTALL_LIBDIR}"
#     )
# endif()

# Install targets:
install(TARGETS exaegir DESTINATION ${CMAKE_INSTALL_LIBDIR})

add_executable(runexaegir
  runexaegir.cxx
  )
target_link_libraries(runexaegir exaegir)
set_target_properties(runexaegir PROPERTIES INSTALL_RPATH_USE_LINK_PATH 1)
if(UNIX AND NOT APPLE)
  set_target_properties(runexaegir
    PROPERTIES INSTALL_RPATH "\$ORIGIN/../${CMAKE_INSTALL_LIBDIR}"
    )
endif()

# Install Targets:
install(TARGETS runexaegir DESTINATION "${CMAKE_INSTALL_BINDIR}")

# Install resource files:
install(DIRECTORY resources/ DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}/exaegir-${ExAegirVersion}")

install(PROGRAMS "${CMAKE_CURRENT_BINARY_DIR}/exaegir-config" DESTINATION "${CMAKE_INSTALL_BINDIR}")

  # end
