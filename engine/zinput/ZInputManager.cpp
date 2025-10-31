#include "precompiled_engine.h"

#ifndef _ITF_ZINPUT_MANAGER_H
#include "engine/zinput/ZInputManager.h"
#endif //_ITF_ZINPUT_MANAGER_H

#ifndef _ITF_INPUTLISTENER_H_
#include "engine/zinput/ZInputListener.h"
#endif //_ITF_INPUTLISTENER_H_

#ifndef _ITF_INPUTDEVICE_H_
#include "engine/zinput/ZInputDevice.h"
#endif //_ITF_INPUTDEVICE_H_

// ---------------------------------------------------------------
// Devices includes
#include "engine/zinput/PadX360/ZPadX360_common.h"

#ifndef _ITF_INPUTDEVICE_CLASSIC_WII_H_
#include "engine/zinput/wii/ZClassic_wii.h"
#endif //_ITF_INPUTDEVICE_CLASSIC_WII_H_

#ifndef _ITF_INPUTDEVICE_WIIMOTE_WII_H_
#include "engine/zinput/wii/Zwiimote_wii.h"
#endif //_ITF_INPUTDEVICE_WIIMOTE_WII_H_

#ifndef _ITF_INPUTDEVICE_NUNCHUCK_WII_H_
#include "engine/zinput/wii/Znunchuck_wii.h"
#endif //_ITF_INPUTDEVICE_NUNCHUCK_WII_H_

#ifndef _ITF_INPUTDEVICE_PS3_H_
#include "engine/zinput/ps3/ZPad_ps3.h"
#endif //_ITF_INPUTDEVICE_PS3_H_

#include "engine/zinput/ps5/ZPad_ps5.h"

#include "engine/zinput/Nintendo/ZPad_Nintendo.h"

// ---------------------------------------------------------------

namespace ITF
{

    ZInputManager::ZInputManager()
    {
        for (u32 i = 0; i < ACTIONMAP_MAX_PLAYER; i++)
            m_playersConfig[i] = StringID::Invalid;

        m_listOfListeners.resize ( Cat_Count );    
    }

    ZInputManager::~ZInputManager()
    {
        for ( unsigned int i = 0; i < m_devices.size(); i++ )
        {
            SF_DEL(m_devices[i]);
        }
        m_devices.clear();
        m_templateClientHandler.freeUsedTemplates();
    }

    void ZInputManager::update()
    {
        if ( !m_actionMaps.size() )
            return;

        clean_removedListeners();
        update_devices();

        ActionMapsContainer::iterator itActionsEnd = m_actionMaps.end();
        for (ActionMapsContainer::iterator itAction = m_actionMaps.begin(); itAction != itActionsEnd; ++itAction)
        {
            ActionMapInternal* actionMapInternal = &(itAction->second);

            if(!actionMapInternal || !actionMapInternal->_actif)
                continue;

            process_actionDevices(actionMapInternal);
            update_actionMap(actionMapInternal);        
        }
    }

    /*static*/ void ZInputManager::adjustSelectAndBackAction(ZAction & _action)
    {
        if (SYSTEM_ADAPTER->isBackAndSelectButtonsInverted())
        {
            static const StringID ActionsToInvert[] =
            {
                ITF_GET_STRINGID_CRC(WM_ENTER,2267322818),
                ITF_GET_STRINGID_CRC(WM_BACK,4244387740),
                ITF_GET_STRINGID_CRC(MENU_VALID,1084313942),
                ITF_GET_STRINGID_CRC(MENU_ONPRESSED,185785632),
                ITF_GET_STRINGID_CRC(MENU_ONRELEASED,14213630),
                ITF_GET_STRINGID_CRC(MENU_BACK,2477582220)
            };

            static const struct InputPair
            {
                StringID initial;
                StringID converted;
            } InputInvert[] =
            {
                { ITF_GET_STRINGID_CRC(BUTTON_CROSS,2040208284),  ITF_GET_STRINGID_CRC(BUTTON_CIRCLE,1221459298) },
                { ITF_GET_STRINGID_CRC(BUTTON_CIRCLE,1221459298), ITF_GET_STRINGID_CRC(BUTTON_CROSS,2040208284)}
            };

            for(ux i = 0; i<sizeof(ActionsToInvert)/sizeof(ActionsToInvert[0]); ++i)
            {
                if (ActionsToInvert[i] == _action.m_action)
                {
                    typedef ITF_VECTOR<ZInput> VecInput;
                    VecInput & actInputs = _action.m_inputs;
                    for(VecInput::iterator it = actInputs.begin(), itEnd = actInputs.end();
                        it!=itEnd;
                        ++it)
                    {
                        for(ux j = 0; j<sizeof(InputInvert)/sizeof(InputInvert[0]); ++j)
                        {
                            if (InputInvert[j].initial == it->m_control)
                            {
                                it->m_control = InputInvert[j].converted;
                                break;
                            }
                        }
                    }
                }
            }
        }
    }

