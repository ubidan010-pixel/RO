#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_SHOOTERPIRATESHIPAICOMPONENT_H_
#include "rayman/gameplay/Components/AI/Shooter/Ray_ShooterPirateShipAIComponent.h"
#endif //_ITF_RAY_SHOOTERPIRATESHIPAICOMPONENT_H_

#ifndef _ITF_AIUTILS_H_
#include "gameplay/AI/Utils/AIUtils.h"
#endif //_ITF_AIUTILS_H_

#ifndef _ITF_RAY_GAMEMATERIAL_H_
#include "rayman/gameplay/Ray_GameMaterial.h"
#endif //_ITF_RAY_GAMEMATERIAL_H_

namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(Ray_ShooterPirateShipAIComponent)

    BEGIN_SERIALIZATION_CHILD(Ray_ShooterPirateShipAIComponent)

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

    const f32 k_fogColorInterpolationDuration = 0.25f;


    Ray_ShooterPirateShipAIComponent::Ray_ShooterPirateShipAIComponent() 
        : Super()
        , m_bounceBallonAnimCursor( 99.f )
        , m_curFogColor()
        , m_destFogColor()
        , m_fogInterpolTimer( 0.0f )
        , m_useTempateFogParams ( btrue )
        , m_instanceDynamicFogColor( Color::black() )
        , m_instanceDynamicFogMaxDepth( 30.0f )
        , m_useModifierFogParams ( bfalse )
        , m_modifierDynamicFogColor( Color::black() )
        , m_modifierDynamicFogMaxDepth( 30.0f )
        , m_lastCamZ( 0.0f )
        , m_lastCamDepth( 0.0f )
    {
    }

    void Ray_ShooterPirateShipAIComponent::preChangeProperties()
    {
        if ( !m_useTempateFogParams )
        {
            m_curFogColor = m_instanceDynamicFogColor;
        }
        else
        {
            m_curFogColor.m_r = getTemplate()->getDynamicFogDefaultColor().m_x / 255.0f;
            m_curFogColor.m_g = getTemplate()->getDynamicFogDefaultColor().m_y / 255.0f;
            m_curFogColor.m_b = getTemplate()->getDynamicFogDefaultColor().m_z / 255.0f;
        }
        m_destFogColor = m_curFogColor;
        onDepthChanged( m_actor->getDepth(), m_actor->getDepth() );
    }

    //----------------------------------------------------------------------------------------------------------------------------------
    void Ray_ShooterPirateShipAIComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        Super::onActorLoaded(_hotReload);

        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(PunchStim,200533519), this);
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor, ITF_GET_STRINGID_CRC(Ray_EventDynamicFogParamSet,2084909099), this );
        m_bounceBallonAnimCursor = 99.0f;

        if ( m_useTempateFogParams )
        {
            m_curFogColor.m_r = getTemplate()->getDynamicFogDefaultColor().m_x / 255.0f;
            m_curFogColor.m_g = getTemplate()->getDynamicFogDefaultColor().m_y / 255.0f;
            m_curFogColor.m_b = getTemplate()->getDynamicFogDefaultColor().m_z / 255.0f;
        }
        else
        {
            m_curFogColor = m_instanceDynamicFogColor;
        }
        m_destFogColor = m_curFogColor;
    }

    //----------------------------------------------------------------------------------------------------------------------------------
    void Ray_ShooterPirateShipAIComponent::Update( f32 _deltaTime )
    {
        Super::Update(_deltaTime);

        m_bounceBallonAnimCursor = f32_Min( m_bounceBallonAnimCursor + _deltaTime, 99.f );

        if ( ( m_lastCamZ != CAMERACONTROLLERMANAGER->getCamPos().m_z ) || ( m_fogInterpolTimer > 0.0f ) )
        {
            m_fogInterpolTimer = f32_Max( 0.0f, m_fogInterpolTimer - _deltaTime );
            m_curFogColor = Color::Interpolate( m_curFogColor, m_destFogColor, 1.0f - ( m_fogInterpolTimer / k_fogColorInterpolationDuration ) );
            
            onDepthChanged( m_actor->getDepth(), m_actor->getDepth() );
            m_lastCamZ = CAMERACONTROLLERMANAGER->getCamPos().m_z;
        }
    }
    //----------------------------------------------------------------------------------------------------------------------------------
    void Ray_ShooterPirateShipAIComponent::onBecomeActive( )
    {
        Super::onBecomeActive();

        m_fogInterpolTimer = 0.0;
        onDepthChanged( m_actor->getDepth(), m_actor->getDepth() );
    }

    //----------------------------------------------------------------------------------------------------------------------------------
    void Ray_ShooterPirateShipAIComponent::onEvent( Event* _event )
    {
        Super::onEvent(_event);

        if ( Ray_EventDynamicFogParamSet* fogParam = _event->DynamicCast<Ray_EventDynamicFogParamSet>(ITF_GET_STRINGID_CRC(Ray_EventDynamicFogParamSet,2084909099)) )
        {
            m_useModifierFogParams = btrue;
            m_modifierDynamicFogColor = fogParam->getFogColor();
            m_modifierDynamicFogMaxDepth = fogParam->getFogMaxDepth();

            m_destFogColor = m_modifierDynamicFogColor;
            m_fogInterpolTimer = k_fogColorInterpolationDuration;

            onDepthChanged( m_actor->getDepth(), m_actor->getDepth() );
        }
    }
   
    //----------------------------------------------------------------------------------------------------------------------------------
    void Ray_ShooterPirateShipAIComponent::processHit( HitStim* _hit )
    {
        Super::processHit( _hit );

        if ( _hit->getContacts().size() > 0 )
        {
            const SCollidableContact&  contact = _hit->getContacts()[0];

            PolyLine* poly(NULL);
            const PolyLineEdge* edge(NULL);

            if ( contact.m_collidableUserData == m_actor->getRef() )
            {
                return;
            }

            if ( contact.m_edgeIndex != U32_INVALID )
            {
                AIUtils::getPolyLine(contact.m_collidableUserData, contact.m_edgeIndex, poly, edge);

                if ( poly && edge )
                {
                    if ( poly->m_isCollisionActive )
                    {
                        const GameMaterial_Template *  gmat = World::getGameMaterial(edge->getGameMaterial());
                        if ( gmat )
                        {
                            if ( const Ray_GameMaterial_Template * raygmat = gmat->DynamicCast<Ray_GameMaterial_Template>(ITF_GET_STRINGID_CRC(Ray_GameMaterial_Template,2777394804)) )                                
                            {
                                if (  raygmat->getIsBounce()  )
                                {
                                    m_bounceBallonAnimCursor = 0.f;
                                }
                            }
                        }
                    }
                }
            }
        }
        
    }

    //----------------------------------------------------------------------------------------------------------------------------------
    void Ray_ShooterPirateShipAIComponent::onPieceHit( Actor * _piece )
    {
        Super::onPieceHit( _piece );
    }

    //----------------------------------------------------------------------------------------------------------------------------------
    void Ray_ShooterPirateShipAIComponent::onBehaviorFinished()
    {
        Super::onBehaviorFinished();
    }

    //----------------------------------------------------------------------------------------------------------------------------------
    void Ray_ShooterPirateShipAIComponent::updateAnimInput()
    {
        Super::updateAnimInput();

        if (  m_animatedComponent != NULL  )
        {
            m_animatedComponent->setInput( getTemplate()->getBounceBallonInput(), m_bounceBallonAnimCursor );
        }
    }  

    //--------------------------------------------------------------------------------------------------------------------------------
    void Ray_ShooterPirateShipAIComponent::onDepthChanged( f32 _oldZ, f32 _newZ )
    {
        Super::onDepthChanged( _oldZ, _newZ );

        f32 dynFogMaxDepth = m_useModifierFogParams ? m_modifierDynamicFogMaxDepth : ( m_useTempateFogParams ? getTemplate()->getDynamicFogMaxDepth() : m_instanceDynamicFogMaxDepth );

        if ( ( getHealth() > 0 ) && ( getTemplate()->useDynamicFog() || !m_useTempateFogParams || m_useModifierFogParams ) && ( dynFogMaxDepth != 0.0f ) )
        {
            // Check if the main camera depth is not zero.
            // in game, depth is not zero. Only controlled camera (cine, trajectory etc.) can have zero value
            ICameraController * mainController = CAMERACONTROLLERMANAGER->getMainCameraController();
            if ( ( mainController != NULL ) && ( mainController->getDepth() != 0.0f ) )
            {
                m_lastCamDepth = mainController->getDepth();
            }
            f32 distToCamZ = f32_Max( 0.0f, CAMERACONTROLLERMANAGER->getCamPos().m_z - m_lastCamDepth - _newZ );

            m_destFogColor.m_a = f32_Min( distToCamZ, dynFogMaxDepth ) / dynFogMaxDepth;
            if ( m_fogInterpolTimer == 0.0f )
            {
                m_curFogColor.m_a = m_destFogColor.m_a;
            }

            ActorBindHandler & bindHandler = m_actor->getChildrenBindHandler();
            const ObjectRefList&  children = bindHandler.getChildren();
            Ray_EventDynamicFogParamSet fogEvent( m_curFogColor.getAsU32(), dynFogMaxDepth );

            // Update all children
            for ( u32 i = 0; i < children.size(); ++i )
            {
                BaseObject * obj = children[i].getObject();
                if ( obj != NULL )
                {
                    Actor * actor = obj->DynamicCast<Actor>(ITF_GET_STRINGID_CRC(Actor,2546623115));
                    if ( actor != NULL )
                    {   
                        if ( GraphicComponent * animComp = actor->GetComponent<GraphicComponent>() )
                        {
                            animComp->setColorFog( m_curFogColor );
                        }
                        actor->onEvent( &fogEvent );
                    }
                }
            }
            m_animComponent->setColorFog( m_curFogColor );
        }
    }

    //----------------------------------------------------------------------------------------------------------------------------------------

    IMPLEMENT_OBJECT_RTTI(Ray_ShooterPirateShipAIComponent_Template)
    BEGIN_SERIALIZATION_CHILD(Ray_ShooterPirateShipAIComponent_Template)
        SERIALIZE_MEMBER( "bounceBallonInput",          m_bounceBallonInput );
        SERIALIZE_MEMBER( "useDynamicFog",              m_useDynamicFog );  
        SERIALIZE_MEMBER( "dynamicFogDefaultColor",     m_dynamicFogDefaultColor );  
        SERIALIZE_MEMBER( "dynamicFogMaxDepth",         m_dynamicFogMaxDepth );  
    END_SERIALIZATION()

    Ray_ShooterPirateShipAIComponent_Template::Ray_ShooterPirateShipAIComponent_Template()
    : Super()
    , m_useDynamicFog( bfalse )
    , m_dynamicFogDefaultColor( Vec3d::Zero )
    , m_dynamicFogMaxDepth( 1.f )     
    {
    }

    Ray_ShooterPirateShipAIComponent_Template::~Ray_ShooterPirateShipAIComponent_Template()
    {
            
    }

} // namespace ITF

