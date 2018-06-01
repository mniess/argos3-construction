#include "footbot_construction.h"
#include <argos3/core/simulator/physics_engine/physics_engine.h>

/****************************************/
CFootBotConstruction::SDiffusionParams::SDiffusionParams() :
    GoStraightAngleRange(CRadians(-1.0f), CRadians(1.0f)) {}

void CFootBotConstruction::SDiffusionParams::Init(TConfigurationNode &t_node) {
  try {
    CRange<CDegrees> cGoStraightAngleRangeDegrees(CDegrees(-10.0f), CDegrees(10.0f));
    GetNodeAttribute(t_node, "go_straight_angle_range", cGoStraightAngleRangeDegrees);
    GoStraightAngleRange.Set(ToRadians(cGoStraightAngleRangeDegrees.GetMin()),
                             ToRadians(cGoStraightAngleRangeDegrees.GetMax()));
    GetNodeAttribute(t_node, "delta", Delta);
  }
  catch (CARGoSException &ex) {
    THROW_ARGOSEXCEPTION_NESTED("Error initializing controller diffusion parameters.", ex);
  }
}

void CFootBotConstruction::SWheelTurningParams::Init(TConfigurationNode &t_node) {
  try {
    TurningMechanism = NO_TURN;
    CDegrees cAngle;
    GetNodeAttribute(t_node, "hard_turn_angle_threshold", cAngle);
    HardTurnOnAngleThreshold = ToRadians(cAngle);
    GetNodeAttribute(t_node, "soft_turn_angle_threshold", cAngle);
    SoftTurnOnAngleThreshold = ToRadians(cAngle);
    GetNodeAttribute(t_node, "no_turn_angle_threshold", cAngle);
    NoTurnAngleThreshold = ToRadians(cAngle);
    GetNodeAttribute(t_node, "max_speed", MaxSpeed);
  }
  catch (CARGoSException &ex) {
    THROW_ARGOSEXCEPTION_NESTED("Error initializing controller wheel turning parameters.", ex);
  }
}

CFootBotConstruction::SStateData::SStateData() = default;

void CFootBotConstruction::SStateData::Init(TConfigurationNode &t_node) {
  //State will be set by SetState() function
  Reset();
}

void CFootBotConstruction::SStateData::Reset() {
  Action = SStateData::NOACTION;
  State = SStateData::STATE_EXPLORE;
  //State will be set by SetState() function
}

bool CFootBotConstruction::SRule::Switch(Real light, int ticks, bool seesCylinder) {
  bool timeRule = minTicksInState < ticks;
  bool cylinderRule = cylinderInRange == 0
      || cylinderInRange == seesCylinder; // 0: indifferent, -1 must not see cylinder, 1 must see cylinder
  bool lightRule = maxLight < minLight || (light < maxLight && light > minLight);
  return timeRule && cylinderRule && lightRule;
}

/****************************************/
/****************************************/

CFootBotConstruction::CFootBotConstruction() :
    m_pcWheels(nullptr),
    m_pcLEDs(nullptr),
    m_pcGripper(nullptr),
    m_pcProximity(nullptr),
    m_pcLight(nullptr),
    m_pcCamera(nullptr),
    m_pcRNG(nullptr) {}

/****************************************/

