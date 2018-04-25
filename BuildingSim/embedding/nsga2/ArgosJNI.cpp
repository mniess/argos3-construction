#include <algorithm>

#include <argos3/core/simulator/simulator.h>
#include <loop_functions/construction_loop_functions/ArgosControl.h>

#include <de_uniluebeck_iti_PopCode_MOEA_PopCode.h>

JNIEXPORT jdoubleArray JNICALL Java_de_uniluebeck_iti_PopCode_MOEA_PopCode_LaunchArgos(JNIEnv *env,
                                                                                       jobject,
                                                                                       jintArray p_array,
                                                                                       jint evals,
                                                                                       jstring genomeType,
                                                                                       jint seed) {
  //Create genome array
  jsize len = env->GetArrayLength(p_array);
  jint *body = env->GetIntArrayElements(p_array, nullptr);
  int genome[len];
  std::copy(body, body + len, genome);
  env->ReleaseIntArrayElements(p_array, body, 0);

  //Get GenomeType String
  const char *cstrGenomeType = env->GetStringUTFChars(genomeType, 0);
  std::string strGenomeType = std::string(cstrGenomeType);
  env->ReleaseStringUTFChars(genomeType, cstrGenomeType);

  //Run
  double *results = ArgosControl::GetInstance().LaunchArgos(genome, len, evals, strGenomeType, seed);

  //Convert results to JavaArray
  jboolean isCopy;
  jdoubleArray jOutput = env->NewDoubleArray(2);
  jdouble jArrayElems[2];
  jArrayElems[0] = results[0];
  jArrayElems[1] = results[1];
  env->SetDoubleArrayRegion(jOutput, 0, 2, jArrayElems);
  return jOutput;
}

JNIEXPORT jint JNICALL Java_de_uniluebeck_iti_PopCode_MOEA_PopCode_InitArgos(JNIEnv *, jobject) {
  ArgosControl::GetInstance().InitArgos();
  return 0;
}

JNIEXPORT jint JNICALL Java_de_uniluebeck_iti_PopCode_MOEA_PopCode_DestroyArgos(JNIEnv *, jobject) {
  ArgosControl::GetInstance().DestroyArgos();
  return 0;
}