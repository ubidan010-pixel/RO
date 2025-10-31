#ifndef _ITF_AICOMPONENT_H_
#define _ITF_AICOMPONENT_H_

#ifndef _ITF_ACTORCOMPONENT_H_
#include "engine/actors/actorcomponent.h"
#endif //_ITF_ACTORCOMPONENT_H_

#ifndef _ITF_GAMEPLAYTYPES_H_
#include "gameplay/GameplayTypes.h"
#endif //_ITF_GAMEPLAYTYPES_H_

#ifndef _ITF_INPUTCRITERIAS_H_
#include "engine/blendTree/InputCriterias.h"
#endif //_ITF_INPUTCRITERIAS_H_

#ifndef _ITF_ACTORSMANAGER_H_
#include "engine/actors/managers/actorsmanager.h"
#endif //_ITF_ACTORSMANAGER_H_

#ifndef _ITF_AIBEHAVIOR_H_
#include "gameplay/ai/Behaviors/AIBehavior.h"
#endif //_ITF_AIBEHAVIOR_H_

namespace ITF
{
    class AIBehavior;
    class PhantomComponent;
    class FXControllerComponent;
    class SoundComponent;
    class FxBankComponent;
    class HitStim;

    class AIComponent : public ActorComponent, IUpdateAnimInputCriteria, IUpdateSoundInputCriteria, IUpdateFxInputCriteria
    {
        DECLARE_OBJECT_CHILD_RTTI(AIComponent,ActorComponent,3090898017);
        DECLARE_SERIALIZE()
        DECLARE_VALIDATE_COMPONENT()

    public:
        typedef SafeArray <AIBehavior*> BehaviorsArray;
        static const u32 s_maxHitLevels;

        AIComponent();
        virtual ~AIComponent();

        virtual bbool       needsUpdate() const { return btrue; }
        virtual bbool       needsDraw() const { return bfalse; }
        virtual bbool       needsDraw2D() const { return bfalse; }

        virtual void        onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual void        onCheckpointLoaded();
        virtual void        onResourceReady();
        virtual void        onBecomeActive();
        virtual void        onBecomeInactive();

        virtual void        Update( f32 _dt );
        virtual void        onEvent( Event* _event);

        virtual void        onForceMove();

        virtual void        onDepthChanged( f32 _oldZ, f32 _newZ );

        virtual void        updateAnimInput();
        virtual void        updateSoundInput();
        virtual void        updateFxInput(){}
        void                computeRandomAnimNumber();

		virtual void        onBehaviorFinished();

        const Vec2d&        getCurrentGravity() const { return m_currentGravity; }
        const Vec2d&        getCurrentGravityDir() const { return m_currentGravityDir; }

        const SafeArray<AIBehavior*> &getBehaviors() const {return m_behaviors;}
        ITF_INLINE AIBehavior*  getCurrentBehavior() const { return m_currentBehavior; }
        ITF_INLINE AIBehavior*  getPreviousBehavior() const { return m_previousBehavior; }
        ITF_INLINE bbool        isCurrentBehavior(AIBehavior * _behavior) const { return _behavior != NULL && m_currentBehavior == _behavior;}
        const AIBehavior*       getBehaviorFromName( const StringID & _behaviorName ) const;

        // register phantom and AI
        void                    registerObjects();
        void                    unregisterObjects();

        // below: specific to AIs that can fight
        ITF_INLINE Faction      getFaction() const { return m_faction_current; }
        void                    setFaction( Faction _value );
        ITF_INLINE i32          getHealth() const { return m_health_current; }
        ITF_INLINE void         setHealth( i32 _value ) { m_health_current = _value; }

        virtual void            receiveHit( HitStim* _hit, bbool _damageDependsOnLevel = btrue, u32 _forcedDamage = 0 );
        virtual void            receiveHit( ActorRef _attacker, u32 _level, const Vec2d& _dir, const Vec3d& _fxPos, bbool _damageDependsOnLevel = btrue, u32 _forcedDamage = 0 );
        ITF_INLINE ActorRef     getLastAttacker() const { return m_lastAttacker; }
        ITF_INLINE u32          getLastHitLevel() const { return m_lastHitLevel; }
        ITF_INLINE const Vec2d& getLastHitDir() const { return m_lastHitDir; }
        ITF_INLINE const Vec3d& getLastHitFxPos() const { return m_lastHitFxPos; }

