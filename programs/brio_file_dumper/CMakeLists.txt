add_executable(brio_file_dumper brio_file_dumper.cc)

target_link_libraries(brio_file_dumper PRIVATE
  Falaise
  Bayeux::Bayeux
  Boost::program_options)

target_clang_format(brio_file_dumper)

 # - Ensure link to internal and external deps
set_target_properties(brio_file_dumper PROPERTIES INSTALL_RPATH_USE_LINK_PATH 1)

if(UNIX AND NOT APPLE)
  set_target_properties(brio_file_dumper
    PROPERTIES INSTALL_RPATH "\$ORIGIN/../${CMAKE_INSTALL_LIBDIR}"
    )
elseif(APPLE)
  # Temporary setting - needs testing
  set_target_properties(brio_file_dumper
    PROPERTIES
      INSTALL_RPATH "@loader_path/../${CMAKE_INSTALL_LIBDIR}"
    )
endif()

# - Install
install(TARGETS brio_file_dumper
  EXPORT FalaiseTargets
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  )