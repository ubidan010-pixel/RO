#ifndef _ITF_RAY_AIFLYIDLEACTION_H_
#define _ITF_RAY_AIFLYIDLEACTION_H_

#ifndef _ITF_AIACTION_H_
#include "gameplay/ai/Actions/AIAction.h"
#endif //_ITF_AIACTION_H_

#ifndef _ITF_VEC2D_H_
#include "core/math/vec2d.h"
#endif //_ITF_VEC2D_H_

namespace ITF
{
    class Ray_AIFlyIdleAction_Template;

    class Ray_AIFlyIdleAction : public AIAction
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_AIFlyIdleAction, AIAction,3784423920);

    public:

        Ray_AIFlyIdleAction();
        virtual ~Ray_AIFlyIdleAction();

        virtual void                        onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual void                        onActivate();
        virtual void                        update( f32 _delta );
        virtual void                        onForceMove();

    private:
        ITF_INLINE const Ray_AIFlyIdleAction_Template* getTemplate() const;

        class PhysComponent*                m_physComponent;
        Vec2d                               m_startPos;
        Vec2d                               m_offset;
        f32                                 m_offsetTimer;

        static const f32                    m_calculateOffsetTime;
    };

    class Ray_AIFlyIdleAction_Template : public AIAction_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_AIFlyIdleAction_Template,AIAction_Template,3616031432);
        DECLARE_SERIALIZE();
        DECLARE_AIACTION_TEMPLATE(Ray_AIFlyIdleAction);

    public:

        Ray_AIFlyIdleAction_Template();
        ~Ray_AIFlyIdleAction_Template(){}

        ITF_INLINE f32 getStiff() const { return m_stiff; }
        ITF_INLINE f32 getDamp() const { return m_damp; }

    private:
        f32 m_stiff;
        f32 m_damp;
    };

    ITF_INLINE const Ray_AIFlyIdleAction_Template* Ray_AIFlyIdleAction::getTemplate() const
    {
        return static_cast<const Ray_AIFlyIdleAction_Template*>(m_template);
    }
}

#endif //_ITF_RAY_AIFLYIDLEACTION_H_

