#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAYWMNEWCOMPONENT_H_
#include "rayman/gameplay/Components/WorldMap/Ray_WM_NewComponent.h"
#endif //_ITF_RAYWMNEWCOMPONENT_H_

#ifndef _ITF_RAY_GAMEMANAGER_H_
#include "rayman/gameplay/Ray_GameManager.h"
#endif //_ITF_RAY_GAMEMANAGER_H_

#ifndef _ITF_UITEXTMANAGER_H_
#include "engine/actors/managers/UITextManager.h"
#endif //_ITF_UITEXTMANAGER_H_

#define X_RESOLUTION_REFERENCE  1920
#define Y_RESOLUTION_REFERENCE  1080

namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(Ray_WM_NewComponent_Template)
    BEGIN_SERIALIZATION(Ray_WM_NewComponent_Template)
        SERIALIZE_MEMBER("scale",m_scale);
        SERIALIZE_MEMBER("yOffsetFromBottom",m_yOffsetFromBottom);
        SERIALIZE_MEMBER("xOffsetFromLeft",m_xOffsetFromLeft);
        SERIALIZE_MEMBER("transitionTime",m_transitionTime);
        SERIALIZE_MEMBER("waitTime",m_waitTime);
    END_SERIALIZATION()

    //-------------------------------------------------------------------------------------

    IMPLEMENT_OBJECT_RTTI(Ray_WM_NewComponent)

    BEGIN_SERIALIZATION(Ray_WM_NewComponent)
    END_SERIALIZATION()

    void Ray_WM_NewComponent::onActorClearComponents()
    {
        if ( EVENTMANAGER )
        {
            EVENTMANAGER->unregisterEvent(ITF_GET_STRINGID_CRC(Ray_EventWMShowNewNotification,1272081763), this);
        }
    }

    void Ray_WM_NewComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        Super::onActorLoaded(_hotReload);

        m_animLightCmp = m_actor->GetComponent<AnimLightComponent>();
        if(m_animLightCmp)
        {
            m_animLightCmp->setRank2D(UI_TEXTMANAGER->getAnimDepthRank());
            m_animLightCmp->setAlpha(0.0f);
        }

        // Register global event Ray_EventWMDisplayWorldRecap
        EVENTMANAGER_REGISTER_EVENT_LISTENER(ITF_GET_STRINGID_CRC(Ray_EventWMShowNewNotification,1272081763),this);
    }

    void Ray_WM_NewComponent::onEvent(Event * _event)
    {
        Super::onEvent(_event);

        // Check display / hide recap event
        if ( Ray_EventWMShowNewNotification* evShowNew = _event->DynamicCast<Ray_EventWMShowNewNotification>(ITF_GET_STRINGID_CRC(Ray_EventWMShowNewNotification,1272081763)) )
        {
            m_allowNotification = evShowNew->show();
        }
    }

    void Ray_WM_NewComponent::onBecomeActive()
    {
        Super::onBecomeActive();
    }

    void Ray_WM_NewComponent::showNew(const StringID& _worldTag)
    {
        if ( !_worldTag.isValid() )
            return;

        RAY_GAMEMANAGER->getWorldsWithWarnings(m_newWorlds);

        m_currentworldTag = _worldTag;
        u32 size = m_newWorlds.size();
        if ( size >= 2
            || ( (size == 1) && (m_newWorlds[0] != m_currentworldTag) ) )
        {
            // new world, then display it
            m_state = STATE_WAITING_BEFORE_DISPLAYING;
        }
    }

    void Ray_WM_NewComponent::hideNew()
    {
        m_state = STATE_HIDING;
    }

    void Ray_WM_NewComponent::Update( f32 _dt )
    {
        Super::Update(_dt);

        switch (m_state)
        {
        case STATE_WAITING_BEFORE_DISPLAYING:
            {
                m_currentTransitionTime += _dt;
                if ( m_currentTransitionTime >= getTemplate()->getWaitTime() )
                {
                    m_state = STATE_DISPLAYING;
                    m_currentTransitionTime = 0.0f;
                }
            }
            break;
        case STATE_DISPLAYING:
            {
                if ( m_animLightCmp )
                {
                    m_currentTransitionTime += _dt;
                    f32 alpha = 0.0f;
                    if ( m_currentTransitionTime >= getTemplate()->getTransitionTime() )
                    {
                        alpha = 1.0f;
                        m_currentTransitionTime = 0.0f;
                        m_state = STATE_DISPLAYED;
                    }
                    else
                    {
                        alpha = m_currentTransitionTime / getTemplate()->getTransitionTime();
                    }

                    m_animLightCmp->setAlpha(alpha);
                }
                else
                {
                    m_state = STATE_DISPLAYED;
                }
            }
            break;
        case STATE_DISPLAYED:
            {
                const StringID& currentLevelTag = RAY_GAMEMANAGER->getWMCurrentLevelTag();
                if ( !currentLevelTag.isValid() || !m_allowNotification || RAY_GAMEMANAGER->isPlayingWorldMapUnlockSequenceAndDisplayingUnlocks())
                {
                    hideNew();
                }
            }
            break;
        case STATE_HIDING:
            {
                if ( m_animLightCmp )
                {

                    m_currentTransitionTime += _dt;
                    f32 alpha = 0.0f;
                    if ( m_currentTransitionTime >= getTemplate()->getTransitionTime() )
                    {
                        alpha = 1.0f;
                        m_currentTransitionTime = 0.0f;
                        m_state = STATE_HIDDEN;
                    }
                    else
                    {
                        alpha = m_currentTransitionTime / getTemplate()->getTransitionTime();
                    }

                    m_animLightCmp->setAlpha(1.0f-alpha);
                }
                else
                {
                    m_state = STATE_HIDDEN;
                }
            }
            break;
        case STATE_HIDDEN:
            {
                const StringID& currentLevelTag = RAY_GAMEMANAGER->getWMCurrentLevelTag();
                // only display new if we're in a level map
                if ( currentLevelTag.isValid() && m_allowNotification && !RAY_GAMEMANAGER->isPlayingWorldMapUnlockSequenceAndDisplayingUnlocks())
                {
                    showNew(RAY_GAMEMANAGER->getWMCurrentWorldTag());
                }
            }
            break;
        default:
            ITF_ASSERT(0);
            break;
        }
    }

    void Ray_WM_NewComponent::Draw2D()
    {
        Super::Draw2D();

        Vec2d safeMarginVec, safeSize;
        GFX_ADAPTER->computeSafeFrame(safeMarginVec, safeSize);

        Vec3d pos = Vec3d(0.0f,0.0f,0.0f);
        pos.m_x = safeMarginVec.m_x + getTemplate()->getxOffsetFromLeft() * m_screenScale.m_x;
        pos.m_y = safeSize.m_y + safeMarginVec.m_y - getTemplate()->getyOffsetFromBottom() * m_screenScale.m_y;

        m_actor->setPos(Vec3d(pos.m_x,pos.m_y,5.0f));
        
        m_screenScale.m_x   = (f32)GFX_ADAPTER->getScreenWidth() / X_RESOLUTION_REFERENCE;
        m_screenScale.m_y   = (f32)GFX_ADAPTER->getScreenHeight() / Y_RESOLUTION_REFERENCE;

        m_actor->setScale(getTemplate()->getScale() * m_screenScale);
    }

};
