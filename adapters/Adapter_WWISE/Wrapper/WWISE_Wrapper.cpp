#include "precompiled_Adapter_WWISE.h"
#include "adapters/Adapter_WWISE/Wrapper/WWISE_Wrapper.h"

#if defined(ITF_SUPPORT_WWISE) && defined(ITF_USE_WWISE_WRAPPER)

#include "adapters/Adapter_WWISE/AudioSDK/error.h"
#include "adapters/Adapter_WWISE/Helper/WWISE_Helper.h"
#include "AK/MusicEngine/Common/AkMusicEngine.h"

#ifndef AK_OPTIMIZED
#include "AK/Comm/AkCommunication.h"
#endif


namespace
{

	//
	// ===================================================================================================================
	// ===================================================================================================================
	//
	void printError(const char *_function, const char *_file, int _line, AKRESULT _result)
	{
		const char	*error = ITF::Wwise::Helper::getAkResultString(_result);
		if(error)
		{
			AUDIO_WARNING("[WWISE ENGINE] \"%s\" return \"%s\" (code %d)", _function, error, _result);
		}
	}

#define CHECK_RESULT(__res) \
		if(((__res) != AK_Success) && ((__res) != AK_PartialSuccess)) \
		{ \
			printError(__FUNCTION__, __FILE__, __LINE__, __res); \
		}
}

//
// =======================================================================================================================
// =======================================================================================================================
//
// AKRESULT AK_SoundEngine_SetListenerPosition(const AkListenerPosition &in_Position, AkUInt32 in_uIndex)
// {
//     const AKRESULT	res;
//     //= AK::SoundEngine::SetListenerPosition(in_Position, in_uIndex);
// 	CHECK_RESULT(res);
// 	return res;
// }

//
// =======================================================================================================================
// =======================================================================================================================
//
void AK_SoundEngine_GetDefaultInitSettings(AkInitSettings &out_settings)
{
	AK::SoundEngine::GetDefaultInitSettings(out_settings);
}

//
// =======================================================================================================================
// =======================================================================================================================
//
void AK_SoundEngine_GetDefaultPlatformInitSettings(AkPlatformInitSettings &out_platformSettings)
{
	AK::SoundEngine::GetDefaultPlatformInitSettings(out_platformSettings);
}

//
// =======================================================================================================================
// =======================================================================================================================
//
AKRESULT AK_SoundEngine_RegisterGameObj(AkGameObjectID in_gameObjectID)
{
	const AKRESULT	res = AK::SoundEngine::RegisterGameObj(in_gameObjectID);
	CHECK_RESULT(res);
	return res;
}

//
// =======================================================================================================================
// =======================================================================================================================
//
AKRESULT AK_SoundEngine_RegisterGameObj(AkGameObjectID in_gameObjectID, const char *in_pszObjName)
{
	const AKRESULT	res = AK::SoundEngine::RegisterGameObj(in_gameObjectID, in_pszObjName);
	CHECK_RESULT(res);
	return res;
}

//
// =======================================================================================================================
// =======================================================================================================================
//
AKRESULT AK_SoundEngine_UnregisterGameObj(AkGameObjectID in_gameObjectID)
{
	const AKRESULT	res = AK::SoundEngine::UnregisterGameObj(in_gameObjectID);
	CHECK_RESULT(res);
	return res;
}

//
// =======================================================================================================================
// =======================================================================================================================
//
AKRESULT AK_SoundEngine_SetPosition(AkGameObjectID in_GameObjectID, const AkSoundPosition &in_Position)
{
	const AKRESULT	res = AK::SoundEngine::SetPosition(in_GameObjectID, in_Position);
	CHECK_RESULT(res);
	return res;
}

//
// =======================================================================================================================
// =======================================================================================================================
//
AKRESULT AK_SoundEngine_RegisterPlugin(AkPluginType in_eType, AkUInt32 in_ulCompanyID, AkUInt32 in_ulPluginID, AkCreatePluginCallback in_pCreateFunc, AkCreateParamCallback in_pCreateParamFunc)
{
	const AKRESULT	res = AK::SoundEngine::RegisterPlugin(in_eType, in_ulCompanyID, in_ulPluginID, in_pCreateFunc, in_pCreateParamFunc);
	CHECK_RESULT(res);
	return res;
}

