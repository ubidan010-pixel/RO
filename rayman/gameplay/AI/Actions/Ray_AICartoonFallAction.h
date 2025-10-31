#ifndef _ITF_RAY_AICARTOONFALLACTION_H_
#define _ITF_RAY_AICARTOONFALLACTION_H_

#ifndef _ITF_AIACTION_H_
#include "gameplay/ai/Actions/AIAction.h"
#endif //_ITF_AIACTION_H_

namespace ITF
{
    class StickToPolylinePhysComponent;

    /*
        The character runs off an edge, unsticks: we get activated.
        Phase 1: air run (no gravity but air friction) until we don't have any more speed
        Phase 2: prefall ("uh-oh I'm gonna fall" moment)
        Phase 3: fall (we can give an impulse speed and extra gravity at this point)
        Phase 4: we hit the ground (stick)
        When crash landing is done, we exit.
    */
    class Ray_AICartoonFallAction : public AIAction
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_AICartoonFallAction, AIAction,2666416038);

    public:

        Ray_AICartoonFallAction();
        virtual ~Ray_AICartoonFallAction();

        virtual void        onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual void        onActivate();
        virtual void        onDeactivate();
        virtual void        update( f32 _delta );
        virtual void        updateAnimInputs();

    private:

        ITF_INLINE const class Ray_AICartoonFallAction_Template* getTemplate() const;

        StickToPolylinePhysComponent*   m_physComponent;

        f32         m_prevGravityMultiplier;
        f32         m_prevAirFrictionMultiplier;
        bbool       m_airRunning;
        bbool       m_preFalling;
        bbool       m_falling;
        bbool       m_grounded;
    };

    class Ray_AICartoonFallAction_Template : public AIAction_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_AICartoonFallAction_Template,AIAction_Template,586916384);
        DECLARE_SERIALIZE();
        DECLARE_AIACTION_TEMPLATE(Ray_AICartoonFallAction);

    public:

        Ray_AICartoonFallAction_Template();
        ~Ray_AICartoonFallAction_Template(){}

        f32         getGravityMultiplierDuringRun() const { return m_gravityMultiplierDuringRun; }
        f32         getGravityMultiplierDuringFall() const { return m_gravityMultiplierDuringFall; }
        f32         getAirFrictionMultiplierDuringRun() const { return m_airFrictionMultiplierDuringRun; }
        f32         getMinSpeedForFall() const { return m_minSpeedForFall; }
        f32         getInitialFallSpeed() const { return m_initialFallSpeed; }

    private:

        f32         m_gravityMultiplierDuringRun;
        f32         m_gravityMultiplierDuringFall;
        f32         m_airFrictionMultiplierDuringRun;
        f32         m_minSpeedForFall;
        f32         m_initialFallSpeed;
    };

    ITF_INLINE const class Ray_AICartoonFallAction_Template * Ray_AICartoonFallAction::getTemplate() const
    {
        return static_cast<const class Ray_AICartoonFallAction_Template*>(m_template);
    }
}

#endif // _ITF_RAY_AICARTOONFALLACTION_H_