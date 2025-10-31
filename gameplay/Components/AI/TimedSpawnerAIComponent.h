#ifndef _ITF_TIMEDSPAWNERAICOMPONENT_H_
#define _ITF_TIMEDSPAWNERAICOMPONENT_H_

#ifndef _ITF_AICOMPONENT_H_
#include "gameplay/Components/AI/AIComponent.h"
#endif //_ITF_AICOMPONENT_H_

#ifndef _ITF_TIMEDSPAWNERCOMPONENT_H_
#include "gameplay/Components/Common/TimedSpawnerComponent.h"
#endif //_ITF_TIMEDSPAWNERCOMPONENT_H_

namespace ITF
{
    class TimedSpawnerAIComponent : public AIComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(TimedSpawnerAIComponent, AIComponent,4093386385);
        DECLARE_SERIALIZE()

    public:

        TimedSpawnerAIComponent();
        virtual ~TimedSpawnerAIComponent();

        virtual void        Update( f32 _dt );
        virtual void        onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual void        onEvent( Event* _event);
        virtual void        onBehaviorFinished();

        ITF_INLINE Actor *  getJustSpawnedActor()           { return m_timedSpawner.getJustSpawnedActor(); }
        
        ITF_INLINE void     setSpawnerData( const TimedSpawnerData & _data )    { m_timedSpawnerData = _data; }
        ITF_INLINE void     setSpawnDelay( f32 _delay )                         { m_timedSpawnerData.m_spawnDelay = _delay; }

    protected:
        ITF_INLINE const class TimedSpawnerAIComponent_Template * getTemplate() const;

        virtual void        processHit( HitStim * _hitStim );

        TimedSpawner        m_timedSpawner;
        TimedSpawnerData    m_timedSpawnerData;
        TimedSpawnerData    m_timedSpawnerDataInstance;

        AIBehavior*         m_disableBehavior;
        AIBehavior*         m_activateBehavior;
        AIBehavior*         m_anticipateBehavior;
        AIBehavior*         m_spawnBehavior;
        class AIReceiveHitBehavior *  m_receiveHitBehavior;
        AIBehavior *        m_deathBehavior;
    };

    class TimedSpawnerAIComponent_Template : public AIComponent_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(TimedSpawnerAIComponent_Template,AIComponent_Template,2720447795);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(TimedSpawnerAIComponent);

    public:

        TimedSpawnerAIComponent_Template();
        ~TimedSpawnerAIComponent_Template();

        ITF_INLINE const class TemplateAIBehavior*              getDisableBehavior()        const { return m_disableBehavior; }
        ITF_INLINE const class TemplateAIBehavior*              getActivateBehavior()       const { return m_activateBehavior; }
        ITF_INLINE const class TemplateAIBehavior*              getAnticipateBehavior()     const { return m_anticipateBehavior; }
        ITF_INLINE const class TemplateAIBehavior*              getSpawnBehavior()          const { return m_spawnBehavior; }
        ITF_INLINE const class AIReceiveHitBehavior_Template *  getReceiveHitBehavior()     const { return m_receiveHitBehavior; }
        ITF_INLINE const class TemplateAIBehavior*              getDeathBehavior()          const { return m_deathBehavior; }
        ITF_INLINE const class TimedSpawnerComponent_Template * getTimedSpawnerTemplate()   const { return &m_timedSpawnerTemplate; }
        ITF_INLINE const f32 &                                  getAnticipateDuration()     const { return m_anticipateDuration; }

    private:
        class TemplateAIBehavior*         m_disableBehavior;
        class TemplateAIBehavior*         m_activateBehavior;
        class TemplateAIBehavior*         m_anticipateBehavior;
        class TemplateAIBehavior*         m_spawnBehavior;
        class AIReceiveHitBehavior_Template * m_receiveHitBehavior;
        class TemplateAIBehavior*         m_deathBehavior;

        TimedSpawnerComponent_Template    m_timedSpawnerTemplate;
        f32                               m_anticipateDuration;
    };

    ITF_INLINE const class TimedSpawnerAIComponent_Template * TimedSpawnerAIComponent::getTemplate() const
    {
        return static_cast<const class TimedSpawnerAIComponent_Template *>(m_template);
    }
}

#endif // _ITF_TIMEDSPAWNERAICOMPONENT_H_