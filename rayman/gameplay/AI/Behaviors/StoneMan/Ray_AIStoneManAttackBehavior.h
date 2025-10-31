#ifndef _ITF_RAY_AISTONEMANATTACKBEHAVIOR_H_
#define _ITF_RAY_AISTONEMANATTACKBEHAVIOR_H_

#ifndef _ITF_AIBEHAVIOR_H_
#include "gameplay/ai/Behaviors/AIBehavior.h"
#endif //_ITF_AIBEHAVIOR_H_

namespace ITF
{
    //forward
    class Ray_AIStoneManAttackBehavior_Template;
    class Ray_AIHunterLaunchBulletAction;
    class Ray_AIHunterLaunchBulletAction_Template;
    class Ray_StoneManAIComponent;

    //-------------------------------------------------------
    // Ray_AIStoneManAttackBehavior
    //-------------------------------------------------------
    class Ray_AIStoneManAttackBehavior : public AIBehavior
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_AIStoneManAttackBehavior, AIBehavior,3101260151)
        DECLARE_VALIDATE_BEHAVIOR()

    public:

        Ray_AIStoneManAttackBehavior();
        virtual ~Ray_AIStoneManAttackBehavior();

        virtual void            onActorLoaded(Pickable::HotReloadType _hotReload);
        virtual void            onActivate();
        virtual void            update(f32 _dt);
        virtual void            onEvent(Event* _event);
        virtual void            onActionFinished();

    private:

        ITF_INLINE const class Ray_AIStoneManAttackBehavior_Template * getTemplate() const;

        void startSleep();
        void updateSleep(const f32 _dt);

        void startWakeUp();
        void onFinishedWakeUp();
        
        void startRespawnBoulder();
        void onFinishedRespawnBoulder();
        
        void startIdle();
        void updateIdle(const f32 _dt);

        void startAnticipation();
        void onFinishedAnticipation();

        void startLaunch();
        void onFinishedLaunch();

        bbool isCurrentActionLaunch() const;

        void tryToStartLaunch(const u32 _actionIndex);

        //Data
    private:

        AIAction * m_actionRespawnBoulder;
        AIAction * m_actionIdle;
        AIAction * m_actionAnticipation;
        AIAction * m_actionSleep;
        AIAction * m_actionWakeUp;
        SafeArray<Ray_AIHunterLaunchBulletAction *> m_launchActionList;

        Ray_StoneManAIComponent * m_stoneManAIComponent;

        u32 m_launchActionIndex;

        bbool m_enabled;
        bbool m_linkedToActivator;
        bbool m_launchRequested;
    };



    //-------------------------------------------------------
    // Ray_AIStoneManAttackBehavior_Template
    //-------------------------------------------------------
    class Ray_AIStoneManAttackBehavior_Template : public TemplateAIBehavior
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_AIStoneManAttackBehavior_Template,TemplateAIBehavior,2380287857);
        DECLARE_SERIALIZE();
        DECLARE_AIBEHAVIOR_TEMPLATE(Ray_AIStoneManAttackBehavior);

    public:

        Ray_AIStoneManAttackBehavior_Template();
        ~Ray_AIStoneManAttackBehavior_Template();

        ITF_INLINE const AIAction_Template * getActionRespawnBoulder() const;
        ITF_INLINE const AIAction_Template * getActionIdle() const;
        ITF_INLINE const AIAction_Template * getActionAnticipation() const;
        ITF_INLINE const AIAction_Template * getActionSleep() const;
        ITF_INLINE const AIAction_Template * getActionWakeUp() const;

        typedef ITF_VECTOR <Ray_AIHunterLaunchBulletAction_Template *>  LaunchActionList;
        ITF_INLINE const LaunchActionList & getLaunchActionList() const;


    private:

        AIAction_Template * m_actionRespawnBoulder;
        AIAction_Template * m_actionIdle;
        AIAction_Template * m_actionAnticipation;
        AIAction_Template * m_actionSleep;
        AIAction_Template * m_actionWakeUp;
        LaunchActionList m_launchActionList;
    };



    //-------------------------------------------------------
    // Ray_AIStoneManAttackBehavior inlined functions definition
    //-------------------------------------------------------

    ITF_INLINE const Ray_AIStoneManAttackBehavior_Template * Ray_AIStoneManAttackBehavior::getTemplate() const
    {
        return static_cast<const Ray_AIStoneManAttackBehavior_Template *>(m_template);
    }


    //-------------------------------------------------------
    // Ray_AIStoneManAttackBehavior_Template inlined functions definition
    //-------------------------------------------------------

    ITF_INLINE const AIAction_Template * Ray_AIStoneManAttackBehavior_Template::getActionRespawnBoulder() const 
    { 
        return m_actionRespawnBoulder; 
    }

    ITF_INLINE const AIAction_Template * Ray_AIStoneManAttackBehavior_Template::getActionIdle() const 
    { 
        return m_actionIdle; 
    }

    ITF_INLINE const AIAction_Template * Ray_AIStoneManAttackBehavior_Template::getActionAnticipation() const 
    { 
        return m_actionAnticipation; 
    }

    ITF_INLINE const AIAction_Template * Ray_AIStoneManAttackBehavior_Template::getActionSleep() const 
    { 
        return m_actionSleep; 
    }

    ITF_INLINE const AIAction_Template * Ray_AIStoneManAttackBehavior_Template::getActionWakeUp() const 
    { 
        return m_actionWakeUp; 
    }

    ITF_INLINE const Ray_AIStoneManAttackBehavior_Template::LaunchActionList & Ray_AIStoneManAttackBehavior_Template::getLaunchActionList() const 
    { 
        return m_launchActionList;
    }
}

#endif //_ITF_RAY_AISTONEMANATTACKBEHAVIOR_H_