//
// =======================================================================================================================
// =======================================================================================================================
//
AkUInt32 AK_SoundEngine_GetIDFromString(const char *in_pszString)
{
	const AkUInt32	res = AK::SoundEngine::GetIDFromString(in_pszString);
	return res;
}

#ifdef AK_SUPPORT_WCHAR

//
// =======================================================================================================================
// =======================================================================================================================
//
AkUInt32 AK_SoundEngine_GetIDFromString(const wchar_t *in_pszString)
{
	const AkUInt32	res = AK::SoundEngine::GetIDFromString(in_pszString);
	return res;
}
#endif

//
// =======================================================================================================================
// =======================================================================================================================
//
AKRESULT AK_SoundEngine_RenderAudio()
{
	const AKRESULT	res = AK::SoundEngine::RenderAudio();
	CHECK_RESULT(res);
	return res;
}

//
// =======================================================================================================================
// =======================================================================================================================
//
AkPlayingID AK_SoundEngine_PostEvent
			(
				AkUniqueID				in_eventID,
				AkGameObjectID			in_gameObjectID,
				AkUInt32				in_uFlags,
				AkCallbackFunc			in_pfnCallback,
				void					*in_pCookie,
				AkUInt32				in_cExternals,
				AkExternalSourceInfo	*in_pExternalSources,
				AkPlayingID				in_PlayingID
			)
{
	AUDIO_ASSERT(in_eventID != AK_INVALID_UNIQUE_ID, "you are trying to post an invalid event");

	const AkPlayingID	res = AK::SoundEngine::PostEvent(in_eventID, in_gameObjectID, in_uFlags, in_pfnCallback, in_pCookie, in_cExternals, in_pExternalSources, in_PlayingID);
	return res;
}

//
// =======================================================================================================================
// =======================================================================================================================
//
void AK_SoundEngine_StopPlayingID(AkPlayingID in_playingID, AkTimeMs in_uTransitionDuration, AkCurveInterpolation in_eFadeCurve)
{
	AUDIO_ASSERT(in_playingID != AK_INVALID_PLAYING_ID, "you are trying to stop an invalid ID");
	AK::SoundEngine::StopPlayingID(in_playingID, in_uTransitionDuration, in_eFadeCurve);
}

//
// =======================================================================================================================
// =======================================================================================================================
//
AKRESULT AK_SoundEngine_SetRTPCValue(AkRtpcID in_rtpcID, AkRtpcValue in_value, AkGameObjectID in_gameObjectID, AkTimeMs in_uValueChangeDuration, AkCurveInterpolation in_eFadeCurve)
{
	AUDIO_ASSERT(in_rtpcID != AK_INVALID_RTPC_ID, "you are trying to set an invalid RTPC ID");

	const AKRESULT	res = AK::SoundEngine::SetRTPCValue(in_rtpcID, in_value, in_gameObjectID, in_uValueChangeDuration, in_eFadeCurve);
	CHECK_RESULT(res);
	return res;
}

//
// =======================================================================================================================
// =======================================================================================================================
//

AKRESULT AK_SoundEngine_Query_GetRTPCValue(AkRtpcID in_rtpcID, AkGameObjectID in_gameObjectID, AkPlayingID in_playingID, AkRtpcValue& out_rValue, AK::SoundEngine::Query::RTPCValue_type& io_rValueType)
{
    const AKRESULT	res = AK::SoundEngine::Query::GetRTPCValue(in_rtpcID, in_gameObjectID, in_playingID,out_rValue, io_rValueType);

    if ((res != AK_Success) && (res != AK_IDNotFound))
    {
        printError(__FUNCTION__, __FILE__, __LINE__, res);
    }

    return AK_Success;
}


//
// =======================================================================================================================
// =======================================================================================================================
//
AkGameObjectID AK_SoundEngine_Query_GetGameObjectFromPlayingID(AkPlayingID in_playingID)
{
	AkGameObjectID	res = AK::SoundEngine::Query::GetGameObjectFromPlayingID(in_playingID);
	return res;
}

