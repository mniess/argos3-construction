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

void CConstructionLoopFunctions::Init(TConfigurationNode &t_node) {

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
/*************************************a***/

CColor CConstructionLoopFunctions::GetFloorColor(const CVector2 &c_position_on_plane) {
  return CColor::WHITE;
}

/****************************************/
/****************************************/

void CConstructionLoopFunctions::PreStep() {
  CSpace::TMapPerType &tFBMap = GetSpace().GetEntitiesByType("foot-bot");
  /* Go through them */
  for (CSpace::TMapPerType::iterator it = tFBMap.begin(); it != tFBMap.end(); ++it) {
    /* Create a pointer to the current foot-bot */
    CFootBotEntity *pcFB = any_cast<CFootBotEntity *>(it->second);
    CGripperEquippedEntity &cGEE = pcFB->GetGripperEquippedEntity();

    if (cGEE.IsGripping()) {
      /* Get the gripped entity*/
      CEntity &cE = cGEE.GetGrippedEntity().GetRootEntity();

      if ("cylinder" == cE.GetTypeDescription()) {
        /* cast to Cylinder*/
        CCylinderEntity &cylinder = dynamic_cast<CCylinderEntity&>(cE);
        cylinder.GetLEDEquippedEntity().SetAllLEDsColors(CColor::BLACK);
      }
    }
  }
}

/****************************************/
/****************************************/

REGISTER_LOOP_FUNCTIONS(CConstructionLoopFunctions, "construction_loop_functions")
