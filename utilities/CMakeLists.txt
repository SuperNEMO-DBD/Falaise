# - Create Catch testing utility library
# As tests are spread around in Falaise, vendor Catch inside utilities
# Create static library holding catch main that Falaise targets can link to
#
add_library(FLCatch STATIC catch/catch.hpp catch/flcatch-main.cpp)
target_compile_features(FLCatch PUBLIC ${FALAISE_CXX_COMPILE_FEATURES})
target_include_directories(FLCatch PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/catch)
target_link_libraries(FLCatch PUBLIC Falaise)
