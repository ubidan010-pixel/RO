#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_PLAYERDEADSOUL2DCOMPONENT_H_
#include "rayman/gameplay/Components/player/Ray_PlayerDeadSoul2DComponent.h"
#endif //_ITF_RAY_PLAYERDEADSOUL2DCOMPONENT_H_

#ifndef _ITF_RAY_GAMEMANAGER_H_
#include "rayman/gameplay/Ray_GameManager.h"
#endif //_ITF_RAY_GAMEMANAGER_H_

#ifndef _ITF_VIEW_H_
#include "engine/display/View.h"
#endif //_ITF_VIEW_H_

#ifndef _ITF_FXCONTROLLERCOMPONENT_H_
#include "gameplay/components/Misc/FXControllerComponent.h"
#endif //_ITF_FXCONTROLLERCOMPONENT_H_

#ifndef _ITF_RAY_PLAYERCONTROLLERCOMPONENT_H_
#include "rayman/gameplay/Components/Player/Ray_PlayerControllerComponent.h"
#endif //_ITF_RAY_PLAYERCONTROLLERCOMPONENT_H_


namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(Ray_PlayerDeadSoul2DComponent)

    BEGIN_SERIALIZATION_CHILD(Ray_PlayerDeadSoul2DComponent)
    END_SERIALIZATION()

    BEGIN_VALIDATE_COMPONENT(Ray_PlayerDeadSoul2DComponent)
        VALIDATE_COMPONENT_PARAM("m_fxControllerComponent", m_fxControllerComponent, "Ray_PlayerDeadSoul2DComponent needs a FXControllerComponent" );
    END_VALIDATE_COMPONENT()

    //------------------------------------------------------------------
    Ray_PlayerDeadSoul2DComponent::Ray_PlayerDeadSoul2DComponent()
        : Super()
        , m_fxControllerComponent(NULL)
        , m_soulBubbleVisible(bfalse)
        , m_playerIndex(U32_INVALID)
        , m_fxHandle(U32_INVALID)
    {
    }

    //------------------------------------------------------------------
    Ray_PlayerDeadSoul2DComponent::~Ray_PlayerDeadSoul2DComponent()
    {
    }

    //------------------------------------------------------------------
    void Ray_PlayerDeadSoul2DComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        Super::onActorLoaded(_hotReload);

        //get fx controller component
        m_fxControllerComponent = m_actor->GetComponent<FXControllerComponent>();

        //register to player index changes event
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventPlayerIndexChange,2964720152),this);
    }

    //------------------------------------------------------------------
    void Ray_PlayerDeadSoul2DComponent::onEvent( Event* _event )
    {
        Super::onEvent(_event);

        if ( EventPlayerIndexChange* playerIndex = _event->DynamicCast<EventPlayerIndexChange>(ITF_GET_STRINGID_CRC(EventPlayerIndexChange,2964720152)) )
        {
            m_playerIndex = playerIndex->getNewIndex();
        }
    }

    //------------------------------------------------------------------
    void Ray_PlayerDeadSoul2DComponent::Update( f32 _deltaTime )
    {
        Super::Update(_deltaTime);

        if ( m_playerIndex == U32_INVALID )
        {
            return;
        }

        const Player * const player = GAMEMANAGER->getPlayer(m_playerIndex);
        if ( !player )
        {
            return;
        }

        Actor * const playerActor = player->getCurrentActor().getActor();
        if ( !playerActor )
        {
            return;
        }

        Ray_PlayerControllerComponent* pcComponent = playerActor->GetComponent<Ray_PlayerControllerComponent>();

        if ( !pcComponent )
        {
            return;
        }

        //check if component has to draw something
        const bbool visibleForPlayer = player->getActive() && pcComponent->isInDeadSoul();
        const bbool visibleForGame = RAY_GAMEMANAGER->getNumActivePlayers() > RAY_GAMEMANAGER->getNumDeadPlayers();
        const bbool visible = visibleForPlayer && visibleForGame;

        //if state changed
        if( visible != m_soulBubbleVisible )
        {
            //store new state
            m_soulBubbleVisible = visible;
            //toggle fx 
            toggleFX(m_soulBubbleVisible);
        }

        if( m_soulBubbleVisible )
        {
            updateFXColor(player);
        }

        updateActor3DPosition(playerActor->getPos());
    }

    //------------------------------------------------------------------
    void Ray_PlayerDeadSoul2DComponent::toggleFX(const bbool _active)
    {
        ITF_ASSERT(m_fxControllerComponent);
        if( _active )
        {
            ITF_ASSERT(m_fxHandle == U32_INVALID);
            m_fxHandle = m_fxControllerComponent->playFX( getTemplate()->getFxName() );
        }
        else
        {
            ITF_ASSERT(m_fxHandle != U32_INVALID);
            m_fxControllerComponent->stopFX( getTemplate()->getFxName() );
            m_fxHandle = U32_INVALID;
        }
    }

    //------------------------------------------------------------------
    void Ray_PlayerDeadSoul2DComponent::updateActor3DPosition( const Vec3d& _pos )
    {
        m_actor->setPos( _pos );
    }

    //------------------------------------------------------------------
    void Ray_PlayerDeadSoul2DComponent::updateFXColor(const Player * const _player) const
    {
        ITF_ASSERT(_player);
        ITF_ASSERT(m_fxControllerComponent);
        ITF_ASSERT(m_fxHandle != U32_INVALID);

        const StringID & playerIdInfoName = _player->getCurrentPlayerIDInfo();
        const PlayerIDInfo * const playerInfo = GAMEMANAGER->getPlayerIDInfo(playerIdInfoName);
        if( playerInfo )
        {
            const FXControl * const fxControl = m_fxControllerComponent->getFXControl( getTemplate()->getFxName() );
            if( fxControl )
            {
                const Color & color = playerInfo->getDeathBubbleColor();
                for(u32 index = 0; index < fxControl->m_particles.size(); ++index)
                {
                    ITF_ParticleGenerator * const generator = m_fxControllerComponent->getParticleGenerator(m_fxHandle, fxControl->m_particles[index]);
                    if( generator )
                    {
                        generator->applyDefaultColor(color);
                    }
                }
            }
        }
    }

    //-------------------------------------------------------------------------------------
    IMPLEMENT_OBJECT_RTTI(Ray_PlayerDeadSoul2DComponent_Template)
    BEGIN_SERIALIZATION(Ray_PlayerDeadSoul2DComponent_Template)
        SERIALIZE_MEMBER("fxName", m_fxName);
    END_SERIALIZATION()

    //------------------------------------------------------------------
    Ray_PlayerDeadSoul2DComponent_Template::Ray_PlayerDeadSoul2DComponent_Template()
    : m_fxName("BubbleView")
    {
    }

    //------------------------------------------------------------------
    Ray_PlayerDeadSoul2DComponent_Template::~Ray_PlayerDeadSoul2DComponent_Template()
    {
    }
};