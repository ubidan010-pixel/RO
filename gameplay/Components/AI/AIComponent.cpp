#include "precompiled_gameplay.h"

#ifndef _ITF_AICOMPONENT_H_
#include "gameplay/Components/AI/AIComponent.h"
#endif //_ITF_AICOMPONENT_H_

#ifndef _ITF_AIBEHAVIOR_H_
#include "gameplay/ai/Behaviors/AIBehavior.h"
#endif //_ITF_AIBEHAVIOR_H_

#ifndef _ITF_PHYSPHANTOM
#include "engine/physics/PhysPhantom.h"
#endif //_ITF_PHYSPHANTOM

#ifndef _ITF_PHYSWORLD_H_
#include "engine/physics/PhysWorld.h"
#endif //_ITF_PHYSWORLD_H_

#ifndef _ITF_AIMANAGER_H_
#include "gameplay/managers/AIManager.h"
#endif //_ITF_AIMANAGER_H_

#ifndef _ITF_GAMEMANAGER_H_
#include "gameplay/Managers/GameManager.h"
#endif //_ITF_GAMEMANAGER_H_

#ifndef _ITF_AIUTILS_H_
#include "gameplay/AI/Utils/AIUtils.h"
#endif //_ITF_AIUTILS_H_

#ifndef _ITF_FXCONTROLLERCOMPONENT_H_
#include "gameplay/components/Misc/FXControllerComponent.h"
#endif //_ITF_FXCONTROLLERCOMPONENT_H_

#ifndef _ITF_PHANTOMCOMPONENT_H_
#include "gameplay/Components/Misc/PhantomComponent.h"
#endif //_ITF_PHANTOMCOMPONENT_H_

#ifndef _ITF_CHEATMANAGER_H_
#include "engine/gameplay/CheatManager.h"
#endif //_ITF_CHEATMANAGER_H_

#ifndef _ITF_SOUNDCOMPONENT_H_
#include "engine/actors/components/SoundComponent.h"
#endif //_ITF_SOUNDCOMPONENT_H_

#ifndef _ITF_FXBANKCOMPONENT_H_
#include "engine/actors/components/FxBankComponent.h"
#endif //_ITF_FXBANKCOMPONENT_H_

namespace ITF
{
	const u32 AIComponent::s_maxHitLevels = 4;


    ///////////////////////////////////////////////////////////////////////////////////////////
    IMPLEMENT_OBJECT_RTTI(AIComponent_Template)
    BEGIN_SERIALIZATION(AIComponent_Template)
        SERIALIZE_MEMBER("registerToAIManager", m_registerToAIManager);
        SERIALIZE_MEMBER("faction", m_faction_default);
        SERIALIZE_MEMBER("health", m_health_default);
        SERIALIZE_CONTAINER("damageLevels", m_damageLevels);
        SERIALIZE_MEMBER("scaleRandomFactor", m_scaleRandomFactor);    
        SERIALIZE_MEMBER("listenToActivateEvent", m_listenToActivateEvent);    
    END_SERIALIZATION()

        AIComponent_Template::AIComponent_Template()
        : m_scaleRandomFactor(0.f)
        , m_registerToAIManager(btrue)
        , m_faction_default(0)  // NEUTRAL
        , m_health_default(100)
        , m_listenToActivateEvent(bfalse)
    {
        m_damageLevels.resize(AIComponent::s_maxHitLevels);
        m_damageLevels[0] = 5;
        m_damageLevels[1] = 25;
        m_damageLevels[2] = 50;
        m_damageLevels[3] = 100;
    }

    AIComponent_Template::~AIComponent_Template()
    {
    }

    IMPLEMENT_OBJECT_RTTI(AIComponent)
    BEGIN_SERIALIZATION_CHILD(AIComponent)
        BEGIN_CONDITION_BLOCK(ESerializeGroup_Checkpoint)
            SERIALIZE_MEMBER("health", m_health_current);
        END_CONDITION_BLOCK()
    END_SERIALIZATION()

    BEGIN_VALIDATE_COMPONENT(AIComponent)

        // validate behaviors
        const u32 behaviorCount = m_behaviors.size();
        for ( u32 i = 0; i < behaviorCount; i++ )
        {
            m_behaviors[i]->validate(_isComponentValidated);
        }

        VALIDATE_COMPONENT_PARAM("damageLevels", getTemplate()->getDamageLevels().size() == s_maxHitLevels, "AIs must define %d damage levels", s_maxHitLevels);

