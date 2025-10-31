#ifndef _ITF_RAY_SHOOTERSPAWNERCOMPONENT_H_
#define _ITF_RAY_SHOOTERSPAWNERCOMPONENT_H_

#ifndef _ITF_TIMEDSPAWNERCOMPONENT_H_
#include "gameplay/Components/Common/TimedSpawnerComponent.h"
#endif //_ITF_TIMEDSPAWNERCOMPONENT_H_

#ifndef _ITF_COMPONENTSUTILS_H_
#include "gameplay/Components/Common/ComponentsUtils.h"
#endif //_ITF_COMPONENTSUTILS_H_


namespace ITF
{
   class Ray_ShooterSpawnerComponent : public TimedSpawnerComponent
    {
        DECLARE_OBJECT_CHILD_RTTI( Ray_ShooterSpawnerComponent, TimedSpawnerComponent ,4291761801)
    public:
        DECLARE_SERIALIZE()

        Ray_ShooterSpawnerComponent();
        virtual ~Ray_ShooterSpawnerComponent();

        virtual bbool               needsUpdate() const { return btrue; }
        virtual bbool               needsDraw() const { return bfalse; }
        virtual bbool               needsDraw2D() const { return bfalse; }
        virtual void				onEvent( Event * _event);
        virtual void                onBecomeInactive();

        virtual void                Update( f32 _deltaTime );
		virtual void                onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual void                onCheckpointLoaded();
#ifdef ITF_SUPPORT_EDITOR
        virtual void                drawEdit( class ActorDrawEditInterface* _drawInterface, u32 _flags ) const;
        virtual void                onEditorMove(bbool _modifyInitialPos = btrue);
#endif // ITF_SUPPORT_EDITOR

        bbool                       hasAllSpawneeDead();
        ITF_INLINE const ActorRef & getLastDeadSpawnee( bbool & _hasBeenVacuumed, ActorRef & _hasBeenVacuumedOwner ) const  { _hasBeenVacuumed = m_lastDeadSpawneeHasBeenVacuumed; _hasBeenVacuumedOwner = m_lastDeadSpawneeHasBeenVacuumedOwner; return m_lastDeadSpawnee; }
        ITF_INLINE bbool            isSpawnFinished() const                 { return m_timedSpawner.isSpawnFinished(); }
        ITF_INLINE bbool            isCameraRelative() const;
        ITF_INLINE bbool            needSpawneeBind() const;

        StringID::StringIdValueType getSelectedTweenID() const              { return ( ( m_modifierTweenId != StringID::InvalidId ) ? m_modifierTweenId : m_selectedTweenId ); }
        
        static void                 onSpawneeCreated( Actor * _spawner, Actor * _spawnee );
        void                        onSpawneeCreated( Actor * _spawnee );

    protected:

        virtual const Path &        getSpawnActorPath() const;
        virtual void                declareSpawnee();
        virtual void                processStartEvent( Event * _event );
        virtual void                disable();
        void                        applyCamRelativeInitialPos();
        void                        processModifierEvent( Ray_EventShooterSpawnerDataSet * _modifier );
        
        // returns true is all linked spawner (children) have no more alive valid spawnee
        bbool                       checkLinkedSpawners();
        void                        checkActorBindedToSpawnee();
        void                        activateRewards();
        bbool                       checkRewardsDestruction();
        void                        clearBinds();
        
        struct SpawnedObjData
        {
            SpawnedObjData():m_isDead(bfalse), m_justSpawned( btrue ) { m_bindedObj.clear(); }

            ActorRef                m_spawnedObj;
            bbool                   m_isDead;
            SafeArray<ActorRef>     m_bindedObj;
            bbool                   m_justSpawned;
        };
        struct ActorBindedToSpawnee
        {
            ActorBindedToSpawnee():m_spawneeNum(0){}
            ActorRef                m_actorToBind;
            u32                     m_spawneeNum;
        };
        class RewardGenerated
        {
        public:
            RewardGenerated() : m_hasBeenVisible(bfalse){}
            RewardGenerated( ActorRef _reward ) : m_reward( _reward ), m_hasBeenVisible(bfalse){}
            ActorRef                m_reward;
            bbool                   m_hasBeenVisible;
        };
        
        ITF_VECTOR<SpawnedObjData>  m_spawnedObjDataList;

        SafeArray<ActorRef>         m_rewardLinkedSpawnerList;
        SafeArray<bbool>            m_rewardLinkedSpawnerDeadList;
        SafeArray<ActorRef>         m_rewardObjList;
        bbool                       m_checkRewards;

        bbool                       m_checkStopUpdate;
        bbool                       m_updateWaitDeadSpawneeDisable;
        bbool                       m_linkedSpawnersDisabled;
        bbool                       m_allSpawneeDead;
        bbool                       m_checkDisableNextFrame;
        ActorRef                    m_lastDeadSpawnee;
        bbool                       m_lastDeadSpawneeHasBeenVacuumed;
        ActorRef                    m_lastDeadSpawneeHasBeenVacuumedOwner;

        const ActorSpawnBank_Template::ActorSpawnData * m_spawnData;
        Event*                      m_spawnData_SpawneeOnSpawnEvent;
        bbool                       m_restartSpawnIfAllPrevSpawneeAreDead;
        
        SpawneeGenerator                m_rewardGenerator;
        SpawneeGenerator                m_reward5xGenerator;
        Ray_EventSpawnReward *          m_vacuumedReward;
        SafeArray<RewardGenerated>      m_rewardGeneratedList;

        ActorRef                        m_timedSpawnerModifierActor;        // used by spawned actor
        SafeArray<ActorBindedToSpawnee> m_actorsBindedToSpawnee;            // list of actor which will be binded to spawned actors

