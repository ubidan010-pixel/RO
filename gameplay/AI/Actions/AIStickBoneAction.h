#ifndef _ITF_AISTICKBONEACTION_H_
#define _ITF_AISTICKBONEACTION_H_

#ifndef _ITF_AIACTION_H_
#include "gameplay/ai/Actions/AIAction.h"
#endif //_ITF_AIACTION_H_

namespace ITF
{
    class AIStickBoneAction : public AIAction
    {
        DECLARE_OBJECT_CHILD_RTTI(AIStickBoneAction, AIAction,2300451683);

    public:

        AIStickBoneAction();
        virtual ~AIStickBoneAction();

        virtual void                        onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual void                        onActivate();
        virtual void                        update( f32 _delta );

        void                                setStickActor(ActorRef _actor         ) { m_stickActor = _actor;}
        bbool                               isSticked();

    private:
        ITF_INLINE const class AIStickBoneAction_Template * getTemplate() const;

        PhysComponent*                      m_physComponent;

        f32                                 m_stickCurrentTime;

        ActorRef                            m_stickActor;
        u32                                 m_stickBoneIndex;
    };

    class AIStickBoneAction_Template : public AIAction_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(AIStickBoneAction_Template,AIAction_Template,1011191924);
        DECLARE_SERIALIZE();
        DECLARE_AIACTION_TEMPLATE(AIStickBoneAction);

    public:

        AIStickBoneAction_Template();
        ~AIStickBoneAction_Template(){}

        ITF_INLINE f32 getStickTime() const { return m_stickTime; }
        ITF_INLINE StringID getStickBoneName() const { return m_stickBoneName; }
        ITF_INLINE bbool getStickBoneEnd() const { return m_stickBoneEnd; }
    private:
        f32                                 m_stickTime;
        StringID                            m_stickBoneName;
        bbool                               m_stickBoneEnd;
    };

    const class AIStickBoneAction_Template * AIStickBoneAction::getTemplate() const
    {
        return static_cast<const class AIStickBoneAction_Template *>(m_template);
    }
}

#endif // _ITF_AISTICKBONEACTION_H_