void CFootBotConstruction::Init(TConfigurationNode &t_node) {
  try {
    m_pcWheels = GetActuator<CCI_DifferentialSteeringActuator>("differential_steering");
    m_pcLEDs = GetActuator<CCI_LEDsActuator>("leds");
    m_pcGripper = GetActuator<CCI_FootBotGripperActuator>("footbot_gripper");
    m_pcTurret = GetActuator<CCI_FootBotTurretActuator>("footbot_turret");
    m_pcProximity = GetSensor<CCI_FootBotProximitySensor>("footbot_proximity");
    m_pcLight = GetSensor<CCI_FootBotLightSensor>("footbot_light");
    m_pcCamera = GetSensor<CCI_ColoredBlobOmnidirectionalCameraSensor>("colored_blob_omnidirectional_camera");
    m_pcTurretEnc = GetSensor<CCI_FootBotTurretEncoderSensor>("footbot_turret_encoder");
    /* Diffusion algorithm */
    m_sDiffusionParams.Init(GetNode(t_node, "diffusion"));
    /* Wheel turning */
    m_sWheelTurningParams.Init(GetNode(t_node, "wheel_turning"));
    m_sStateData.Init(GetNode(t_node, "state"));
  }
  catch (CARGoSException &ex) {
    THROW_ARGOSEXCEPTION_NESTED(
        "Error initializing the foot-bot construction controller for robot \"" << GetId() << "\"", ex);
  }

  /* Create a random number generator. We use the 'argos' category so
     that creation, reset, seeding and cleanup are managed by ARGoS. */
  m_pcRNG = CRandom::CreateRNG("argos");

  Reset();
}

void CFootBotConstruction::Reset() {
  m_sStateData.Reset();

  m_pcLEDs->SetAllColors(CColor::WHITE);

  GripperLocked = false;
  m_pcGripper->Unlock();

  m_pcCamera->Enable();

  m_pcTurret->SetPositionControlMode();
  m_pcTurret->SetRotation(CRadians::ZERO);
}

void CFootBotConstruction::ControlStep() {
  if (m_sStateData.Action != SStateData::NOACTION) { //Actions have Priority
    bool success = false;
    switch (m_sStateData.Action) {
    case SStateData::ACTION_PICKUP: {
      success = PickUpAction();
      break;
    }
    case SStateData::ACTION_DROP: {
      success = DropAction();
      break;
    }
    default: {
      LOGERR << "Action not defined, there's a bug!" << std::endl;
    }
    }

    if (success) {
      m_sStateData.Action = SStateData::NOACTION;
    }

  } else { // Perform Movement and check whether to switch
    m_sStateData.TicksInState++;
    switch (m_sStateData.State) {
    case SStateData::STATE_EXPLORE: {
      Explore();
      if (ExplorePhototaxisRule.Switch(LightIntensity(), m_sStateData.TicksInState, seesCylinder())) {
        SetState(SStateData::STATE_PHOTOTAXIS);
        m_sStateData.Action = ExplorePhototaxisRule.drop ? SStateData::ACTION_DROP : SStateData::ACTION_PICKUP;
      }
      break;
    }
    case SStateData::STATE_PHOTOTAXIS: {
      Phototaxis();
      if (PhototaxisAntiphototaxisRule.Switch(LightIntensity(), m_sStateData.TicksInState, seesCylinder())) {
        SetState(SStateData::STATE_ANTIPHOTOTAXIS);
        m_sStateData.Action = PhototaxisAntiphototaxisRule.drop ? SStateData::ACTION_DROP : SStateData::ACTION_PICKUP;
      }
      break;
    }
    case SStateData::STATE_ANTIPHOTOTAXIS: {
      AntiPhototaxis();
      if (AntiPhototaxisExploreRule.Switch(LightIntensity(), m_sStateData.TicksInState, seesCylinder())) {
        SetState(SStateData::STATE_EXPLORE);
        m_sStateData.Action = AntiPhototaxisExploreRule.drop ? SStateData::ACTION_DROP : SStateData::ACTION_PICKUP;
      }
      break;
    }
    default: {
      LOGERR << "State not defined, there's a bug!" << std::endl;
    }
    }
  }
}

/****************************************/

CVector2 CFootBotConstruction::LightVector() {
  /* Get readings from light sensor */
  const CCI_FootBotLightSensor::TReadings &tLightReads = m_pcLight->GetReadings();
  if (tLightReads.empty()) {
    return CVector2(); //return zero vector
  }
  /* Sum them together */
  CVector2 cAccumulator;
  Real maxVal = 0;
  for (auto tLightRead : tLightReads) {
    cAccumulator += CVector2(tLightRead.Value, tLightRead.Angle);
    if (maxVal < tLightRead.Value)
      maxVal = tLightRead.Value;
  }
  return maxVal * cAccumulator.Normalize();
}

