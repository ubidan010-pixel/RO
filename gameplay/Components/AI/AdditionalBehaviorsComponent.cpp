#include "precompiled_gameplay.h"

#ifndef _ITF_ADDITIONALBEHAVIORSCOMPONENT_H_
#include "gameplay/Components/AI/AdditionalBehaviorsComponent.h"
#endif //_ITF_ADDITIONALBEHAVIORSCOMPONENT_H_

#ifndef _ITF_PHYSCOMPONENT_H_
#include "engine/actors/components/physcomponent.h"
#endif //_ITF_PHYSCOMPONENT_H_

#ifndef _ITF_AICOMPONENT_H_
#include "gameplay/Components/AI/AIComponent.h"
#endif //_ITF_AICOMPONENT_H_

namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(AdditionalBehaviorsComponent)
    IMPLEMENT_OBJECT_RTTI(AdditionalBehaviorsComponent_Template)

    BEGIN_SERIALIZATION(AdditionalBehaviorsComponent)
    END_SERIALIZATION()
    BEGIN_VALIDATE_COMPONENT(AdditionalBehaviorsComponent)
        VALIDATE_COMPONENT_PARAM("", m_aiComponent, "AdditionalBehaviorsComponent requires a AiComponent");
    END_VALIDATE_COMPONENT()

    AdditionalBehaviorsComponent::AdditionalBehaviorsComponent()
    : Super()
    , m_aiComponent(NULL)
    , m_physComponent(NULL)
    , m_skipBvhValidation( bfalse )
    {
    }

    AdditionalBehaviorsComponent::~AdditionalBehaviorsComponent()
    {
    }

    //----------------------------------------------------------------------------------------------------------------------------------
    void AdditionalBehaviorsComponent::onActorLoaded( Pickable::HotReloadType _hotReload )
    {
        Super::onActorLoaded( _hotReload );
        
        m_aiComponent = m_actor->GetComponent<AIComponent>();
        m_physComponent = m_actor->GetComponent<PhysComponent>();

        // register bhv data
        //
        if ( m_aiComponent )
        {
            for ( u32 i = 0; i < getTemplate()->getExternBehaviorDataList().size(); ++i )
            {
                const ExternBehaviorData_Template & dataTemplate = getTemplate()->getExternBehaviorDataList()[i];

                ITF_ASSERT( ( dataTemplate.m_behaviorTemplate != NULL ) || dataTemplate.m_behaviorName.isValid() );
                
                ExternBehaviorData data;
                data.m_template = &dataTemplate;
                if ( dataTemplate.m_behaviorTemplate != NULL )
                {
                    data.m_behavior = m_aiComponent->createAndAddAiBehaviorExtern( dataTemplate.m_behaviorTemplate );
                }            
                m_externBehaviorDataList.push_back(data);
            }
        }

        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventChangeExternBhvValidationQuery,1163849318),this);

        // register trigger event 
        //
        if ( getTemplate()->getOnTriggerActiveBhv().isValid() ||  getTemplate()->getOnTriggerDesactivateBhv().isValid() )
        {
            ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventTrigger,1343042510),this);
        }

        // start bhv valid ?
        //
        if ( getTemplate()->getStartBhv().isValid() )
        {
            changeBehaviorQuery( getTemplate()->getStartBhv() );
        }

        // disable phys
        if ( ( m_physComponent != NULL ) && getTemplate()->getDisablePhys() )
        {
            m_physComponent->setDisabled( btrue );
        }
    }

    //----------------------------------------------------------------------------------------------------------------------------------
    void AdditionalBehaviorsComponent::onCheckpointLoaded()
    {
        Super::onCheckpointLoaded();

        if ( getTemplate()->getStartBhv().isValid() )
        {
            changeBehaviorQuery( getTemplate()->getStartBhv(), btrue );
        }
    }

    //----------------------------------------------------------------------------------------------------------------------------------
    void AdditionalBehaviorsComponent::onEvent( Event * _event )
    {
        Super::onEvent(_event);

        if ( EventChangeExternBhvValidationQuery * changeValidation = _event->DynamicCast<EventChangeExternBhvValidationQuery>(ITF_GET_STRINGID_CRC(EventChangeExternBhvValidationQuery,1163849318)) )
        {
            processChangeExternBhvValidationQuery( changeValidation );
        }
        else if ( EventTrigger * trigger = _event->DynamicCast<EventTrigger>(ITF_GET_STRINGID_CRC(EventTrigger,1343042510)) ) 
        {
            if ( trigger->getActivated() && getTemplate()->getOnTriggerActiveBhv().isValid() )
            {
                const StringID * bhvToStart = &( getTemplate()->getOnTriggerActiveBhv() );

                if ( ( m_aiComponent != NULL ) && getTemplate()->getOnTriggerActiveCheckNextBhv() &&  
                     ( m_aiComponent->getCurrentBehavior() != NULL ) && ( m_aiComponent->getPreviousBehavior() != NULL )
                   )
                {
                    ExternBehaviorData * bhvData = getBehaviorData( m_aiComponent->getCurrentBehavior()->getTemplate()->getName() );
                    if ( ( bhvData != NULL ) && ( bhvData->m_template != NULL ) )
                    {
                        // get next Bhv
                        const StringID & nextBhv = bhvData->m_template->getNextBehavior( m_aiComponent->getPreviousBehavior()->getTemplate()->getName(), bfalse );

                        if ( nextBhv.isValid() )
                        {
                            bhvToStart = &nextBhv;
                        }
                    }
                }
                if ( bhvToStart != NULL )
                {
                    changeBehaviorQuery( *bhvToStart );
                }
            }
            else if ( !trigger->getActivated() && getTemplate()->getOnTriggerDesactivateBhv().isValid() )
            {
                changeBehaviorQuery( getTemplate()->getOnTriggerDesactivateBhv() );
            }
        }
    }

    //----------------------------------------------------------------------------------------------------------------------------------
    void AdditionalBehaviorsComponent::changeBehaviorQuery( const StringID &  _bhvName, bbool _forceRestart /*btrue*/ )
    {
        if ( m_aiComponent == NULL )
            return;

        AIBehavior * prevBhv = m_aiComponent->getCurrentBehavior();
    
        // request bhv change
        EventChangeBehaviourQuery newBhv;
        newBhv.setWantedBehaviourName( _bhvName );
        newBhv.setForceRestart( _forceRestart );
        m_actor->onEvent( &newBhv );

        if ( newBhv.isChangeDone() )
        {
            if ( ( m_physComponent != NULL ) && !getTemplate()->getDisablePhys() )
            {
                // restore prev bhv phys state
                if ( ( prevBhv != NULL ) && prevBhv->isExtern() && ( prevBhv->getTemplate()->getName() != _bhvName ) )
                {
                    ExternBehaviorData * curBhvData = getBehaviorData( prevBhv->getTemplate()->getName() );
                    restorePhysState( curBhvData );
                }

                // check phys desactivation for new bhv 
                if ( ExternBehaviorData * bhvData = getBehaviorData( _bhvName ) )
                {
                    if ( ( bhvData != NULL ) && ( bhvData->m_template != NULL ) && ( ( prevBhv == NULL ) || ( prevBhv->getTemplate()->getName() != bhvData->m_template->getBehaviorName() ) ) )
                    {
                        if ( bhvData->m_template->m_disablePhys )
                        {
                            bhvData->m_prevPhysState = m_physComponent->getDisabled();
                            m_physComponent->setDisabled( btrue );
                        }
                    }
                }
            }
        }
    }

    //----------------------------------------------------------------------------------------------------------------------------------
    void AdditionalBehaviorsComponent::processChangeExternBhvValidationQuery( EventChangeExternBhvValidationQuery * _query )
    {
        // Extern bhv finished
        if ( _query->isBhvFinished() )
        {
            _query->setChangeValidated( onBehaviorFinished( m_aiComponent->getCurrentBehavior()->getTemplate()->getName(), _query->getWantedBehaviourName(), bfalse ) );
        }
        else if ( _query->isBhvStopped() )
        {
            _query->setChangeValidated( onBehaviorFinished( m_aiComponent->getCurrentBehavior()->getTemplate()->getName(), _query->getWantedBehaviourName(), btrue ) );
        }
        else
        {
            if ( !m_skipBvhValidation )
            {
                _query->setChangeValidated( isBehaviorCuttableBy( m_aiComponent->getCurrentBehavior()->getTemplate()->getName(), _query->getWantedBehaviourName() ) );
            }
        }
    }

    //----------------------------------------------------------------------------------------------------------------------------------
    // returns btrue if the new bhv is validated
    //
    bbool AdditionalBehaviorsComponent::onBehaviorFinished( const StringID &  _bhvName, const StringID & _newBhvName, bbool _stopped )
    {
        bbool res = btrue;
        
        ExternBehaviorData * bhvData = getBehaviorData( _bhvName );
        if ( ( bhvData != NULL ) && ( bhvData->m_template != NULL ) )
        {
            // get next Bhv
            const StringID & nextBhv = bhvData->m_template->getNextBehavior( m_aiComponent->getPreviousBehavior()->getTemplate()->getName(), !_stopped );

            if ( !nextBhv.isValid() )
            {
                res = isBehaviorCuttableBy( bhvData, _newBhvName );
                restorePhysState( bhvData );
            }
            else
            {
                // restore phys data before changing bhv
                if ( !isCurrentBehavior( nextBhv ) )
                {
                    restorePhysState( bhvData );
                }

                m_skipBvhValidation = btrue;
                changeBehaviorQuery( nextBhv );
                m_skipBvhValidation = bfalse;
                res = bfalse;
            }
        }
        return res;
    }

    //----------------------------------------------------------------------------------------------------------------------------------
    void AdditionalBehaviorsComponent::restorePhysState( ExternBehaviorData * _bhvData )
    {
        if ( !getTemplate()->getDisablePhys() )
        {
            if ( ( _bhvData != NULL ) && ( m_physComponent != NULL ) && _bhvData->m_template->m_disablePhys )
            {
                m_physComponent->setDisabled( _bhvData->m_prevPhysState );
            }        
        }
    }

    //----------------------------------------------------------------------------------------------------------------------------------
    AdditionalBehaviorsComponent::ExternBehaviorData *  AdditionalBehaviorsComponent::getBehaviorData( const StringID &  _bhvName )
    {
        for ( u32 i = 0; i < m_externBehaviorDataList.size(); ++i )
        {
            ExternBehaviorData & data = m_externBehaviorDataList[i];
            if ( data.m_template->getBehaviorName() != _bhvName )
            {
                continue;
            }
            return &data;
        }
        return NULL;
    }
    //----------------------------------------------------------------------------------------------------------------------------------
    bbool AdditionalBehaviorsComponent::isCurrentBehavior( const StringID &  _bhvName )
    {
        { return ( m_aiComponent ? ( m_aiComponent->getCurrentBehavior() ? ( m_aiComponent->getCurrentBehavior()->getTemplate()->getName() == _bhvName ) : bfalse ) : bfalse ); }
    }     

    //----------------------------------------------------------------------------------------------------------------------------------
    bbool AdditionalBehaviorsComponent::isBehaviorCuttableBy( const StringID & _bhvName, const StringID & _newBhvName )
    {
        if ( _bhvName == _newBhvName )
        {
            return btrue;
        }
        ExternBehaviorData * bhvData = getBehaviorData( _bhvName );
        return isBehaviorCuttableBy( bhvData, _newBhvName );
    }

    //----------------------------------------------------------------------------------------------------------------------------------
    bbool AdditionalBehaviorsComponent::isBehaviorCuttableBy( ExternBehaviorData *_curBhvData, const StringID & _newBhvName )
    {
        if ( _curBhvData != NULL )
        {
            // Has a valid list
            if ( _curBhvData->m_template->m_cuttableBehaviors.size() > 0 )
            {
                if ( !_newBhvName.isValid() )
                    return bfalse;

                for ( ITF_VECTOR<StringID>::const_iterator it = _curBhvData->m_template->m_cuttableBehaviors.begin(); it != _curBhvData->m_template->m_cuttableBehaviors.end(); it++)
                {
                    if ( *it == _newBhvName )
                    {
                        return btrue;
                    }
                }
                return bfalse;
            }
        }
        return btrue;
    }                   

    //----------------------------------------------------------------------------------------------------------------------------------
    //----------------------------------------------------------------------------------------------------------------------------------
    //----------------------------------------------------------------------------------------------------------------------------------
    BEGIN_SERIALIZATION(AdditionalBehaviorsComponent_Template)
        SERIALIZE_CONTAINER_OBJECT  ( "externBehaviorDataList",             m_externBehaviorDataList );
        SERIALIZE_MEMBER            ( "startBhv",                           m_startBhv );
        SERIALIZE_MEMBER            ( "onTriggerActiveBhv",                 m_onTriggerActiveBhv );
        SERIALIZE_MEMBER            ( "onTriggerDesactivateBhv",            m_onTriggerDesactivateBhv );
        SERIALIZE_MEMBER            ( "disablePhys",                        m_disablePhys );        
        SERIALIZE_MEMBER            ( "onTriggerActiveCheckNextBhv",        m_onTriggerActiveCheckNextBhv );
    END_SERIALIZATION()

    AdditionalBehaviorsComponent_Template::AdditionalBehaviorsComponent_Template()
    : Super()
    , m_disablePhys( bfalse )
    , m_onTriggerActiveCheckNextBhv( bfalse )
    {
    }

    //----------------------------------------------------------------------------------------------------------------------------------
    BEGIN_SERIALIZATION(ExternBehaviorData_Template)
        SERIALIZE_OBJECT_WITH_FACTORY(  "behaviorTemplate",     m_behaviorTemplate, ACTORSMANAGER->getAIBehaviorsFactory() );
        SERIALIZE_MEMBER(               "behaviorName",         m_behaviorName );
        SERIALIZE_CONTAINER(            "cuttableBehaviors",    m_cuttableBehaviors );
        SERIALIZE_CONTAINER(            "disablePhys",          m_disablePhys );
        SERIALIZE_CONTAINER_OBJECT(     "nextBehaviors",        m_nextBehaviors );
        SERIALIZE_MEMBER(               "defaultNextBehavior",  m_defaultNextBehavior );
    END_SERIALIZATION()

    BEGIN_SERIALIZATION_SUBCLASS( ExternBehaviorData_Template, NextBhvData )
        SERIALIZE_MEMBER(               "previousBvh",          m_previousBvh );
        SERIALIZE_MEMBER(               "nextBvh",              m_nextBvh );
    END_SERIALIZATION()

    ExternBehaviorData_Template::~ExternBehaviorData_Template()
    { 
        SF_DEL(m_behaviorTemplate); 
    }

    //----------------------------------------------------------------------------------------------------------------------------------
    const StringID & ExternBehaviorData_Template::getBehaviorName() const
    { 
        return ( ( m_behaviorTemplate != NULL )? m_behaviorTemplate->getName() : m_behaviorName ); 
    }

    //----------------------------------------------------------------------------------------------------------------------------------
    const StringID & ExternBehaviorData_Template::getNextBehavior( const StringID & _prevBhv, bbool _getDefaultNext ) const
    {
        for ( u32 i = 0; i < m_nextBehaviors.size() ; ++i )
        {
            const NextBhvData & nextBhvData = m_nextBehaviors[i];
            if ( nextBhvData.m_previousBvh == _prevBhv )
            {
                return nextBhvData.m_nextBvh;
            }
        }
        if ( _getDefaultNext && m_defaultNextBehavior.isValid() )
        {
            return m_defaultNextBehavior;
        }
        return StringID::Invalid;
    }
}
