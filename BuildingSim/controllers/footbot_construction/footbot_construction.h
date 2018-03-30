/*
 *
 * This controller is meant to be used with the XML file:
 *    experiments/construction.argos
 */

#ifndef FOOTBOT_CONSTRUCTION_H
#define FOOTBOT_CONSTRUCTION_H

/*
 * Include some necessary headers.
 */
#include <argos3/core/control_interface/ci_controller.h>
#include <argos3/core/utility/math/rng.h>
#include <argos3/core/utility/configuration/argos_configuration.h>
#include <argos3/plugins/robots/generic/control_interface/ci_differential_steering_actuator.h>
#include <argos3/plugins/robots/generic/control_interface/ci_leds_actuator.h>
#include <argos3/plugins/robots/generic/control_interface/ci_range_and_bearing_actuator.h>
#include <argos3/plugins/robots/foot-bot/control_interface/ci_footbot_gripper_actuator.h>
#include <argos3/plugins/robots/foot-bot/control_interface/ci_footbot_turret_actuator.h>
#include <argos3/plugins/robots/generic/control_interface/ci_range_and_bearing_sensor.h>
#include <argos3/plugins/robots/foot-bot/control_interface/ci_footbot_proximity_sensor.h>
#include <argos3/plugins/robots/foot-bot/control_interface/ci_footbot_light_sensor.h>
#include <argos3/plugins/robots/foot-bot/control_interface/ci_footbot_motor_ground_sensor.h>
#include <argos3/plugins/robots/foot-bot/control_interface/ci_footbot_turret_encoder_sensor.h>
#include <argos3/plugins/robots/generic/control_interface/ci_colored_blob_omnidirectional_camera_sensor.h>

/*
 * All the ARGoS stuff in the 'argos' namespace.
 * With this statement, you save typing argos:: every time.
 */
using namespace argos;

/*
 * A controller is simply an implementation of the CCI_Controller class.
 */
class CFootBotConstruction : public CCI_Controller {

public:
  /*
  * The following variables are used as parameters for the
  * diffusion algorithm. You can set their value in the <parameters>
  * section of the XML configuration file, under the
  * <controllers><footbot_construction_controller><parameters><diffusion>
  * section.
  */
  struct SDiffusionParams {
    /*
     * Maximum tolerance for the proximity reading between
     * the robot and the closest obstacle.
     * The proximity reading is 0 when nothing is detected
     * and grows exponentially to 1 when the obstacle is
     * touching the robot.
     */
    Real Delta;
    /* Angle tolerance range to go straight. */
    CRange<CRadians> GoStraightAngleRange;

    /* Constructor */
    SDiffusionParams();

    /* Parses the XML section for diffusion */
    void Init(TConfigurationNode &t_tree);
  };

  /*
   * The following variables are used as parameters for
   * turning during navigation. You can set their value
   * in the <parameters> section of the XML configuration
   * file, under the
   * <controllers><footbot_construction_controller><parameters><wheel_turning>
   * section.
   */
  struct SWheelTurningParams {
    /*
     * The turning mechanism.
     * The robot can be in three different turning states.
     */
    enum ETurningMechanism {
      NO_TURN = 0, // go straight
      SOFT_TURN,   // both wheels are turning forwards, but at different speeds
      HARD_TURN    // wheels are turning with opposite speeds
    } TurningMechanism;
    /*
     * Angular thresholds to change turning state.
     */
    CRadians HardTurnOnAngleThreshold;
    CRadians SoftTurnOnAngleThreshold;
    CRadians NoTurnAngleThreshold;
    /* Maximum wheel speed */
    Real MaxSpeed;

    void Init(TConfigurationNode &t_tree);
  };

  /*
   * Contains all the state information about the controller.
   */
  struct SStateData {
    enum EState {
      STATE_PHOTOTAXIS = 0,
      STATE_ANTIPHOTOTAXIS,
      STATE_EXPLORE
    } State = STATE_EXPLORE;

    enum EAction {
      NOACTION = 0,
      ACTION_PICKUP,
      ACTION_DROP
    } Action = NOACTION;

    int TicksInState = 0;
    SStateData();
    void Init(TConfigurationNode &t_node);
    void Reset();
  };

