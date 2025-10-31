#ifndef _ITF_RAY_AIEXPLODEACTION_H_
#define _ITF_RAY_AIEXPLODEACTION_H_

#ifndef _ITF_AIACTION_H_
#include "gameplay/ai/Actions/AIAction.h"
#endif //_ITF_AIACTION_H_

#ifndef _ITF_SPAWNER_H_
#include "engine/actors/managers/Spawner.h"
#endif

namespace ITF
{
    class Ray_AIExplodeAction : public AIAction
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_AIExplodeAction, AIAction,211892041);

    public:

        Ray_AIExplodeAction();
        virtual ~Ray_AIExplodeAction();

        virtual void            update( f32 _delta );
        virtual void            onEvent( Event* _event);
        virtual void            onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual void            onActivate();
        virtual void            onDeactivate();
    protected:
        ITF_INLINE const class Ray_AIExplodeAction_Template * getTemplate() const;

        void                sendStim();
        void                initReward();
        void                spawnReward();

        PhysShapeCircle		m_stimShapeCircle;
        f32                 m_curExplodeRadius;
        f32                 m_curExplodeTime;
        SpawneeGenerator    m_fragmentsGenerator;
        bbool               m_spawnFragments;
        f32                 m_waitTimerBeforeDestruction;       // to avoid FX cut....

        Ray_EventSpawnReward*   m_reward;
    };

    class Ray_AIExplodeAction_Template : public AIAction_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_AIExplodeAction_Template,AIAction_Template,3552256988);
        DECLARE_SERIALIZE();
        DECLARE_AIACTION_TEMPLATE(Ray_AIExplodeAction);

    public:

        Ray_AIExplodeAction_Template();
        ~Ray_AIExplodeAction_Template();

        ITF_INLINE const f32                    getMinRadius()  const       { return m_minRadius; }
        ITF_INLINE const f32                    getMaxRadius()  const       { return m_maxRadius; }
        ITF_INLINE const f32                    getDuration()   const       { return m_duration; }
        ITF_INLINE const bbool                  getCheckEnv()   const       { return m_checkEnv; }
        ITF_INLINE const EReceivedHitType       getHitType()    const       { return m_hitType; }
        ITF_INLINE const u32                    getHitLevel()   const       { return m_hitLevel; }
        ITF_INLINE const bbool                  getDestroyAtEnd()const      { return m_destroyAtEnd; }
        ITF_INLINE const StringID &             getFxName()const            { return m_fxName; }
#ifdef ITF_SUPPORT_DEBUGFEATURE
        ITF_INLINE const bbool                  getDbgDrawExplodeRadius()const{ return m_dbgDrawExplodeRadius; }
#endif // ITF_SUPPORT_DEBUGFEATURE
        
        ITF_INLINE const Path &                 getSpawnFragmentsPath()const{ return m_spawnFragmentsPath; }
        ITF_INLINE const u32 &                  getSpawnFragmentsNb()const  { return m_spawnFragmentsNb; }
        ITF_INLINE const Angle &                getSpawnFragmentsDeltaAngle()const  { return m_spawnFragmentsDeltaAngle; }
        ITF_INLINE const Angle &                getSpawnFragmentsStartAngle()const  { return m_spawnFragmentsStartAngle; }


        ITF_INLINE const Ray_EventSpawnReward*  getReward() const           { return m_reward; }
        ITF_INLINE const SafeArray<u32,4> &     getNumRewards() const       { return m_numRewards; }
        
    private:

        f32                     m_minRadius;
        f32                     m_maxRadius;
        f32                     m_duration;
        bbool                   m_checkEnv;
        EReceivedHitType        m_hitType;
        u32                     m_hitLevel;
        bbool                   m_destroyAtEnd;
#ifdef ITF_SUPPORT_DEBUGFEATURE
        bbool                   m_dbgDrawExplodeRadius;
#endif // ITF_SUPPORT_DEBUGFEATURE
        
        Path                    m_spawnFragmentsPath;
        u32                     m_spawnFragmentsNb;
        Angle                   m_spawnFragmentsDeltaAngle;
        Angle                   m_spawnFragmentsStartAngle;
        
        StringID                m_fxName;

        Ray_EventSpawnReward*   m_reward;
        SafeArray<u32,4>        m_numRewards;
        
    };

    ITF_INLINE const class Ray_AIExplodeAction_Template * Ray_AIExplodeAction::getTemplate() const
    {
        return static_cast<const class Ray_AIExplodeAction_Template *>(m_template);
    }
}

#endif //_ITF_RAY_AIEXPLODEACTION_H_
