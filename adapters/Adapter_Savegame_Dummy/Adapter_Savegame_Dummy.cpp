#include "precompiled_Adapter_Savegame_Dummy.h"

#include "adapters/Adapter_Savegame_Dummy/Adapter_Savegame_Dummy.h"

namespace ITF
{
    Adapter_Savegame_Dummy::Adapter_Savegame_Dummy()
    {
        ITF_SAVE_NOT_IMPLEMENTED();
    }

    Adapter_Savegame_Dummy::~Adapter_Savegame_Dummy() = default;

    void Adapter_Savegame_Dummy::init()
    {
        ITF_SAVE_NOT_IMPLEMENTED();
    }

    void Adapter_Savegame_Dummy::term()
    {
        ITF_SAVE_NOT_IMPLEMENTED();
    }

    Adapter_Savegame_Dummy::ErrorCode Adapter_Savegame_Dummy::startChooseDevice(u32 _playerIndex, u32 _minFreeSize, bbool _isSilentWhenChosenNone)
    {
        ITF_SAVE_NOT_IMPLEMENTED();
        return ErrorCode::Error_Ok;
    }

    Adapter_Savegame_Dummy::ErrorCode Adapter_Savegame_Dummy::startSavegameEnumerationUpdate(u32 _playerIndex)
    {
        ITF_SAVE_NOT_IMPLEMENTED();
        return ErrorCode::Error_Ok;
    }

    Adapter_Savegame_Dummy::ErrorCode Adapter_Savegame_Dummy::startSavegameEnumerationUpdateOnEveryDevice(u32 _playerIndex)
    {
        ITF_SAVE_NOT_IMPLEMENTED();
        return ErrorCode::Error_Ok;
    }

    Adapter_Savegame_Dummy::ErrorCode Adapter_Savegame_Dummy::startSave(u32 _playerIndex, u32 _slotIndex, const String8& _baseName, const String& displayName, const void* _data, u32 _dataSize, bbool _warnIfOverwriteForNewGame)
    {
        ITF_SAVE_NOT_IMPLEMENTED();
        return ErrorCode::Error_Ok;
    }

    Adapter_Savegame_Dummy::ErrorCode Adapter_Savegame_Dummy::startLoad(u32 _playerIndex, u32 _slotIndex, const String8& baseName)
    {
        ITF_SAVE_NOT_IMPLEMENTED();
        return ErrorCode::Error_Ok;
    }

    u32 Adapter_Savegame_Dummy::getEnumeratedContentCount(u32 _playerIndex)
    {
        ITF_SAVE_NOT_IMPLEMENTED();
        return 0;
    }

    bbool Adapter_Savegame_Dummy::getEnumeratedContent(u32 _playerIndex, u32 _index, String8& _baseName, String& _displayName, bbool& _emptyContent)
    {
        ITF_SAVE_NOT_IMPLEMENTED();

        _baseName.setTextFormat("Savegame_%u_%u", _playerIndex, _index);
        _displayName = _baseName;
        _emptyContent = btrue;

        return btrue;
    }

    void Adapter_Savegame_Dummy::update()
    {
        ITF_SAVE_NOT_IMPLEMENTED();
    }

    bbool Adapter_Savegame_Dummy::hasPendingOperation()
    {
        ITF_SAVE_NOT_IMPLEMENTED();
        return bfalse;
    }

    bbool Adapter_Savegame_Dummy::isAskingWhatToDoBecauseFileExist()
    {
        ITF_SAVE_NOT_IMPLEMENTED();
        return bfalse;
    }

    void Adapter_Savegame_Dummy::signalOkForOverwrite(ChoiceWhenFileExists _choice)
    {
        ITF_SAVE_NOT_IMPLEMENTED();
    }

    bbool Adapter_Savegame_Dummy::isAskingWhatToDoBecauseFileExistForNewGame()
    {
        ITF_SAVE_NOT_IMPLEMENTED();
        return bfalse;
    }

    void Adapter_Savegame_Dummy::signalOkForOverwriteForNewGame(ChoiceWhenFileExistsForNewGame _choice)
    {
        ITF_SAVE_NOT_IMPLEMENTED();
    }

    bbool Adapter_Savegame_Dummy::isAskingWhatToDoBecauseDeviceIsNoLongerValidForSave()
    {
        ITF_SAVE_NOT_IMPLEMENTED();
        return bfalse;
    }

    void Adapter_Savegame_Dummy::signalAnswerDeviceNoLongerValidForSave(ChoiceWhenDeviceNoLongerValidForSave _choice)
    {
        ITF_SAVE_NOT_IMPLEMENTED();
    }

    bbool Adapter_Savegame_Dummy::isAskingWhatToDoBecauseOfSaveError()
    {
        ITF_SAVE_NOT_IMPLEMENTED();
        return bfalse;
    }

    void Adapter_Savegame_Dummy::signalAnswerSaveError(ChoiceWhenSaveError _choice)
    {
        ITF_SAVE_NOT_IMPLEMENTED();
    }

    bbool Adapter_Savegame_Dummy::isAskingWhatToDoBecauseOfChoseNoDevice()
    {
        ITF_SAVE_NOT_IMPLEMENTED();
        return bfalse;
    }

    void Adapter_Savegame_Dummy::signalAnswerChoseNoDevice(ChoiceWhenDeviceChosenAsNone _choice)
    {
        ITF_SAVE_NOT_IMPLEMENTED();
    }

    bbool Adapter_Savegame_Dummy::isAskingWhatToDoBecauseDeviceNoLongerValidForLoad()
    {
        ITF_SAVE_NOT_IMPLEMENTED();
        return bfalse;
    }

    void Adapter_Savegame_Dummy::signalAnswerDeviceNoLongerValidForLoad(ChoiceWhenDeviceNoLongerValidForLoad _choice)
    {
        ITF_SAVE_NOT_IMPLEMENTED();
    }

    bbool Adapter_Savegame_Dummy::isAskingWhatToDoBecauseOfLoadError()
    {
        ITF_SAVE_NOT_IMPLEMENTED();
        return bfalse;
    }

    void Adapter_Savegame_Dummy::signalAnswerLoadError(ChoiceWhenLoadError _choice)
    {
        ITF_SAVE_NOT_IMPLEMENTED();
    }

    bbool Adapter_Savegame_Dummy::isAskingWhatToDoBecauseLoadCantFind()
    {
        ITF_SAVE_NOT_IMPLEMENTED();
        return bfalse;
    }

    void Adapter_Savegame_Dummy::signalAnswerToLoadCantFind(ChoiceWhenLoadCantFind _choice)
    {
        ITF_SAVE_NOT_IMPLEMENTED();
    }
}
