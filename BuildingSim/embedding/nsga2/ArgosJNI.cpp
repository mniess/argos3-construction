#include <argos3/core/simulator/simulator.h>
#include <argos3/core/simulator/loop_functions.h>
#include <loop_functions/construction_loop_functions/construction_loop_functions.h>

#include <de_uniluebeck_iti_PopCode_MOEA_NSGA2PopCode.h>

JNIEXPORT jdouble JNICALL Java_de_uniluebeck_iti_PopCode_MOEA_NSGA2PopCode_LaunchArgos(JNIEnv *, jobject){
  return 4.2;
}

float LaunchARGoS() {
  static argos::CSimulator& cSimulator = argos::CSimulator::GetInstance();
  /* Get a reference to the loop functions */
  static CConstructionLoopFunctions& cLoopFunctions = dynamic_cast<CConstructionLoopFunctions&>(cSimulator.GetLoopFunctions());

  Real performance = 0.0f;
  for(size_t i = 0; i < 3; ++i) {
    cLoopFunctions.SetTrial(i);

    cSimulator.Reset();
    cLoopFunctions.ConfigureFromGenome();
    cSimulator.Execute();
    performance = Max(performance, cLoopFunctions.Performance());
  }
  /* Return the negative result of the evaluation because nsgaII is minimizing*/
  return -performance;
}

int InitARGoS() {

  argos::CSimulator& cSimulator = argos::CSimulator::GetInstance();
  cSimulator.SetExperimentFileName("experiments/construction.argos");
  cSimulator.LoadExperiment();
  return 0;
}

int DestroyARGoS(){
argos::CSimulator& cSimulator = argos::CSimulator::GetInstance();
cSimulator.Destroy();
return 0;
}