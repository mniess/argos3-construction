#include <argos3/plugins/robots/foot-bot/simulator/footbot_entity.h>
#include "construction_loop_functions.h"
#include <argos3/plugins/simulator/entities/gripper_equipped_entity.h>
#include <argos3/plugins/simulator/entities/cylinder_entity.h>

/****************************************/
/****************************************/

CConstructionLoopFunctions::CConstructionLoopFunctions() {

}

/****************************************/
/****************************************/

void CConstructionLoopFunctions::Init(TConfigurationNode& t_node) {

}

/****************************************/
/****************************************/

void CConstructionLoopFunctions::Reset() {
}

/****************************************/
/****************************************/

void CConstructionLoopFunctions::Destroy() {
}

/****************************************/
/****************************************/

CColor CConstructionLoopFunctions::GetFloorColor(const CVector2& c_position_on_plane) {
   return CColor::WHITE;
}

/****************************************/
/****************************************/

void CConstructionLoopFunctions::PreStep() {
    CSpace::TMapPerType& tFBMap = GetSpace().GetEntitiesByType("foot-bot");
    /* Go through them */
    for(CSpace::TMapPerType::iterator it = tFBMap.begin(); it != tFBMap.end(); ++it) {
        /* Create a pointer to the current foot-bot */
        CFootBotEntity* pcFB = any_cast<CFootBotEntity*>(it->second);
        CGripperEquippedEntity e = pcFB->GetGripperEquippedEntity();
        if(e.IsGripping()) {
            //CEmbodiedEntity eme = e.GetGrippedEntity();
            //if("cylinder" == eme.GetTypeDescription() ){
                //CCylinderEntity cylinder = any_cast<CCylinderEntity>(eme);
                //cylinder.DisableLEDs();
            //}
        }
    }
}

/****************************************/
/****************************************/

REGISTER_LOOP_FUNCTIONS(CConstructionLoopFunctions, "construction_loop_functions")
