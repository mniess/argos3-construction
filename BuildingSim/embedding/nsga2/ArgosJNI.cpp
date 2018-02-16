#include <algorithm>

#include <argos3/core/simulator/simulator.h>
#include <loop_functions/construction_loop_functions/ArgosControl.h>

#include <de_uniluebeck_iti_PopCode_MOEA_NSGA2PopCode.h>

JNIEXPORT jdouble JNICALL Java_de_uniluebeck_iti_PopCode_MOEA_NSGA2PopCode_LaunchArgos(JNIEnv *env,
                                                                                       jobject,
                                                                                       jintArray p_array) {
  /* Return the negative result of the evaluation because nsgaII is minimizing*/
  jsize len = env->GetArrayLength(p_array);
  jint *body = env->GetIntArrayElements(p_array, nullptr);
  int genome[len];
  std::copy(body, body + len, genome);
  env->ReleaseIntArrayElements(p_array, body, 0);
//  for (int j = 0; j < len; ++j) {
//    printf("%d, ", genome[j]);
//  }
//  std::cout << "length=" << len << std::endl;
//  std::cout.flush();
  return -ArgosControl::GetInstance().LaunchArgos(genome,len);
}

JNIEXPORT jint JNICALL Java_de_uniluebeck_iti_PopCode_MOEA_NSGA2PopCode_InitArgos(JNIEnv *, jobject) {
  ArgosControl::GetInstance().InitArgos();
  return 0;
}

JNIEXPORT jint JNICALL Java_de_uniluebeck_iti_PopCode_MOEA_NSGA2PopCode_DestroyArgos(JNIEnv *, jobject) {
  ArgosControl::GetInstance().DestroyArgos();
  return 0;
}