    END_VALIDATE_COMPONENT()



    void AIComponent::resetAI()
    {
        if ( getTemplate() )
        {
            setFaction(getTemplate()->getFaction_default());
            setHealth(getTemplate()->getHealth_default());
        }
        setNullBehavior();
    }

    AIComponent::AIComponent()
        : Super()
        , m_animComponent(NULL)
        , m_animatedComponent(NULL)
        , m_phantomComponent(NULL)
        , m_fxController(NULL)
        , m_currentBehavior(NULL)
		, m_previousBehavior(NULL)
        , m_randomAnim(0.f)
        , m_currentGravity(-Vec2d::Up)
        , m_currentGravityDir(-Vec2d::Up)
        , m_isRegisteredToAIManager(bfalse)
        , m_faction_current(0)  // NEUTRAL
        , m_health_current(100)
        , m_lastAttacker(ITF_INVALID_OBJREF)
        , m_lastHitLevel(U32_INVALID)
		, m_behaviorsInit( bfalse )
    {
    }

    AIComponent::~AIComponent()
    {
        clear();
    }

    void AIComponent::clear()
    {
        m_currentBehavior = NULL;
		m_previousBehavior = NULL;
        resetAI();
        unregisterObjects();

        for (u32 i = 0; i < m_behaviors.size(); ++i)
        {
            SF_DEL(m_behaviors[i]);
        }

        m_behaviors.clear();
    }

    void AIComponent::addBehavior( AIBehavior* _behavior )
    {
        ITF_WARNING(m_actor, m_behaviors.find(_behavior) < 0, "Do not add the same behavior multiple times !");
        ITF_WARNING(m_actor, _behavior, "Do not add an invalid behavior !");

        if (_behavior)
        {
            m_behaviors.push_back(_behavior);
        }
    }

    void AIComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        Super::onActorLoaded(_hotReload);

        resetAI();

        if (getTemplate()->getScaleRandomFactor() != 0.f)
        {
            f32 randomFactor = 1 + Seeder::getSharedSeeder().GetFloat(-getTemplate()->getScaleRandomFactor(), getTemplate()->getScaleRandomFactor());
            m_actor->setScale( m_actor->getScale() * randomFactor );
        }

        registerObjects();

        m_animComponent     = m_actor->GetComponent<AnimLightComponent>();      // mandatory, validated
        m_animatedComponent = m_actor->GetComponent<AnimatedComponent>();
        m_phantomComponent  = m_actor->GetComponent<PhantomComponent>();        // not mandatory
        m_fxController      = m_actor->GetComponent<FXControllerComponent>();   // not mandatory
        m_soundComponent    = m_actor->GetComponent<SoundComponent>();          // not mandatory
        m_fxBankComponent   = m_actor->GetComponent<FxBankComponent>();         // not mandatory

        if (m_animatedComponent)
        {
            m_animatedComponent->setUpdateAnimInput(this);
        }
        if (m_soundComponent)
        {
            m_soundComponent->setUpdateSoundInput(this);
        }
        if (m_fxBankComponent)
        {
            m_fxBankComponent->setUpdateFxInput(this);
        }
        
