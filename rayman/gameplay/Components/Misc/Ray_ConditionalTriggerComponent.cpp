#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_CONDITIONALTRIGGERCOMPONENT_H_
#include "rayman/gameplay/Components/Misc/Ray_ConditionalTriggerComponent.h"
#endif //_ITF_RAY_CONDITIONALTRIGGERCOMPONENT_H_

#ifndef _ITF_LINKMANAGER_H_
#include "gameplay/Managers/LinkManager.h"
#endif //_ITF_LINKMANAGER_H_

#ifndef _ITF_LINKCOMPONENT_H_
#include "gameplay/Components/Misc/LinkComponent.h"
#endif //_ITF_LINKCOMPONENT_H_

#ifndef _ITF_SCENE_H_
#include "engine/scene/scene.h"
#endif //_ITF_SCENE_H_

namespace ITF
{
    #define m_conditionName_Dead           ITF_GET_STRINGID_CRC(dead,1809176293)
    #define m_conditionName_TriggerEvent   ITF_GET_STRINGID_CRC(trigger,161061850)
    #define m_conditionName_IsEnable       ITF_GET_STRINGID_CRC(enable,589187077)
    #define m_conditionName_IsOpen         ITF_GET_STRINGID_CRC(open,812432275)

    //------------------------------------------------------------------------------------------------------------------------------------
    IMPLEMENT_OBJECT_RTTI(Ray_ConditionalTriggerComponent)

    BEGIN_SERIALIZATION_CHILD(Ray_ConditionalTriggerComponent)
    END_SERIALIZATION()

    //------------------------------------------------------------------------------------------------------------------------------------
    Ray_ConditionalTriggerComponent::Ray_ConditionalTriggerComponent()
    :Super()
    {
    }

    //------------------------------------------------------------------------------------------------------------------------------------
    Ray_ConditionalTriggerComponent::~Ray_ConditionalTriggerComponent()
    {
    }

