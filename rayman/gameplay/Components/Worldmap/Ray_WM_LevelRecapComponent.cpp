#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAYWMLEVELRECAPCOMPONENT_H_
#include "rayman/gameplay/Components/WorldMap/Ray_WM_LevelRecapComponent.h"
#endif //_ITF_RAYWMLEVELRECAPCOMPONENT_H_

#ifndef _ITF_RAY_GAMEMANAGER_H_
#include "rayman/gameplay/Ray_GameManager.h"
#endif //_ITF_RAY_GAMEMANAGER_H_

#ifndef _ITF_RAY_AIUTILS_H_
#include "Rayman/gameplay/AI/Utils/Ray_AIUtils.h"
#endif //_ITF_RAY_AIUTILS_H_

#ifndef _ITF_UITEXTMANAGER_H_
#include "engine/actors/managers/UITextManager.h"
#endif //_ITF_UITEXTMANAGER_H_

namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(Ray_WM_LevelRecapComponent_Template)

    BEGIN_SERIALIZATION(Ray_WM_LevelRecapComponent_Template)
    END_SERIALIZATION()

    //-------------------------------------------------------------------------------------

    IMPLEMENT_OBJECT_RTTI(Ray_WM_LevelRecapComponent)

    BEGIN_SERIALIZATION(Ray_WM_LevelRecapComponent)
    END_SERIALIZATION()

    void Ray_WM_LevelRecapComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        Super::onActorLoaded(_hotReload);

        m_animatedComponent = m_actor->GetComponent<AnimatedComponent>();
        if (m_animatedComponent)
        {
            m_animatedComponent->setUpdateAnimInput(this);
            m_animatedComponent->setRank2D(UI_TEXTMANAGER->getTextsDepthRank());
        }

    }

    void Ray_WM_LevelRecapComponent::onBecomeActive()
    {
        Super::onBecomeActive();
    }

    void Ray_WM_LevelRecapComponent::Update( f32 _dt )
    {
        Super::Update(_dt);

        switch (m_state)
        {
        case STATE_DISPLAYING:
            {
                m_currentTransitionTime += _dt;
                Vec2d scale = Vec2d::One;
                if ( m_currentTransitionTime >= m_transitionTime )
                {
                    scale   = m_scaleEnd;
                    m_state = STATE_DISPLAYED;
                }
                else
                {
                    scale = m_scaleStart + (m_scaleEnd - m_scaleStart) * m_currentTransitionTime / m_transitionTime;
                }

                m_actor->setScale(scale);
            }
        	break;
        case STATE_DISPLAYED:
            break;
        case STATE_HIDING:
            {
                m_currentTransitionTime += _dt;
                Vec2d scale = Vec2d::One;
                if ( m_currentTransitionTime >= m_transitionTime )
                {
                    scale   = m_scaleEnd;
                    m_state = STATE_HIDDEN;
                }
                else
                {
                    scale = m_scaleStart + (m_scaleEnd - m_scaleStart) * m_currentTransitionTime / m_transitionTime;
                }

                m_actor->setScale(scale);
            }
            break;
        case STATE_HIDDEN:
            break;
        default:
            ITF_ASSERT(0);
            break;
        }
    }

    void Ray_WM_LevelRecapComponent::Draw2D()
    {
        Super::Draw2D();

    }

    void Ray_WM_LevelRecapComponent::updateAnimInput()
    {
        ITF_ASSERT(m_animatedComponent);
        if (!m_animatedComponent)
        {
            return;
        }

        ESpot_State state = RAY_GAMEMANAGER->getWMSpotState(m_levelTag);

        if ( state == SPOT_STATE_CLOSED
            || state == SPOT_STATE_CANNOT_ENTER )
            m_animatedComponent->setAlpha(0.5f);
        else
            m_animatedComponent->setAlpha(1.0f);

        StringID type = RAY_GAMEMANAGER->getMapConfigType(m_levelTag);
        bbool isComplete = bfalse;
        if ( RAY_GAMEMANAGER->getLevelData(m_levelTag) )
            m_electoons = Ray_AIUtils::computeElectoonMask(type, RAY_GAMEMANAGER->getLevelData(m_levelTag), NULL, bfalse, isComplete);

        m_animatedComponent->setInput(ITF_GET_STRINGID_CRC(electoon1,1434682974), m_electoons & 1);
        m_animatedComponent->setInput(ITF_GET_STRINGID_CRC(electoon2,925843018), (m_electoons >> 1) & 1);
        m_animatedComponent->setInput(ITF_GET_STRINGID_CRC(electoon3,900890873), (m_electoons >> 2) & 1);
        m_animatedComponent->setInput(ITF_GET_STRINGID_CRC(electoon4,4023379268), (m_electoons >> 3) & 1);
        m_animatedComponent->setInput(ITF_GET_STRINGID_CRC(electoon5,4245588725), (m_electoons >> 4) & 1);
        m_animatedComponent->setInput(ITF_GET_STRINGID_CRC(electoon6,3475466807), (m_electoons >> 5) & 1);

        const Ray_GameManagerConfig_Template::MissionConfig *config = RAY_GAMEMANAGER->findMissionConfigByType(type, btrue);

        u32 medaillonSize = config->m_medalSlots.size();
        m_animatedComponent->setInput(ITF_GET_STRINGID_CRC(is3Slots,3580378956),medaillonSize == 3);
        m_animatedComponent->setInput(ITF_GET_STRINGID_CRC(is6Slots,415348464),medaillonSize == 6);

        m_animatedComponent->setInput(ITF_GET_STRINGID_CRC(isChest,1355776323),(type.GetValue() == ITF_GET_STRINGID_CRC(chest,2256079071)));
        m_animatedComponent->setInput(ITF_GET_STRINGID_CRC(isCompleted,7003086),RAY_GAMEMANAGER->getWMSpotState(m_levelTag) == SPOT_STATE_COMPLETED);
    }

    void Ray_WM_LevelRecapComponent::show(const Vec2d& _scaleStart, const Vec2d& _scaleEnd, const float& _time)
    {
        m_scaleStart    = _scaleStart;
        m_scaleEnd      = _scaleEnd;
        m_transitionTime= _time;
        m_currentTransitionTime = 0.0f;
        m_state         = STATE_DISPLAYING;
    }

    void Ray_WM_LevelRecapComponent::hide(const Vec2d& _scaleStart, const Vec2d& _scaleEnd, const float& _time)
    {
        m_scaleStart    = _scaleStart;
        m_scaleEnd      = _scaleEnd;
        m_transitionTime= _time;
        m_currentTransitionTime = 0.0f;
        m_state         = STATE_HIDING;
    }
};
