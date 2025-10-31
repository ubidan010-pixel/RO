#ifndef _ITF_WAR_GAMEINTERFACE_H_
#define _ITF_WAR_GAMEINTERFACE_H_

#ifndef _ITF_GAMEINTERFACE_H_
#include "engine/gameplay/GameInterface.h"
#endif //_ITF_GAMEINTERFACE_H_

namespace ITF
{
    class War_GameInterface : public GameInterface
    {
        typedef GameInterface Super;
    public:
        War_GameInterface();
        virtual ~War_GameInterface();

        virtual void            registerObjects() const;
        virtual void            init();
        virtual void            preWorldUpdate( f32 _dt );
        virtual void            postWorldUpdate(f32 _dt);
    };
}

#endif //_ITF_WAR_GAMEINTERFACE_H_
