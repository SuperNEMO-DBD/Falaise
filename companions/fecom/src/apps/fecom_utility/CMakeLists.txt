
add_definitions(-std=c++11)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${PROJECT_BINARY_DIR}/src/libs/libfecom)
include_directories(${PROJECT_SOURCE_DIR}/src/libs/libfecom)

add_executable(fecom_fake_hit_generator fake_hit_generator.cxx)
target_link_libraries(fecom_fake_hit_generator fecom)

add_executable(fecom_fake_hit_generator_2 fake_hit_generator_2.cxx)
target_link_libraries(fecom_fake_hit_generator_2 fecom)

# - end of CMakeLists.txt