  struct SRule {
    int minTicksInState;
    int cylinderInRange; //-1 must not be in Range, 1 must be in Range, 0 inactive
    Real minLight;
    Real maxLight;
    int drop;
    bool Switch(Real light, int ticks, bool seesCylinder);
    SRule() : minTicksInState(0), cylinderInRange(0), minLight(0), maxLight(1), drop(1) {}
    SRule(int minT, int cInR, Real minL, Real maxL, bool d)
        : minTicksInState(minT), cylinderInRange(cInR), minLight(minL), maxLight(maxL), drop(d) {}
  };

public:

  /* Class constructor. */
  CFootBotConstruction();
  /* Class destructor. */
  virtual ~CFootBotConstruction() {}

  /*
   * This function initializes the controller.
   * The 't_node' variable points to the <parameters> section in the XML
   * file in the <controllers><footbot_construction_controller> section.
   */
  virtual void Init(TConfigurationNode &t_node);

  /*
   * This function is called once every time step.
   * The length of the time step is set in the XML file.
   */
  virtual void ControlStep();

  /*
   * This function resets the controller to its state right after the
   * Init().
   * It is called when you press the reset button in the GUI.
   */
  virtual void Reset();

  /*
   * Called to cleanup what done by Init() when the experiment finishes.
   * In this example controller there is no need for clean anything up,
   * so the function could have been omitted. It's here just for
   * completeness.
   */
  virtual void Destroy() {}

  void SetRules(int rules[], std::string genomeType);

  SStateData::EState GetState() { return m_sStateData.State; };
  SStateData::EAction GetAction() { return m_sStateData.Action; };

private:

  /*
   * Calculates the vector to the light. Used to perform
   * phototaxis and antiphototaxis.
   */
  CVector2 LightVector();

  /*
   * Calculates the diffusion vector. If there is a close obstacle,
   * it points away from it; it there is none, it points forwards.
   * The b_collision parameter is used to return true or false whether
   * a collision avoidance just happened or not. It is necessary for the
   * collision rule.
   */
  CVector2 DiffusionVector(bool *b_collision);

  /*
   * Get a vector, that points to the closest led in specific color
   * perceived by the omnicam.
   */
  CVector2 LedVector(CColor color) const;

  /*
   * Gets a direction vector as input and transforms it into wheel
   * actuation.
   */
  void SetWheelSpeeds(const CVector2 &c_heading);

  /*
   * Executes the exploring state.
   */
  void Explore();

  /*
   * Executes phototaxis.
   */
  void Phototaxis();

  /*
   * Executes antiphototaxis.
   */
  void AntiPhototaxis();

  /*
   * DropAction
   */
  bool DropAction();

  /*
   * Pickup the nearest Cylinder
   */
  bool PickUpAction();

  /* Pointer to the differential steering actuator */
  CCI_DifferentialSteeringActuator *m_pcWheels;
  /* Pointer to the LEDs actuator */
  CCI_LEDsActuator *m_pcLEDs;
  /* Pointer to the foot-bot gripper actuator */
  CCI_FootBotGripperActuator *m_pcGripper;
  /* Pointer to the foot-bot turret actuator */
  CCI_FootBotTurretActuator *m_pcTurret;
  /* Pointer to the foot-bot proximity sensor */
  CCI_FootBotProximitySensor *m_pcProximity;
  /* Pointer to the foot-bot light sensor */
  CCI_FootBotLightSensor *m_pcLight;
  /* Pointer to the foot-bot turret encoder */
  CCI_FootBotTurretEncoderSensor *m_pcTurretEnc;

  CCI_ColoredBlobOmnidirectionalCameraSensor *m_pcCamera;
  /* The random number generator */
  CRandom::CRNG *m_pcRNG;

  /* The controller state information */
  SStateData m_sStateData;
  /* The turning parameters */
  SWheelTurningParams m_sWheelTurningParams;
  /* The diffusion parameters */
  SDiffusionParams m_sDiffusionParams;

  int randomWalkCounter = 0;
  CVector2 currMoveV;

  bool GripperLocked = false;

  CVector2 RandomVector(int minDeg, int maxDeg);

  void SetState(SStateData::EState newState);

  Real LightIntensity();
  bool HasItem();

  SRule PhototaxisAntiphototaxisRule;
  SRule ExplorePhototaxisRule;
  SRule AntiPhototaxisExploreRule;

  bool seesCylinder();

};

#endif
