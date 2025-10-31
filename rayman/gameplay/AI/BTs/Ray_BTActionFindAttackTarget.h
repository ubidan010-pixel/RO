#ifndef _ITF_RAY_BTACTIONFINDATTACKTARGET_H_
#define _ITF_RAY_BTACTIONFINDATTACKTARGET_H_

#ifndef _ITF_BTACTION_H_
#include "gameplay/ai/BTs/BTAction.h"
#endif // _ITF_BTACTION_H_

namespace ITF
{

    class Ray_BTActionFindAttackTarget : public BTAction
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_BTActionFindAttackTarget,BTAction,2114936255);

    public:

        Ray_BTActionFindAttackTarget();
        ~Ray_BTActionFindAttackTarget() {}

        virtual void                            init( BehaviorTree* _tree, const BTNode_Template* _template );
        virtual bbool                           validate( Actor* _actor ) const;
        virtual bbool                           decide( f32 _dt );

    private:

        const class Ray_BTActionFindAttackTarget_Template*  getTemplate() const;

        bbool                                   isValidTarget( Actor* _actor, f32 _radiusSq, const Vec2d& _myPos ) const;

        static const f32 s_thresholdY;

        class StickToPolylinePhysComponent*     m_physComponent;
    };

    class Ray_BTActionFindAttackTarget_Template : public BTAction_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_BTActionFindAttackTarget_Template,BTAction_Template,994221613);
        DECLARE_SERIALIZE();
        DECLARE_BTNODE_TEMPLATE(Ray_BTActionFindAttackTarget);

    public:

        Ray_BTActionFindAttackTarget_Template();
        ~Ray_BTActionFindAttackTarget_Template() {}

        f32                 getRadius() const { return m_radius; }
        const StringID&     getFact() const { return m_fact; }
        bbool               getFilterByHeight() const { return m_filterByHeight; }

    private:

        f32                 m_radius;
        StringID            m_fact;
        bbool               m_filterByHeight;
    };

    ITF_INLINE const Ray_BTActionFindAttackTarget_Template*  Ray_BTActionFindAttackTarget::getTemplate() const {return static_cast<const Ray_BTActionFindAttackTarget_Template*>(m_template);}

} // namespace ITF

#endif // _ITF_RAY_BTACTIONFINDATTACKTARGET_H_
