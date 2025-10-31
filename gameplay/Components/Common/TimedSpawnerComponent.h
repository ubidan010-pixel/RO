#ifndef _ITF_TIMEDSPAWNERCOMPONENT_H_
#define _ITF_TIMEDSPAWNERCOMPONENT_H_

#ifndef _ITF_SPAWNER_H_
#include "engine/actors/managers/Spawner.h"
#endif

namespace ITF
{
    //*************************************************************************************************
    //                                 Timed spawner DATA
    //*************************************************************************************************
    class TimedSpawnerData
    {
    public:
        DECLARE_SERIALIZE()
        TimedSpawnerData() 
            : m_spawnDelay(0)
            , m_spawnRate(1)
            , m_burstCount(-1)
            , m_burstElementsCount(-1)
            , m_burstDelay( 0.0f ){}

        TimedSpawnerData( const TimedSpawnerData & _data ) 
        {
            m_spawnDelay            = _data.m_spawnDelay;
            m_spawnRate             = _data.m_spawnRate;
            m_burstElementsCount    = _data.m_burstElementsCount;
            m_burstCount            = _data.m_burstCount;
            m_burstDelay            = _data.m_burstDelay;   
        }
        TimedSpawnerData( const EventTimedSpawnerDataSet * _event ) 
        {
            _event->getData( m_spawnDelay, m_spawnRate, m_burstElementsCount, m_burstCount, m_burstDelay );
        }
        // instance vars
        f32	m_spawnDelay;           // first spawn after 'delay' seconds
        f32 m_spawnRate;            // then spawn every 'rate' seconds
        i32 m_burstElementsCount;   // number of spawned object (-1 is infinite)
        i32 m_burstCount;           // number of burst to do (-1 is infinite)
        f32 m_burstDelay;           // delay between burst

        static void EventToData( const EventTimedSpawnerDataSet & _event, TimedSpawnerData & _data ) 
        {
            _event.getData( _data.m_spawnDelay, _data.m_spawnRate, _data.m_burstElementsCount, _data.m_burstCount, _data.m_burstDelay );
        }

        static void DataToEvent( const TimedSpawnerData & _data, EventTimedSpawnerDataSet & _event ) 
        {
            _event.setData( _data.m_spawnDelay, _data.m_spawnRate, _data.m_burstElementsCount, _data.m_burstCount, _data.m_burstDelay );
        }
    };


    //*************************************************************************************************
    //                                      Timed spawner
    // Spawns actors at intervals.
    //*************************************************************************************************
    class TimedSpawner
    {
    public:
        TimedSpawner();
        virtual ~TimedSpawner();

        typedef void ( *OnSpawneeCreatedCB)( Actor * _spawner, Actor * _spawnee );

        virtual void                        initialize( Actor * _actor, const TimedSpawnerData * _data, const Event * _onSpawnEvent );
        virtual void                        update( f32 _deltaTime );
        virtual void                        declareSpawnee( const Path & _path );
        void                                setActivate( bbool _activate, bbool _reset = btrue );
        ITF_INLINE void                     setTimedSpawnData( const TimedSpawnerData * _data ) { m_data = _data; }
        ITF_INLINE void                     registerOnSpawneeCreatedCB( OnSpawneeCreatedCB  _createSpawneeCB )  { m_createSpawneeCB = _createSpawneeCB; }
        ITF_INLINE void                     warmSpawnOnly ( bbool _warmOnly )           { m_warmSpawnOnly = _warmOnly; }
        ITF_INLINE void                     recycleSpawnee( bbool _recycle )            { m_recycleSpawnee = _recycle; }
        ITF_INLINE void                     useSpawnerScale( bbool _useScale )          { m_useSpawnerScale = _useScale; }

        ITF_INLINE bbool                    isSpawnFinished() const                     { return m_spawnFinished; }
        ITF_INLINE bbool                    isSpawneeValid() const                      { return m_spawneeValid; }
        ITF_INLINE Actor *                  getJustSpawnedActor() const                 { return m_justSpawnedActor; }
        ITF_INLINE bbool                    warmIsSpawnNeeded() const                   { return m_warmNeedSpawn; }
        bbool                               isBurstFinished() const;
        
        ITF_INLINE const TimedSpawnerData*	getTimedSpawnerData()const                  { return m_data; }
        ITF_INLINE f32	                    getNextSpawnDelay()const                    { return ( ( m_data == NULL ) ? F32_INFINITY : Max( ( m_data->m_spawnRate - m_spawnTimer ) +  Max( m_data->m_spawnDelay - m_timer, 0.f ), 0.f ) ); }
        ITF_INLINE bbool	                isActivated()const                          { return m_activated; }

