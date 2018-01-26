add_library(footbot_construction SHARED footbot_construction.h footbot_construction.cpp)
target_link_libraries(footbot_construction
        argos3core_simulator
        argos3plugin_simulator_footbot
        argos3plugin_simulator_genericrobot)