//
// =======================================================================================================================
// =======================================================================================================================
//
AKRESULT AK_SoundEngine_Query_GetActiveGameObjects(AK::SoundEngine::Query::AkGameObjectsList &io_GameObjectList)
{
	const AKRESULT	res = AK::SoundEngine::Query::GetActiveGameObjects(io_GameObjectList);
	if(res != AK_Success)
	{
		printError(__FUNCTION__, __FILE__, __LINE__, res);
	}

	return res;
}

//
// =======================================================================================================================
// =======================================================================================================================
//
AKRESULT AK_SoundEngine_SetSwitch(AkSwitchGroupID in_switchGroup, AkSwitchStateID in_switchState, AkGameObjectID in_gameObjectID)
{
	AUDIO_ASSERT(in_switchGroup != AK_INVALID_UNIQUE_ID, "you are trying to set an invalid switch group ID");

	const AKRESULT	res = AK::SoundEngine::SetSwitch(in_switchGroup, in_switchState, in_gameObjectID);
	CHECK_RESULT(res);
	return res;
}

//
// =======================================================================================================================
// =======================================================================================================================
//
AKRESULT AK_SoundEngine_SetState(AkStateGroupID in_stateGroup, AkStateID in_state)
{
	AUDIO_ASSERT(in_stateGroup != AK_INVALID_UNIQUE_ID, "you are trying to set an invalid state group ID");
	AUDIO_ASSERT(in_state != AK_INVALID_UNIQUE_ID, "you are trying to set an invalid state");

	const AKRESULT	res = AK::SoundEngine::SetState(in_stateGroup, in_state);
	CHECK_RESULT(res);
	return res;
}

//
// =======================================================================================================================
// =======================================================================================================================
//
AKRESULT AK_SoundEngine_SetBusEffect(AkUniqueID in_audioNodeID, AkUInt32 in_uFXIndex, AkUniqueID in_shareSetID)
{
	AUDIO_ASSERT(in_audioNodeID != AK_INVALID_AUX_ID, "you are trying to set an invalid bus");
	const AKRESULT	res = AK::SoundEngine::SetBusEffect(in_audioNodeID, in_uFXIndex, in_shareSetID);
	CHECK_RESULT(res);
	return res;
}

//
// =======================================================================================================================
// =======================================================================================================================
//
AKRESULT AK_SoundEngine_LoadBank(const wchar_t *in_pszString,  AkBankID &out_bankID)
{
	const AKRESULT	res = AK::SoundEngine::LoadBank(in_pszString, out_bankID);
	CHECK_RESULT(res);
	return res;
}

//
// =======================================================================================================================
// =======================================================================================================================
//
AKRESULT AK_SoundEngine_LoadBank(const char *in_pszString,  AkBankID &out_bankID)
{
	const AKRESULT	res = AK::SoundEngine::LoadBank(in_pszString, out_bankID);

	CHECK_RESULT(res);
	return res;
}

//
// =======================================================================================================================
// =======================================================================================================================
//
AKRESULT AK_SoundEngine_LoadBank(const wchar_t *in_pszString, AkBankCallbackFunc in_pfnBankCallback, void *in_pCookie,  AkBankID &out_bankID)
{
	const AKRESULT	res = AK::SoundEngine::LoadBank(in_pszString, in_pfnBankCallback, in_pCookie,  out_bankID);
	CHECK_RESULT(res);
	return res;
}

//
// =======================================================================================================================
// =======================================================================================================================
//
AKRESULT AK_SoundEngine_LoadBank(const char *in_pszString, AkBankCallbackFunc in_pfnBankCallback, void *in_pCookie,  AkBankID &out_bankID)
{
	const AKRESULT	res = AK::SoundEngine::LoadBank(in_pszString, in_pfnBankCallback, in_pCookie, out_bankID);
	CHECK_RESULT(res);
	return res;
}

//
// =======================================================================================================================
// =======================================================================================================================
//
AKRESULT AK_SoundEngine_UnloadBank(const wchar_t *in_pszString, const void *in_pInMemoryBankPtr)
{
	const AKRESULT	res = AK::SoundEngine::UnloadBank(in_pszString, in_pInMemoryBankPtr);
	CHECK_RESULT(res);
	return res;
}