CVector2 CFootBotConstruction::DiffusionVector(bool *b_collision) {
  /* Get readings from proximity sensor */
  const CCI_FootBotProximitySensor::TReadings &tProxReads = m_pcProximity->GetReadings();
  /* Sum them together */
  CVector2 cDiffusionVector;
  for (auto tProxRead : tProxReads) {
    cDiffusionVector += CVector2(tProxRead.Value, tProxRead.Angle);
  }
  /* If the angle of the vector is small enough and the closest obstacle
     is far enough, ignore the vector and go straight, otherwise return
     it */
  if (m_sDiffusionParams.GoStraightAngleRange.WithinMinBoundIncludedMaxBoundIncluded(cDiffusionVector.Angle()) &&
      cDiffusionVector.Length() < m_sDiffusionParams.Delta) {
    if (b_collision != nullptr)
      *b_collision = false;
    return CVector2::X;
  } else {
    if (b_collision != nullptr)
      *b_collision = true;
    cDiffusionVector.Normalize();
    return -cDiffusionVector;
  }
}

CVector2 CFootBotConstruction::LedVector(CColor color) const {
  CVector2 ledV;
  const CCI_ColoredBlobOmnidirectionalCameraSensor::SReadings camReadings = m_pcCamera->GetReadings();
  if (camReadings.BlobList.empty()) {
    return ledV;
  }
  for (auto &blob : camReadings.BlobList) {
    if (blob->Color == color) {
      CVector2 blobV(blob->Distance, blob->Angle);
      if (blobV.Length() + 1 < ledV.Length() || ledV.Length() == 0) {
        ledV = blobV;
      }
    }
  }

  return ledV;
}

CVector2 CFootBotConstruction::RandomVector(int minDeg = 0, int maxDeg = 360) {
  const CRange<SInt32> range = CRange<SInt32>(minDeg, maxDeg);
  int rndDeg = m_pcRNG->Uniform(range);
  return CVector2(1, ToRadians(CDegrees(rndDeg)));
}

/****************************************/

bool CFootBotConstruction::HasItem() {
  return GripperLocked;
}

bool CFootBotConstruction::seesCylinder() {
  return !m_pcCamera->GetReadings().BlobList.empty();
}

Real CFootBotConstruction::LightIntensity() {
  const CCI_FootBotLightSensor::TReadings &tLightReads = m_pcLight->GetReadings();
  Real maxVal = 0;
  for (auto tLightRead : tLightReads) {
    if (maxVal < tLightRead.Value)
      maxVal = tLightRead.Value;
  }
  return maxVal;
}