        const u32 behaviorCount = m_behaviors.size();
        for ( u32 i = 0; i < behaviorCount; i++ )
        {
            m_behaviors[i]->init(m_actor, this);
        }
		m_behaviorsInit = btrue;

        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventQueryIsDead,2482198443),this);
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventSequenceActorActivate,1119571699),this);
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventChangeBehaviourQuery,1120766952),this);

        if (getTemplate()->getListenToActivateEvent())
        {
            ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventActivate,2639596100),this);
        }
    }

    void AIComponent::onCheckpointLoaded()
    {
        Super::onCheckpointLoaded();
    }

    void AIComponent::onResourceReady()
    {
        Super::onResourceReady();

        const u32 behaviorCount = m_behaviors.size();
        for ( u32 i = 0; i < behaviorCount; i++ )
        {
            m_behaviors[i]->onResourceReady();
        }
    }

    void AIComponent::onBecomeActive()
    {
        Super::onBecomeActive();

        if ( m_currentBehavior )
        {
            m_currentBehavior->onActorBecomeActive();
        }

        registerObjects();
    }

    void AIComponent::onBecomeInactive()
    {
        Super::onBecomeInactive();

        if ( m_currentBehavior )
        {
            m_currentBehavior->onActorBecomeInactive();
        }

        unregisterObjects();
    }

    void AIComponent::registerObjects()
    {
        if ( !m_actor->isActive() || !m_actor->isEnabled() )
        {
            return;
        }

        if ( !m_isRegisteredToAIManager && getTemplate()->getRegisterToAIManager() )
        {
            const DepthRange& depthRange(m_actor->getDepth());
            ITF_ASSERT(depthRange.isValid());
            AIData * data = AI_MANAGER->addActor(m_actor->getRef(), depthRange);
            if (data)
                data->m_faction = getFaction();
            m_isRegisteredToAIManager = btrue;
        }
    }

    void AIComponent::unregisterObjects()
    {
        if ( m_isRegisteredToAIManager && getTemplate()->getRegisterToAIManager() )
        {
            const DepthRange& depthRange(m_actor->getDepth());
            ITF_ASSERT(depthRange.isValid());
            AI_MANAGER->removeActor(m_actor->getRef(),depthRange);
            m_isRegisteredToAIManager = bfalse;
        }
    }

    void AIComponent::Update( f32 _dt )
    {
        m_currentGravity = PHYSWORLD->getGravity(m_actor->get2DPos(),m_actor->getDepth());
        m_currentGravityDir = m_currentGravity;
        m_currentGravityDir.normalize();

        if ( m_currentBehavior )
        {
            m_currentBehavior->update(_dt);
        }
    }

    void AIComponent::onForceMove()
    {
        if ( m_currentBehavior )
        {
            m_currentBehavior->onForceMove();
        }

    }

    void AIComponent::onEvent( Event* _event)
    {
        Super::onEvent(_event);

        if (EventQueryIsDead* query = _event->DynamicCast<EventQueryIsDead>(ITF_GET_STRINGID_CRC(EventQueryIsDead,2482198443)))
        {
            query->setIsDead( getHealth() <= 0 );
        }
        else if (EventSequenceActorActivate * sequenceActivate = _event->DynamicCast<EventSequenceActorActivate>(ITF_GET_STRINGID_CRC(EventSequenceActorActivate,1119571699)))
        {
            if ( sequenceActivate->getActivate() )
            {
                pause();
            }
            else
            {
                unpause();
            }
        }
		else if (EventChangeBehaviourQuery * changeBhvQuery = _event->DynamicCast<EventChangeBehaviourQuery>(ITF_GET_STRINGID_CRC(EventChangeBehaviourQuery,1120766952)))
		{
            AIBehavior* behavior = changeBhvQuery->getWantedBehaviour();
            if ( behavior == NULL )
            {
                behavior = getBehaviorFromName( changeBhvQuery->getWantedBehaviourName() );
            }
			if ( onChangeBehaviorQuery( behavior ) )
			{
				setBehavior( behavior, changeBhvQuery->getForceRestart() );
                changeBhvQuery->setChangeDone( ( m_currentBehavior == behavior ) ? btrue : bfalse );
			}
		}
        else if (EventActivate * onActivate = _event->DynamicCast<EventActivate>(ITF_GET_STRINGID_CRC(EventActivate,2639596100)))
        {
            processActivate(onActivate);

        }
        else if (EventSetOriginalSender* setOriginalSenderEvt = _event->DynamicCast<EventSetOriginalSender>(ITF_GET_STRINGID_CRC(EventSetOriginalSender,1174287572)))
        {
            // all behaviors, even inactive, need to know about the original sender 
            // NB: for rewards' Action_KillHunterWithOwnBullet
            const u32 behaviorCount = m_behaviors.size();
            for (u32 i=0; i<behaviorCount; ++i)
            {
                m_behaviors[i]->onEvent(_event);
            }
        }
        if ( m_currentBehavior )
        {
            m_currentBehavior->onEvent(_event);
        }
    }

    void AIComponent::setBehavior( AIBehavior* _behavior, bbool _restart /*= bfalse*/ )
    {
        if ( _behavior == NULL )
        {
            ITF_ASSERT_MSG(0, "trying to set a NULL behavior! if this was intentional, use setNullBehavior() instead");
            return;
        }

        if ( _behavior == m_currentBehavior && !_restart )
        {
            return;
        }

        if ( m_currentBehavior )
        {
            if ( m_currentBehavior->isExtern() )
            {
                EventChangeExternBhvValidationQuery validationQuery( _behavior->getTemplate()->getName() );
                m_actor->onEvent( &validationQuery );
                if ( !validationQuery.isChangeValidated() )
                    return;
            }
            m_currentBehavior->onDeactivate();
        }

		m_previousBehavior = m_currentBehavior;
        m_currentBehavior = _behavior;

        if ( m_currentBehavior )
        {
            m_currentBehavior->onActivate();
        }
    }

	void AIComponent::setBehavior( const StringID & _behaviorName, bbool _restart /*= bfalse*/ )
	{
        AIBehavior* bhv = getBehaviorFromName( _behaviorName );
        if ( bhv != NULL )
        {
		    setBehavior( bhv, _restart );
		}
	}

    AIBehavior* AIComponent::getBehaviorFromName( const StringID & _behaviorName )
    {
        // Avoid Duplication in const and Non-const Member Function
        // Effective C++ 3d ed p.23
        return const_cast<AIBehavior*>(static_cast<const AIComponent&>(*this).getBehaviorFromName(_behaviorName));
    }

    const AIBehavior* AIComponent::getBehaviorFromName( const StringID & _behaviorName ) const
    {
        if ( !_behaviorName.isValid() )
        {
            return NULL;
        }

        AIBehavior * curBhv;
        for ( u32 i = 0 ; i < m_behaviors.size(); ++i )
        {
            curBhv = m_behaviors[i];

            if ( curBhv->getTemplate()->getName() == _behaviorName )
            {
                return curBhv;
            }
        }
        return NULL;
    }

    void AIComponent::setNullBehavior()
    {
        if ( m_currentBehavior )
        {
            m_currentBehavior->onDeactivate();
        }

		m_previousBehavior = m_currentBehavior;
        m_currentBehavior = NULL;
    }

	bbool AIComponent::onChangeBehaviorQuery( AIBehavior* _wantedBehavior )
	{
		i32 idx = m_behaviors.find( _wantedBehavior );
		ITF_ASSERT_MSG( idx >= 0, "ChangeBehaviorQuery : can't find wanted behavior !" );
		if ( idx < 0 )
		{
			return bfalse;
		}
		return btrue;
	}

    void AIComponent::receiveHit(
        HitStim* _hit,
        bbool _damageDependsOnLevel /*= btrue*/, u32 _forcedDamage /*= 0*/ )
    {
        receiveHit(
            _hit->getSender(), _hit->getLevel(), _hit->getDirection(), _hit->getFxPos(),
            _damageDependsOnLevel, _forcedDamage
        );
    }

    void AIComponent::receiveHit(
        ActorRef _attacker, u32 _level, 
        const Vec2d& _dir, const Vec3d& _fxPos,
        bbool _damageDependsOnLevel /*= btrue*/, u32 _forcedDamage /*= 0*/ )
    {
        _level = Clamp(_level, static_cast<u32>(0), getTemplate()->getDamageLevels().size() - 1);

        m_lastAttacker = _attacker;
        m_lastHitLevel = _level;
        m_lastHitDir = _dir;
        m_lastHitFxPos = _fxPos;


        // apply damage

        if (!CHEATMANAGER->getEnableInvincibleEnemies())
        {
            if ( _damageDependsOnLevel )
            {
                m_health_current -= getTemplate()->getDamageLevels()[_level];
                m_health_current = Max(m_health_current, 0);
            }
            else
            {
                m_health_current -= _forcedDamage;
                m_health_current = Max(m_health_current, 0);
            }
        }
    }


    void AIComponent::computeRandomAnimNumber()
    {
        m_randomAnim = Seeder::getSharedSeeder().GetFloat();
    }

    void AIComponent::updateAnimInput()
    {
        if ( m_animatedComponent )
        {
            m_animatedComponent->setInput(ITF_GET_STRINGID_CRC(RandomN,218439229), m_randomAnim);
            m_animatedComponent->setInput(ITF_GET_STRINGID_CRC(Faction,2062476556), getFaction());
            m_animatedComponent->setInput(ITF_GET_STRINGID_CRC(Health,3634223137), getHealth());

            if ( m_currentBehavior )
            {
                m_currentBehavior->updateAnimInputs();
            }
        }
    }
    void AIComponent::updateSoundInput()
    {
        if ( m_currentBehavior )
        {
            m_currentBehavior->updateSoundInputs();
        }
    }

    AIBehavior * AIComponent::createAiBehavior( const TemplateAIBehavior * _template )
    {
        if (_template)
        {
            AIBehavior * behavior = _template->createAIBehavior();
            addBehavior(behavior);
            behavior->setTemplate(_template);
            return behavior;
        }
        return NULL;
    }

    void AIComponent::onDepthChanged( f32 _oldZ, f32 _newZ )
    {
        if (m_isRegisteredToAIManager)
        {
            AI_MANAGER->changeDepth(m_actor->getRef(),_oldZ,_newZ);
        }
    }

	void AIComponent::onBehaviorFinished()
	{
		if ( m_currentBehavior->isExtern() )
		{
			onBehaviorExternFinished();
		}
	}

	void AIComponent::onBehaviorExternFinished( bbool _stopped /*= bfalse */)
	{
        if ( !m_currentBehavior )
            return;

        EventChangeExternBhvValidationQuery validationQuery;
        validationQuery.setBhvFinished( !_stopped );
        validationQuery.setBhvStopped( _stopped );
        if ( m_previousBehavior )
        {
            validationQuery.setWantedBehaviourName( m_previousBehavior->getTemplate()->getName() );
        }
        m_actor->onEvent( &validationQuery );
        if ( !validationQuery.isChangeValidated() )
            return;

		// Current is an external state, back to previous bhv
		//
		if ( m_previousBehavior )
		{
			setBehavior( m_previousBehavior );
		}
	}

	AIBehavior* AIComponent::createAndAddAiBehaviorExtern( const TemplateAIBehavior * _template )
	{
		AIBehavior* bhv = createAiBehavior( _template );
		ITF_ASSERT_MSG( bhv != NULL, "can't create extern behavior !" );

		// all behaviors still initialized, init it
		if ( bhv != NULL )
		{
			bhv->setHasExtern();
			if ( m_behaviorsInit )
			{
				bhv->init( m_actor, this );
			}
		}
		return bhv;
	}

    void AIComponent::setFaction( Faction _value )
    {
        m_faction_current = _value;
        if ( !m_isRegisteredToAIManager || !getTemplate()->getRegisterToAIManager() )
            return;

        const DepthRange& depthRange(m_actor->getDepth());
        ITF_ASSERT(depthRange.isValid());
        AIData * data = AI_MANAGER->getAIData(m_actor->getRef(),m_actor->getDepth());
        if (data)
            data->m_faction = _value;
    }

    void AIComponent::processActivate( EventActivate * onActivate )
    {
        if (getTemplate()->getListenToActivateEvent())
        {
            if (onActivate->getActivated())
            {
                m_actor->enable();
            }
            else
            {
                m_actor->disable();
            }
        }
    }

