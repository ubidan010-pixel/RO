#ifndef _ITF_WAR_GAMEMANAGER_H_
#define _ITF_WAR_GAMEMANAGER_H_

#ifndef _ITF_GAMEMANAGER_H_
#include "gameplay/Managers/GameManager.h"
#endif //_ITF_GAMEMANAGER_H_


namespace ITF
{
    #define WAR_GAMEMANAGER War_GameManager::get()

    class ArchiveMemory;
  
    class War_GameManager : public GameManager
    {
    public:
        typedef GameManager Super;
        War_GameManager() {};
        ~War_GameManager() {};

        static void                 create()    { s_instance = newAlloc(mId_Singleton, War_GameManager()); }
        static void                 destroy()   { SF_DEL(s_instance); }
        static War_GameManager*     get()       { return static_cast<War_GameManager*>(s_instance); }  

        virtual u32     getMaxPlayerCount() const { return 4; }

    private:
    
    };
}

#endif //_ITF_WAR_GAMEMANAGER_H_

