#ifndef _ITF_CHEATMANAGER_H_
#define _ITF_CHEATMANAGER_H_

#ifndef _ITF_INPUTADAPTER_H_
#include "engine/AdaptersInterfaces/InputAdapter.h"
#endif //_ITF_INPUTADAPTER_H_

#ifndef _ITF_GAMEMANAGER_H_
#include "gameplay/managers/GameManager.h"
#endif //_ITF_GAMEMANAGER_H_

#ifndef _ITF_INPUTLISTENER_H_
#include "engine/zinput/ZInputListener.h"
#endif //_ITF_INPUTLISTENER_H_

namespace ITF
{

    class CheatManager : public Interface_InputListener, public IInputListener
    {
    public:

        CheatManager();
    virtual ~CheatManager();

        virtual void        init( const class CommandArgs& _args );

        void                setPause( bbool _val );
        void                togglePause() { setPause(!m_pause); }
        bbool               getPause() const { return m_pause && !m_pauseStep; }

        void                setPauseStep() { m_pauseStep = btrue; }
        void                resetPauseStep() { m_pauseStep = bfalse; }
        bbool               getPauseStep() const { return m_pauseStep; }

        f32                 getTimeStepMult() const { return m_timeStepMult; }
        void                IncrTimeStepMult();
        void                DecrTimeStepMult();
        void                ResetTimeStepMult() { m_timeStepMult = 1.f; }

        static float        getPauseStepDt();

        virtual void        update(f32 _dt);

        bbool               getDebugCharacters() const { return m_debugCharacters; }
        void                setDebugCharacters( bbool _val ) { m_debugCharacters = _val; }

        bbool               getActive() const { return m_isActive;}    
        void                setActive(bbool _active);

        bbool               getCanListenActions() const { return m_canListenActions;}    
        
        bbool               getEnableInvincibleEnemies() const { return m_enableInvincibleEnemies; }
        void                setEnableInvincibleEnemies( bbool _val ) { m_enableInvincibleEnemies = _val; }

        bbool               isResetCharacterDebugTrajectories() const { return m_resetCharacterDebugTrajectories != 0; }

        bbool               isPlayerInvincible() const { return m_playerInvincible; }
        void                setPlayerInvincible( bbool _val ) { m_playerInvincible = _val; }

        // Interface_InputListener
        virtual bbool       onMouseButton( InputAdapter::MouseButton _but, InputAdapter::PressStatus _status );
        virtual bbool       onMousePos( i32 _x, i32 _y ) { return btrue; }
        virtual bbool       onMouseWheel( i32 _wheelValue, i32 _wheelDelta ) { return btrue; }
        virtual bbool       onKey( i32 _key, InputAdapter::PressStatus _status );


        virtual void        Receive( u32 deviceID /* player */, f32 axis, const StringID& action ) ;
        virtual void        onReceive( u32 deviceID /* player */, f32 axis, const StringID& action ) {}

    protected:
        virtual void        teleportToCheckpoint(GameManager::teleportTarget _target);

    private:

        f32                 m_timeStepMult;
        bbool               m_pause;
        bbool               m_pauseStep;
        bbool               m_debugCharacters;
        bbool               m_isActive;
        u32                 m_currentCheckpointIndex;
        bbool               m_enableInvincibleEnemies;
        u32                 m_resetCharacterDebugTrajectories;
        bbool               m_playerInvincible;

        bbool               m_canListenActions;
        bbool               m_frameWaited;
        bbool               m_listenerRegistered;
    };

}

#endif // _ITF_CHEATMANAGER_H_

