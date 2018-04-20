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

double ArgosControl::LaunchArgos(int genome[], int length, int evaluations, std::string genomeType, int seed) {

  static argos::CSimulator &cSimulator = argos::CSimulator::GetInstance();
  /* Get a reference to the loop functions */
  static CConstructionLoopFunctions
      &cLoopFunctions = dynamic_cast<CConstructionLoopFunctions &>(cSimulator.GetLoopFunctions());

  cLoopFunctions.ConfigureFromGenome(genome, length, genomeType);
  Real performance;
  for (size_t i = 0; i < evaluations; ++i) {
    try {
      cLoopFunctions.SetTrial(i);

      //different setup for every run
      cSimulator.Reset(seed*evaluations + i);

      cSimulator.Execute();
      performance = cLoopFunctions.cylinderCoverage();// + cLoopFunctions.robotFracInCircle();
    } catch (CARGoSException &ex) {
    }
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
  cSimulator.SetExperimentFileName("experiments/construction.argos");
  cSimulator.LoadExperiment();
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
  //                  [             ][             ][             ]
  int fullGene[150] = {4, 0, 3, 2, 1, 0, 0, 4, 0, 0, 0, 0, 1, 0, 0,
                       4, 0, 0, 3, 1, 3, 1, 1, 0, 0, 0, 0, 3, 4, 1,
                       4, 0, 0, 3, 1, 3, 1, 1, 0, 0, 0, 0, 3, 4, 1,
                       4, 0, 0, 3, 1, 3, 1, 1, 0, 0, 0, 0, 3, 4, 1,
                       4, 0, 0, 3, 1, 3, 1, 1, 0, 0, 0, 0, 3, 4, 1,
                       4, 0, 0, 3, 1, 3, 1, 1, 0, 0, 0, 0, 3, 4, 1,
                       4, 0, 0, 3, 1, 3, 1, 1, 0, 0, 0, 0, 3, 4, 1,
                       4, 0, 0, 3, 1, 3, 1, 1, 0, 0, 0, 0, 3, 4, 1,
                       4, 0, 0, 3, 1, 3, 1, 1, 0, 0, 0, 0, 3, 4, 1,
                       4, 0, 0, 3, 1, 3, 1, 1, 0, 0, 0, 0, 3, 4, 1};

  //Last 61pc19 --> 0.15
  int evolvGene[80] = {3, 1, 0, 1, 0, 1, 0, 1,
                       1, 0, 2, 0, 1, 2, 1, 1,
                       3, 1, 0, 1, 0, 1, 0, 1,
                       0, 0, 4, -1, 0, 1, 4, 0,
                       2, 0, 1, 0, 1, 4, 3, 1,
                       4, 0, 3, 1, 0, 3, 4, 1,
                       4, 0, 3, 1, 0, 3, 4, 1,
                       2, 0, 2, 1, 1, 2, 1, 0,
                       0, 1, 3, -1, 0, 0, 1, 1,
                       2, 0, 1, 0, 1, 4, 3, 1};

  int errorGene[80] = {4, 1, 2, 0, 0, 4, 2, 0,
                       1, 0, 0, 0, 0, 4, 4, 0,
                       1, 1, 3, 1, 1, 4, 2, 1,
                       4, 1, 3, -1, 0, 4, 0, 0,
                       3, 1, 1, -1, 1, 4, 3, 0,
                       2, 1, 1, 1, 0, 4, 2, 1,
                       2, 0, 0, 0, 1, 4, 2, 1,
                       1, 0, 3, 0, 0, 4, 2, 1,
                       1, 1, 3, 0, 0, 4, 1, 0,
                       0, 0, 0, -1, 1, 2, 1, 1};
  //Set genome
  genome = fullGene;

  std::cout << c.LaunchArgos(genome, 150, 1, "full",0);
  //std::cout << c.LaunchArgos(genome,80,1,"");
  LOG.Flush();
  //c.DestroyArgos();
}