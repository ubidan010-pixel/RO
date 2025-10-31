#ifndef _ITF_RAY_AIMAINLUMFOLLOWACTORACTION_H_
#define _ITF_RAY_AIMAINLUMFOLLOWACTORACTION_H_

#ifndef _ITF_AIFOLLOWACTORACTION_H_
#include "gameplay/ai/Actions/AIFollowActorAction.h"
#endif //_ITF_AIFOLLOWACTORACTION_H_

namespace ITF
{
    class  Ray_AIMainLumFollowActorAction : public AIFollowActorAction
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_AIMainLumFollowActorAction,AIFollowActorAction,3868496117);

    public:

        Ray_AIMainLumFollowActorAction();
        virtual ~Ray_AIMainLumFollowActorAction();

        virtual void                        onActivate();
        virtual void                        update( f32 _delta );
        virtual void                        onEvent(Event * _event);
        virtual void                        updateAnimInputs();

       
        virtual Vec3d                       getFollowPos() const ;

        void                                grow() { m_growCount++;}

    protected:
        ITF_INLINE const class Ray_AIMainLumFollowActorAction_Template* getTemplate() const;

        void                                clear();
        bbool                               m_flipped;
        f32                                 m_timer;
        Vec3d                               m_currentRelativePos;
        bbool                               m_growing;
        u32                                 m_growCount;
        bbool                               m_growStop;

    };

    class Ray_AIMainLumFollowActorAction_Template : public AIFollowActorAction_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_AIMainLumFollowActorAction_Template,AIFollowActorAction_Template,2923911931);
        DECLARE_AIACTION_TEMPLATE(Ray_AIMainLumFollowActorAction);
        DECLARE_SERIALIZE();

    public:

        Ray_AIMainLumFollowActorAction_Template();
        ~Ray_AIMainLumFollowActorAction_Template(){}

        f32                                 getTimeToFlip() const { return m_timeToFlip; }
        const StringID&                     getGrowStopMarker() const { return m_growStopMarker; }

    private:

        f32                                 m_timeToFlip;
        StringID                            m_growStopMarker;
    };

    ITF_INLINE const class Ray_AIMainLumFollowActorAction_Template * Ray_AIMainLumFollowActorAction::getTemplate() const
    {
        return static_cast<const class Ray_AIMainLumFollowActorAction_Template*>(m_template);
    }
}

#endif //_ITF_RAY_AIMAINLUMFOLLOWACTORACTION_H_