#ifdef ITF_SUPPORT_DEBUGFEATURE
    void AIComponent::getDebugText( String8& _text, const String8& _separator /*= " - "*/ )
    {
        _text += StringID(GetObjectClassCRC()).getDebugString();
        if (m_currentBehavior)
        {
            _text += _separator;
            m_currentBehavior->getDebugText(_text);
        }
        else
        {
            _text += _separator + "NO BEHAVIOR";
        }
        String8 health;
        health.setTextFormat("health: %d",m_health_current);
        _text += _separator + health;
    }
#endif // ITF_FINAL

#ifdef ITF_SUPPORT_EDITOR
    void AIComponent::drawEdit( ActorDrawEditInterface* _drawInterface, u32 _flags ) const
    {
        if (_flags != ActorComponent::DrawEditFlag_All)
            return;

        String8 text;
        if (m_currentBehavior)
            m_currentBehavior->getDebugText(text, "\n");
        else
            text = "NO BEHAVIOR";

        Vec3d textPos;
        GFX_ADAPTER->compute3DTo2D(m_actor->getPos() + Vec3d(0, 1.5f, 0), textPos);
        GFX_ADAPTER->drawDBGText(text, Color::white(), textPos.m_x, textPos.m_y);

        if (m_currentBehavior)
        {
            m_currentBehavior->drawEdit(_drawInterface, _flags);
        }
    }

    void AIComponent::onEditorMove(bbool _modifyInitialPos)
    {
        Super::onEditorMove(_modifyInitialPos);

        unregisterObjects();
        registerObjects();
    }
#endif // ITF_SUPPORT_EDITOR
}
