#include "foraging_qt_user_functions.h"
#include <controllers/footbot_foraging/footbot_foraging.h>

using namespace argos;

/****************************************/
/****************************************/

CForagingQTUserFunctions::CForagingQTUserFunctions() {
   RegisterUserFunction<CForagingQTUserFunctions,CFootBotEntity>(&CForagingQTUserFunctions::Draw);
}

/****************************************/
/****************************************/

void CForagingQTUserFunctions::Draw(CFootBotEntity& c_entity) {
    auto& cController = dynamic_cast<CFootBotForaging&>(c_entity.GetControllableEntity().GetController());
   CFootBotForaging::SFoodData& sFoodData = cController.GetFoodData();
   if(sFoodData.HasFoodItem) {
      DrawCylinder(
         CVector3(0.0f, 0.0f, 0.3f), 
         CQuaternion(),
         0.1f,
         0.05f,
         CColor::BLACK);
   }
   DrawText(CVector3(0.0, 0.0, 0.3),   // position
            c_entity.GetId());
}

/****************************************/
/****************************************/

REGISTER_QTOPENGL_USER_FUNCTIONS(CForagingQTUserFunctions, "foraging_qt_user_functions")
