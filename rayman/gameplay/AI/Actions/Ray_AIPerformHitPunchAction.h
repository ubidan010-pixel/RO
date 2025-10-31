#ifndef _ITF_RAY_AIPERFORMHITPUNCHACTION_H_
#define _ITF_RAY_AIPERFORMHITPUNCHACTION_H_

#ifndef _ITF_RAY_AIPERFORMHITACTION_H_
#include "rayman/gameplay/AI/Actions/Ray_AIPerformHitAction.h"
#endif //_ITF_RAY_AIPERFORMHITACTION_H_

namespace ITF
{
    class Ray_AIPerformHitPunchAction : public Ray_AIPerformHitAction
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_AIPerformHitPunchAction, Ray_AIPerformHitAction,533168033);

    public:

        Ray_AIPerformHitPunchAction();
        virtual ~Ray_AIPerformHitPunchAction();

        virtual void        onActivate();
        virtual void        onActorLoaded(Pickable::HotReloadType /*_hotReload*/);

    protected:
        ITF_INLINE const class Ray_AIPerformHitPunchAction_Template * getTemplate() const;

        virtual void        performHit();

        PhysShape*          m_shape;

    };
    class Ray_AIPerformHitPunchAction_Template : public Ray_AIPerformHitAction_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_AIPerformHitPunchAction_Template,Ray_AIPerformHitAction_Template,2190342275);
        DECLARE_SERIALIZE();
        DECLARE_AIACTION_TEMPLATE(Ray_AIPerformHitPunchAction);

    public:

        Ray_AIPerformHitPunchAction_Template();
        ~Ray_AIPerformHitPunchAction_Template(){SF_DEL(m_shape);}

        ITF_INLINE const PhysShape* getShape() const { return m_shape; }
        ITF_INLINE const Vec2d &getShapeOffset() const { return m_shapeOffset; }
    private:
        PhysShape*          m_shape;
        Vec2d               m_shapeOffset;
    };

    ITF_INLINE const class Ray_AIPerformHitPunchAction_Template * Ray_AIPerformHitPunchAction::getTemplate() const
    {
        return static_cast<const class Ray_AIPerformHitPunchAction_Template *>(m_template);
    }
}

#endif //_ITF_RAY_AIPERFORMHITPUNCHACTION_H_

