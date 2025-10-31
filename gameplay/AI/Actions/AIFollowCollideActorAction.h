

#ifndef _ITF_AIFOLLOWCOLLIDEACTORACTION_H_
#define _ITF_AIFOLLOWCOLLIDEACTORACTION_H_

#ifndef _ITF_AIACTION_H_
#include "gameplay/ai/Actions/AIAction.h"
#endif //_ITF_AIACTION_H_

#ifndef _ITF_VEC2D_H_
#include "core/math/vec2d.h"
#endif //_ITF_VEC2D_H_

#ifndef _ITF_AIFOLLOWACTORACTION_H_
#include "gameplay/ai/Actions/AIFollowActorAction.h"
#endif //_ITF_AIFOLLOWACTORACTION_H_

namespace ITF
{

    class AIFollowCollideActorAction : public AIFollowActorAction
    {
        DECLARE_OBJECT_CHILD_RTTI(AIFollowCollideActorAction, AIAction,390664510);

    public:

        AIFollowCollideActorAction();
        virtual ~AIFollowCollideActorAction();

        virtual void                        onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual void                        onActivate();
        virtual void                        update( f32 _delta );
        
    private:

        class StickToPolylinePhysComponent* m_stickPhysComponent;
        f32                                 m_contactOutPower;
        
        Vec2d                               m_addedForce;
        f32                                 m_addedForceTime;
        f32                                 m_addedForceTotalTime;
        f32                                 m_way;
    };

    class AIFollowCollideActorAction_Template : public AIFollowActorAction_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(AIFollowCollideActorAction_Template,AIFollowActorAction_Template,1575891110);
        DECLARE_SERIALIZE();
        DECLARE_AIACTION_TEMPLATE(AIFollowCollideActorAction);

    public:

        AIFollowCollideActorAction_Template(){}
        ~AIFollowCollideActorAction_Template(){}


    private:
    };
}

#endif // _ITF_AIFOLLOWCOLLIDEACTORACTION_H_

