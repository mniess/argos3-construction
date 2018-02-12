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
  TConfigurationNode& tNSGA2 = GetNode(t_node, "nsga2");

  GetNodeAttribute(tConstruction, "buildingRange", m_sConstructionParams.buildingRange);
  GetNodeAttribute(tConstruction, "arenaX", m_sConstructionParams.arenaX);
  GetNodeAttribute(tConstruction, "arenaY", m_sConstructionParams.arenaY);
  GetNodeAttribute(tConstruction, "numRobots", m_sConstructionParams.numRobots);

  /* Create a new RNG */
  CRandom::CRNG *m_pcRNG = CRandom::CreateRNG("argos");
  for (int i = 0; i < m_sConstructionParams.numRobots; ++i) {
    std::string id = "fb" + ToString(i);
    CVector3 pos = CVector3(m_pcRNG->Uniform(m_sConstructionParams.arenaX),m_pcRNG->Uniform(m_sConstructionParams.arenaY),0);
    CQuaternion cq_orientation;
    CRadians orientation = m_pcRNG->Uniform(CRadians::UNSIGNED_RANGE);
    cq_orientation.FromEulerAngles(orientation,CRadians::ZERO,CRadians::ZERO);

    CFootBotEntity* m_pcFootBot = new CFootBotEntity(id,"ffc",pos,cq_orientation);
    AddEntity(*m_pcFootBot);
  }
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
  if(m_sConstructionParams.buildingRange.WithinMinBoundIncludedMaxBoundIncluded(c_position_on_plane.Length()))
    return CColor::YELLOW;
  return CColor::WHITE;
}

/****************************************/
/****************************************/

void CConstructionLoopFunctions::PreStep() {
  /* Enable all Cylinders*/
  CSpace::TMapPerType &tCMap = GetSpace().GetEntitiesByType("cylinder");
  for (CSpace::TMapPerType::iterator it = tCMap.begin(); it != tCMap.end(); ++it) {
    /* Create a pointer to the current cylinder */
    CCylinderEntity *pcC = any_cast<CCylinderEntity *>(it->second);
    pcC->GetLEDEquippedEntity().SetAllLEDsColors(CColor::PURPLE);
  }

  /* Disable gripped Cylinders*/
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

void CConstructionLoopFunctions::ConfigureFromGenome(){

}

Real CConstructionLoopFunctions::Performance() {
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
    if(m_sConstructionParams.buildingRange.WithinMinBoundIncludedMaxBoundIncluded(cylinderPos.Length())) {
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