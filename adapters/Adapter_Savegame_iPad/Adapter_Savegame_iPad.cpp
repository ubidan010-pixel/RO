#include "precompiled_Adapter_Savegame_iPad.h"

#ifndef _ITF_ADAPTER_SAVEGAME_IPAD_H__
#include "adapters/Adapter_Savegame_iPad/Adapter_Savegame_iPad.h"
#endif

#ifndef _ITF_ERRORHANDLER_H_
#include "core/error/ErrorHandler.h"
#endif //_ITF_ERRORHANDLER_H_

#ifndef _ITF_SYSTEMADAPTER_
#include "core/AdaptersInterfaces/SystemAdapter.h"
#endif //_ITF_SYSTEMADAPTER_

namespace ITF
{

    Adapter_Savegame_iPad::Adapter_Savegame_iPad() :
        m_mainState(State_Idle),
        m_contentEnumeration_PlayerIndex(0),
        m_frameSinceLastStateChange(0),
        m_IOOperationThread(0),
        m_wasEnumerated(bfalse),
        m_choseDevice_PlayerIndex(0)
        {
        }

    void Adapter_Savegame_iPad::init()
    {
		setDeviceChosenFlags(btrue, btrue);
    }

    void Adapter_Savegame_iPad::tests()
    {
    }

    void Adapter_Savegame_iPad::term()
    {
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void Adapter_Savegame_iPad::handle_MustChooseDevice()
    {
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    bbool Adapter_Savegame_iPad::getEnumeratedContent(u32 /*_playerIndex*/, u32 _index, String8 &_baseName, String &_displayName, bbool &_emptySave)
    {
        return Error_Ok;
    }
    ///////////////////////////////////////////////////////////////////////////////////////////
    bbool Adapter_Savegame_iPad::startContentEnumeration(u32 _player)
    {
        return btrue;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void Adapter_Savegame_iPad::handle_JustClosedDeviceSelectorGUI()
    {
    }

    ///////////////////////////////////////////////////////////////////////////////////////////

    Adapter_Savegame::ErrorCode Adapter_Savegame_iPad::startChooseDevice(u32 _playerIndex, u32 _minFreeSize, bbool _isSilentWhenChosenNone)
    {
        return Error_Ok;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void Adapter_Savegame_iPad::handle_AsynchronousContentEnumeration()
    {
    }

    Adapter_Savegame::ErrorCode Adapter_Savegame_iPad::startLoad(u32 _playerIndex, const String8 &_baseName)
    {
        return Error_Ok;
	}
	
	Adapter_Savegame::ErrorCode Adapter_Savegame_iPad::startSavegameEnumerationUpdate(u32 _player)
	{
		return Error_Ok;
	}

    ///////////////////////////////////////////////////////////////////////////////////////////
    Adapter_Savegame::ErrorCode Adapter_Savegame_iPad::startSave(u32 _playerIndex, const String8 &baseName, const String &displayName, const void *_data, u32 _dataSize, bbool)
    {
        return Error_Ok;
    }

    
    ///////////////////////////////////////////////////////////////////////////////////////////
    void Adapter_Savegame_iPad::handle_Save_CreateContent()
    {
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void Adapter_Savegame_iPad::handle_Load_WaitingBeforeIdle()
    {
    }
    ///////////////////////////////////////////////////////////////////////////////////////////
    String8 Adapter_Savegame_iPad::getSavegamePath(const String8 &_filename)
    {
        return String8("");
    }
    ///////////////////////////////////////////////////////////////////////////////////////////
    DWORD Adapter_Savegame_iPad::saveThreadRoutine(void *_param)
    {
		return 0;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void Adapter_Savegame_iPad::handle_WritingSaveFile()
    {
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void Adapter_Savegame_iPad::setMainState(MainState _newState)
    {
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void Adapter_Savegame_iPad::update()
    {        
    }
}
