#ifndef _ITF_GAMESCREEN_INITIAL_H_
#define _ITF_GAMESCREEN_INITIAL_H_

#ifndef _ITF_GAMESCREENBASE_H_
#include "gameplay/Managers/GameScreens/GameScreenBase.h"
#endif //_ITF_GAMESCREENBASE_H_


namespace ITF
{
    //////////////////////////////////////////////////////////////////////////
    ///The main menu
    class GameScreen_Initial : public GameScreenBase
    {
        DECLARE_OBJECT_CHILD_RTTI(GameScreen_Initial,GameScreenBase,2831340246);
    public:
        //Implementation of base class members

        GameScreen_Initial() {}
        ~GameScreen_Initial() {}

        virtual void init() {}

        virtual void close() {}

        virtual void update() {}

        virtual bbool hasGameplay() {return bfalse;}
        
        virtual bbool canDisplayTRCMessages() { return bfalse; }

        virtual bbool checkSignInOutChanges() { return bfalse; }
    private:
    };
    
}


#endif
