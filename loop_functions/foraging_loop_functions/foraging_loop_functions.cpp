#include "foraging_loop_functions.h"

/****************************************/
/****************************************/

CForagingLoopFunctions::CForagingLoopFunctions() {

}

/****************************************/
/****************************************/

void CForagingLoopFunctions::Init(TConfigurationNode& t_node) {
}

/****************************************/
/****************************************/

void CForagingLoopFunctions::Reset() {
}

/****************************************/
/****************************************/

void CForagingLoopFunctions::Destroy() {
}

/****************************************/
/****************************************/

CColor CForagingLoopFunctions::GetFloorColor(const CVector2& c_position_on_plane) {
   return CColor::WHITE;
}

/****************************************/
/****************************************/

void CForagingLoopFunctions::PreStep() {

}

/****************************************/
/****************************************/

REGISTER_LOOP_FUNCTIONS(CForagingLoopFunctions, "foraging_loop_functions")