    //------------------------------------------------------------------------------------------------------------------------------------
    void Ray_ConditionalTriggerComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        Super::onActorLoaded(_hotReload);

        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventTrigger,1343042510), this);

        m_receivedTriggerEventActors.clear();

        Scene * scene = m_actor->getScene();
        if ( scene->getAlwaysActiveActors().find( m_actor ) >= 0 )
        {
            scene->setAlwaysActiveActor( m_actor, bfalse );
        }
    }
    //------------------------------------------------------------------------------------------------------------------------------------
    void Ray_ConditionalTriggerComponent::onBecomeActive()
    {
        Super::onBecomeActive();

        // Stay always active until condition check end
        //
        Scene * scene = m_actor->getScene();
        if ( scene->getAlwaysActiveActors().find(m_actor) < 0 )
        {
            scene->setAlwaysActiveActor(m_actor);
        }

        // Init parent data
        //
        m_parentDataList.clear();

        ObjectRefList * parents  = LINKMANAGER->getParents( m_actor->getRef() );
        if ( parents != NULL )
        {
            for ( u32 i = 0; i < parents->size() ; ++i )
            {
                Actor * actor = AIUtils::getActor( (*parents)[i] );
                if ( actor != NULL )
                {
                    if ( LinkComponent * linkComp = actor->GetComponent<LinkComponent>() )
                    {
                        const LinkComponent::ChildrenList & parentChildren = linkComp->getChildren();
                        
                        for ( u32 u = 0; u < parentChildren.size() ; ++u )
                        {
                            const ChildEntry & childEntry = parentChildren[u];

                            if ( BaseObject * child = linkComp->getChildObject( childEntry.getPath() ) )
                            {
                                const ChildEntry::TagValueList & tagList = childEntry.getTagValueList();

                                if ( ( child->getRef() == m_actor->getRef() ) && ( tagList.size() > 0 ) )
                                {
                                    ConditionType condition = Invalid;

                                    // link has conditional tag ?
                                    for ( u32 w = 0; w < tagList.size(); ++w )
                                    {
                                        i32 tagValue = 1;
                                        if ( !tagList[w].m_value.isEmpty() )
                                        {
                                            childEntry.getTagValue( tagList[w].m_tag, tagValue );
                                        }
                                        condition = getLinkTagConditionType( tagList[w].m_tag, tagValue );
                                        if ( condition != Invalid )
                                        {
                                            break;
                                        }
                                    }

                                    if ( condition != Invalid )
                                    {
                                        ParentData data;
                                        data.m_parent = (*parents)[i];
                                        data.m_condition = condition;
                                        m_parentDataList.push_back( data );
                                    }
                                    else 
                                    {
                                        ITF_WARNING( m_actor, condition != Invalid, "Invalid condition type with %s parent", actor->getUserFriendly().cStr() );
                                    }
                                    break;
                                }
                            }
                        }
                    }
                }
            }            
        }

        // check received events during inactivation
        //
        for( u32 i = 0; i < m_receivedTriggerEventActors.size(); ++i )
        {
            for ( u32 u = 0; u < m_parentDataList.size() ; ++u )
            {
                ParentData & data = m_parentDataList[u];
                if ( data.m_parent == m_receivedTriggerEventActors[i] )
                {
                    if ( data.m_condition == TriggerEvent )
                    {
                        data.m_conditionValidated = btrue;
                    }
                    else
                    {
                        ITF_ASSERT( 0 );// invalid actor data init ??
                    }
                    break;
                }
            }
        }
        m_receivedTriggerEventActors.clear();
    }    
    //----------------------------------------------------------------------------------------------------------------------------
    void Ray_ConditionalTriggerComponent::onActorRemovedFromScene(Scene* _pScene, bbool _forDeletion)
    {
        Super::onActorRemovedFromScene( _pScene, _forDeletion );

        if ( _pScene->getAlwaysActiveActors().find( m_actor ) >= 0 )
        {
            _pScene->setAlwaysActiveActor( m_actor, bfalse );
        }
    }

    //------------------------------------------------------------------------------------------------------------------------------------
    void Ray_ConditionalTriggerComponent::onEvent( Event* _event )
    {
        Super::onEvent( _event );

        if ( EventTrigger* trig = _event->DynamicCast<EventTrigger>(ITF_GET_STRINGID_CRC(EventTrigger,1343042510)) )
        {
            // if disabled of not active, store received event sender
            if ( !m_actor->isActive() || !m_actor->isEnabled() )
            {
                if ( m_receivedTriggerEventActors.find( trig->getSender() ) < 0 )
                {
                    m_receivedTriggerEventActors.push_back( trig->getSender() );
                }
            }
            else
            {
                for ( u32 u = 0; u < m_parentDataList.size() ; ++u )
                {
                    ParentData & data = m_parentDataList[u];
                    
                    if ( ( data.m_condition == TriggerEvent ) && !data.m_conditionValidated )
                    {
                        if ( data.m_parent == _event->getSender() )
                        {
                            data.m_conditionValidated = btrue;
                            
                            // re-check to be sure that other condition are still valid
                            checkAllConditions( btrue );
                            break;
                        }
                    }
                }
            }
        }
    }

    //------------------------------------------------------------------------------------------------------------------------------------
    Ray_ConditionalTriggerComponent::ConditionType Ray_ConditionalTriggerComponent::getLinkTagConditionType( const StringID & _tag, i32 _value )
    {
        if ( _tag == m_conditionName_Dead )
        {
            return  ( ( _value == 0 ) ? IsNotDead : IsDead );
        }
        else if ( _tag == m_conditionName_TriggerEvent )
        {
            return TriggerEvent;
        }
        else if ( _tag == m_conditionName_IsEnable )
        {
            return  ( ( _value == 0 ) ? IsDisable : IsEnable );
        }
        else if ( _tag == m_conditionName_IsOpen )
        {
            return ( ( _value == 0 ) ? isNotOpen : isOpen );;
        }
        return Invalid;
    }

    //------------------------------------------------------------------------------------------------------------------------------------
    void Ray_ConditionalTriggerComponent::getLinkTagFromConditionType( ConditionType _conditionType, StringID & _res_tag, i32 &_res_value ) const
    {
        switch ( _conditionType )
        {
            case IsDead :
            case IsNotDead :
            {
                _res_tag = m_conditionName_Dead;
                _res_value = ( _conditionType == IsDead ) ? 1 : 0;
                break;
            }
            case TriggerEvent :
            {
                _res_tag = m_conditionName_TriggerEvent;
                _res_value = 1;
                break;
            }
            case IsEnable :
            case IsDisable :
            {
                _res_tag = m_conditionName_IsEnable;
                _res_value = ( _conditionType == IsEnable  ) ? 1 : 0;
                break;
            }
            case isOpen :
            case isNotOpen :
            {
                _res_tag = m_conditionName_IsOpen;
                _res_value = ( _conditionType == isOpen  ) ? 1 : 0;
                break;
            }
            default:
            {
                _res_tag = "Invalid";
                _res_value = 0;
            }
        }
    }

    //------------------------------------------------------------------------------------------------------------------------------------
    void Ray_ConditionalTriggerComponent::checkAllConditions( bbool _forceCheck /*= bfalse*/ )
    {
        // check all parent conditions
        //
        bbool allConditionsValidated = btrue;

        for ( u32 u = 0; u < m_parentDataList.size() ; ++u )
        {
            ParentData & data = m_parentDataList[u];
            if ( !data.m_conditionValidated || _forceCheck )
            {
                Actor * actor = AIUtils::getActor( data.m_parent );
                if ( actor != NULL )
                {
                    switch( data.m_condition )
                    {
                    case IsDead :
                        {
                            EventQueryIsDead query;
                            actor->onEvent(&query);
                            if ( !query.getIsDead() )   
                                allConditionsValidated = bfalse;
                            else                        
                                data.m_conditionValidated = btrue;
                            break;
                        }
                    case IsNotDead :
                        {
                            EventQueryIsDead query;
                            actor->onEvent(&query);
                            if ( query.getIsDead() )   
                                allConditionsValidated = bfalse;
                            else                        
                                data.m_conditionValidated = btrue;
                            break;
                        }
                    case IsEnable :
                        {
                            if ( !actor->isEnabled() )  
                                allConditionsValidated = bfalse;
                            else                        
                                data.m_conditionValidated = btrue;
                            break;
                        }
                    case IsDisable :
                        {
                            if ( actor->isEnabled() )
                                allConditionsValidated = bfalse;
                            else
                                data.m_conditionValidated = btrue;
                            break;
                        }
                    case isOpen :
                        {
                            Ray_EventQueryOpen queryOpen;
                            actor->onEvent(&queryOpen);
                            if ( !queryOpen.isOpen() )
                                allConditionsValidated = bfalse;
                            else
                                data.m_conditionValidated = btrue;
                            break;
                        }
                    case isNotOpen :
                        {
                            Ray_EventQueryOpen queryOpen;
                            actor->onEvent(&queryOpen);
                            if ( queryOpen.isOpen() )
                                allConditionsValidated = bfalse;
                            else
                                data.m_conditionValidated = btrue;
                            break;
                        }
                    default:
                        {
                            if ( !_forceCheck )
                            {
                                allConditionsValidated = bfalse;
                            }
                            break;
                        }
                    }
                }
                else
                {
                    allConditionsValidated = bfalse;
                }
            }
            if ( !allConditionsValidated )
            {
                data.m_conditionValidated = bfalse;
                break;
            }
        }

        // conditions validated, send event
        //-----------------------------------
        if ( !m_disabled && allConditionsValidated )
        {
            m_disabled = btrue;
            sendEvent( m_actor->getRef(), btrue );
            m_actor->disable();

            // Remove always active
            //
            Scene * scene = m_actor->getScene();
            if ( scene->getAlwaysActiveActors().find( m_actor ) >= 0 )
            {
                scene->setAlwaysActiveActor( m_actor, bfalse );
            }
        }        
    }


    //------------------------------------------------------------------------------------------------------------------------------------
    void Ray_ConditionalTriggerComponent::Update( f32 _deltaTime )
    {
        Super::Update(_deltaTime);

        checkAllConditions();
    }

