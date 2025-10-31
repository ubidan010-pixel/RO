#ifndef _ITF_RAY_PCSTATEBOUNCETOLAYER_H_
#define _ITF_RAY_PCSTATEBOUNCETOLAYER_H_

#ifndef _ITF_RAY_PCSTATE_H_
#include "rayman/gameplay/Components/Player/PlayerControllerStates/Ray_PCState.h"
#endif //_ITF_RAY_PCSTATE_H_

class StateBounceToLayer : public Ray_State
{
    DECLARE_OBJECT_CHILD_RTTI(StateBounceToLayer,Ray_State,2604686680)

public:

    StateBounceToLayer();
    virtual ~StateBounceToLayer() {};

    virtual void    onEnter();
    virtual void    onExit();
    virtual bbool   checkStateChange( f32 _dt );
    virtual void    update( f32 _dt );
    virtual void    onEvent(Event * _event);
#ifndef ITF_FINAL
    //virtual void    drawDebug();
#endif // ITF_FINAL

    void            setData( ObjectRef _target, const Vec2d& _offset, bbool _hurt );

    virtual StringID getAnimAction() const
    {
        return ITF_GET_STRINGID_CRC(BOUNCE_TO_LAYER,2591375191);
    }

private:

    void            updatePosition( f32 _dt );
    void            updateSpeed( f32 _dt );
    Vec3d           getTargetPosition() const;

    ActorRef    m_fxRef;
    ObjectRef   m_target;
    Vec3d       m_startPos;
    f32         m_totalTime;
    f32         m_timeCounter;
    Vec2d       m_targetOffset;
    f32         m_gravity;
    Vec2d       m_currentSpeed;
    Vec2d       m_prevPos;
    Vec3d       m_targetPoint;
    bbool       m_hurt;
    bbool       m_triggerDeath;
};

#endif //_ITF_RAY_PCSTATEBOUNCETOLAYER_H_