		class AIBehavior*		createAndAddAiBehaviorExtern(const class TemplateAIBehavior * _template);
		virtual void			onBehaviorExternFinished( bbool _stopped = bfalse );

#ifdef ITF_SUPPORT_DEBUGFEATURE
        virtual void            getDebugText( String8& _text, const String8& _separator = " - " );
#endif // ITF_SUPPORT_DEBUGFEATURE

#ifdef ITF_SUPPORT_EDITOR
        virtual void            drawEdit( ActorDrawEditInterface* _drawInterface, u32 _flags  ) const;
        virtual void            onEditorMove(bbool _modifyInitialPos = btrue);
#endif // ITF_SUPPORT_EDITOR

    protected:
        ITF_INLINE  const class AIComponent_Template* getTemplate() const;
        void                clear();
        void                postLoadCheckpoint();
        virtual void        resetAI();
        class AIBehavior*   createAiBehavior(const class TemplateAIBehavior * _template);
        template <typename T> T* createAiBehaviorAndCast(const class TemplateAIBehavior * _template);

        void                setBehavior( AIBehavior* _behavior, bbool _restart = bfalse );
		void				setBehavior( const StringID & _behaviorName, bbool _restart = bfalse );
        void                setNullBehavior();
        AIBehavior*         getBehaviorFromName( const StringID & _behaviorName );
		virtual bbool       onChangeBehaviorQuery( AIBehavior* _wantedBehavior );				// return true changes to wanted behavior
        virtual void        processActivate( EventActivate * onActivate );

        AnimLightComponent*     m_animComponent;
        AnimatedComponent*      m_animatedComponent;
        PhantomComponent*       m_phantomComponent;
        FXControllerComponent*  m_fxController;
        SoundComponent*         m_soundComponent;
        FxBankComponent*        m_fxBankComponent;

        BehaviorsArray      m_behaviors;
		AIBehavior*         m_currentBehavior;
		AIBehavior*         m_previousBehavior;

        f32                 m_randomAnim;

        Vec2d               m_currentGravity;
        Vec2d               m_currentGravityDir;


        // below: specific to AIs that can fight
        bbool               m_isRegisteredToAIManager;
        Faction             m_faction_current;
        i32                 m_health_current;

        ActorRef            m_lastAttacker;
        u32                 m_lastHitLevel;
        Vec2d               m_lastHitDir;
        Vec3d               m_lastHitFxPos;

        bbool               m_ready;
		bbool				m_behaviorsInit;
    private:
        void                addBehavior( AIBehavior* _behavior );

    };


    class AIComponent_Template : public TemplateActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(AIComponent_Template,TemplateActorComponent,3904365824);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(AIComponent);

    public:

        AIComponent_Template();
        ~AIComponent_Template();

        ITF_INLINE bbool getRegisterToAIManager() const { return m_registerToAIManager; }
        ITF_INLINE Faction getFaction_default() const { return m_faction_default; }
        ITF_INLINE i32 getHealth_default() const { return m_health_default; }
        ITF_INLINE const SafeArray<u32> &getDamageLevels() const { return m_damageLevels; }
        ITF_INLINE f32 getScaleRandomFactor() const { return m_scaleRandomFactor; }
        ITF_INLINE ITF::bbool getListenToActivateEvent() const { return m_listenToActivateEvent; }

    private:
        bbool               m_registerToAIManager;
        Faction             m_faction_default;
        i32                 m_health_default;
        SafeArray <u32>     m_damageLevels;
        f32                 m_scaleRandomFactor;    // actor's scale will be randomly augmented/reduced by that factor
        bbool               m_listenToActivateEvent;
    };

    ITF_INLINE  const class AIComponent_Template* AIComponent::getTemplate() const
    {
        return static_cast<const class AIComponent_Template*>(m_template);
    }

    template<typename T>
    T* AIComponent::createAiBehaviorAndCast(const TemplateAIBehavior * _template)
    {
        if (_template)
        {
            AIBehavior * behavior = _template->createAIBehavior();
            T* castedBehavior = behavior->template DynamicCast<T>(T::GetClassCRCStatic());
            if (castedBehavior)
            {
                addBehavior(behavior);
                behavior->setTemplate(_template);
                return castedBehavior;
            }
            SF_DEL(behavior);
        }
        return NULL;
    }
}

#endif // _ITF_AICOMPONENT_H_
