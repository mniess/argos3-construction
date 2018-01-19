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
   DrawText(CVector3(0.0, 0.0, 0.3),   // position
            c_entity.GetId());
}

/****************************************/
/****************************************/

REGISTER_QTOPENGL_USER_FUNCTIONS(CForagingQTUserFunctions, "foraging_qt_user_functions")
