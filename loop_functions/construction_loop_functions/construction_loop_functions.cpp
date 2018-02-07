#include <argos3/plugins/robots/foot-bot/simulator/footbot_entity.h>
#include "construction_loop_functions.h"
#include <argos3/plugins/simulator/entities/gripper_equipped_entity.h>
#include <argos3/plugins/simulator/entities/cylinder_entity.h>

#include <list>
#include <algorithm>

/****************************************/
/****************************************/

CConstructionLoopFunctions::CConstructionLoopFunctions() {

}

/****************************************/
/****************************************/

void CConstructionLoopFunctions::Init(TConfigurationNode &t_node) {
  TConfigurationNode& tConstruction = GetNode(t_node, "construction");
  GetNodeAttribute(tConstruction, "buildingRadius", buildingRadius);

  /* Create a new RNG */
  //m_pcRNG = CRandom::CreateRNG("argos");

}

/****************************************/
/****************************************/

void CConstructionLoopFunctions::Reset() {
}

/****************************************/
/****************************************/

void CConstructionLoopFunctions::Destroy() {
}

void CConstructionLoopFunctions::PostExperiment(){
  Performance();
}

/****************************************/
/*************************************a***/

CColor CConstructionLoopFunctions::GetFloorColor(const CVector2 &c_position_on_plane) {
  float a = 0.25;
  if(c_position_on_plane.Length() < buildingRadius+a && c_position_on_plane.Length() > buildingRadius-a)
    return CColor::YELLOW;
  return CColor::WHITE;
}

/****************************************/
/****************************************/

void CConstructionLoopFunctions::PreStep() {
  CSpace::TMapPerType &tCMap = GetSpace().GetEntitiesByType("cylinder");
  for (CSpace::TMapPerType::iterator it = tCMap.begin(); it != tCMap.end(); ++it) {
    /* Create a pointer to the current cylinder */
    CCylinderEntity *pcC = any_cast<CCylinderEntity *>(it->second);
    pcC->GetLEDEquippedEntity().SetAllLEDsColors(CColor::PURPLE);

  }

  CSpace::TMapPerType &tFBMap = GetSpace().GetEntitiesByType("foot-bot");
  /* Go through them */
  for (CSpace::TMapPerType::iterator it = tFBMap.begin(); it != tFBMap.end(); ++it) {
    /* Create a pointer to the current foot-bot */
    CFootBotEntity *pcFB = any_cast<CFootBotEntity *>(it->second);
    CGripperEquippedEntity &cGEE = pcFB->GetGripperEquippedEntity();
    LOG << cGEE.GetDirection() << std::endl;
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

void CConstructionLoopFunctions::ConfigureFromGenome(){

}

Real CConstructionLoopFunctions::Performance() {
  CRange<Real> vaildCylinderRange = CRange<Real>(buildingRadius-0.2,buildingRadius+0.2);
  std::list<CVector2> validCylinders;
  Real rayCastHit = 0;
  CSpace::TMapPerType &tCMap = GetSpace().GetEntitiesByType("cylinder");
  Real c_radius = any_cast<CCylinderEntity *>(tCMap.begin()->second)->GetRadius();
  /*find valid cylinders*/
  for (CSpace::TMapPerType::iterator it = tCMap.begin(); it != tCMap.end(); ++it) {
    /* Create a pointer to the current cylinder */
    CCylinderEntity *pcC = any_cast<CCylinderEntity *>(it->second);
    /*get position of cylinder*/
    CVector2 cylinderPos;

    pcC->GetEmbodiedEntity().GetOriginAnchor().Position.ProjectOntoXY(cylinderPos);
    LOG << pcC->GetContext();
    if(vaildCylinderRange.WithinMinBoundIncludedMaxBoundIncluded(cylinderPos.Length())) {
      validCylinders.push_back(cylinderPos);
    }
  }
  LOG << validCylinders.size() << " Cylinders in Range; " ;
  /*Do a raycast to every degree*/
  for (int i = 0; i < 360; ++i) {
    CRadians rayCastAngle = ToRadians(CDegrees(i));
    CVector2 rayCast = CVector2(1,rayCastAngle);
    /* Check if any valid cylinder intersect the raycast*/
    for (CVector2 currCyl:validCylinders) {
      Real dot = currCyl.DotProduct(rayCast);
      if(dot < 0) {
        continue;
      }
      CVector2 nearesPoint = CVector2(dot,rayCastAngle);
      Real distance = CVector2(nearesPoint.GetX()-currCyl.GetX(),nearesPoint.GetY()-currCyl.GetY()).Length();
      if(distance < c_radius){
        rayCastHit+=1;
        break;
      }
    }
  }
  LOG << "Hits: " << rayCastHit<< " (" << rayCastHit/360 << ")"<<std::endl;
  return rayCastHit/360.0;
  return 0;
}

/****************************************/
/****************************************/

REGISTER_LOOP_FUNCTIONS(CConstructionLoopFunctions, "construction_loop_functions")