    void ZInputManager::load_configFile( const Path& path )
    {
        m_templateClientHandler.addUsedTemplate(path.getStringID());
        const ZInputManager_Template* pTemplate = TEMPLATEDATABASE->getTemplate<ZInputManager_Template>(&m_templateClientHandler, path);
        if (!pTemplate)
        {
            ITF_ASSERT_MSG(0, "Couldn't load input file: %s", path.getString8().cStr());
            return;
        }  
#ifdef ITF_WII
        MEM_M_PushExt(MEM_C_MEM1_ALLOC);
#endif

        STD_PAIR<StringID, ActionMapInternal> newInputFile;
        ActionMapInternal actionMapInternal ; 
        actionMapInternal._actif    = btrue;
        actionMapInternal._config   = pTemplate->getConfig();
        ITF_ASSERT(pTemplate->getCategory()<Cat_Count);
        actionMapInternal._category = (Category)pTemplate->getCategory();

        u32 actionMapCount = pTemplate->getActionMap().size();
        for(u32 i = 0; i < actionMapCount; i++)
        {
            ZAction action = pTemplate->getActionMap()[i];
            adjustSelectAndBackAction(action);
            actionMapInternal._actionMap.push_back(action);
            actionMapInternal._actionMap[i].Reset();
        }

        newInputFile.second = actionMapInternal;

        // Set the key with the actions map name
        newInputFile.first = pTemplate->getName();
        m_actionMaps.insert(newInputFile);
#ifdef ITF_WII
        MEM_M_PopExt();
#endif
    }

    void ZInputManager::update_actionMap( ActionMapInternal* _actionMapInternal )
    {
        ITF_ASSERT(_actionMapInternal && _actionMapInternal->_actif);

        // Listener iterator .. 
        Listeners::const_iterator  itListenerBegin  = m_listOfListeners[_actionMapInternal->_category].begin();
        Listeners::const_iterator  itListenerEnd    = m_listOfListeners[_actionMapInternal->_category].end();

        ActionMap& mapList = _actionMapInternal->_actionMap;
        u32 actionCount = mapList.size();
        for (u32 i = 0; i < actionCount; i++)
        {
            ZAction* action = & (mapList[i]); 

            for (u32 player = 0; player < ACTIONMAP_MAX_PLAYER; player++)
            {
                if (action->m_matchs[player] > 0 && action->m_matchs[player] == action->m_inputs.size())
                {
                    if ( !m_playersConfig[player].isValid() || m_playersConfig[player] == _actionMapInternal->_config )
                    {
                        Listeners::const_iterator it = itListenerBegin;
                        while ( it != itListenerEnd )
                        {
                            if((*it)->canReceive())
                                (*it)->Receive(player, action->m_axis[player], action->m_action);
                            ++ it;
                        }

                    }               
                }
            }

            // Reset action for next frame
            action->Reset();
        }

        clean_removedListeners();
    }

    void ZInputManager::update_devices()
    {
        for (u32 i = 0; i < m_devices.size(); ++i)
        {
            m_devices[i]->Update();
        }
    }

    void ZInputManager::process_actionDevices( ActionMapInternal* _actionMapInternal )
    {
        for (u32 i = 0; i < m_devices.size(); ++i)
        {
            m_devices[i]->ProcessActions( &_actionMapInternal->_actionMap );
        }
    }

#define FOR_MAXPAD   for (u32 pad=0; pad<maxPAD; pad++)    