//
// =======================================================================================================================
// =======================================================================================================================
//
AKRESULT AK_SoundEngine_UnloadBank(const char *in_pszString, const void *in_pInMemoryBankPtr)
{
	const AKRESULT	res = AK::SoundEngine::UnloadBank(in_pszString, in_pInMemoryBankPtr);
	CHECK_RESULT(res);
	return res;
}

//
// =======================================================================================================================
// =======================================================================================================================
//
AKRESULT AK_SoundEngine_Query_GetPlayingIDsFromGameObject(AkGameObjectID in_GameObjId, AkUInt32 &io_ruNumIDs, AkPlayingID *out_aPlayingIDs)
{
	const AKRESULT	res = AK::SoundEngine::Query::GetPlayingIDsFromGameObject(in_GameObjId, io_ruNumIDs, out_aPlayingIDs);
	CHECK_RESULT(res);
	return res;
}

//
// =======================================================================================================================
// =======================================================================================================================
//
AKRESULT AK_SoundEngine_Init(AkInitSettings *in_pSettings, AkPlatformInitSettings *in_pPlatformSettings)
{
	const AKRESULT	res = AK::SoundEngine::Init(in_pSettings, in_pPlatformSettings);
	CHECK_RESULT(res);
	return res;
}

//
// =======================================================================================================================
// =======================================================================================================================
//
bool AK_SoundEngine_IsInitialized()
{
	const bool	res = AK::SoundEngine::IsInitialized();
	return res;
}

//
// =======================================================================================================================
// =======================================================================================================================
//
void AK_SoundEngine_Term()
{
	AK::SoundEngine::Term();
}

AK::StreamMgr::IAkFileLocationResolver *
AK_StreamMgr_GetFileLocationResolver()
{
	AK::StreamMgr::IAkFileLocationResolver * res = AK::StreamMgr::GetFileLocationResolver();
	return res;
}

//
// =======================================================================================================================
// =======================================================================================================================
//
void AK_StreamMgr_SetFileLocationResolver(AK::StreamMgr::IAkFileLocationResolver *in_pFileLocationResolver)
{
	AK::StreamMgr::SetFileLocationResolver(in_pFileLocationResolver);
}

//
// =======================================================================================================================
// =======================================================================================================================
//
AKRESULT AK_StreamMgr_CreateDevice(const AkDeviceSettings &in_settings, AK::StreamMgr::IAkLowLevelIOHook *in_pLowLevelHook, AkDeviceID& out_idDevice)
{
	const AKRESULT	res = AK::StreamMgr::CreateDevice(in_settings, in_pLowLevelHook, out_idDevice);
	return res;
}

//
// =======================================================================================================================
// =======================================================================================================================
//
AKRESULT AK_StreamMgr_DestroyDevice(AkDeviceID in_deviceID)
{
	const AKRESULT	res = AK::StreamMgr::DestroyDevice(in_deviceID);
	CHECK_RESULT(res);
	return res;
}

//
// =======================================================================================================================
// =======================================================================================================================
//
void AK_StreamMgr_GetDefaultSettings(AkStreamMgrSettings &out_settings)
{
	AK::StreamMgr::GetDefaultSettings(out_settings);
}

//
// =======================================================================================================================
// =======================================================================================================================
//
void AK_StreamMgr_GetDefaultDeviceSettings(AkDeviceSettings &out_settings)
{
	AK::StreamMgr::GetDefaultDeviceSettings(out_settings);
}

//
// =======================================================================================================================
// =======================================================================================================================
//
void AK_MusicEngine_GetDefaultInitSettings(AkMusicSettings &out_settings)
{
	AK::MusicEngine::GetDefaultInitSettings(out_settings);
}

