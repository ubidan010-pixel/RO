#ifndef _ITF_AIFOLLOWACTORACTION_H_
#define _ITF_AIFOLLOWACTORACTION_H_

#ifndef _ITF_AIACTION_H_
#include "gameplay/ai/Actions/AIAction.h"
#endif //_ITF_AIACTION_H_

#ifndef _ITF_VEC2D_H_
#include "core/math/vec2d.h"
#endif //_ITF_VEC2D_H_

namespace ITF
{
    class AIFollowActorAction : public AIAction
    {
        DECLARE_OBJECT_CHILD_RTTI(AIFollowActorAction, AIAction,3338112704);

    public:

        AIFollowActorAction();
        virtual ~AIFollowActorAction();

        virtual void                        onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual void                        onActivate();
        virtual void                        update( f32 _delta );
        
        void                                setFollowActor(ObjectRef _actor         );
        void                                setRelativPos(const Vec3d & _relativPos ) { m_relativPos = _relativPos;       }
        void                                setRelativPosToActor();
        void                                setRelativPosToFollower();
        
        virtual Vec3d                       getFollowPos() const;

    protected:
        ITF_INLINE const class AIFollowActorAction_Template * getTemplate() const;

        Actor*                              getFollowActor() const;

        ObjectRef                           m_followActor;
        class PhysComponent*                      m_physComponent;
        AnimLightComponent*                 m_followAnimLight;

        Vec3d                               m_relativPos;

        Vec3d                               m_lastPos;
    };

    class AIFollowActorAction_Template : public AIAction_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(AIFollowActorAction_Template,AIAction_Template,1310157366);
        DECLARE_SERIALIZE();
        DECLARE_AIACTION_TEMPLATE(AIFollowActorAction);

    public:

        AIFollowActorAction_Template();
        ~AIFollowActorAction_Template(){}

        ITF_INLINE const Vec3d & getRelativPosToActor() const { return m_relativPosToActor; }
        ITF_INLINE const Vec3d & getRelativPosToFollower() const { return m_relativPosToFollower; }
        ITF_INLINE f32 getAcceleration() const { return m_acceleration; }
        ITF_INLINE f32 getFriction() const { return m_friction; }
        ITF_INLINE bbool getFlip() const { return m_doFlip; }

    private:
        Vec3d                               m_relativPosToActor;
        Vec3d                               m_relativPosToFollower;
        f32                                 m_acceleration;
        f32                                 m_friction;
        bbool                               m_doFlip;
    };

    ITF_INLINE const class AIFollowActorAction_Template * AIFollowActorAction::getTemplate() const
    {
        return static_cast<const class AIFollowActorAction_Template *>(m_template);
    }

}

#endif // _ITF_AIFOLLOWACTORACTION_H_

