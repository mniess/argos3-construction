#ifndef ARGOS3_CONSTRUCTION_ARGOSCONTROL_H
#define ARGOS3_CONSTRUCTION_ARGOSCONTROL_H

class ArgosControl {
private:
  ArgosControl() = default;

public:
  ArgosControl(const ArgosControl&) = delete;
  ArgosControl& operator=(const ArgosControl&) = delete;

  static ArgosControl& GetInstance() {static ArgosControl control; return control; };
  void InitArgos();
  double LaunchArgos(int genome[], int length);
  void DestroyArgos();

private:
  double Evaluate();

};

#endif //ARGOS3_CONSTRUCTION_ARGOSCONTROL_H