        Actor *                             createSpawnee();
        ITF_INLINE const SpawneeGenerator&  getGenerator() const { return m_generator; }
        void                                reset();

    protected:
        
        SpawneeGenerator            m_generator;
        ActorRef                    m_spawnedActor;
        bbool                       m_spawneeValid;
        bbool                       m_spawnFinished;

        f32                         m_timer;
        f32                         m_spawnTimer;
        f32                         m_burstTimer;
        i32                         m_burstElementsNb;
        i32                         m_burstNb;
        bbool						m_activated;
        bbool                       m_recycleSpawnee;               // flag used to keep spawned actor and recyle it
        bbool                       m_useSpawnerScale;
        
        bbool                       m_warmSpawnOnly;
        bbool                       m_warmNeedSpawn;

        Event *                     m_onSpawnEvent;
        const TimedSpawnerData *    m_data;
        Actor*                      m_actor;

        Actor*                      m_justSpawnedActor;
        OnSpawneeCreatedCB          m_createSpawneeCB;
    };


    //*************************************************************************************************
    //                              Component of timed spawner
    //*************************************************************************************************
    class TimedSpawnerComponent : public ActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(TimedSpawnerComponent, ActorComponent,3330253882);

    public:
        DECLARE_SERIALIZE()

        TimedSpawnerComponent();
        virtual ~TimedSpawnerComponent();

        virtual bbool               needsUpdate() const { return btrue; }
        virtual bbool               needsDraw() const { return bfalse; }
        virtual bbool               needsDraw2D() const { return bfalse; }
		virtual void				onEvent( Event * _event);
        virtual void				onCheckpointLoaded();

        virtual void                onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual void                Update( f32 _deltaTime );

    protected:
        
        virtual const Path &        getSpawnActorPath() const;
        virtual void                declareSpawnee();
        virtual void                processStartEvent( Event * _event );
        virtual void                processStopEvent( Event * _event );
        virtual void                disable();

        TimedSpawner                m_timedSpawner;
        TimedSpawnerData            m_timedSpawnerData;
        bbool                       m_modifierUsed;

    private:

        ITF_INLINE const class TimedSpawnerComponent_Template*  getTemplate() const;
        //ITF_INLINE void             resetTimer();
        ITF_INLINE const f32        getSpawnDelay()         const;
        ITF_INLINE const f32        getSpawnRate()          const;
        ITF_INLINE const i32		getBurstCount()         const;
        ITF_INLINE const i32		getBurstElementCount()  const;
        ITF_INLINE const f32		getBurstDelay()         const;
        
