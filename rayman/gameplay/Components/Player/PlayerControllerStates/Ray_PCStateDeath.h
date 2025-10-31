#ifndef _ITF_RAY_PCSTATEDEATH_H_
#define _ITF_RAY_PCSTATEDEATH_H_

#ifndef _ITF_RAY_PCSTATE_H_
#include "rayman/gameplay/Components/Player/PlayerControllerStates/Ray_PCState.h"
#endif //_ITF_RAY_PCSTATE_H_

class StateDeath : public Ray_State
{
    DECLARE_OBJECT_CHILD_RTTI(StateDeath,Ray_State,582353510)

public:

    StateDeath()
        : m_triggerDeadSoul(bfalse)
        , m_triggerSequence(bfalse)
        , m_spawnFX(bfalse)
        , m_explodeDirectly(bfalse)
        , m_deactivateOnFinish(bfalse)
        , m_reviveOnFirstLivePlayer(bfalse)
        , m_gameContinues(-1)
    {}
    virtual ~StateDeath();

    virtual void    onEnter();
    virtual void    onExit();
    virtual void    onEvent(Event * _event);
    virtual bbool   checkStateChange( f32 _dt );
    virtual void    update(f32 _dt);
    virtual StringID getAnimAction() const
    {
        return m_explodeDirectly ? ITF_GET_STRINGID_CRC(DEATH_EXPLODE,1295641326) : ITF_GET_STRINGID_CRC(DEATH,2142372323);
    }

    ITF_INLINE bbool getExplodeDirectly() const { return m_explodeDirectly; }
    ITF_INLINE void setExplodeDirectly( const bbool _value ) { m_explodeDirectly = _value; }
    ITF_INLINE void setDeactivateOnFinish() { m_deactivateOnFinish = btrue; }
    void            reviveOnFirstLivePlayer() { m_reviveOnFirstLivePlayer = btrue; }

    void            destroyFX();

private:

    bbool           m_triggerDeadSoul;
    bbool           m_triggerSequence;
    bbool           m_spawnFX;
    bbool           m_explodeDirectly;
    bbool           m_deactivateOnFinish;
    bbool           m_reviveOnFirstLivePlayer;
    i32             m_gameContinues;    // -1: don't know, 0: false, 1: true
    bbool           gameContinues();

    void spawnOffscreenFX();
    void updateOffscreenFX();
    void bringToScreenEdge();
    bbool getScreenAABBCropped( const Margin& _margin, AABB& _screenAABB );
    ActorRef m_offscreenFX;
    ActorRefList m_fxList;
};

#endif //_ITF_RAY_PCSTATEDEATH_H_


