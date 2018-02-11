
add_library(nsga2_construction SHARED ArgosJNI.cpp)

target_link_libraries(nsga2_construction
        argos3core_simulator
        footbot_construction
        construction_loop_functions)

find_package(JNI REQUIRED)
include_directories(${CMAKE_SOURCE_DIR} ${JNI_INCLUDE_DIRS})

SET( CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS}" )

