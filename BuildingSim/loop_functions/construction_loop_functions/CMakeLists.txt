link_directories(${CMAKE_BINARY_DIR}/controllers/construction_construction)
set(construction_loop_functions_SOURCES
        ArgosControl.cpp
        construction_loop_functions.cpp)

if (ARGOS_COMPILE_QTOPENGL)
    set(construction_loop_functions_SOURCES
            ${construction_loop_functions_SOURCES}
            construction_qt_user_functions.cpp)
endif (ARGOS_COMPILE_QTOPENGL)

add_library(construction_loop_functions SHARED ${construction_loop_functions_SOURCES})
target_link_libraries(construction_loop_functions
        footbot_construction
        argos3core_simulator
        argos3plugin_simulator_dynamics2d
        argos3plugin_simulator_entities
        argos3plugin_simulator_footbot
        argos3plugin_simulator_genericrobot
        argos3plugin_simulator_media)

add_executable(argosControl ArgosControl.cpp)
target_link_libraries(argosControl
        construction_loop_functions
        footbot_construction
        argos3core_simulator
        argos3plugin_simulator_dynamics2d
        argos3plugin_simulator_entities
        argos3plugin_simulator_footbot
        argos3plugin_simulator_genericrobot
        argos3plugin_simulator_media)

if (ARGOS_COMPILE_QTOPENGL)
    target_link_libraries(argosControl argos3plugin_simulator_qtopengl)
endif (ARGOS_COMPILE_QTOPENGL)