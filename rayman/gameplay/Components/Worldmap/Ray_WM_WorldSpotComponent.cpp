#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAYWMWORLDSPOTCOMPONENT_H_
#include "rayman/gameplay/Components/WorldMap/Ray_WM_WorldSpotComponent.h"
#endif //_ITF_RAYWMWORLDSPOTCOMPONENT_H_

#ifndef _ITF_RAY_GAMEMANAGER_H_
#include "rayman/gameplay/Ray_GameManager.h"
#endif //_ITF_RAY_GAMEMANAGER_H_

#ifndef _ITF_AIUTILS_H_
#include "gameplay/AI/Utils/AIUtils.h"
#endif //_ITF_AIUTILS_H_

#ifndef _ITF_MUSICMANAGER_H_
#include "engine/gameplay/musicmanager.h"
#endif //_ITF_MUSICMANAGER_H_

#ifndef _ITF_OBJECTPATH_H_
#include "core/ObjectPath.h"
#endif //_ITF_OBJECTPATH_H_

#ifndef _ITF_SCENEOBJECTPATH_H_
#include "engine/scene/SceneObjectPath.h"
#endif //_ITF_SCENEOBJECTPATH_H_

#ifndef _ITF_LINKCOMPONENT_H_
#include "gameplay/Components/Misc/LinkComponent.h"
#endif //_ITF_LINKCOMPONENT_H_

namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(Ray_WM_WorldSpotComponent_Template)

    BEGIN_SERIALIZATION_CHILD(Ray_WM_WorldSpotComponent_Template)
    END_SERIALIZATION()

    //-------------------------------------------------------------------------------------

    IMPLEMENT_OBJECT_RTTI(Ray_WM_WorldSpotComponent)

    BEGIN_SERIALIZATION_CHILD(Ray_WM_WorldSpotComponent)
    END_SERIALIZATION()

    void Ray_WM_WorldSpotComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        Super::onActorLoaded(_hotReload);

        // Define state
        setState(RAY_GAMEMANAGER->getWMSpotState(m_tag));
    }

    void Ray_WM_WorldSpotComponent::onSceneActive()
    {
        EventGeneric eventGeneric;
        eventGeneric.setId(ITF_GET_STRINGID_CRC(Close,3883599349));

        LinkComponent * pLinkComponent = m_actor->GetComponent<LinkComponent>();
        const LinkComponent::ChildrenList& children  = pLinkComponent->getChildren();
        for (u32 i=0; i<children.size(); ++i)
        {
            Actor *target = (Actor*)SceneObjectPathUtils::getObjectFromRelativePath(m_actor, children[i].getPath());
            if (target)
            {
                if( target->getObjectType()==BaseObject::eActor)
                {
                    target->onEvent(&eventGeneric);
                }

                bool titleType = bfalse;
                if (children[i].getTagValue(ITF_GET_STRINGID_CRC(title,2090710162), titleType) && titleType)
                {
                    UITextBoxesComponent * textBoxesComponent = target->GetComponent<UITextBoxesComponent>();
                    if ( textBoxesComponent != NULL )
                    {
                        m_titleRef = target->getRef();
                        textBoxesComponent->setLineId(getTemplate()->getTextNameID(),RAY_GAMEMANAGER->getTitleId(m_tag));
                    }
                }
            }
        }

        ESpot_State defaultState = RAY_GAMEMANAGER->getWMSpotState(m_tag);

        if ( defaultState == SPOT_STATE_CLOSED )
        {
            changeState(RAY_GAMEMANAGER->getDefaultState(m_tag));
        }
        else
        {
            changeState(defaultState);
        }

        // NB[Laurent]: our parent class needs to know about our state in its
        // onSceneActive to properly unroll footpaths
        Super::onSceneActive();
    }

    void Ray_WM_WorldSpotComponent::Update( f32 _dt )
    {
        Super::Update(_dt);
    }

    void Ray_WM_WorldSpotComponent::Draw()
    {
        Super::Draw();

    }

    void Ray_WM_WorldSpotComponent::onEnter(bbool _changeMusic /*= bfalse*/)
    {
        Super::onEnter();

        // Remove saved level spot as we're in world selection
        RAY_GAMEMANAGER->setWMCurrentLevel(ObjectPath(), StringID::Invalid);

        // Save current position on world map
        ObjectPath path;
        SceneObjectPathUtils::getAbsolutePathFromObject(m_actor, path);
        RAY_GAMEMANAGER->setWMCurrentWorld(path, getTag());

        // Special case for land of the dead as it's a hybrid world / level
        if ( getTag() == s_DE_A1 )
        {
            RAY_GAMEMANAGER->setWMCurrentLevel(path, m_tag);
        }
        RAY_GAMEMANAGER->resumeActivity();
        // Play worldmap music
        if ( _changeMusic )
        {
        const StringID worldMap = ITF_GET_STRINGID_CRC(WorldMap,1734041925);
            const StringID& music = RAY_GAMEMANAGER->getMapConfigMusic(worldMap);
            if ( music.isValid() )
            {
                MUSICMANAGER->play(METRONOME_TYPE_GAMEPLAY,music,0.5f, RAY_GAMEMANAGER->getMusicVolume(worldMap));
            }
        }

        // Title appear
        if ( m_titleRef.isValid() )
        {
            AnimatedComponent * pTitleAnimatedComp = m_titleRef.getActor()->GetComponent<AnimatedComponent>();
            if ( pTitleAnimatedComp )
            {
                pTitleAnimatedComp->setAnim(ITF_GET_STRINGID_CRC(opened,143470637));
            }
        }

        if ( getState() != SPOT_STATE_NEW )
        {
            // Display world recap
            Ray_EventWMDisplayWorldRecap evDisplayRecap;
            evDisplayRecap.setDisplay(btrue);
            evDisplayRecap.setWorldTag(getTag());
            EVENTMANAGER->broadcastEvent(&evDisplayRecap);
        }
    }

    void Ray_WM_WorldSpotComponent::onExit()
    {
        Super::onExit();

        // Title disappear
        if ( m_titleRef.isValid() )
        {
            AnimatedComponent * pTitleAnimatedComp = m_titleRef.getActor()->GetComponent<AnimatedComponent>();
            if ( pTitleAnimatedComp )
            {
                pTitleAnimatedComp->setAnim(ITF_GET_STRINGID_CRC(closed,2010016917));
            }
        }

        // Hide world recap
        Ray_EventWMDisplayWorldRecap evDisplayRecap;
        evDisplayRecap.setDisplay(bfalse);
        EVENTMANAGER->broadcastEvent(&evDisplayRecap);
    }

    void Ray_WM_WorldSpotComponent::onAction()
    {
        switch ( getState() )
        {
        case SPOT_STATE_CLOSED:
        case SPOT_STATE_CANNOT_ENTER:
            ITF_ASSERT_MSG(0,"You shouldn't be able to go here !");
            break;
        case SPOT_STATE_NEW:
            {
                changeState(SPOT_STATE_OPEN);
            }
        case SPOT_STATE_OPEN:
            {
                if ( getTag() == s_DE_A1 )
                {
                    RAY_GAMEMANAGER->setWMSpotHasWarning(getTag(),bfalse);
                }
            }
        case SPOT_STATE_COMPLETED:
            {
                if ( getTag() == s_DE_A1 )
                {
                    loadMap();
                }
            }
            break;
        default:
            ITF_ASSERT(0); // Should never be here
        }
    }

    void Ray_WM_WorldSpotComponent::loadMap()
    {
        const Path& path = RAY_GAMEMANAGER->getLevelPath(m_tag);

        if ( !path.isEmpty() )
        {
            //GAMEMANAGER->loadGameplayMap(path, btrue, bfalse);
            RAY_GAMEMANAGER->loadMapThroughLoadingScreen(m_tag);
        }
        onExit();
    }

    void Ray_WM_WorldSpotComponent::onConnection()
    {
        Super::onConnection();

        ESpot_State currentState = getState();
        if (RAY_GAMEMANAGER->getDefaultState(m_tag) == SPOT_STATE_CLOSED
            && currentState == SPOT_STATE_CLOSED
            && !RAY_GAMEMANAGER->isStillUnderLockCondition(getTag()))
        {
            changeState(SPOT_STATE_NEW);
        }
    }
};
