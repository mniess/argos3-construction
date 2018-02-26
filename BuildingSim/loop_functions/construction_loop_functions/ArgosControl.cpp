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
  static CConstructionLoopFunctions
      &cLoopFunctions = dynamic_cast<CConstructionLoopFunctions &>(cSimulator.GetLoopFunctions());

  cLoopFunctions.ConfigureFromGenome(genome, length);
  Real performance = 0.0f;
  for (size_t i = 0; i < 1; ++i) {
    cLoopFunctions.SetTrial(i);

    cSimulator.Reset(i + 2);

    cSimulator.Execute();
    performance = Max(performance, cLoopFunctions.Performance());
  }
  return performance;
}

void ArgosControl::DestroyArgos() {
  argos::CSimulator &cSimulator = argos::CSimulator::GetInstance();
  cSimulator.Destroy();
}

int main(int argc, const char *argv[]) {
  int *genome;
  //Robot Gene:
  // [    AP->E    ][      E->P        ][          P->AP         ]
  // minTime , Drop, minTime, Cyl, Drop, lowLight, highLight, Drop
  ArgosControl &c = ArgosControl::GetInstance();
  argos::CSimulator &cSimulator = argos::CSimulator::GetInstance();
  cSimulator.SetExperimentFileName("experiments/construction-nsga2.argos");
  cSimulator.LoadExperiment();
  int variant = 0;
  switch (variant) {
  case 0: {
    int manualGene[80] = {4, 1, 0, 1, 0, 3, 4, 1,
                          4, 1, 0, 1, 0, 3, 4, 1,
                          4, 1, 0, 1, 0, 3, 4, 1,
                          4, 1, 0, 1, 0, 3, 4, 1,
                          4, 1, 0, 1, 0, 3, 4, 1,
                          4, 1, 0, 1, 0, 3, 4, 1,
                          4, 1, 0, 1, 0, 3, 4, 1,
                          4, 1, 0, 1, 0, 3, 4, 1,
                          4, 1, 0, 1, 0, 3, 4, 1,
                          4, 1, 0, 1, 0, 3, 4, 1};
    genome = manualGene;
    break;
  }
  case 1: {
    int evolvedGene[80] = {4, 1, 1, -1, 0, 3, 4, 0,
                           3, 0, 1, 0, 0, 1, 4, 0,
                           4, 1, 3, 0, 0, 2, 0, 1,
                           1, 0, 2, 0, 1, 0, 4, 1,
                           1, 0, 4, 1, 0, 2, 4, 1,
                           4, 0, 2, 0, 1, 3, 1, 0,
                           3, 0, 3, 0, 1, 3, 4, 1,
                           3, 0, 2, -1, 1, 1, 4, 0,
                           0, 0, 2, 0, 1, 1, 1, 1,
                           3, 1, 3, 1, 0, 3, 4, 0};
    genome = evolvedGene;
    break;
  }
  }
  c.LaunchArgos(genome, 80);
  c.DestroyArgos();
}