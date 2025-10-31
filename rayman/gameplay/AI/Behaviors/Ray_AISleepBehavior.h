#ifndef _ITF_RAY_AISLEEPBEHAVIOR_H_
#define _ITF_RAY_AISLEEPBEHAVIOR_H_

#ifndef _ITF_AIBEHAVIOR_H_
#include "gameplay/ai/Behaviors/AIBehavior.h"
#endif //_ITF_AIBEHAVIOR_H_

namespace ITF
{
    class AIAction;

    class Ray_AISleepBehavior : public AIBehavior
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_AISleepBehavior, AIBehavior,3190741816)
        DECLARE_SERIALIZE()

    public:

        Ray_AISleepBehavior();
        virtual ~Ray_AISleepBehavior();

        virtual void        onActivate();
        virtual void        onDeactivate();
        virtual void        onActionFinished();
        virtual void        onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual void        onEvent( Event* _event );

        void                wakeUp();

    private:

        ITF_INLINE const class Ray_AISleepBehavior_Template * getTemplate() const;

        AIAction*                           m_idle;
        AIAction*                           m_wakeUp;
        StickToPolylinePhysComponent *      m_physComponent;

    };

    class Ray_AISleepBehavior_Template : public TemplateAIBehavior
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_AISleepBehavior_Template,TemplateAIBehavior,3655251520);
        DECLARE_SERIALIZE();
        DECLARE_AIBEHAVIOR_TEMPLATE(Ray_AISleepBehavior);

    public:

        Ray_AISleepBehavior_Template();
        ~Ray_AISleepBehavior_Template();

        class AIAction_Template*            getIdle() const { return m_idle; }
        class AIAction_Template*            getWakeUp() const { return m_wakeUp; }
        bbool                               getDeactivatePhysics() const { return m_deactivatePhysics; }
        bbool                               getWakeUpOnTrigger() const { return m_wakeUpOnTrigger; }

    private:

        class AIAction_Template*            m_idle;
        class AIAction_Template*            m_wakeUp;
        bbool                               m_deactivatePhysics;
        bbool                               m_wakeUpOnTrigger;
    };

    ITF_INLINE const class Ray_AISleepBehavior_Template * Ray_AISleepBehavior::getTemplate() const
    {
        return static_cast<const class Ray_AISleepBehavior_Template *>(m_template);
    }
}

#endif //_ITF_RAY_AISLEEPBEHAVIOR_H_