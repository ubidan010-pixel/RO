#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAYWMLEVELSPOTCOMPONENT_H_
#include "rayman/gameplay/Components/WorldMap/Ray_WM_LevelSpotComponent.h"
#endif //_ITF_RAYWMLEVELSPOTCOMPONENT_H_

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

#ifndef _ITF_UITEXTMANAGER_H_
#include "engine/actors/managers/UITextManager.h"
#endif //_ITF_UITEXTMAN

#ifndef _ITF_VIEW_H_
#include "engine/display/View.h"
#endif //_ITF_VIEW_H_

#ifndef _ITF_RAY_GAMESCREEN_WORLDMAPLOADING_H_
#include "rayman/gameplay/GameScreens/Ray_GameScreen_WorldMapLoading.h"
#endif //_ITF_RAY_GAMESCREEN_WORLDMAPLOADING_H_
namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(Ray_WM_LevelSpotComponent_Template)

    BEGIN_SERIALIZATION_CHILD(Ray_WM_LevelSpotComponent_Template)
        SERIALIZE_MEMBER("textCageID",m_textCageID);
        SERIALIZE_MEMBER("textCoinID",m_textCoinID);
        SERIALIZE_MEMBER("textLumID",m_textLumID);
        SERIALIZE_MEMBER("textTimeID",m_textTimeID);
		SERIALIZE_MEMBER("textRelicID",m_textRelicID);
    END_SERIALIZATION()

    //-------------------------------------------------------------------------------------

    IMPLEMENT_OBJECT_RTTI(Ray_WM_LevelSpotComponent)

    BEGIN_SERIALIZATION_CHILD(Ray_WM_LevelSpotComponent)
    END_SERIALIZATION()

    void Ray_WM_LevelSpotComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        Super::onActorLoaded(_hotReload);

        // Define state
        setState(RAY_GAMEMANAGER->getWMSpotState(m_tag));
    }

    void Ray_WM_LevelSpotComponent::updateState()
    {
        // Define state depending on conditions
        ESpot_State defaultState = RAY_GAMEMANAGER->getWMSpotState(m_tag);

        if ( defaultState == SPOT_STATE_CLOSED )
        {
            changeState(RAY_GAMEMANAGER->getDefaultState(m_tag));
        }
        else if ( defaultState == SPOT_STATE_CANNOT_ENTER )
        {
            if ( canEnter() )
            {
                changeState(SPOT_STATE_NEW);
            }
        }
        else
        {
            changeState(defaultState);
        }
    }

    void Ray_WM_LevelSpotComponent::onSceneActive()
    {
        const StringID type = RAY_GAMEMANAGER->getMapConfigType(m_tag);
        const Ray_GameManagerConfig_Template::MissionConfig *config = RAY_GAMEMANAGER->findMissionConfigByType(type, btrue);
        if ( config->m_medalSlots.size() == 3 )
        {
            m_displayLumAttack = btrue;
        }
        else if ( config->m_medalSlots.size() == 6 )
        {
            m_displayLumAttack = btrue;
            m_displayTimeAttack = btrue;
        }

		if(RAY_GAMEMANAGER->getMaxRelics(m_tag))
		{
			m_displayRelicCount = btrue;
		}

        updateState();

        // NB[Laurent]: our parent class needs to know about our state in its
        // onSceneActive to properly unroll footpaths
        Super::onSceneActive();
    }

    void Ray_WM_LevelSpotComponent::Update( f32 _dt )
    {
        Super::Update(_dt);
    }

    void Ray_WM_LevelSpotComponent::Draw()
    {
        Super::Draw();

    }

    void Ray_WM_LevelSpotComponent::onAction()
    {
        switch ( getState() )
        {
        case SPOT_STATE_CLOSED:
            ITF_ASSERT_MSG(0,"You shouldn't be able to go here !");
            break;
        case SPOT_STATE_NEW:
            {
                // We don't want the visual to change its state immediately, but only when we go back to worldmap.
                // To do so, we just change its state in the persistent game data and then the actor will update its state
                // automatically in onSceneActive()
                saveState(SPOT_STATE_OPEN);
            }
        case SPOT_STATE_OPEN:
        case SPOT_STATE_COMPLETED:
            {
                RAY_GAMEMANAGER->setWMSpotHasWarning(getTag(),bfalse);
                loadMap();
            }
            break;
        case SPOT_STATE_CANNOT_ENTER:
            // display message to tell the player why he can't enter in this level
            break;
        default:
            ITF_ASSERT(0); // Should never be here
        }
    }

    void Ray_WM_LevelSpotComponent::onConnection()
    {
        Super::onConnection();

        ESpot_State currentState = getState();
        if (currentState != SPOT_STATE_OPEN && currentState != SPOT_STATE_COMPLETED)
        {
            // Check conditions
            if (canEnter())
            {
                changeState(SPOT_STATE_NEW);
            }
            else
            {
                changeState(SPOT_STATE_CANNOT_ENTER);
            }
        }
    }

    void Ray_WM_LevelSpotComponent::onEnter(bbool _changeMusic /*= bfalse*/)
    {
        Super::onEnter();

        i32 electoonsRequired = Max(0,(i32)RAY_GAMEMANAGER->getMinElectoons(getTag()) - (i32)RAY_GAMEMANAGER->getElectoonsCount());

        String var;
        var.setTextFormat("%d",electoonsRequired);
        UI_TEXTMANAGER->addVariable(ITF_GET_STRINGID_CRC(electoonsRequired,2669374188),var);

        // Save current position on level map
        ObjectPath path;
        SceneObjectPathUtils::getAbsolutePathFromObject(m_actor, path);
        RAY_GAMEMANAGER->setWMCurrentLevel(path, m_tag);
        RAY_GAMEMANAGER->startActivity(RAY_GAMEMANAGER->getWMCurrentWorldTag());
        // Play worldmap music
        if ( _changeMusic )
        {
            const StringID& levelMusic = RAY_GAMEMANAGER->getMapConfigMusic(RAY_GAMEMANAGER->getWMCurrentWorldTag());
            if ( levelMusic.isValid() )
            {
                MUSICMANAGER->play(METRONOME_TYPE_GAMEPLAY,levelMusic,0.5f, RAY_GAMEMANAGER->getMusicVolume(RAY_GAMEMANAGER->getWMCurrentWorldTag()));
            }

        }
    }

    void Ray_WM_LevelSpotComponent::onExit()
    {
        Super::onExit();
    }

    void Ray_WM_LevelSpotComponent::loadMap()
    {
        const Path& path = RAY_GAMEMANAGER->getLevelPath(m_tag);

        if ( !path.isEmpty() )
        {
            //GAMEMANAGER->loadGameplayMap(path, btrue, bfalse);
            RAY_GAMEMANAGER->loadMapThroughLoadingScreen(m_tag);
        }
        onExit();
    }

    bbool Ray_WM_LevelSpotComponent::canEnter()
    {
        return RAY_GAMEMANAGER->getMinElectoons(m_tag) <= RAY_GAMEMANAGER->getElectoonsCount()
            && RAY_GAMEMANAGER->getMinTeeth(m_tag) <= RAY_GAMEMANAGER->getTotalTakenToothCount();
    }

    void Ray_WM_LevelSpotComponent::writeInfo()
    {
        if ( m_textBoxesComponent == NULL )
            return;

        Super::writeInfo();

        String8 valueString = String8::emptyString;

        if ( getState() != SPOT_STATE_CLOSED
            && getState() != SPOT_STATE_NEW )
        {
            // lum Counter
            if ( m_displayLumAttack )
            {
                valueString.setTextFormat("%d",RAY_GAMEMANAGER->getLevelBestLumAttack(getTag()));
            }

            m_textBoxesComponent->setText(getTemplate()->getTextLumID(),valueString);

           // relic Counter
			String8 relicCountValueString = String8::emptyString;
            if ( m_displayRelicCount )
            {
                relicCountValueString.setTextFormat("%d/%d",RAY_GAMEMANAGER->getTakenRelicCount(getTag()),RAY_GAMEMANAGER->getMaxRelics(getTag()));
            }

			m_textBoxesComponent->setText(getTemplate()->getTextRelicID(),relicCountValueString);

            // time Counter
            if ( m_displayTimeAttack )
            {
                u32 bestTimeAttack = RAY_GAMEMANAGER->getLevelBestTime(getTag());
                if (bestTimeAttack==U32_INVALID)
                {
                    valueString = "--:--";
                }
                else
                {
                    u32 seconds = bestTimeAttack%60;
                    u32 minutes = bestTimeAttack/60;
                    if (minutes>99)
                    {
                        valueString = "99:99";
                    }
                    else
                    {
                        valueString.setTextFormat("%02d:%02d", minutes, seconds);
                    }
                }
            }
            else
            {
                valueString = "";
            }

            m_textBoxesComponent->setText(getTemplate()->getTextTimeID(),valueString);
        }
        else
        {
            m_textBoxesComponent->setText(getTemplate()->getTextLumID(),valueString);
            m_textBoxesComponent->setText(getTemplate()->getTextTimeID(),valueString);
			m_textBoxesComponent->setText(getTemplate()->getTextRelicID(),valueString);
        }
    }

};
