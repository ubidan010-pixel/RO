#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAYWMWORLDRECAPCOMPONENT_H_
#include "rayman/gameplay/Components/WorldMap/Ray_WM_WorldRecapComponent.h"
#endif //_ITF_RAYWMWORLDRECAPCOMPONENT_H_

#ifndef _ITF_RAYWMLEVELRECAPCOMPONENT_H_
#include "rayman/gameplay/Components/WorldMap/Ray_WM_LevelRecapComponent.h"
#endif //_ITF_RAYWMLEVELRECAPCOMPONENT_H_

#ifndef _ITF_RAY_GAMEMANAGER_H_
#include "rayman/gameplay/Ray_GameManager.h"
#endif //_ITF_RAY_GAMEMANAGER_H_

#ifndef _ITF_UITEXTMANAGER_H_
#include "engine/actors/managers/UITextManager.h"
#endif //_ITF_UITEXTMANAGER_H_

#ifndef _ITF_TRANSFORM_H_
#include "core/math/transform.h"
#endif //_ITF_TRANSFORM_H_

#define MIN_NB_ELEM     5
#define MAX_NB_ELEM     8

#define X_RESOLUTION_REFERENCE  1920
#define Y_RESOLUTION_REFERENCE  1080

namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(Ray_WM_WorldRecapComponent_Template)
    BEGIN_SERIALIZATION(Ray_WM_WorldRecapComponent_Template)
        SERIALIZE_MEMBER("recapElem",m_recapElem);
        SERIALIZE_MEMBER("yBGScale",m_yBGScale);
        SERIALIZE_MEMBER("xMaxBGScale",m_xMaxBGScale);
        SERIALIZE_MEMBER("elemMaxLength",m_elemMaxLength);
        SERIALIZE_MEMBER("elemMaxScale",m_elemMaxScale);
        SERIALIZE_MEMBER("yOffsetFromBottom",m_yOffsetFromBottom);
        SERIALIZE_MEMBER("transitionTime",m_transitionTime);
        SERIALIZE_MEMBER("boneToFollow",m_boneToFollow);
    END_SERIALIZATION()

    //-------------------------------------------------------------------------------------

    IMPLEMENT_OBJECT_RTTI(Ray_WM_WorldRecapComponent)

    BEGIN_SERIALIZATION(Ray_WM_WorldRecapComponent)
    END_SERIALIZATION()

    void Ray_WM_WorldRecapComponent::onActorClearComponents()
    {
        if ( EVENTMANAGER )
        {
            EVENTMANAGER->unregisterEvent(ITF_GET_STRINGID_CRC(Ray_EventWMDisplayWorldRecap,554350578), this);
        }

        deleteLevelRecaps();
    }

    void Ray_WM_WorldRecapComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        Super::onActorLoaded(_hotReload);

        if ( !getTemplate()->getRecapElem().isEmpty() )
            SPAWNER->declareNeedsSpawnee(m_actor, &m_recapSpawnee, getTemplate()->getRecapElem());

        m_animLightCmp = m_actor->GetComponent<AnimLightComponent>();
        if(m_animLightCmp)
        {
            m_animLightCmp->setRank2D(UI_TEXTMANAGER->getAnimDepthRank());
            m_animLightCmp->setAlpha(0.0f);
        }

        // Register global event Ray_EventWMDisplayWorldRecap
        EVENTMANAGER_REGISTER_EVENT_LISTENER(ITF_GET_STRINGID_CRC(Ray_EventWMDisplayWorldRecap,554350578),this);

        // Spawn all element needed to display
        for ( u32 i = 0 ; i < MAX_NB_ELEM ; i++ )
        {
            Actor * levelRecapAct = m_recapSpawnee.getSpawnee( m_actor->getScene(),getElemPosAt(i, MAX_NB_ELEM));
            if ( !levelRecapAct )
                continue;

            // Is this an appropriate actor ?
            Ray_WM_LevelRecapComponent * levelRecapCmp = levelRecapAct->GetComponent<Ray_WM_LevelRecapComponent>();
            if ( levelRecapCmp )
            {
                levelRecap_t levelRecapElem     = levelRecap_t();
                levelRecapElem.m_recapActorRef  = levelRecapAct->getRef();
                levelRecapElem.m_pRecapComp     = levelRecapCmp;

                // Disable it until we explicitly ask to display
                //levelRecapAct->disable();

                // Keep track of it
                m_levelRecaps.push_back(levelRecapElem);
            }
        }

        m_elemSpace     = getTemplate()->getElemMaxLength() / (MAX_NB_ELEM + 1);
        m_bgElemScale   = getTemplate()->getXMaxBGScale() / MAX_NB_ELEM;
    }

    void Ray_WM_WorldRecapComponent::onBecomeActive()
    {
        Super::onBecomeActive();
    }

    void Ray_WM_WorldRecapComponent::onEvent(Event * _event)
    {
        Super::onEvent(_event);

        // Check display / hide recap event
        if ( Ray_EventWMDisplayWorldRecap* evDisplayWorldRecap = _event->DynamicCast<Ray_EventWMDisplayWorldRecap>(ITF_GET_STRINGID_CRC(Ray_EventWMDisplayWorldRecap,554350578)) )
        {
            if ( evDisplayWorldRecap->getDisplay() )
                showRecap(evDisplayWorldRecap->getWorldTag());
            else
                hideRecap();
        }
    }

    void Ray_WM_WorldRecapComponent::Update( f32 _dt )
    {
        Super::Update(_dt);

        switch (m_state)
        {
        case STATE_DISPLAYING_BACKGROUND:
            {
                if ( m_animLightCmp )
                {
                    m_currentTransitionTime += _dt;
                    f32 alpha = 0.0f;
                    if ( m_currentTransitionTime >= getTemplate()->getTransitionTime() )
                    {
                        alpha = 1.0f;
                        m_currentTransitionTime = 0.0f;
                        m_state = STATE_DISPLAYING_LEVELS;
                    }
                    else
                    {
                        alpha = m_currentTransitionTime / getTemplate()->getTransitionTime();
                    }

                    m_animLightCmp->setAlpha(alpha);
                }
                else
                {
                    m_state = STATE_DISPLAYING_LEVELS;
                }
            }
            break;
        case STATE_DISPLAYING_LEVELS:
            {
                for (u32 i = 0 ; i < m_currentNbLevels ; i++)
                {
                    if ( m_levelRecaps[i].m_recapActorRef.isValid()
                        && m_levelRecaps[i].m_pRecapComp != NULL )
                    {
                        //m_levelRecaps[i].m_recapActorRef.getActor()->enable();
                        m_levelRecaps[i].m_pRecapComp->show(Vec2d::Zero, getTemplate()->getElemMaxScale() * m_screenScale, getTemplate()->getTransitionTime());
                    }
                }

                m_state = STATE_DISPLAYED;
            }
        	break;
        case STATE_DISPLAYED:
            break;
        case STATE_HIDING_LEVELS:
            {
                for (u32 i = 0 ; i < m_currentNbLevels ; i++)
                {
                    if ( m_levelRecaps[i].m_recapActorRef.isValid()
                        && m_levelRecaps[i].m_pRecapComp != NULL )
                        m_levelRecaps[i].m_pRecapComp->hide(getTemplate()->getElemMaxScale() * m_screenScale, Vec2d::Zero, getTemplate()->getTransitionTime());
                }

                m_state = STATE_WAITING_LEVELS_HIDDEN;
            }
            break;
        case STATE_WAITING_LEVELS_HIDDEN:
            {
                bbool allHidden = btrue;
                for (u32 i = 0 ; (i < m_currentNbLevels) && allHidden ; i++)
                {
                    if ( m_levelRecaps[i].m_pRecapComp != NULL && m_levelRecaps[i].m_pRecapComp->isHidden() )
                        allHidden = bfalse;
                }

                if ( allHidden )
                    m_state = STATE_HIDING_BACKGROUND;
            }
            break;
        case STATE_HIDING_BACKGROUND:
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
            break;
        default:
            ITF_ASSERT(0);
            break;
        }
    }

    void Ray_WM_WorldRecapComponent::Draw2D()
    {
        Super::Draw2D();

        m_screenScale.m_x   = (f32)GFX_ADAPTER->getScreenWidth() / X_RESOLUTION_REFERENCE;
        m_screenScale.m_y   = (f32)GFX_ADAPTER->getScreenHeight() / Y_RESOLUTION_REFERENCE;

        Vec2d safeMarginVec, safeSize;
        GFX_ADAPTER->computeSafeFrame(safeMarginVec, safeSize);

        m_actor->setPos(Vec3d(safeMarginVec.m_x + safeSize.m_x / 2.0f,safeSize.m_y + safeMarginVec.m_y - (getTemplate()->getyOffsetFromBottom() * m_screenScale.m_x),5.0f));
        f32 nbElems = m_currentNbLevels > 0 ? m_currentNbLevels : 1.0f;
        m_actor->setScale(Vec2d(m_bgElemScale * nbElems,getTemplate()->getYBGScale()) * m_screenScale);

        // Update level recap position depending on bone pos
        if ( m_animLightCmp )
        {
            // Get bone info
            if ( m_boneIndex == U32_INVALID && getTemplate()->getBoneToFollow().isValid() )
                m_boneIndex = m_animLightCmp->getBoneIndex(getTemplate()->getBoneToFollow());

            if ( m_boneIndex != U32_INVALID && m_currentNbLevels > 0 )
            {
                Vec3d bonePos    = Vec3d(1.0f,0.0f,0.0f);

                m_animLightCmp->getBonePos(m_boneIndex,bonePos);

                transform2d xf;

                xf.m_pos = m_actor->get2DPos();
                xf.m_rot.m_col1 = Vec2d::Right.Rotate(m_actor->getAngle());
                xf.m_rot.m_col2 = xf.m_rot.m_col1.getPerpendicular();

                bonePos.m_y *= -1.f;
                bonePos = mulTransform2d(xf,bonePos.truncateTo2D()*m_actor->getScale()).to3d(bonePos.m_z);

                // Update actor's position
                for ( u32 i = 0 ; i < m_currentNbLevels ; i++ )
                {
                    Actor * levelRecapAct = m_levelRecaps[i].m_recapActorRef.getActor();
                    if ( !levelRecapAct )
                        continue;

                    Vec3d currentPos = getElemPosAt(i,m_currentNbLevels);
                    currentPos.m_y = bonePos.m_y;

                    levelRecapAct->setPos(currentPos);
                }
            }
        }
    }

    void Ray_WM_WorldRecapComponent::showRecap(const StringID& _worldTag)
    {
        ITF_WARNING_CATEGORY(GPP,NULL,_worldTag.isValid(),"Invalid world tag used for world recap !");

        if ( _worldTag.isValid()
            && m_state != STATE_DISPLAYING_BACKGROUND
            && m_state != STATE_DISPLAYED
            && m_state != STATE_DISPLAYING_LEVELS )
        {
            m_currentWorldTag = _worldTag;

            // Get total number of levels to display
            ITF_VECTOR<StringID> levelTags;
            RAY_GAMEMANAGER->getLevelTagsFromWorld( _worldTag, levelTags );
            m_currentNbLevels = levelTags.size();

            if ( m_currentNbLevels > 0 )
            {
                for ( u32 i = 0 ; i < m_currentNbLevels ; i++ )
                {
                    Actor * levelRecapAct = m_levelRecaps[i].m_recapActorRef.getActor();
                    if ( !levelRecapAct )
                        continue;

                    // Is this an appropriate actor ?
                    if ( m_levelRecaps[i].m_pRecapComp )
                    {
                        // enable it
                        levelRecapAct->enable();

                        // Set pos
                        levelRecapAct->setPos(getElemPosAt(i, m_currentNbLevels));

                        // Set level tag to this actor so that it can set its visual correctly
                        m_levelRecaps[i].m_pRecapComp->setLevelTag(levelTags[i]);
                    }
                }


                m_state = STATE_DISPLAYING_BACKGROUND;
            }
        }
    }

    void Ray_WM_WorldRecapComponent::hideRecap()
    {
        if ( m_currentNbLevels > 0
            && m_state != STATE_HIDDEN
            && m_state != STATE_HIDING_LEVELS
            && m_state != STATE_WAITING_LEVELS_HIDDEN
            && m_state != STATE_HIDING_BACKGROUND )
            m_state = STATE_HIDING_LEVELS;
    }

    void Ray_WM_WorldRecapComponent::deleteLevelRecaps()
    {
        u32 size = m_levelRecaps.size();

        for ( u32 i = 0 ; i < size ; i++ )
        {
            Actor * actor = m_levelRecaps[i].m_recapActorRef.getActor();
            if ( actor )
                actor->requestDestruction();
        }

        m_levelRecaps.clear();
    }

    Vec3d Ray_WM_WorldRecapComponent::getElemPosAt(u32 _index, u32 _total)
    {
        f32 screenSpace = m_elemSpace * m_screenScale.m_x;
        float xPos = screenSpace * _index;
        Vec3d pos = m_actor->getPos();
        pos.m_x = pos.m_x - ((_total-1) * screenSpace * 0.5f) + xPos;
        pos.m_z = 4.0f;

        return pos;
    }
};
