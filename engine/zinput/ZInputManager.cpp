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

#include "engine/zinput/XBoxSeries/ZPad_XBoxSeries.h"

#ifndef _ITF_ZPAD_BASE_H_
#include "engine/zinput/ZPad_Base.h"
#endif //_ITF_ZPAD_BASE_H_

// ---------------------------------------------------------------

namespace ITF
{

    ZInputManager::ZInputManager()
        : m_suppressReceive(bfalse)
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
                        if (m_suppressReceive)
                            continue;
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
#endif //ITF_NINTENDO
    }

    void ZInputManager::addXBoxSeries_device(u32 maxPAD)
    {
#ifdef ITF_XBOX_SERIES
        FOR_MAXPAD m_devices.push_back(new (MemoryId::mId_System) ZPad_XBoxSeries(pad));
#endif //ITF_XBOX_SERIES

    }

    void ZInputManager::SetRemap(u32 _playerIndex, u32 _logicalControl, u32 _physicalControl)
    {
        for (u32 i = 0; i < m_devices.size(); ++i)
        {
            if (m_devices[i]->GetId() == _playerIndex)
            {
                m_devices[i]->SetRemap(_logicalControl, _physicalControl);
            }
        }
    }

    void ZInputManager::ResetRemapping(u32 _playerIndex)
    {
        for (u32 i = 0; i < m_devices.size(); ++i)
        {
            if (m_devices[i]->GetId() == _playerIndex)
            {
                m_devices[i]->ResetRemapping();
            }
        }
    }

    u32 ZInputManager::GetStandardControlFromAction(EGameAction _action)
    {
        switch (_action)
        {
        case Action_Up:    return ZPad_Base::STICK_L_UP;
        case Action_Down:  return ZPad_Base::STICK_L_DOWN;
        case Action_Left:  return ZPad_Base::STICK_L_LEFT;
        case Action_Right: return ZPad_Base::STICK_L_RIGHT;
        case Action_Run:   return ZPad_Base::TRIGGER_RIGHT;
        case Action_Jump:  return ZPad_Base::BUTTON_FACE_SOUTH;
        case Action_Hit:   return ZPad_Base::BUTTON_FACE_WEST;
        case Action_Back:  return ZPad_Base::BUTTON_FACE_EAST;
        default: return U32_INVALID;
        }
    }

    bbool ZInputManager::IsActionRemapAllowed(EGameAction _action, u32 _physicalControl) const
    {
        const auto isDirectionalControl = [](u32 control) {
            switch (control)
            {
            case ZPad_Base::DPAD_UP:
            case ZPad_Base::DPAD_DOWN:
            case ZPad_Base::DPAD_LEFT:
            case ZPad_Base::DPAD_RIGHT:
            case ZPad_Base::STICK_L_UP:
            case ZPad_Base::STICK_L_DOWN:
            case ZPad_Base::STICK_L_LEFT:
            case ZPad_Base::STICK_L_RIGHT:
            case ZPad_Base::STICK_R_UP:
            case ZPad_Base::STICK_R_DOWN:
            case ZPad_Base::STICK_R_LEFT:
            case ZPad_Base::STICK_R_RIGHT:
                return btrue;
            default:
                return bfalse;
            }
        };

        const bbool isDirectionalAction = (_action == Action_Up || _action == Action_Down || _action == Action_Left || _action == Action_Right);
        if (isDirectionalAction)
        {
            return isDirectionalControl(_physicalControl);
        }

        return !isDirectionalControl(_physicalControl);
    }

    void ZInputManager::SetActionRemap(u32 _playerIndex, EGameAction _action, u32 _physicalControl)
    {
        const auto remapDirectionalGroup = [&](u32 upControl, u32 downControl, u32 leftControl, u32 rightControl)
        {
            const EGameAction directionalActions[] = {
                Action_Up,
                Action_Down,
                Action_Left,
                Action_Right,
            };

            const u32 physicalControls[] = {
                upControl,
                downControl,
                leftControl,
                rightControl,
            };

            for (u32 i = 0; i < sizeof(directionalActions) / sizeof(directionalActions[0]); ++i)
            {
                u32 logicalControl = GetStandardControlFromAction(directionalActions[i]);
                if (logicalControl != U32_INVALID)
                {
                    LOG("[ZInputManager] SetActionRemap: Player %d, Action %d -> Physical %d (Logical %d)\n", _playerIndex, directionalActions[i], physicalControls[i], logicalControl);
                    SetRemap(_playerIndex, logicalControl, physicalControls[i]);
                }
            }
        };

        const bbool isDirectionalAction = (_action == Action_Up || _action == Action_Down || _action == Action_Left || _action == Action_Right);
        if (isDirectionalAction)
        {
            switch (_physicalControl)
            {
            case ZPad_Base::DPAD_UP:
            case ZPad_Base::DPAD_DOWN:
            case ZPad_Base::DPAD_LEFT:
            case ZPad_Base::DPAD_RIGHT:
                remapDirectionalGroup(ZPad_Base::DPAD_UP, ZPad_Base::DPAD_DOWN, ZPad_Base::DPAD_LEFT, ZPad_Base::DPAD_RIGHT);
                return;

            case ZPad_Base::STICK_L_UP:
            case ZPad_Base::STICK_L_DOWN:
            case ZPad_Base::STICK_L_LEFT:
            case ZPad_Base::STICK_L_RIGHT:
                remapDirectionalGroup(ZPad_Base::STICK_L_UP, ZPad_Base::STICK_L_DOWN, ZPad_Base::STICK_L_LEFT, ZPad_Base::STICK_L_RIGHT);
                return;

            case ZPad_Base::STICK_R_UP:
            case ZPad_Base::STICK_R_DOWN:
            case ZPad_Base::STICK_R_LEFT:
            case ZPad_Base::STICK_R_RIGHT:
                remapDirectionalGroup(ZPad_Base::STICK_R_UP, ZPad_Base::STICK_R_DOWN, ZPad_Base::STICK_R_LEFT, ZPad_Base::STICK_R_RIGHT);
                return;

            default:
                LOG("[ZInputManager] SetActionRemap rejected: Action %d cannot use Physical %d\n", _action, _physicalControl);
                return;
            }
        }
        else if (!IsActionRemapAllowed(_action, _physicalControl))
        {
            LOG("[ZInputManager] SetActionRemap rejected: Action %d cannot use Physical %d\n", _action, _physicalControl);
            return;
        }

        u32 logicalControl = GetStandardControlFromAction(_action);

        if (logicalControl != U32_INVALID)
        {
            LOG("[ZInputManager] SetActionRemap: Player %d, Action %d -> Physical %d (Logical %d)\n", _playerIndex, _action, _physicalControl, logicalControl);
            SetRemap(_playerIndex, logicalControl, _physicalControl);
        }
        else
        {
            LOG("[ZInputManager] SetActionRemap: Invalid Action %d for Player %d\n", _action, _playerIndex);
        }
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

    u32 ZInputManager::GetPhysicalFromAction(u32 _playerIndex, EGameAction _action)
    {
        u32 logicalControl = GetStandardControlFromAction(_action);
        if (logicalControl == U32_INVALID) return U32_INVALID;

        for (u32 i = 0; i < m_devices.size(); ++i)
        {
            if (m_devices[i] && m_devices[i]->GetId() == _playerIndex)
            {
                return m_devices[i]->GetRemap(logicalControl);
            }
        }
        return logicalControl; 
    }

    u32 ZInputManager::GetFirstActiveControl(u32 _playerIndex)
    {
        for (u32 i = 0; i < m_devices.size(); ++i)
        {
            if (m_devices[i] && m_devices[i]->GetId() == _playerIndex)
            {
                u32 control = m_devices[i]->GetFirstActiveControl();
                if (control != U32_INVALID)
                {
                    return control;
                }
            }
        }
        return U32_INVALID;
    }

    IMPLEMENT_OBJECT_RTTI(ZInputManager_Template)
        BEGIN_SERIALIZATION(ZInputManager_Template)
        SERIALIZE_MEMBER("name", m_name);
    SERIALIZE_MEMBER("config", m_config);
    SERIALIZE_MEMBER("category", m_category);
    SERIALIZE_CONTAINER_OBJECT("actions",m_actionMap);
    END_SERIALIZATION()

} //namespace ITF