//
// =======================================================================================================================
// =======================================================================================================================
//
// AkMemPoolId AK_MemoryMgr_CreatePool(void *in_pMemAddress, AkUInt32 in_uMemSize, AkUInt32 in_uBlockSize, AkUInt32 in_eAttributes, AkUInt32 in_uBlockAlign /* 0 */ )
// {
// 	const AkMemPoolId	res = AK::MemoryMgr::CreatePool(in_pMemAddress, in_uMemSize, in_uBlockSize, in_eAttributes, in_uBlockAlign);
// 	return res;
// }

//
// =======================================================================================================================
// =======================================================================================================================
//
// AKRESULT AK_MemoryMgr_DestroyPool(AkMemPoolId in_poolId)
// {
// 	const AKRESULT	res = AK::MemoryMgr::DestroyPool(in_poolId);
// 	CHECK_RESULT(res);
// 	return res;
// }

//
// =======================================================================================================================
// =======================================================================================================================
//
// void *AK_MemoryMgr_GetBlock(AkMemPoolId in_poolId)
// {
// 	void	*res = AK::MemoryMgr::GetBlock(in_poolId);
// 	return res;
// }

//
// =======================================================================================================================
// =======================================================================================================================
//
// AKRESULT AK_MemoryMgr_ReleaseBlock(AkMemPoolId in_poolId, void *in_pMemAddress)
// {
// 	const AKRESULT	res = AK::MemoryMgr::ReleaseBlock(in_poolId, in_pMemAddress);
// 	CHECK_RESULT(res);
// 	return res;
// }

//
// =======================================================================================================================
// =======================================================================================================================
//
const AkOSChar *AK_StreamMgr_GetCurrentLanguage()
{
	const AkOSChar	*res = AK::StreamMgr::GetCurrentLanguage();
	return res;
}

//
// =======================================================================================================================
// =======================================================================================================================
//
// AkMemPoolAttributes AK_MemoryMgr_GetPoolAttributes(AkMemPoolId in_poolId)
// {
// 	const AkMemPoolAttributes	res = AK::MemoryMgr::GetPoolAttributes(in_poolId);
// 	return res;
// }

//
// =======================================================================================================================
// =======================================================================================================================
//
// AKRESULT AK_MemoryMgr_CheckPoolId(AkMemPoolId in_poolId)
// {
// 	const AKRESULT	res = AK::MemoryMgr::CheckPoolId(in_poolId);
// 	CHECK_RESULT(res);
// 	return res;
// }

//
// =======================================================================================================================
// =======================================================================================================================
//
// AkUInt32 AK_MemoryMgr_GetBlockSize(AkMemPoolId in_poolId)
// {
// 	const AkUInt32	res = AK::MemoryMgr::GetBlockSize(in_poolId);
// 	return res;
// }

AK::IAkStreamMgr *
AK_IAkStreamMgr_Get()
{
	AK::IAkStreamMgr * res = AK::IAkStreamMgr::Get();
	return res;
}

//
// =======================================================================================================================
// =======================================================================================================================
//
void AK_StreamMgr_RemoveLanguageChangeObserver(void *in_pCookie)
{
	AK::StreamMgr::RemoveLanguageChangeObserver(in_pCookie);
}

//
// =======================================================================================================================
// =======================================================================================================================
//
AKRESULT AK_StreamMgr_AddLanguageChangeObserver(AK::StreamMgr::AkLanguageChangeHandler in_handler, void *in_pCookie)
{
	const AKRESULT	res = AK::StreamMgr::AddLanguageChangeObserver(in_handler, in_pCookie);
	CHECK_RESULT(res);
	return res;
}

//
// =======================================================================================================================
// =======================================================================================================================
//
AKRESULT AK_Monitor_SetLocalOutput(AkUInt32 in_uErrorLevel /* ErrorLevel_All */, AK::Monitor::LocalOutputFunc in_pMonitorFunc /* 0 */ )
{
	const AKRESULT	res = AK::Monitor::SetLocalOutput(in_uErrorLevel, in_pMonitorFunc);
	CHECK_RESULT(res);
	return res;
}

//
// =======================================================================================================================
// =======================================================================================================================
//
AKRESULT AK_StreamMgr_SetCurrentLanguage(const AkOSChar *in_pszLanguageName)
{
	const AKRESULT	res = AK::StreamMgr::SetCurrentLanguage(in_pszLanguageName);
	CHECK_RESULT(res);
	return res;
}


