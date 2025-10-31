#ifndef _ITF_RAY_GAMEINTERFACE_H_
#define _ITF_RAY_GAMEINTERFACE_H_

#ifndef _ITF_GAMEINTERFACE_H_
#include "engine/gameplay/GameInterface.h"
#endif //_ITF_GAMEINTERFACE_H_

namespace ITF
{
    class Ray_GameInterface : public GameInterface
    {
        typedef GameInterface Super;
    public:
        Ray_GameInterface();
        virtual ~Ray_GameInterface();

        // Compulsary methods
        virtual void            registerObjects() const;
        virtual void            init();
        virtual void            preWorldUpdate( f32 _dt );
        virtual void            postWorldUpdate( f32 _dt );

        // Optional methods
        virtual void onActivatePlayer(u32 _playerIndex, class Actor* _player);
        virtual void onPostSceneActivation(const Scene* _pScene, bbool _bActive);
        virtual void updateViewsBehavior(f32 dt);

		virtual const GameMaterial_Template* allocateGameMaterial( const Path& _fileName, bool prefetch = false );
        virtual void reloadGameMaterial( const Path& _fileName ) const;
    };

}

#endif //_ITF_RAY_GAMEINTERFACE_H_
