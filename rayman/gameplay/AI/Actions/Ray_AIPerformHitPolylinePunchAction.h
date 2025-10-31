#ifndef _ITF_RAY_AIPERFORMHITPOLYLINEPUNCHACTION_H_
#define _ITF_RAY_AIPERFORMHITPOLYLINEPUNCHACTION_H_

#ifndef _ITF_RAY_AIPERFORMHITACTION_H_
#include "rayman/gameplay/AI/Actions/Ray_AIPerformHitAction.h"
#endif //_ITF_RAY_AIPERFORMHITACTION_H_

#ifndef _ITF_AIUTILS_H_
#include "gameplay/AI/Utils/AIUtils.h"
#endif //_ITF_AIUTILS_H_

namespace ITF
{
    class Ray_AIPerformHitPolylinePunchAction : public Ray_AIPerformHitAction
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_AIPerformHitPolylinePunchAction, Ray_AIPerformHitAction,3312903636);

    public:

        Ray_AIPerformHitPolylinePunchAction();
        virtual ~Ray_AIPerformHitPolylinePunchAction();

        virtual void        onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual void        onEvent(Event* _event);
        virtual void        onActivate();
        virtual void        update( f32 _delta );

        bbool               isPerformingHit() const { return m_performingHit; }

    protected:

        ITF_INLINE const class Ray_AIPerformHitPolylinePunchAction_Template * getTemplate() const;

        virtual void        performHit();

        AIUtils::HitShape   m_shape;
        Vec2d               m_dir;
        bbool               m_performingHit;
        StringID            m_polyID;
        bbool               m_dirSetFromEvent;
        ITF_MAP<ObjectRef,f32> m_memorizedHits;
    };

    class Ray_AIPerformHitPolylinePunchAction_Template : public Ray_AIPerformHitAction_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_AIPerformHitPolylinePunchAction_Template,Ray_AIPerformHitAction_Template,2136054497);
        DECLARE_AIACTION_TEMPLATE(Ray_AIPerformHitPolylinePunchAction);
        DECLARE_SERIALIZE();

    public:

        Ray_AIPerformHitPolylinePunchAction_Template();
        ~Ray_AIPerformHitPolylinePunchAction_Template(){}

        const Angle& getDirOffset() const { return m_dirOffset; }
        bbool getHitEnvironment() const { return m_hitEnvironment; }
        ITF_INLINE f32 getMemorizeHitTime() const { return m_memorizeHitTime; }


    private:

        Angle               m_dirOffset;
        bbool               m_hitEnvironment;
        f32                 m_memorizeHitTime;
    };

    ITF_INLINE const class Ray_AIPerformHitPolylinePunchAction_Template * Ray_AIPerformHitPolylinePunchAction::getTemplate() const
    {
        return static_cast<const class Ray_AIPerformHitPolylinePunchAction_Template*>(m_template);
    }
}

#endif //_ITF_RAY_AIPERFORMHITPOLYLINEPUNCHACTION_H_

