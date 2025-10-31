#ifndef _ITF_RAY_AIMANAGER_H_
#define _ITF_RAY_AIMANAGER_H_

#ifndef _ITF_AIMANAGER_H_
#include "gameplay/managers/AIManager.h"
#endif //_ITF_AIMANAGER_H_

namespace ITF
{
    struct Ray_AIData : public AIData
    {
        Ray_AIData() 
        {

        }
    };

#define RAY_AI_MANAGER Ray_AIManager::get()

    class Ray_AIManager : public AIManager
    {
    public:
        static void                     create()    { s_instance = newAlloc(mId_Singleton, Ray_AIManager()); }
        static Ray_AIManager *          get() { return static_cast<Ray_AIManager*>(s_instance);}
        virtual AIData *                allocateAiData(){ return newAlloc(mId_Gameplay,Ray_AIData()); }

    };
} // namespace ITF
#endif //_ITF_RAY_AIMANAGER_H_