AK::IAkStreamMgr *
AK_StreamMgr_Create (const AkStreamMgrSettings &in_settings)
{
	AK::IAkStreamMgr * res = AK::StreamMgr::Create(in_settings);
	return res;
}

//
// =======================================================================================================================
// =======================================================================================================================
//
AKRESULT AK_MusicEngine_Init(AkMusicSettings *in_pSettings)
{
	const AKRESULT	res = AK::MusicEngine::Init(in_pSettings);
	CHECK_RESULT(res);
	return res;
}

//
// =======================================================================================================================
// =======================================================================================================================
//
void AK_MusicEngine_Term()
{
	AK::MusicEngine::Term();
}

#ifndef AK_OPTIMIZED

//
// =======================================================================================================================
// =======================================================================================================================
//
AKRESULT AK_Comm_Init(const AkCommSettings &in_settings)
{
	const AKRESULT	res = AK::Comm::Init(in_settings);
	CHECK_RESULT(res);
	return res;
}

//
// =======================================================================================================================
// =======================================================================================================================
//
void AK_Comm_Term()
{
	AK::Comm::Term();
}

//
// =======================================================================================================================
// =======================================================================================================================
//
AKRESULT AK_SoundEngine_SetVolumeThreshold(const AkReal32 &in_fVolumeThresholdDB)
{
	const AKRESULT	res = AK::SoundEngine::SetVolumeThreshold(in_fVolumeThresholdDB);
	CHECK_RESULT(res);
	return res;
}

//
// =======================================================================================================================
// =======================================================================================================================
//
void AK_Comm_GetDefaultInitSettings(AkCommSettings &out_settings)
{
	AK::Comm::GetDefaultInitSettings(out_settings);
}
#endif

//
// =======================================================================================================================
// =======================================================================================================================
//
bool AK_MemoryMgr_IsInitialized()
{
	const bool	res = AK::MemoryMgr::IsInitialized();
	return res;
}

//
// =======================================================================================================================
// =======================================================================================================================
//
void AK_MemoryMgr_Term()
{
	AK::MemoryMgr::Term();
}

//
// =======================================================================================================================
// =======================================================================================================================
//
AKRESULT AK_MusicEngine_GetPlayingSegmentInfo(AkPlayingID in_PlayingID, AkSegmentInfo &out_segmentInfo, bool in_bExtrapolate /* true */ )
{
	const AKRESULT	res = AK::MusicEngine::GetPlayingSegmentInfo(in_PlayingID, out_segmentInfo, in_bExtrapolate);

	if(res != AK_Fail)
	{
		CHECK_RESULT(res);
	}

	return res;
}

//
// =======================================================================================================================
// =======================================================================================================================
//
AKRESULT AK_SoundEngine_SetGameObjectAuxSendValues(AkGameObjectID in_gameObjectID, AkAuxSendValue *in_aAuxSendValues, AkUInt32 in_uNumSendValues)
{
	const AKRESULT	res = AK::SoundEngine::SetGameObjectAuxSendValues(in_gameObjectID, in_aAuxSendValues, in_uNumSendValues);
	CHECK_RESULT(res);
	return res;
}

//
// =======================================================================================================================
// =======================================================================================================================
//
AKRESULT AK_SoundEngine_SetMultiplePositions(AkGameObjectID in_GameObjectID, const AkSoundPosition *in_pPositions, AkUInt16 in_NumPositions, AK::SoundEngine::MultiPositionType in_eMultiPositionType)
{
	const AKRESULT	res = AK::SoundEngine::SetMultiplePositions(in_GameObjectID, in_pPositions, in_NumPositions, in_eMultiPositionType);
	CHECK_RESULT(res);
	return res;
}

//
// =======================================================================================================================
// =======================================================================================================================
//
AKRESULT AK_SoundEngine_ExecuteActionOnEvent
		(
			AkUniqueID								in_eventID,
			AK::SoundEngine::AkActionOnEventType	in_ActionType,
			AkGameObjectID							in_gameObjectID,
			AkTimeMs								in_uTransitionDuration,
			AkCurveInterpolation					in_eFadeCurve,
			AkPlayingID								in_PlayingID
		)
{
	const AKRESULT	res = AK::SoundEngine::ExecuteActionOnEvent(in_eventID, in_ActionType, in_gameObjectID, in_uTransitionDuration, in_eFadeCurve, in_PlayingID);
	CHECK_RESULT(res);
	return res;
}

