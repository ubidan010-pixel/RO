#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_DYNAMICFOGCOMPONENT_H_
#include "rayman/gameplay/components/Misc/Ray_DynamicFogComponent.h"
#endif //_ITF_RAY_DYNAMICFOGCOMPONENT_H_

#ifndef _ITF_LINKCOMPONENT_H_
#include "gameplay/Components/Misc/LinkComponent.h"
#endif //_ITF_LINKCOMPONENT_H_

namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(Ray_DynamicFogComponent)

    BEGIN_SERIALIZATION_CHILD(Ray_DynamicFogComponent)

        BEGIN_CONDITION_BLOCK(ESerializeGroup_DataEditable)
            SERIALIZE_MEMBER( "useTempateFogParams", m_useTempateFogParams );
            if ( !m_useTempateFogParams )
            {
                SERIALIZE_MEMBER( "dynamicFogColor",    m_instanceDynamicFogColor );
                SERIALIZE_MEMBER( "dynamicFogMaxDepth", m_instanceDynamicFogMaxDepth );
            }
        END_CONDITION_BLOCK()

        SERIALIZE_FUNCTION(preChangeProperties,ESerialize_PropertyEdit);

    END_SERIALIZATION()

    Ray_DynamicFogComponent::Ray_DynamicFogComponent() 
        : Super()
        , m_curFogColor()
        , m_useTempateFogParams ( btrue )
        , m_instanceDynamicFogColor( Color::black() )
        , m_instanceDynamicFogMaxDepth( 30.0f )
    {
    }

    void Ray_DynamicFogComponent::preChangeProperties()
    {
        if ( getTemplate() == NULL )
            return;

        if ( !m_useTempateFogParams )
        {
            m_curFogColor = m_instanceDynamicFogColor;
        }
        else 
        {
            m_curFogColor = getTemplate()->getDynamicFogDefaultColor();
        }

        computeFog(m_actor->getDepth());

        if ( getTemplate()->isModifier() )
        {
            processTriggered();
        }
    }

    //----------------------------------------------------------------------------------------------------------------------------------
    void Ray_DynamicFogComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        Super::onActorLoaded(_hotReload);

        m_graphicComponents = m_actor->GetComponents<GraphicComponent>();

        ACTOR_REGISTER_EVENT_COMPONENT(m_actor, ITF_GET_STRINGID_CRC(Ray_EventDynamicFogParamSet,2084909099), this );
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor, ITF_GET_STRINGID_CRC(Ray_EventDynamicFogParamQuery,2314851366), this );

        if ( getTemplate()->isModifier() )
        {
            ACTOR_REGISTER_EVENT_COMPONENT(m_actor, ITF_GET_STRINGID_CRC(EventTrigger,1343042510), this );
        }

        if ( m_useTempateFogParams )
        {
            m_curFogColor = getTemplate()->getDynamicFogDefaultColor();
        }
        else
        {
            m_curFogColor = m_instanceDynamicFogColor;
        }
    }

    //----------------------------------------------------------------------------------------------------------------------------------
    void Ray_DynamicFogComponent::onEvent( Event * _event )
    {
        Super::onEvent( _event );

        if ( Ray_EventDynamicFogParamSet* fogParam = _event->DynamicCast<Ray_EventDynamicFogParamSet>(ITF_GET_STRINGID_CRC(Ray_EventDynamicFogParamSet,2084909099)) )
        {
            if ( !getTemplate()->isModifier() )
            {
                m_useTempateFogParams = bfalse;
                m_instanceDynamicFogColor = fogParam->getFogColor();
                m_curFogColor = m_instanceDynamicFogColor;
                m_instanceDynamicFogMaxDepth = fogParam->getFogMaxDepth();
            }
        }
        else if ( Ray_EventDynamicFogParamQuery* fogParamQuery = _event->DynamicCast<Ray_EventDynamicFogParamQuery>(ITF_GET_STRINGID_CRC(Ray_EventDynamicFogParamQuery,2314851366)) )
        {
            fogParamQuery->setFogParams( getFogColor(), getFogMaxDepth() );
        }
        else if ( EventTrigger* triggered = _event->DynamicCast<EventTrigger>(ITF_GET_STRINGID_CRC(EventTrigger,1343042510)) )
        {
            processTriggered();
        }
    }

    //----------------------------------------------------------------------------------------------------------------------------------
    void Ray_DynamicFogComponent::processTriggered()
    {
        if ( getTemplate()->isModifier() )
        {
            Ray_EventDynamicFogParamSet fogParamSet( getFogColor(), getFogMaxDepth() );
            fogParamSet.setSender( m_actor->getRef() );

            LinkComponent * linkComp = m_actor->GetComponent<LinkComponent>();
            if ( linkComp != NULL )
            {
                linkComp->sendEventToChildren( &fogParamSet );
            }
            // send event on me to allow other component check
            else
            {
                m_actor->onEvent( &fogParamSet );
            }
        }
    }

    //----------------------------------------------------------------------------------------------------------------------------------
    void Ray_DynamicFogComponent::Update( f32 _dt )
    {
        if ( getTemplate()->isDataOnly() )
        {
            m_actor->disable();
        }
        
        Super::Update(_dt);

        // we have to compute the fog on update
        // because both the actor's and camera's depth can change
        // (so onDepthChanged() is not enough)
        computeFog(m_actor->getDepth());
    }

    void Ray_DynamicFogComponent::computeFog( const f32 _z )
    {
        f32 dynFogMaxDepth = m_useTempateFogParams ? getTemplate()->getDynamicFogMaxDepth() : m_instanceDynamicFogMaxDepth;

        if ( ( getTemplate()->useDynamicFog() || !m_useTempateFogParams ) && ( dynFogMaxDepth != 0.0f ) )
        {
            f32 distToCamZ = f32_Max( 0.0f, CAMERACONTROLLERMANAGER->getCamPos().m_z - CAMERACONTROLLERMANAGER->getDepth() - _z );

            m_curFogColor.m_a = f32_Min( distToCamZ, dynFogMaxDepth ) / dynFogMaxDepth;

            for (u32 i = 0; i < m_graphicComponents.size(); i++)
            {
                m_graphicComponents[i]->setColorFog( m_curFogColor );
            }


            // Update all children
            ActorBindHandler & bindHandler = m_actor->getChildrenBindHandler();
            const ObjectRefList&  children = bindHandler.getChildren();
            Ray_EventDynamicFogParamSet fogEvent( m_curFogColor.getAsU32(), dynFogMaxDepth );

            for ( u32 i = 0; i < children.size(); ++i )
            {
                BaseObject * obj = children[i].getObject();
                if ( obj != NULL )
                {
                    Actor * actor = obj->DynamicCast<Actor>(ITF_GET_STRINGID_CRC(Actor,2546623115));
                    if ( actor != NULL )
                    {
                        const SafeArray<GraphicComponent*> graphicComponents = actor->GetComponents<GraphicComponent>();
                        for (u32 i = 0; i < graphicComponents.size(); i++)
                        {
                            graphicComponents[i]->setColorFog( m_curFogColor );
                        }
                        actor->onEvent( &fogEvent );
                    }
                }
            }
        }
    }

    //----------------------------------------------------------------------------------------------------------------------------------------

    IMPLEMENT_OBJECT_RTTI(Ray_DynamicFogComponent_Template)
    BEGIN_SERIALIZATION(Ray_DynamicFogComponent_Template)
        SERIALIZE_MEMBER( "useDynamicFog",              m_useDynamicFog );  
        SERIALIZE_MEMBER( "dynamicFogDefaultColor",     m_dynamicFogDefaultColor );  
        SERIALIZE_MEMBER( "dynamicFogMaxDepth",         m_dynamicFogMaxDepth );  
        SERIALIZE_MEMBER( "isDataOnly",                 m_isDataOnly );
        SERIALIZE_MEMBER( "isModifier",                 m_isModifier );
    END_SERIALIZATION()

    Ray_DynamicFogComponent_Template::Ray_DynamicFogComponent_Template()
    : Super()
    , m_useDynamicFog( bfalse )
    , m_dynamicFogDefaultColor( 0 )
    , m_dynamicFogMaxDepth( 30.f )
    , m_isDataOnly( bfalse )
    , m_isModifier( bfalse )
    {
    }

    Ray_DynamicFogComponent_Template::~Ray_DynamicFogComponent_Template()
    {
            
    }

} // namespace ITF

