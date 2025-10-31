#ifndef _ITF_RAY_PCSTATEREVIVE_H_
#define _ITF_RAY_PCSTATEREVIVE_H_

#ifndef _ITF_RAY_PCSTATE_H_
#include "rayman/gameplay/Components/Player/PlayerControllerStates/Ray_PCState.h"
#endif //_ITF_RAY_PCSTATE_H_


    class StateRevive_Template;

    class StateRevive : public Ray_State
    {
        DECLARE_OBJECT_CHILD_RTTI(StateRevive, Ray_State,392701636)

    public:

        StateRevive();
        virtual ~StateRevive();

        virtual StringID getAnimAction() const
        {
            return ITF_GET_STRINGID_CRC(DEADSOUL_REVIVE,247444888);
        }

        void            setData( ActorRef _otherPlayer, const Vec2d& _dir, const Vec2d& _targetPos );
        virtual void    onEnter();
        virtual void    onExit();
        virtual bbool   checkStateChange( f32 _dt );
        virtual void    update( f32 _dt );
        virtual void    updateAnimInputs();

    private:
        ITF_INLINE const StateRevive_Template* getTemplate() const;

        Vec2d           m_startPos;
        Vec2d           m_startDir;   // not necessarily normalized (multiplied by hit level)
        Vec2d           m_targetPos;
        ActorRef        m_target;
        f32             m_timer;

        bbool           m_shouldRevive;
    };

    class StateRevive_Template
    {
        DECLARE_SERIALIZE();

    public:

        StateRevive_Template();
        virtual ~StateRevive_Template();

        ITF_INLINE f32 getDuration() const { return m_duration; }
        ITF_INLINE f32 getBezierHitMultiplier() const { return m_bezierHitMultiplier; }
        ITF_INLINE f32 getBezierMidRadius() const { return m_bezierMidRadius; }
        ITF_INLINE f32 getBezierMidInfluence() const { return m_BezierMidInfluence; }

    private:

        f32 m_duration;
        f32 m_bezierHitMultiplier;
        f32 m_bezierMidRadius;
        f32 m_BezierMidInfluence;
    };



#endif //_ITF_RAY_PCSTATEREVIVE_H_