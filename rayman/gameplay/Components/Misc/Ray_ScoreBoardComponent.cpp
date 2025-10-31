#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAYSCOREBOARDCOMPONENT_H_
#include "rayman/gameplay/Components/Misc/Ray_ScoreBoardComponent.h"
#endif //_ITF_RAYSCOREBOARDCOMPONENT_H_

#ifndef _ITF_RAY_REWARDMANAGER_H_
#include "rayman/gameplay/Ray_RewardManager.h"
#endif //_ITF_RAY_REWARDMANAGER_H_

#ifndef _ITF_RAY_GAMEMANAGER_H_
#include "rayman/gameplay/Ray_GameManager.h"
#endif //_ITF_RAY_GAMEMANAGER_H_

#ifndef _ITF_RAY_GAMESCREEN_MAINMENU_H_
#include "rayman/gameplay/GameScreens/Ray_GameScreen_MainMenu.h"
#endif //_ITF_RAY_GAMESCREEN_MAINMENU_H_

#ifndef _ITF_SCENE_H_
#include "engine/scene/scene.h"
#endif //_ITF_SCENE_H_

namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(Ray_ScoreBoardComponent_Template)

    BEGIN_SERIALIZATION(Ray_ScoreBoardComponent_Template)
        SERIALIZE_MEMBER("baseName",m_baseName);
    END_SERIALIZATION()

    //-------------------------------------------------------------------------------------

    IMPLEMENT_OBJECT_RTTI(Ray_ScoreBoardComponent)

    BEGIN_SERIALIZATION_CHILD(Ray_ScoreBoardComponent)
    END_SERIALIZATION()

    Ray_ScoreBoardComponent::Ray_ScoreBoardComponent()
        : m_timer(0.0f)
    {
    }

    Ray_ScoreBoardComponent::~Ray_ScoreBoardComponent()
    {
        EVENTMANAGER->unregisterEvent(ITF_GET_STRINGID_CRC(Ray_EventHUDDisplayElem,2571036057), this);
        EVENTMANAGER->unregisterEvent(ITF_GET_STRINGID_CRC(Ray_EventHUDSetText,1340493890), this);
    }


    void Ray_ScoreBoardComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        Super::onActorLoaded(_hotReload);

        // Register events
        EVENTMANAGER_REGISTER_EVENT_LISTENER(ITF_GET_STRINGID_CRC(Ray_EventHUDDisplayElem,2571036057),this);
        EVENTMANAGER_REGISTER_EVENT_LISTENER(ITF_GET_STRINGID_CRC(Ray_EventHUDSetText,1340493890),this);
    }

    void Ray_ScoreBoardComponent::onEvent(Event* _event)
    {
        Super::onEvent(_event);

        if (Ray_EventHUDDisplayElem* displayElem = _event->DynamicCast<Ray_EventHUDDisplayElem>(ITF_GET_STRINGID_CRC(Ray_EventHUDDisplayElem,2571036057)))
        {
            // Display them sorted
            ActorBindHandler&   abh = m_actor->getChildrenBindHandler();
            ObjectRefList& children = abh.getChildren();
            if ( displayElem->getIndex() < children.size() )
            {
                ObjectRef& objref = children[displayElem->getIndex()];
                if ( objref.isValid() )
                {
                    Actor * actor	= static_cast<Actor*>(GETOBJECT(objref));
                    AIUtils::ActorBindIterator iterator(actor);
                    while ( Actor* child = iterator.getNextChild() )
                    {
                        displayElem->getShow() ? child->enable() : child->disable();
                    }
                    displayElem->getShow() ? actor->enable() : actor->disable();
                }
            }
        }
        else if ( Ray_EventHUDSetText* setText = _event->DynamicCast<Ray_EventHUDSetText>(ITF_GET_STRINGID_CRC(Ray_EventHUDSetText,1340493890)) )
        {
            const StringID& baseName = getTemplate()->getBaseName();
            UI_MENUMANAGER->changeMenuItemFriendly(baseName, setText->getFriendlyName(), setText->getText());
        }
    }

    bbool sortPlayers(std::pair<u32,u32> _p1, std::pair<u32,u32> _p2) { return _p1.second > _p2.second; }

    void Ray_ScoreBoardComponent::onBecomeActive()
    {
        // Disable All child
        World* pWorld = m_actor->getScene()->getWorld();
        if(pWorld && pWorld != CURRENTWORLD)
        {
            AIUtils::ActorBindIterator iterator(m_actor);
            while ( Actor* child = iterator.getNextChild() )
            {
                AIUtils::ActorBindIterator iterator(child);
                while ( Actor* child_child = iterator.getNextChild() )
                {
                    child_child->disable();
                }
                child->disable();
            }
        }

        //const StringID& baseName = getTemplate()->getBaseName();

        //if ( baseName != StringID::Invalid )
        //{
        //    RewardManager::GameSession * gameSession = REWARD_MANAGER->Session_GetPrevious();

        //    u32     totalCounter = 0;
        //    String8 fieldName, valueString;

        //    // Sort players by lums ranking
        //    ITF_VECTOR< std::pair<u32,u32> > sortedPlayers;
        //    for ( u32 i = 0 ; i < RewardManager::PlayersMax ; ++i )
        //    {
        //        sortedPlayers.push_back(std::pair<u32,u32>(i,gameSession->m_actions[i].m_tags[Ray_RewardManager::Action_GetLums]));
        //    }

        //    std::sort(sortedPlayers.begin(), sortedPlayers.end(), sortPlayers);

        //    // Fills in lums counter
        //    for ( u32 i = 0 ; i < RewardManager::PlayersMax ; ++i )
        //    {
        //        fieldName.setTextFormat("Lums%d",i+1);

        //        if ( gameSession->m_actions[sortedPlayers[i].first].m_actif )
        //        {
        //            valueString.setTextFormat("x %d",sortedPlayers[i].second);
        //            totalCounter += sortedPlayers[i].second;
        //        }
        //        else
        //        {
        //            valueString.setTextFormat("",sortedPlayers[i].second);
        //        }
        //         UI_MENUMANAGER->changeMenuItemFriendly(baseName, fieldName, valueString);
        //    }
        //    // Total Lums counter
        //    fieldName.setText("LumCounter");
        //    valueString.setTextFormat("x %d",totalCounter);
        //    UI_MENUMANAGER->changeMenuItemFriendly(baseName, fieldName, valueString);

        //    // Total coins counter
        //    fieldName.setText("CoinCounter");
        //    valueString.setTextFormat("x %d",RAY_GAMEMANAGER->getTakenSkullCoinCount());
        //    UI_MENUMANAGER->changeMenuItemFriendly(baseName, fieldName, valueString);

        //    // Total time counter
        //    fieldName.setText("TimeCounter");
        //    valueString.setTextFormat("%02d:%02d",gameSession->m_LengthInSec / 60, gameSession->m_LengthInSec % 60);
        //    UI_MENUMANAGER->changeMenuItemFriendly(baseName, fieldName, valueString);

        //    // Total cage counter
        //    fieldName.setText("CageCounter");
        //    valueString.setTextFormat("x %d",RAY_GAMEMANAGER->getBrokenCageCount());
        //    UI_MENUMANAGER->changeMenuItemFriendly(baseName, fieldName, valueString);

        //}
    }
};