void CFootBotConstruction::SetWheelSpeeds(const CVector2 &c_heading) {
  /* Get the heading angle */
  CRadians cHeadingAngle = c_heading.Angle().SignedNormalize();
  /* Get the length of the heading vector */
  Real fHeadingLength = c_heading.Length();
  /* Clamp the speed so that it's not greater than MaxSpeed */
  Real fBaseAngularWheelSpeed = Min<Real>(fHeadingLength, m_sWheelTurningParams.MaxSpeed);
  /* State transition logic */
  if (m_sWheelTurningParams.TurningMechanism == SWheelTurningParams::HARD_TURN) {
    if (Abs(cHeadingAngle) <= m_sWheelTurningParams.SoftTurnOnAngleThreshold) {
      m_sWheelTurningParams.TurningMechanism = SWheelTurningParams::SOFT_TURN;
    }
  }
  if (m_sWheelTurningParams.TurningMechanism == SWheelTurningParams::SOFT_TURN) {
    if (Abs(cHeadingAngle) > m_sWheelTurningParams.HardTurnOnAngleThreshold) {
      m_sWheelTurningParams.TurningMechanism = SWheelTurningParams::HARD_TURN;
    } else if (Abs(cHeadingAngle) <= m_sWheelTurningParams.NoTurnAngleThreshold) {
      m_sWheelTurningParams.TurningMechanism = SWheelTurningParams::NO_TURN;
    }
  }
  if (m_sWheelTurningParams.TurningMechanism == SWheelTurningParams::NO_TURN) {
    if (Abs(cHeadingAngle) > m_sWheelTurningParams.HardTurnOnAngleThreshold) {
      m_sWheelTurningParams.TurningMechanism = SWheelTurningParams::HARD_TURN;
    } else if (Abs(cHeadingAngle) > m_sWheelTurningParams.NoTurnAngleThreshold) {
      m_sWheelTurningParams.TurningMechanism = SWheelTurningParams::SOFT_TURN;
    }
  }
  /* Wheel speeds based on current turning state */
  Real fSpeed1 = 0, fSpeed2 = 0;
  switch (m_sWheelTurningParams.TurningMechanism) {
  case SWheelTurningParams::NO_TURN: {
    /* Just go straight */
    fSpeed1 = fBaseAngularWheelSpeed;
    fSpeed2 = fBaseAngularWheelSpeed;
    break;
  }
  case SWheelTurningParams::SOFT_TURN: {
    /* Both wheels go straight, but one is faster than the other */
    Real fSpeedFactor = (m_sWheelTurningParams.HardTurnOnAngleThreshold - Abs(cHeadingAngle))
        / m_sWheelTurningParams.HardTurnOnAngleThreshold;
    fSpeed1 = fBaseAngularWheelSpeed - fBaseAngularWheelSpeed * (1.0 - fSpeedFactor);
    fSpeed2 = fBaseAngularWheelSpeed + fBaseAngularWheelSpeed * (1.0 - fSpeedFactor);
    break;
  }
  case SWheelTurningParams::HARD_TURN: {
    /* Opposite wheel speeds */
    fSpeed1 = -m_sWheelTurningParams.MaxSpeed;
    fSpeed2 = m_sWheelTurningParams.MaxSpeed;
    break;
  }
  }
  /* Apply the calculated speeds to the appropriate wheels */
  Real fLeftWheelSpeed, fRightWheelSpeed;
  if (cHeadingAngle > CRadians::ZERO) {
    /* Turn Left */
    fLeftWheelSpeed = fSpeed1;
    fRightWheelSpeed = fSpeed2;
  } else {
    /* Turn Right */
    fLeftWheelSpeed = fSpeed2;
    fRightWheelSpeed = fSpeed1;
  }
  /* Finally, set the wheel speeds */
  m_pcWheels->SetLinearVelocity(fLeftWheelSpeed, fRightWheelSpeed);
}

/****************************************/

void CFootBotConstruction::Explore() {
  int straightwalkTime = 2; // in seconds
  bool collission;

  randomWalkCounter++;

  if (randomWalkCounter >= CPhysicsEngine::GetInverseSimulationClockTick() * straightwalkTime) {
    currMoveV = RandomVector(-45, 45);
    randomWalkCounter = 0;
  }
  CVector2 cDiff = DiffusionVector(&collission);
  if (collission && !HasItem()) {
    SetWheelSpeeds(m_sWheelTurningParams.MaxSpeed * cDiff);
  } else {
    SetWheelSpeeds(m_sWheelTurningParams.MaxSpeed * currMoveV.Normalize());
  }
}

void CFootBotConstruction::Phototaxis() {
  bool collision;

  CVector2 cDiff = DiffusionVector(&collision);
  CVector2 lightV = LightVector();

  if (lightV.SquareLength() == 0) {
    Explore();
  } else if (collision && !HasItem()) {
    SetWheelSpeeds(m_sWheelTurningParams.MaxSpeed * cDiff);
  } else {
    SetWheelSpeeds(m_sWheelTurningParams.MaxSpeed * (lightV.Normalize() + 0.5 * RandomVector(-45, 0)));
  }
}

void CFootBotConstruction::AntiPhototaxis() {
  bool collision;

  CVector2 cDiff = DiffusionVector(&collision);
  CVector2 lightV = LightVector();

  if (lightV.SquareLength() == 0) {
    Explore();
  } else if (collision && !HasItem()) {
    SetWheelSpeeds(m_sWheelTurningParams.MaxSpeed * cDiff);
  } else {
    SetWheelSpeeds(m_sWheelTurningParams.MaxSpeed * -lightV.Normalize());
  }
}

