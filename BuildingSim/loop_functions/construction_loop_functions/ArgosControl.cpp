#include <unistd.h>

#include <argos3/core/simulator/simulator.h>
#include <argos3/core/simulator/loop_functions.h>
#include <loop_functions/construction_loop_functions/construction_loop_functions.h>

#include "ArgosControl.h"

void ArgosControl::InitArgos() {
  /* Initialize ARGoS */
  argos::CSimulator &cSimulator = argos::CSimulator::GetInstance();
  cSimulator.SetExperimentFileName("experiments/construction-nsga2.argos");
  cSimulator.LoadExperiment();
}

double ArgosControl::LaunchArgos(int genome[], int length) {

  static argos::CSimulator &cSimulator = argos::CSimulator::GetInstance();
  /* Get a reference to the loop functions */
  static CConstructionLoopFunctions &cLoopFunctions = dynamic_cast<CConstructionLoopFunctions &>(cSimulator.GetLoopFunctions());

  cLoopFunctions.ConfigureFromGenome(genome, length);
  Real performance = 0.0f;
  for (size_t i = 0; i < 1; ++i) {
    cLoopFunctions.SetTrial(i);

    cSimulator.Reset(i);

    cSimulator.Execute();
    performance = Max(performance, cLoopFunctions.Performance());
  }
  return performance;
}

void ArgosControl::DestroyArgos() {
  argos::CSimulator &cSimulator = argos::CSimulator::GetInstance();
  cSimulator.Destroy();
}

int main( int argc, const char* argv[] ) {
  ArgosControl& c = ArgosControl::GetInstance();
  argos::CSimulator &cSimulator = argos::CSimulator::GetInstance();
  cSimulator.SetExperimentFileName("experiments/construction.argos");
  cSimulator.LoadExperiment();
  int genome[80] = { 1, 0, 2, -1, 0, 3, 3, 1,
                     2, 0, 3, -1, 0, 3, 3, 1,
                     3, 0, 4, -1, 0, 3, 3, 1,
                     4, 0, 0, -1, 0, 3, 3, 1,
                     0, 0, 1, -1, 0, 3, 3, 1,
                     1, 0, 2,  1, 0, 3, 3, 1,
                     2, 0, 3,  1, 0, 3, 3, 1,
                     3, 0, 4,  1, 0, 3, 3, 1,
                     4, 0, 0,  1, 0, 3, 3, 1,
                     0, 0, 1,  1, 0, 3, 3, 1};
  c.LaunchArgos(genome,80);
  c.DestroyArgos();
}