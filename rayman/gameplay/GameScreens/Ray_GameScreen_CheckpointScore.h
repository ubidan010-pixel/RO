#ifndef _ITF_RAY_CHECKPOINTSCORE_H_
#define _ITF_RAY_CHECKPOINTSCORE_H_

#ifndef _ITF_GAMESCREENBASE_H_
#include "gameplay/Managers/GameScreens/GameScreenBase.h"
#endif //_ITF_GAMESCREENBASE_H_

#ifndef _ITF_RAY_PAUSE_MENUITEMACTIONLISTENER_H_
#include "rayman/gameplay/GameScreens/Ray_Pause_MenuItemActionListener.h"
#endif //_ITF_RAY_PAUSE_MENUITEMACTIONLISTENER_H_

namespace ITF
{
    //////////////////////////////////////////////////////////////////////////
    /// Screen displayed at the end of the level
    class Ray_GameScreen_CheckpointScore : public GameScreenBase, public Ray_Pause_MenuItemActionListener
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_GameScreen_CheckpointScore,GameScreenBase,2049621634)

    public:

        //Implementation of base class members

        Ray_GameScreen_CheckpointScore();
        ~Ray_GameScreen_CheckpointScore();

        virtual void  init();

        virtual void  close();

        virtual void  update();

        virtual bbool hasGameplay() {return btrue;}

        virtual bbool canPause();

        virtual void  onWorldLoaded();

        void          showScores (bbool _isVisible);

        virtual bbool checkSignInOutChanges() { return bfalse; }

    private:

        void            writeStats();

    };
}

#endif
