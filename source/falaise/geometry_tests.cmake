if(FALAISE_GEOMETRY_ENABLE_PLOT_TESTING)
  
  message (STATUS "Adding specific interactive plotting tests for Falaise geometry...")
  
  add_test(NAME testGeometryPointPlot
    COMMAND gnuplot ${CMAKE_CURRENT_SOURCE_DIR}/geometry/testing/test-point.gp
    )
  set_property(TEST testGeometryPointPlot
    APPEND PROPERTY ENVIRONMENT ${_falaise_TEST_ENVIRONMENT}
    )
 
  add_test(NAME testGeometryRectanglePlot
    COMMAND gnuplot ${CMAKE_CURRENT_SOURCE_DIR}/geometry/testing/test-rectangle.gp
    )
  set_property(TEST testGeometryRectanglePlot
    APPEND PROPERTY ENVIRONMENT ${_falaise_TEST_ENVIRONMENT}
    )
  
  add_test(NAME testGeometryCirclePlot
    COMMAND gnuplot ${CMAKE_CURRENT_SOURCE_DIR}/geometry/testing/test-circle.gp
    )
  set_property(TEST testGeometryCirclePlot
    APPEND PROPERTY ENVIRONMENT ${_falaise_TEST_ENVIRONMENT}
    )
 
  add_test(NAME testGeometryLinePlot
    COMMAND gnuplot ${CMAKE_CURRENT_SOURCE_DIR}/geometry/testing/test-line.gp
    )
  set_property(TEST testGeometryLinePlot
    APPEND PROPERTY ENVIRONMENT ${_falaise_TEST_ENVIRONMENT}
    )

  add_test(NAME testGeometryLine2Plot
    COMMAND gnuplot ${CMAKE_CURRENT_SOURCE_DIR}/geometry/testing/test-line-2.gp
    )
  set_property(TEST testGeometryLine2Plot
    APPEND PROPERTY ENVIRONMENT ${_falaise_TEST_ENVIRONMENT}
    )

  add_test(NAME testGeometryLine3Plot
    COMMAND gnuplot ${CMAKE_CURRENT_SOURCE_DIR}/geometry/testing/test-line-3.gp
    )
  set_property(TEST testGeometryLine3Plot
    APPEND PROPERTY ENVIRONMENT ${_falaise_TEST_ENVIRONMENT}
    )
 
  add_test(NAME testGeometryTripletPlot
    COMMAND gnuplot ${CMAKE_CURRENT_SOURCE_DIR}/geometry/testing/test-triplet.gp
    )
  set_property(TEST testGeometryTripletPlot
    APPEND PROPERTY ENVIRONMENT ${_falaise_TEST_ENVIRONMENT}
    )
 
  add_test(NAME testGeometryKinkedCurvePlot
    COMMAND gnuplot ${CMAKE_CURRENT_SOURCE_DIR}/geometry/testing/test-kinked_curve.gp
    )
  set_property(TEST testGeometryKinkedCurvePlot
    APPEND PROPERTY ENVIRONMENT ${_falaise_TEST_ENVIRONMENT}
    )
  
endif()
