#ifndef _ITF_RAY_BTACTIONFOLLOWTARGET_H_
#define _ITF_RAY_BTACTIONFOLLOWTARGET_H_

#ifndef _ITF_BTACTIONWALKTOTARGET_H_
#include "gameplay/ai/BTs/BTActionWalkToTarget.h"
#endif // _ITF_BTACTIONWALKTOTARGET_H_

namespace ITF
{

    class Ray_BTActionFollowTarget : public BTActionWalkToTarget
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_BTActionFollowTarget,BTActionWalkToTarget,2226721074);

    public:

        Ray_BTActionFollowTarget();
        ~Ray_BTActionFollowTarget();

        virtual bbool           validate( Actor* _actor ) const;
        virtual bbool           decide( f32 _dt );
        virtual void            onActivate();
        virtual bbool           update( f32 _dt );

    private:

        const class Ray_BTActionFollowTarget_Template*  getTemplate() const;

        static const f32 s_searchMinTime;
        static const f32 s_searchMaxnTime;

        void                    setSearchTime();
        void                    setCurrentTargetPos();

        ObjectRef               m_finalTarget;
        ObjectRef               m_currentTarget;
        f32                     m_searchTimer;
        Vec3d                   m_prevJumpTarget;
        bbool                   m_jumping;
    };

    class Ray_BTActionFollowTarget_Template : public BTActionWalkToTarget_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_BTActionFollowTarget_Template,BTActionWalkToTarget_Template,1106777397);
        DECLARE_SERIALIZE();
        DECLARE_BTNODE_TEMPLATE(Ray_BTActionFollowTarget);

    public:

        Ray_BTActionFollowTarget_Template();
        ~Ray_BTActionFollowTarget_Template() {}

        f32                 getRange() const { return m_range; }
        const StringID&     getFollowTarget() const { return m_followTarget; }

    private:

        f32                 m_range;
        StringID            m_followTarget;
    };

    ITF_INLINE const Ray_BTActionFollowTarget_Template*  Ray_BTActionFollowTarget::getTemplate() const {return static_cast<const Ray_BTActionFollowTarget_Template*>(m_template);}

} // namespace ITF

#endif // _ITF_RAY_BTACTIONFOLLOWTARGET_H_
