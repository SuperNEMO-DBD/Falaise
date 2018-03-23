include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${PROJECT_BINARY_DIR}/src/libs/libfecom)
include_directories(${PROJECT_SOURCE_DIR}/src/libs/libfecom)

add_executable(flrawana rawana.cxx)
target_link_libraries(flrawana fecom)
target_clang_format(flrawana)
# - end of CMakeLists.txt