        StringID::StringIdValueType     m_modifierTweenId;                  // is the tween id changed by a modifier
        Event *                         m_startEvent;
        Ray_EventDynamicFogParamSet *   m_fogEvent;                         // is the spawnee fog event
        
    private:
       
        // instance vars
        StringID::StringIdValueType m_selectedActorIdInBank;
        StringID::StringIdValueType m_selectedTweenId;

        Vec3d                       m_beforeCamRelativeInitialPos;

        ITF_INLINE const class Ray_ShooterSpawnerComponent_Template * getTemplate() const;
    };

    //-------------------------------------------------------------------------------------
    class Ray_ShooterSpawnerComponent_Template : public TimedSpawnerComponent_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_ShooterSpawnerComponent_Template, TimedSpawnerComponent_Template ,2485681820);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(Ray_ShooterSpawnerComponent);

    public:

        Ray_ShooterSpawnerComponent_Template();
        ~Ray_ShooterSpawnerComponent_Template();

        ITF_INLINE const ActorSpawnBank_Template *      getBank() const { return &m_bank; }
        ITF_INLINE const ITF_VECTOR<StringID> &         getTweenInstructionSetList() const { return m_tweenInstructionSetList; }
        ITF_INLINE const bbool                          isCameraRelative() const    { return m_cameraRelative; }
        ITF_INLINE const bbool                          needSpawneeBind() const     { return m_bindSpawnee; }
        ITF_INLINE const Path&                          getRewardSpawnPath() const  { return m_rewardSpawnPath; }
        ITF_INLINE const Path&                          getReward5xSpawnPath() const{ return m_reward5xSpawnPath; }
        ITF_INLINE const Ray_EventSpawnReward * 		getVacuumedReward() const   { return m_vacuumedReward; }

    private:

        ActorSpawnBank_Template     m_bank;
        ITF_VECTOR<StringID>        m_tweenInstructionSetList;
        bbool                       m_cameraRelative;
        bbool                       m_bindSpawnee;
        Path                        m_rewardSpawnPath;
        Path                        m_reward5xSpawnPath;
        Ray_EventSpawnReward *      m_vacuumedReward;

    };

    ITF_INLINE const class Ray_ShooterSpawnerComponent_Template * Ray_ShooterSpawnerComponent::getTemplate() const
    {
        return static_cast<const class Ray_ShooterSpawnerComponent_Template *>(m_template);
    }

    ITF_INLINE bbool Ray_ShooterSpawnerComponent::isCameraRelative() const
    {
        return getTemplate()->isCameraRelative();
    }
    ITF_INLINE bbool Ray_ShooterSpawnerComponent::needSpawneeBind() const
    {
        return getTemplate()->needSpawneeBind();
    }

    //*************************************************************************************************
    //                              Component of timed spawner modifier
    //*************************************************************************************************
    class Ray_ShooterSpawnerModifierComponent : public TimedSpawnerModifierComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_ShooterSpawnerModifierComponent, TimedSpawnerModifierComponent,3122658323)

    public:
        DECLARE_SERIALIZE()

        Ray_ShooterSpawnerModifierComponent(): Super() , m_selectedTweenId( StringID::InvalidId ) {}
        virtual ~Ray_ShooterSpawnerModifierComponent() {}

        virtual bbool               needsUpdate() const { return bfalse; }
        virtual bbool               needsDraw() const { return bfalse; }
        virtual bbool               needsDraw2D() const { return bfalse; }
        //virtual void				onEvent( Event * _event);
        //virtual void              onActorLoaded(Pickable::HotReloadType /*_hotReload*/);


        static void EventToData( const Ray_EventShooterSpawnerDataSet & _event, TimedSpawnerData & _data, StringID::StringIdValueType & _res_tweenID ) 
        {
            _event.getData( _data.m_spawnDelay, _data.m_spawnRate, _data.m_burstElementsCount, _data.m_burstCount, _data.m_burstDelay );
            _res_tweenID = _event.getTweenID();
        }

        static void DataToEvent( const TimedSpawnerData & _data, const StringID::StringIdValueType & _tweenID, Ray_EventShooterSpawnerDataSet & _event ) 
        {
            _event.setData( _data.m_spawnDelay, _data.m_spawnRate, _data.m_burstElementsCount, _data.m_burstCount, _data.m_burstDelay );
            _event.setTweenID( _tweenID );
        }

#ifdef ITF_SUPPORT_EDITOR
        virtual void                drawEdit( class ActorDrawEditInterface* _drawInterface, u32 _flags ) const;
#endif // ITF_SUPPORT_EDITOR

    private:

        virtual void        sendEventToChildren();

        StringID::StringIdValueType m_selectedTweenId;

        ITF_INLINE const class Ray_ShooterSpawnerModifierComponent_Template*  getTemplate() const;
    };

    ///////////////////////////////////////////////////////////////////////////////////////////
    class Ray_ShooterSpawnerModifierComponent_Template : public TimedSpawnerModifierComponent_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_ShooterSpawnerModifierComponent_Template, TimedSpawnerModifierComponent_Template,153243771);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(Ray_ShooterSpawnerModifierComponent); 
    public :
        ITF_INLINE const ITF_VECTOR<StringID> &         getTweenInstructionSetList() const { return m_tweenInstructionSetList; }
    private:
        ITF_VECTOR<StringID>        m_tweenInstructionSetList;
    };

    const Ray_ShooterSpawnerModifierComponent_Template*  Ray_ShooterSpawnerModifierComponent::getTemplate() const {return static_cast<const Ray_ShooterSpawnerModifierComponent_Template*>(m_template);}

}

#endif // _ITF_RAY_SHOOTERSPAWNERCOMPONENT_H_
