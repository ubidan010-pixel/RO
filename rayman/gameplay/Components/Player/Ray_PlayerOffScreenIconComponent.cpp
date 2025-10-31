#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_PLAYEROFFSCREENICONCOMPONENT_H_
#include "rayman/gameplay/Components/player/Ray_PlayerOffScreenIconComponent.h"
#endif //_ITF_RAY_PLAYEROFFSCREENICONCOMPONENT_H_

#ifndef _ITF_RAY_GAMEMANAGER_H_
#include "rayman/gameplay/Ray_GameManager.h"
#endif //_ITF_RAY_GAMEMANAGER_H_

#ifndef _ITF_ANIMATION_H_
#include "engine/animation/Animation.h"
#endif //_ITF_ANIMATION_H_

#ifndef _ITF_LOADINGSCREEN_H_
#include "engine/loading/loadingscreen.h"
#endif //_ITF_LOADINGSCREEN_H_

#ifndef _ITF_CAMERASUBJECTCOMPONENT_H_
#include "gameplay/Components/Misc/CameraSubjectComponent.h"
#endif //_ITF_CAMERASUBJECTCOMPONENT_H_

#ifndef _ITF_VIEW_H_
#include "engine/display/View.h"
#endif //_ITF_VIEW_H_

#ifndef _ITF_CINEMATICMANAGER_H_
#include "gameplay/managers/CinematicManager.h"
#endif //_ITF_CINEMATICMANAGER_H_

//#define DEBUG_OFFSCREENICON

namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(Ray_PlayerOffScreenIconComponent)

    BEGIN_SERIALIZATION_CHILD(Ray_PlayerOffScreenIconComponent)
    END_SERIALIZATION()

    BEGIN_VALIDATE_COMPONENT(Ray_PlayerOffScreenIconComponent)
        VALIDATE_COMPONENT_PARAM("", m_animComponent, "Ray_PlayerOffScreenIconComponent requires a AnimLightComponent/AnimatedComponent");
        VALIDATE_COMPONENT_PARAM("arrowBoneName", getTemplate()->getArrowBoneName().isValid(), "invalid bone");
        VALIDATE_COMPONENT_PARAM("arrowBoneName", m_arrowBoneIndex != U32_INVALID, "invalid bone");
    END_VALIDATE_COMPONENT()

    Ray_PlayerOffScreenIconComponent::Ray_PlayerOffScreenIconComponent()
        : Super()
        , m_animComponent(NULL)
        , m_arrowBoneIndex(U32_INVALID)
        , m_player(NULL)
        , m_playerActor(NULL)
        , m_lastAnchor(Vec2d::Zero)
        , m_showBubble(bfalse)
        , m_timer(0)
        , m_playerIndex(U32_INVALID)
    {
    }

    Ray_PlayerOffScreenIconComponent::~Ray_PlayerOffScreenIconComponent()
    {
    }

    void Ray_PlayerOffScreenIconComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        Super::onActorLoaded(_hotReload);

        m_animComponent = m_actor->GetComponent<AnimLightComponent>();
        if (m_animComponent)
        {
            m_animComponent->setIsProcedural(btrue);
        }

        Player* myPlayer = GAMEMANAGER->getPlayerFromActor(m_actor->getRef(),bfalse);

        if ( myPlayer )
        {
            m_playerIndex = myPlayer->getIndex();
        }

        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventPlayerIndexChange,2964720152),this);
    }

    void Ray_PlayerOffScreenIconComponent::onResourceReady()
    {
        if (m_animComponent)
        {
            m_arrowBoneIndex = m_animComponent->getBoneIndex(getTemplate()->getArrowBoneName());
        }
    }

    void Ray_PlayerOffScreenIconComponent::initPlayer()
    {
        // need to refetch the player every frame, in case it got swapped
        m_player = NULL;
        m_playerActor = NULL;

        if ( m_playerIndex != U32_INVALID )
        {
            m_player = static_cast<Ray_Player*>(GAMEMANAGER->getPlayer(m_playerIndex));
            ITF_ASSERT(m_player != NULL);

            if (m_player)
            {
                m_playerActor = m_player->getActor();
                ITF_ASSERT(m_playerActor != NULL);
            }
        }
    }

    void Ray_PlayerOffScreenIconComponent::onEvent( Event* _event )
    {
        if ( EventPlayerIndexChange* playerIndex = _event->DynamicCast<EventPlayerIndexChange>(ITF_GET_STRINGID_CRC(EventPlayerIndexChange,2964720152)) )
        {
            m_playerIndex = playerIndex->getNewIndex();
        }
    }

    void Ray_PlayerOffScreenIconComponent::Update( f32 _deltaTime )
    {
        initPlayer();

        if ( !m_playerActor )
        {
            return;
        }



        // check if we need to show the bubble or not
        bbool showBubble = bfalse;

        if ( m_player->getActive() &&
             m_player->getCurrentMode() != RAY_GAMEMODE_WORLDMAP &&
             !m_player->isDead() &&
             !LOADINGSCREEN->isVisible() && 
             !CINEMATICMANAGER->getLockPlayers()
#ifndef DEVELOPER_BEN
             && CAMERA->getMode() == Camera::Camera_InGame
#endif
           )
        {
            EventQueryIsDisabled query;
            m_playerActor->onEvent(&query);

            if (!query.getIsDisabled())
            {
                CameraSubjectComponent* camSubject = m_playerActor->GetComponent<CameraSubjectComponent>();
                ITF_ASSERT(camSubject);
                if (camSubject)
                {
                    // show the bubble if the player is offscreen
                    showBubble = !camSubject->isSubjectOnScreen(btrue);
                }
            }
        }


        // monitor state change
        if (showBubble != m_showBubble)
        {
            m_timer = 0;
        }
        m_showBubble = showBubble;


        // fade the bubble in/out

        f32 alpha = m_animComponent->getAlpha();
        bbool bubbleVisible = alpha > 0;

        if (showBubble)
        {
            if (!bubbleVisible)
            {
                // wait for the 'in' delay
                m_timer += _deltaTime;
                if (m_timer > getTemplate()->getFadeInDelay())
                    bubbleVisible = btrue;
            }

            if (bubbleVisible)
            {
                // fade in
                alpha += _deltaTime / getTemplate()->getFadeInDuration();
                if (alpha > 1)
                {
                    // done fading in
                    m_timer = 0;
                    alpha = 1;
                }
            }
        }
        else
        {
            if (!bubbleVisible)
            {
                // wait for the 'out' delay
                m_timer += _deltaTime;
                if (m_timer > getTemplate()->getFadeOutDelay())
                    bubbleVisible = btrue;
            }

            if (bubbleVisible)
            {
                // fade out
                alpha -= _deltaTime / getTemplate()->getFadeOutDuration();
                if (alpha < 0)
                {
                    // done fading out
                    m_timer = 0;
                    alpha = 0;
                    bubbleVisible = bfalse;
                }
            }
        }

        m_animComponent->setAlpha(alpha);


        // position the bubble along the screen edges
        if (bubbleVisible)
        {
            positionBubble();
        }


#ifdef DEBUG_OFFSCREENICON
        String debugMessage;
        debugMessage.setTextFormat("%s-screen for %.1f s (%.0f %% - bubble visible %s)",
            isPlayerOnScreen ? "ON" : "OFF",
            m_timer,
            alpha * 100,
            bubbleVisible ? "true" : "false");
        GFX_ADAPTER->drawDBGText(debugMessage, 200, (f32)(10 + 20 * m_player->getIndex()));

        CameraSubjectComponent* camSubject = m_playerActor->GetComponent<CameraSubjectComponent>();
        GFX_ADAPTER->drawDBGAABB(camSubject->getAABB());
#endif
    }

    void Ray_PlayerOffScreenIconComponent::positionBubble()
    {
        f32 width = (f32)GFX_ADAPTER->getScreenWidth();
        f32 height = (f32)GFX_ADAPTER->getScreenHeight();


        // get the player position in screen-space
        Vec3d playerPos;
        GFX_ADAPTER->compute3DTo2D(m_playerActor->getPos(), playerPos);
        playerPos.m_y = height - playerPos.m_y;
        Vec2d playerPosition = playerPos.truncateTo2D();

        // clamp along the screen edges (with a border)
        Vec2d anchor(Vec2d::Zero);
        if (m_showBubble)
        {
            anchor.m_x = Clamp(playerPosition.m_x, getTemplate()->getScreenBorder().m_x, width - getTemplate()->getScreenBorder().m_x);
            anchor.m_y = Clamp(playerPosition.m_y, getTemplate()->getScreenBorder().m_y, height - getTemplate()->getScreenBorder().m_y);
            anchor.m_y = height - anchor.m_y;
            m_lastAnchor = anchor;
        }
        else
        {
            // stick to the last position while fading out
            anchor = m_lastAnchor;
        }

        // move the bubble at the intersection
        m_actor->set2DPos(anchor);


        // point the arrow towards the player
        anchor.m_y = height - anchor.m_y;
        Vec2d direction = playerPosition - anchor;
        direction.normalize();

        AnimBoneDyn* bone = m_animComponent->getBone(m_arrowBoneIndex);
        if (bone)
        {
            bone->setDirection(direction);

            // apply changes to patches
            m_animComponent->updatePatchAndAABB();
        }

#ifdef DEBUG_OFFSCREENICON
        Vec3d anchor3d;
        anchor.m_y = height - anchor.m_y;
        GFX_ADAPTER->compute2DTo3D(anchor, 0, anchor3d);
        GFX_ADAPTER->drawDBG3DLine(anchor3d, anchor3d + direction.to3d(), Color::red(), 0, 3); 
#endif // DEBUG_OFFSCREENICON
    }

    //-------------------------------------------------------------------------------------
    IMPLEMENT_OBJECT_RTTI(Ray_PlayerOffScreenIconComponent_Template)
    BEGIN_SERIALIZATION(Ray_PlayerOffScreenIconComponent_Template)
        SERIALIZE_MEMBER("arrowBoneName", m_arrowBoneName);
        SERIALIZE_MEMBER("screenBorder", m_screenBorder);
        SERIALIZE_MEMBER("fadeInDelay", m_fadeInDelay);
        SERIALIZE_MEMBER("fadeInDuration", m_fadeInDuration);
        SERIALIZE_MEMBER("fadeOutDelay", m_fadeOutDelay);
        SERIALIZE_MEMBER("fadeOutDuration", m_fadeOutDuration);
    END_SERIALIZATION()

    Ray_PlayerOffScreenIconComponent_Template::Ray_PlayerOffScreenIconComponent_Template()
    : m_arrowBoneName("arrow")
    , m_screenBorder(Vec2d(0, 0))
    , m_fadeInDelay(0)
    , m_fadeInDuration(0)
    , m_fadeOutDelay(0)
    , m_fadeOutDuration(0)
    {
    }
};