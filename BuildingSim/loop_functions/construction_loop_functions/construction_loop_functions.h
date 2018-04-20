#ifndef CONSTRUCTION_LOOP_FUNCTIONS_H
#define CONSTRUCTION_LOOP_FUNCTIONS_H

#include <argos3/core/simulator/loop_functions.h>
#include <argos3/core/simulator/entity/floor_entity.h>
#include <argos3/core/utility/math/range.h>
#include <argos3/core/utility/math/rng.h>

using namespace argos;

class CConstructionLoopFunctions : public CLoopFunctions {

public:

  struct SConstructionParams {
    CRange<Real> buildingRange;
    int numRobots;
    CRange<Real> arenaX;
    CRange<Real> arenaY;
  };

  CConstructionLoopFunctions();
  virtual ~CConstructionLoopFunctions() {}

  virtual void Init(TConfigurationNode &t_tree);
  virtual void Reset();
  virtual void Destroy();
  virtual void PostExperiment();
  virtual CColor GetFloorColor(const CVector2 &c_position_on_plane);
  virtual void PreStep();

  Real cylinderCoverage();
  Real robotFracInCircle();

  inline void SetTrial(size_t un_trial) {
    m_unCurrentTrial = un_trial;
  }

  /* Configures the robot controller from the genome */
  void ConfigureFromGenome(int genome[], int length, std::string genomeType);

private:
  SConstructionParams m_sConstructionParams;

  size_t m_unCurrentTrial;

  CRandom::CRNG *m_pcRNG;

  void SetRandomPos(CEmbodiedEntity &e);

};

#endif
