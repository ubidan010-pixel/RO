#ifndef _ITF_AIROAMINGBEHAVIOR_H_
#define _ITF_AIROAMINGBEHAVIOR_H_

#ifndef _ITF_AIBEHAVIOR_H_
#include "gameplay/ai/Behaviors/AIBehavior.h"
#endif //_ITF_AIBEHAVIOR_H_

namespace ITF
{
    class AIIdleAction;
    class AIWalkInDirAction;

    class AIRoamingBehavior : public AIBehavior
    {
        DECLARE_OBJECT_CHILD_RTTI(AIRoamingBehavior, AIBehavior,664125257);
        //DECLARE_VALIDATE_BEHAVIOR()

    public:

        AIRoamingBehavior();
        virtual ~AIRoamingBehavior();

        virtual void                        onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual void                        onActivate();
        virtual void                        update( f32 _dt  );
        virtual void                        onActionFinished();

    private:
        ITF_INLINE const class AIRoamingBehavior_Template * getTemplate() const;

        StickToPolylinePhysComponent*       m_physComponent;

        AIIdleAction*                       m_idle;
        AIWalkInDirAction*                  m_walk;

        void                                updateWalk( f32 _dt );
    };


    class AIRoamingBehavior_Template : public TemplateAIBehavior
    {
        DECLARE_OBJECT_CHILD_RTTI(AIRoamingBehavior_Template,TemplateAIBehavior,3697341506);
        DECLARE_SERIALIZE();
        DECLARE_AIBEHAVIOR_TEMPLATE(AIRoamingBehavior);

    public:

        AIRoamingBehavior_Template();
        ~AIRoamingBehavior_Template();

        ITF_INLINE const class AIIdleAction_Template* getIdle() const { return m_idle; }
        ITF_INLINE const class AIWalkInDirAction_Template* getWalk() const { return m_walk; }
    private:
        class AIIdleAction_Template*                       m_idle;
        class AIWalkInDirAction_Template*                  m_walk;
    };

    const class AIRoamingBehavior_Template * AIRoamingBehavior::getTemplate() const
    {
        return static_cast<const class AIRoamingBehavior_Template *>(m_template);
    }
}

#endif //_ITF_AIROAMINGBEHAVIOR_H_