//
// =======================================================================================================================
// =======================================================================================================================
//
AKRESULT AK_SoundEngine_SeekOnEvent(AkUniqueID in_eventID, AkGameObjectID in_gameObjectID, AkTimeMs in_iPosition, bool in_bSeekToNearestMarker)
{
	const AKRESULT	res = AK::SoundEngine::SeekOnEvent(in_eventID, in_gameObjectID, in_iPosition, in_bSeekToNearestMarker);
	CHECK_RESULT(res);
	return res;
}

//
// =======================================================================================================================
// =======================================================================================================================
//
AKRESULT AK_SoundEngine_Query_GetPositioningInfo(AkUniqueID in_ObjectID, AkPositioningInfo &out_rPositioningInfo)
{
	const AKRESULT	res = AK::SoundEngine::Query::GetPositioningInfo(in_ObjectID, out_rPositioningInfo);
	CHECK_RESULT(res);
	return res;
}

//
// =======================================================================================================================
// =======================================================================================================================
//
AKRESULT AK_SoundEngine_GetSourcePlayPosition(AkPlayingID in_PlayingID, AkTimeMs *out_puPosition)
{
	const AKRESULT	res = AK::SoundEngine::GetSourcePlayPosition(in_PlayingID, out_puPosition);
	//CHECK_RESULT(res);
	return res;
}

//
// =======================================================================================================================
// =======================================================================================================================
//
AKRESULT AK_SoundEngine_Query_QueryAudioObjectIDs(AkUniqueID in_eventID, AkUInt32 &io_ruNumItems, AkObjectInfo *out_aObjectInfos)
{
	const AKRESULT	res = AK::SoundEngine::Query::QueryAudioObjectIDs(in_eventID, io_ruNumItems, out_aObjectInfos);
	CHECK_RESULT(res);
	return res;
}

//
// =======================================================================================================================
// =======================================================================================================================
//
AkUniqueID AK_SoundEngine_Query_GetEventIDFromPlayingID(AkPlayingID in_playingID)
{
	const AkUniqueID	res = AK::SoundEngine::Query::GetEventIDFromPlayingID(in_playingID);
	return res;
}

//
// =======================================================================================================================
// =======================================================================================================================
//

//
// =======================================================================================================================
// =======================================================================================================================
//
AKRESULT AK_SoundEngine_StartOutputCapture(const AkOSChar *in_CaptureFileName)
{
	const AKRESULT	res = AK::SoundEngine::StartOutputCapture(in_CaptureFileName);
	CHECK_RESULT(res);
	return res;
}

//
// =======================================================================================================================
// =======================================================================================================================
//
AKRESULT AK_SoundEngine_StopOutputCapture()
{
	const AKRESULT	res = AK::SoundEngine::StopOutputCapture();
	CHECK_RESULT(res);
	return res;
}

//
// =======================================================================================================================
// =======================================================================================================================
//
AKRESULT AK_SoundEngine_Query_GetState(AkStateGroupID in_stateGroup, AkStateID &out_rState)
{
	const AKRESULT	res = AK::SoundEngine::Query::GetState(in_stateGroup, out_rState);
	CHECK_RESULT(res);
	return res;
}

//
// =======================================================================================================================
// =======================================================================================================================
//
AKRESULT AK_SoundEngine_RegisterCodec(AkUInt32 in_ulCompanyID, AkUInt32 in_ulCodecID, AkCreateFileSourceCallback in_pFileCreateFunc, AkCreateBankSourceCallback in_pBankCreateFunc)
{
	const AKRESULT	res = AK::SoundEngine::RegisterCodec(in_ulCompanyID, in_ulCodecID, in_pFileCreateFunc, in_pBankCreateFunc);
	CHECK_RESULT(res);
	return res;
}


#endif