    void ZInputManager::addX360pad_device(u32 maxPAD)
    {
#if defined (ITF_X360) || defined(ITF_WINDOWS)
        FOR_MAXPAD m_devices.push_back(newAlloc(mId_System, ZPad_x360(pad)));
#endif //ITF_X360 || ITF_WINDOWS
    }

    void ZInputManager::addWiimote_device(u32 maxPAD)
    {
#ifdef ITF_WII
        FOR_MAXPAD m_devices.push_back(newAlloc(mId_System, ZWiimote_wii(pad)));
#endif //ITF_WII
    }

    void ZInputManager::addWiinunchuck_device(u32 maxPAD)
    {
#ifdef ITF_WII
        FOR_MAXPAD m_devices.push_back(newAlloc(mId_System, ZNunchuck_wii(pad)));
#endif //ITF_WII
    }

    void ZInputManager::addWiiclassic_device(u32 maxPAD)
    {
#ifdef ITF_WII
        FOR_MAXPAD m_devices.push_back(newAlloc(mId_System, ZClassic_wii(pad)));
#endif //ITF_WII
    }

    void ZInputManager::addPS3pad_device(u32 maxPAD)
    {
#ifdef ITF_PS3
        FOR_MAXPAD m_devices.push_back(newAlloc(mId_System, ZPad_ps3(pad)));
#endif //ITF_PS3
    }

    void ZInputManager::addPS5pad_device(u32 maxPAD)
    {
#ifdef ITF_PS5
        FOR_MAXPAD m_devices.push_back(newAlloc(mId_System, ZPad_ps5(pad)));
#endif //ITF_PS5
    }

    void ZInputManager::addNintendopad_device(u32 maxPAD)
    {
#ifdef ITF_NINTENDO
        FOR_MAXPAD m_devices.push_back(new (MemoryId::mId_System) ZPad_Nintendo(pad));
#endif //ITF_PS5
    }

    void ZInputManager::RemoveListener( IInputListener* _listener )
    {
        if(_listener)
        {
            m_listenersRemoved.push_back(_listener);
            _listener->setCanReceive(bfalse);
        }
    }

    void ZInputManager::setActionMapsActive( const Category _category, bbool _actif )
    {
        ActionMapsContainer::iterator itAction  ;
        for (itAction = m_actionMaps.begin(); itAction !=  m_actionMaps.end(); ++itAction)
        {
            ActionMapInternal* actionMapInternal = &(itAction->second);

            if (actionMapInternal->_category == _category)
                actionMapInternal->_actif = _actif;
        }
    }

    void ZInputManager::ForceConfigForPlayer( u32 _player, const StringID & _config )
    {
        ITF_ASSERT(_player < ACTIONMAP_MAX_PLAYER);
        if(_player < ACTIONMAP_MAX_PLAYER)
            m_playersConfig[_player] = _config;
    }

    void ZInputManager::AddListener( IInputListener* _listener, const Category _category /*= Cat_GamePlay*/ )
    {
        m_listOfListeners[_category].push_back(_listener);
        _listener->setCanReceive(btrue);    
    }

    void ZInputManager::clean_removedListeners()
    {
        Listeners::iterator  ite  = m_listenersRemoved.begin();
        while(ite != m_listenersRemoved.end())
        {
            remove_fromList( (*ite) );
            ++ ite;
        }
        m_listenersRemoved.clear();
    }

    void ZInputManager::remove_fromList( IInputListener* _listener )
    {
        if(_listener)
        {
            for (u32 i = 0; i < Cat_Count; i++)
            {
                Listeners::iterator  itListenerBegin  = m_listOfListeners[i].begin();
                Listeners::iterator  itListenerEnd    = m_listOfListeners[i].end();

                Listeners::iterator it = std::find(itListenerBegin, itListenerEnd, _listener);
                if( it != itListenerEnd )
                {
                    m_listOfListeners[i].erase(it);
                }
            }
        }
    }

    IMPLEMENT_OBJECT_RTTI(ZInputManager_Template)
        BEGIN_SERIALIZATION(ZInputManager_Template)
        SERIALIZE_MEMBER("name", m_name);
    SERIALIZE_MEMBER("config", m_config);
    SERIALIZE_MEMBER("category", m_category);
    SERIALIZE_CONTAINER_OBJECT("actions",m_actionMap);
    END_SERIALIZATION()

} //namespace ITF
