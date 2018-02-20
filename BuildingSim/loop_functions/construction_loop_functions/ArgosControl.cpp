#include <unistd.h>
//#include <thread>

#include <argos3/core/simulator/simulator.h>
#include <argos3/core/simulator/loop_functions.h>
#include <loop_functions/construction_loop_functions/construction_loop_functions.h>
#include <wait.h>

#include "ArgosControl.h"

void ArgosControl::InitArgos() {
//  std::ofstream cLOGFile(std::string("ARGoS_LOG_" + ToString(::getpid())).c_str(), std::ios::out);
//  LOG.DisableColoredOutput();
//  //LOG.RedirectToFile(std::string("ARGoS_LOG_" + ToString(::getpid())).c_str());
//  LOG.GetStream().rdbuf(cLOGFile.rdbuf());
//  std::ofstream cLOGERRFile(std::string("ARGoS_LOGERR_" + ToString(::getpid())).c_str(), std::ios::out);
//  LOGERR.DisableColoredOutput();
//  //LOGERR.RedirectToFile(std::string("ARGoS_LOGERR_" + ToString(::getpid())).c_str());
//  LOGERR.GetStream().rdbuf(cLOGERRFile.rdbuf());
  /* Initialize ARGoS */
  argos::CSimulator &cSimulator = argos::CSimulator::GetInstance();
  cSimulator.SetExperimentFileName("experiments/construction-nsga2.argos");
  cSimulator.LoadExperiment();
  LOG.Flush();
  LOGERR.Flush();
}

double ArgosControl::LaunchArgos(int genome[], int length) {

//  std::ofstream cLOGFile(std::string("ARGoS_LOG_" + ToString(::getpid())).c_str(), std::ios::out);
//  //std::ofstream cLOGFile(std::string("/dev/null").c_str(), std::ios::out);
//  LOG.DisableColoredOutput();
//  //LOG.RedirectToFile(std::string("ARGoS_LOG_" + ToString(::getpid())).c_str());
//  LOG.GetStream().rdbuf(cLOGFile.rdbuf());
//  std::ofstream cLOGERRFile(std::string("ARGoS_LOGERR_" + ToString(::getpid())).c_str(), std::ios::out);
//  LOGERR.DisableColoredOutput();
//  //LOGERR.RedirectToFile(std::string("ARGoS_LOGERR_" + ToString(::getpid())).c_str());
//  LOGERR.GetStream().rdbuf(cLOGERRFile.rdbuf());
  /* Initialize ARGoS */
  argos::CSimulator &cSimulator = argos::CSimulator::GetInstance();
//  cSimulator.SetExperimentFileName("experiments/construction-nsga2.argos");
//  cSimulator.LoadExperiment();
  //LOG.Flush();
  //LOGERR.Flush();

  //argos::CSimulator& cSimulator = argos::CSimulator::GetInstance();
  /* Get a reference to the loop functions */
  CConstructionLoopFunctions
      &cLoopFunctions = dynamic_cast<CConstructionLoopFunctions &>(cSimulator.GetLoopFunctions());

  cLoopFunctions.ConfigureFromGenome(genome, length);
  Real performance = 0.0f;
  for (size_t i = 0; i < 1; ++i) {
    cLoopFunctions.SetTrial(i);

    cSimulator.Reset(i);

    cSimulator.Execute();
    performance = Max(performance, cLoopFunctions.Performance());
    //LOG.Flush();
    //LOGERR.Flush();
  }
  //cSimulator.Destroy();

  return performance;
}

double threadedArgos( int* genome, int length, double *result) {
  pid_t pid = ::fork();
  std::cout << pid <<  std::endl;
  if(pid == 0) {
    ArgosControl& c = ArgosControl::GetInstance();
    *result = 1; //c.LaunchArgos(genome, length);
    std::cout.flush();
    //::sleep(3);
    std::cout << "End of Child " << std::endl;
    std::cout.flush();
    return 1;
    //threadedArgos(c,genome,80,res);
  }else {
    int nSlaveInfo;
    pid_t tSlavePID;
    ::waitpid(0,&nSlaveInfo, WUNTRACED);
  }
}

void ArgosControl::DestroyArgos() {
  argos::CSimulator &cSimulator = argos::CSimulator::GetInstance();
 cSimulator.Destroy();
}

void test(ArgosControl& c, double& i){
  std::cout << "test" << i++ << std::endl;
}

int main(int argc, const char *argv[]) {
  double res = 42;
  ArgosControl &c = ArgosControl::GetInstance();
//  argos::CSimulator &cSimulator = argos::CSimulator::GetInstance();
//  cSimulator.SetExperimentFileName("experiments/construction.argos");
//  cSimulator.LoadExperiment();
  int genome[80] = {1, 0, 2, -1, 0, 3, 3, 1,
                    2, 0, 3, -1, 0, 3, 3, 1,
                    3, 0, 4, -1, 0, 3, 3, 1,
                    4, 0, 0, -1, 0, 3, 3, 1,
                    0, 0, 1, -1, 0, 3, 3, 1,
                    1, 0, 2, 1, 0, 3, 3, 1,
                    2, 0, 3, 1, 0, 3, 3, 1,
                    3, 0, 4, 1, 0, 3, 3, 1,
                    4, 0, 0, 1, 0, 3, 3, 1,
                    0, 0, 1, 1, 0, 3, 3, 1};
  c.InitArgos();
  res = c.LaunchArgos(genome,80);
  res = c.LaunchArgos(genome,80);
  c.DestroyArgos();


  //threadedArgos(genome,80,&res);


  std::cout << "Result is: " << res << std::endl;
  return 0;
}