#ifdef ITF_SUPPORT_EDITOR
    //------------------------------------------------------------------------------------------------------------------------------------
    void Ray_ConditionalTriggerComponent::drawEdit( class ActorDrawEditInterface* _drawInterface, u32 _flags ) const
    {
        Super::drawEdit( _drawInterface, _flags );

        for ( u32 u = 0; u < m_parentDataList.size() ; ++u )
        {
            const ParentData & data = m_parentDataList[u];

            Actor * actor = AIUtils::getActor( data.m_parent );
            if ( actor != NULL )
            {
                String s_text;
                StringID tag;
                i32 tagValue;
                getLinkTagFromConditionType( data.m_condition, tag, tagValue );
                
                s_text.setTextFormat( " %s : %d", tag.getDebugString(), tagValue );
                
                Vec3d res2d;
                Vec3d displayPos = actor->getPos() + ( ( m_actor->getPos() - actor->getPos() ) * 0.5f );
                displayPos.m_y += 0.5f;
                GFX_ADAPTER->compute3DTo2D( displayPos, res2d );
                GFX_ADAPTER->drawDBGText( s_text, data.m_conditionValidated ? Color::green() : Color::red() , res2d.m_x, res2d.m_y );
            }
        }
    }
#endif // ITF_SUPPORT_EDITOR

    //------------------------------------------------------------------------------------------------------------------------------------
    //------------------------------------------------------------------------------------------------------------------------------------
    //------------------------------------------------------------------------------------------------------------------------------------
    IMPLEMENT_OBJECT_RTTI(Ray_ConditionalTriggerComponent_Template)

    BEGIN_SERIALIZATION_CHILD(Ray_ConditionalTriggerComponent_Template)
    END_SERIALIZATION()

    Ray_ConditionalTriggerComponent_Template::Ray_ConditionalTriggerComponent_Template()
    :Super()
    {
    }


};