        // instance vars
        f32						    m_instance_SpawnDelay;           // first spawn after 'delay' seconds
        f32                         m_instance_spawnRate;            // then spawn every 'rate' seconds
        i32                         m_instance_burstElementsCount;   // number of spawned object (-1 is infinite)
        i32                         m_instance_burstCount;           // number of burst to do (-1 is infinite)
        f32                         m_instance_burstDelay;           // delay between burst        
    };

    ///////////////////////////////////////////////////////////////////////////////////////////
    class TimedSpawnerComponent_Template : public TemplateActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(TimedSpawnerComponent_Template, TemplateActorComponent,84813706);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(TimedSpawnerComponent);
    public:
        TimedSpawnerComponent_Template();
        virtual ~TimedSpawnerComponent_Template();

        ITF_INLINE const Path  &		getActorToSpawn()       const {return m_actorToSpawn;}
        ITF_INLINE f32					getSpawnDelay()         const {return m_spawnDelay;}
        ITF_INLINE f32					getSpawnRate()          const {return m_spawnRate;}
        ITF_INLINE i32					getBurstCount()         const {return m_burstCount;}
        ITF_INLINE i32					getBurstElementCount()  const {return m_burstElementsCount;}
        ITF_INLINE f32					getBurstDelay()         const {return m_burstDelay;}
        
        ITF_INLINE bbool				getRecycling()         const {return m_recycling;}

		ITF_INLINE const Event*         getStartEvent()     const { return m_startEvent; }
		ITF_INLINE const Event*         getStopEvent()      const { return m_stopEvent; }
        ITF_INLINE const Event*         getOnSpawnEvent()   const { return m_onSpawnEvent; }

        ITF_INLINE const bbool          useInstanceValues()     const { return m_useInstanceValues; }

    private:

        Path                    m_actorToSpawn;
        f32						m_spawnDelay;           // first spawn after 'delay' seconds
        f32                     m_spawnRate;            // then spawn every 'rate' seconds
        i32                     m_burstElementsCount;   // number of spawned object (-1 is infinite)
        i32                     m_burstCount;           // number of burst to do (-1 is infinite)
        f32                     m_burstDelay;           // delay between burst

        bbool                   m_useInstanceValues;
        bbool                   m_recycling;            // recycle the current spawnee

		Event*                  m_startEvent;
		Event*                  m_stopEvent;
        Event*                  m_onSpawnEvent;           // event sent to spawned objects at spawning
    };

    const TimedSpawnerComponent_Template*  TimedSpawnerComponent::getTemplate() const {return static_cast<const TimedSpawnerComponent_Template*>(m_template);}
    
    ITF_INLINE const f32        TimedSpawnerComponent::getSpawnDelay()          const { return ( getTemplate()->useInstanceValues() ? m_instance_SpawnDelay : getTemplate()->getSpawnDelay() ); }
    ITF_INLINE const f32        TimedSpawnerComponent::getSpawnRate()           const { return ( getTemplate()->useInstanceValues() ? m_instance_spawnRate : getTemplate()->getSpawnRate() ); }
    ITF_INLINE const i32		TimedSpawnerComponent::getBurstCount()          const { return ( getTemplate()->useInstanceValues() ? m_instance_burstCount : getTemplate()->getBurstCount() ); }
    ITF_INLINE const i32		TimedSpawnerComponent::getBurstElementCount()   const { return ( getTemplate()->useInstanceValues() ? m_instance_burstElementsCount : getTemplate()->getBurstElementCount() ); }
    ITF_INLINE const f32		TimedSpawnerComponent::getBurstDelay()          const { return ( getTemplate()->useInstanceValues() ? m_instance_burstDelay : getTemplate()->getBurstDelay() ); }


    //*************************************************************************************************
    //                              Component of timed spawner modifier
    //*************************************************************************************************
    class TimedSpawnerModifierComponent : public ActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(TimedSpawnerModifierComponent, ActorComponent,1576493977);

    public:
        DECLARE_SERIALIZE()

        TimedSpawnerModifierComponent(): Super() {}
        virtual ~TimedSpawnerModifierComponent() {}

        virtual bbool               needsUpdate() const { return bfalse; }
        virtual bbool               needsDraw() const { return bfalse; }
        virtual bbool               needsDraw2D() const { return bfalse; }
        virtual void				onEvent( Event * _event);
        virtual void                onActorLoaded(Pickable::HotReloadType /*_hotReload*/);

        ITF_INLINE const TimedSpawnerData &     getTimedSpawnerData() const;

    private:

        virtual void                sendEventToChildren();

        ITF_INLINE const class TimedSpawnerModifierComponent_Template*  getTemplate() const;
        TimedSpawnerData            m_timedSpawnerData;
    };

    ///////////////////////////////////////////////////////////////////////////////////////////
    class TimedSpawnerModifierComponent_Template : public TemplateActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(TimedSpawnerModifierComponent_Template, TemplateActorComponent,3567246828);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(TimedSpawnerModifierComponent);
    public:
        TimedSpawnerModifierComponent_Template(): Super(), m_useInstanceValues(btrue), m_isTriggered(bfalse) {}
        virtual ~TimedSpawnerModifierComponent_Template(){}
  
        ITF_INLINE const TimedSpawnerData &     getData()           const   { return m_timedSpawnerData; }
        ITF_INLINE const bbool                  useInstanceValues() const   { return m_useInstanceValues; }
        ITF_INLINE const bbool                  isTriggered()       const   { return m_isTriggered; }

    private:

       TimedSpawnerData             m_timedSpawnerData;
       bbool                        m_useInstanceValues;
       bbool                        m_isTriggered;

    };

    const TimedSpawnerModifierComponent_Template*  TimedSpawnerModifierComponent::getTemplate() const {return static_cast<const TimedSpawnerModifierComponent_Template*>(m_template);}
    
    const TimedSpawnerData &     TimedSpawnerModifierComponent::getTimedSpawnerData() const   { return ( getTemplate()->useInstanceValues() ? m_timedSpawnerData : getTemplate()->getData() ); }

}

#endif // _ITF_TIMEDSPAWNERCOMPONENT_H_