/****************************************/

bool CFootBotConstruction::DropAction() {
  GripperLocked = false;
  m_pcGripper->Unlock();
  m_pcTurret->SetPositionControlMode();
  m_pcTurret->SetRotation(CRadians::ZERO);
  return true;
}

bool CFootBotConstruction::PickUpAction() {
  if (HasItem()) { //Do not attempt to pickup another cylinder
    return true;
  }
  CVector2 cCCyl = LedVector(CColor().PURPLE);

  if (cCCyl.Length() != 0) { // Cylinder detected
    if (cCCyl.Length() < 10) { // Cylinder is within reach, grip it!
      if (cCCyl.Angle() < ToRadians(CDegrees(2)) && cCCyl.Angle() > ToRadians(CDegrees(-2))) {
        GripperLocked = true;
        m_pcGripper->LockPositive();
        m_pcTurret->SetPassiveMode();
        return true;
      } else {
        int rotDir = cCCyl.Angle() < CRadians::ZERO ? 1 : -1;
        m_pcWheels->SetLinearVelocity(rotDir, -rotDir);
        return false;
      }
    } else { //move towards cylinder
      SetWheelSpeeds(m_sWheelTurningParams.MaxSpeed * cCCyl.Normalize());
      return false;
    }
  } else {
    /* if no cylinder found don't block*/
    return true;
  }
}

/****************************************/

void CFootBotConstruction::SetState(SStateData::EState newState) {
  switch (newState) {
  case SStateData::STATE_EXPLORE: {
    m_pcLEDs->SetAllColors(CColor::WHITE);
    break;
  }
  case SStateData::STATE_PHOTOTAXIS: {
    m_pcLEDs->SetAllColors(CColor::GREEN);
    break;
  }
  case SStateData::STATE_ANTIPHOTOTAXIS: {
    m_pcLEDs->SetAllColors(CColor::BLUE);
    break;
  }
  default: {
    LOGERR << "We can't be here, there's a bug!" << std::endl;
  }
  }

  randomWalkCounter = 0;
  m_sStateData.TicksInState = 0;
  m_sStateData.State = newState;
}

void CFootBotConstruction::SetRules(int rules[], std::string genomeType) {
  int maxLightGene = 4; // maxvalue allowed in gene
  int maxTimeGene = 4; // maxvalue allowed in gene
  int minLight = 0;
  Real maxLight = 1; // maximum the light sensor detects
  Real maxTime = 10; // maximum time in seconds
  Real LightFactor = maxLight / maxLightGene;
  Real TimeFactor = (maxTime / maxTimeGene) * CPhysicsEngine::GetInverseSimulationClockTick();
  //SRule(int minT, int cInR, int minL, int maxL, bool d)
  if (genomeType.find("full") != std::string::npos || genomeType.find("Full") != std::string::npos) {
    AntiPhototaxisExploreRule = SRule(rules[0]* TimeFactor,rules[1],rules[2] * LightFactor, rules[3] * LightFactor, rules[4] == 1);
    ExplorePhototaxisRule = SRule(rules[5]* TimeFactor,rules[6],rules[7] * LightFactor, rules[8] * LightFactor, rules[9] == 1);
    PhototaxisAntiphototaxisRule = SRule(rules[10]* TimeFactor,rules[11],rules[12] * LightFactor, rules[13] * LightFactor, rules[14] == 1);
  } else {
    AntiPhototaxisExploreRule = SRule(rules[0] * TimeFactor, 0, minLight, maxLight, rules[1] == 1);
    ExplorePhototaxisRule = SRule(rules[2] * TimeFactor, rules[3], minLight, maxLight, rules[4] == 1);
    PhototaxisAntiphototaxisRule = SRule(0, 0, rules[5] * LightFactor, rules[6] * LightFactor, rules[7] == 1);
  }
}

REGISTER_CONTROLLER(CFootBotConstruction, "footbot_construction_controller")

