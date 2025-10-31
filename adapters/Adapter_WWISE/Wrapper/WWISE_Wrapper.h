#ifndef __WWISE_WRAPPER_H__
#define __WWISE_WRAPPER_H__

#ifdef ITF_SUPPORT_WWISE
#include "AK/SoundEngine/Common/AkTypes.h"
#include "AK/SoundEngine/Common/AkCallback.h"
#include "adapters/Adapter_WWISE/Adapter_WWISE.h"
#include "AK/SoundEngine/Common/IAkPlugin.h"
#include "AK/SoundEngine/Common/AkStreamMgrModule.h"
#include "AK/Tools/Common/AkMonitorError.h"
#include "AK/SoundEngine/Common/AkSoundEngine.h"
#include "AK/SoundEngine/Common/AkQueryParameters.h"

//$4
// ***********************************************************************************************************************
// ***********************************************************************************************************************
//

//Volume Threshold for AK::SoundEngine::SetVolumeThreshold must be a value between 0 and -96.3 dB
const AkReal32 MIN_VOLUME_THRESHOLD = 0.f;
const AkReal32 MAX_VOLUME_THRESHOLD = -96.3f; 

#ifndef AK_OPTIMIZED
#define ITF_USE_WWISE_WRAPPER
#endif
#ifdef ITF_USE_WWISE_WRAPPER
class	IAkStreamMgr;
class	IAkFileLocationResolver;
class	IAkLowLevelIOHook;
struct AkCommSettings;
struct AkSegmentInfo;

AKRESULT AK_SoundEngine_RegisterCodec(AkUInt32 in_ulCompanyID, AkUInt32 in_ulCodecID, AkCreateFileSourceCallback in_pFileCreateFunc, AkCreateBankSourceCallback in_pBankCreateFunc);
AKRESULT AK_SoundEngine_Query_GetState(AkStateGroupID in_stateGroup, AkStateID &out_rState);
AKRESULT AK_SoundEngine_StopOutputCapture();
AKRESULT AK_SoundEngine_StartOutputCapture(const AkOSChar *in_CaptureFileName);
AKRESULT AK_SoundEngine_RegisterAllPlugins();
AkUniqueID AK_SoundEngine_Query_GetEventIDFromPlayingID(AkPlayingID in_playingID);
AKRESULT AK_SoundEngine_Query_QueryAudioObjectIDs(AkUniqueID in_eventID, AkUInt32 &io_ruNumItems, AkObjectInfo *out_aObjectInfos);
AKRESULT AK_SoundEngine_Query_GetPositioningInfo(AkUniqueID in_ObjectID, AkPositioningInfo &out_rPositioningInfo);
AKRESULT AK_SoundEngine_SeekOnEvent(AkUniqueID in_eventID, AkGameObjectID in_gameObjectID, AkTimeMs in_iPosition, bool in_bSeekToNearestMarker = false);
AKRESULT AK_SoundEngine_ExecuteActionOnEvent(AkUniqueID								 in_eventID,
											 AK::SoundEngine::AkActionOnEventType	 in_ActionType,
											 AkGameObjectID							 in_gameObjectID = AK_INVALID_GAME_OBJECT,
											 AkTimeMs								 in_uTransitionDuration = 0,
											 AkCurveInterpolation					 in_eFadeCurve = AkCurveInterpolation_Linear,
											 AkPlayingID							 in_PlayingID = AK_INVALID_PLAYING_ID);
AKRESULT AK_SoundEngine_SetMultiplePositions(AkGameObjectID						 in_GameObjectID,
											 const AkSoundPosition				 *in_pPositions,
											 AkUInt16							 in_NumPositions,
											 AK::SoundEngine::MultiPositionType	 in_eMultiPositionType = AK::SoundEngine::MultiPositionType_MultiDirections);
AKRESULT AK_SoundEngine_SetGameObjectAuxSendValues(AkGameObjectID in_gameObjectID, AkAuxSendValue *in_aAuxSendValues, AkUInt32 in_uNumSendValues);
AKRESULT AK_SoundEngine_SetListenerPosition(const AkListenerPosition &in_Position, AkUInt32 in_uIndex = 0);
void AK_SoundEngine_GetDefaultInitSettings(AkInitSettings &out_settings);
void AK_SoundEngine_GetDefaultPlatformInitSettings(AkPlatformInitSettings &out_platformSettings);
AKRESULT AK_SoundEngine_RegisterGameObj(AkGameObjectID in_gameObjectID);
AKRESULT AK_SoundEngine_RegisterGameObj(AkGameObjectID in_gameObjectID, const char *in_pszObjName);
AKRESULT AK_SoundEngine_UnregisterGameObj(AkGameObjectID in_gameObjectID);
AKRESULT AK_SoundEngine_SetPosition(AkGameObjectID in_GameObjectID, const AkSoundPosition &in_Position);
AKRESULT AK_SoundEngine_RegisterPlugin(AkPluginType in_eType, AkUInt32 in_ulCompanyID, AkUInt32 in_ulPluginID, AkCreatePluginCallback in_pCreateFunc, AkCreateParamCallback in_pCreateParamFunc);
AKRESULT AK_SoundEngine_RegisterAllCodecPlugins();
AKRESULT AK_SoundEngine_RegisterAllEffectPlugins();
AKRESULT AK_SoundEngine_RegisterAllRumblePlugins();
AKRESULT AK_SoundEngine_RegisterConvolutionReverbPlugin();
AKRESULT AK_SoundEngine_RegisterSoundSeedPlugins();
AKRESULT AK_SoundEngine_RegisterMcDSPPlugins();
AKRESULT AK_SoundEngine_RegisteriZotopePlugins();
AKRESULT AK_SoundEngine_GetSourcePlayPosition(AkPlayingID in_PlayingID, AkTimeMs *out_puPosition);
AkUInt32 AK_SoundEngine_GetIDFromString(const char *in_pszString);
AkUInt32 AK_SoundEngine_GetIDFromString(const wchar_t *in_pszString);
AKRESULT AK_SoundEngine_RenderAudio();
AkPlayingID AK_SoundEngine_PostEvent(AkUniqueID				 in_eventID,
									 AkGameObjectID			 in_gameObjectID,
									 AkUInt32				 in_uFlags = 0,
									 AkCallbackFunc			 in_pfnCallback = NULL,
									 void					 *in_pCookie = NULL,
									 AkUInt32				 in_cExternals = 0,
									 AkExternalSourceInfo	 *in_pExternalSources = NULL,
									 AkPlayingID			 in_PlayingID = AK_INVALID_PLAYING_ID);
void AK_SoundEngine_StopPlayingID(AkPlayingID in_playingID, AkTimeMs in_uTransitionDuration = 0, AkCurveInterpolation in_eFadeCurve = AkCurveInterpolation_Linear);
AKRESULT AK_SoundEngine_Query_GetRTPCValue(AkRtpcID in_rtpcID, AkGameObjectID in_gameObjectID, AkPlayingID in_playingID, AkRtpcValue& out_rValue, AK::SoundEngine::Query::RTPCValue_type& io_rValueType);
AkGameObjectID AK_SoundEngine_Query_GetGameObjectFromPlayingID(AkPlayingID in_playingID);
AKRESULT AK_SoundEngine_Query_GetActiveGameObjects(AK::SoundEngine::Query::AkGameObjectsList &io_GameObjectList);
AKRESULT AK_SoundEngine_SetRTPCValue(AkRtpcID				 in_rtpcID,
									 AkRtpcValue			 in_value,
									 AkGameObjectID			 in_gameObjectID = AK_INVALID_GAME_OBJECT,
									 AkTimeMs				 in_uValueChangeDuration = 0,
									 AkCurveInterpolation	 in_eFadeCurve = AkCurveInterpolation_Linear);
AKRESULT AK_SoundEngine_SetSwitch(AkSwitchGroupID in_switchGroup, AkSwitchStateID in_switchState, AkGameObjectID in_gameObjectID);
AKRESULT AK_SoundEngine_SetState(AkStateGroupID in_stateGroup, AkStateID in_state);
AKRESULT AK_SoundEngine_SetBusEffect(AkUniqueID in_audioNodeID, AkUInt32 in_uFXIndex, AkUniqueID in_shareSetID);
AKRESULT AK_SoundEngine_LoadBank(const wchar_t *in_pszString, AkBankID &out_bankID);
AKRESULT AK_SoundEngine_LoadBank(const wchar_t *in_pszString, AkBankCallbackFunc in_pfnBankCallback, void *in_pCookie, AkBankID &out_bankID);
AKRESULT AK_SoundEngine_LoadBank(const char *in_pszString, AkBankID &out_bankID);
AKRESULT AK_SoundEngine_LoadBank(const char *in_pszString, AkBankCallbackFunc in_pfnBankCallback, void *in_pCookie,  AkBankID &out_bankID);
AKRESULT AK_SoundEngine_UnloadBank(const wchar_t *in_pszString, const void *in_pInMemoryBankPtr);
AKRESULT AK_SoundEngine_UnloadBank(const char *in_pszString, const void *in_pInMemoryBankPtr);
AKRESULT AK_SoundEngine_Query_GetPlayingIDsFromGameObject(AkGameObjectID in_GameObjId, AkUInt32 &io_ruNumIDs, AkPlayingID *out_aPlayingIDs);
AKRESULT AK_SoundEngine_Init(AkInitSettings *in_pSettings, AkPlatformInitSettings *in_pPlatformSettings);
bool AK_SoundEngine_IsInitialized();
void AK_SoundEngine_Term();
AKRESULT AK_SoundEngine_SetVolumeThreshold(const AkReal32 &in_fVolumeThresholdDB);
AK::IAkStreamMgr * AK_StreamMgr_Create (const AkStreamMgrSettings &in_settings);
AK::StreamMgr::IAkFileLocationResolver * AK_StreamMgr_GetFileLocationResolver();
void		AK_StreamMgr_SetFileLocationResolver(AK::StreamMgr::IAkFileLocationResolver *in_pFileLocationResolver);
AKRESULT	AK_StreamMgr_CreateDevice(const AkDeviceSettings &in_settings, AK::StreamMgr::IAkLowLevelIOHook *in_pLowLevelHook, AkDeviceID& out_idDevice);
AKRESULT	AK_StreamMgr_DestroyDevice(AkDeviceID in_deviceID);
void		AK_StreamMgr_GetDefaultSettings(AkStreamMgrSettings &out_settings);
void		AK_StreamMgr_GetDefaultDeviceSettings(AkDeviceSettings &out_settings);
AK::IAkStreamMgr * AK_IAkStreamMgr_Get();
void				AK_StreamMgr_RemoveLanguageChangeObserver(void *in_pCookie);
AKRESULT			AK_StreamMgr_AddLanguageChangeObserver(AK::StreamMgr::AkLanguageChangeHandler in_handler, void *in_pCookie);
void				AK_MusicEngine_GetDefaultInitSettings(AkMusicSettings &out_settings);
AKRESULT			AK_MemoryMgr_Init(AkMemSettings *in_pSettings);
AkMemPoolId			AK_MemoryMgr_CreatePool(void *in_pMemAddress, AkUInt32 in_uMemSize, AkUInt32 in_uBlockSize, AkUInt32 in_eAttributes, AkUInt32 in_uBlockAlign = 0);
AKRESULT			AK_MemoryMgr_DestroyPool(AkMemPoolId in_poolId);
void				*AK_MemoryMgr_GetBlock(AkMemPoolId in_poolId);
AKRESULT			AK_MemoryMgr_ReleaseBlock(AkMemPoolId in_poolId, void *in_pMemAddress);
const AkOSChar		*AK_StreamMgr_GetCurrentLanguage();
AKRESULT			AK_StreamMgr_SetCurrentLanguage(const AkOSChar *in_pszLanguageName);
//AkMemPoolAttributes AK_MemoryMgr_GetPoolAttributes(AkMemPoolId in_poolId);
AKRESULT			AK_MemoryMgr_CheckPoolId(AkMemPoolId in_poolId);
AkUInt32			AK_MemoryMgr_GetBlockSize(AkMemPoolId in_poolId);
AKRESULT			AK_Monitor_SetLocalOutput(AkUInt32 in_uErrorLevel = AK::Monitor::ErrorLevel_All, AK::Monitor::LocalOutputFunc in_pMonitorFunc = 0);

AKRESULT			AK_MusicEngine_Init(AkMusicSettings *in_pSettings);
void				AK_MusicEngine_Term();
AKRESULT			AK_Comm_Init(const AkCommSettings &in_settings);
void				AK_Comm_Term();
bool				AK_MemoryMgr_IsInitialized();
void				AK_MemoryMgr_Term();
AKRESULT			AK_MusicEngine_GetPlayingSegmentInfo(AkPlayingID in_PlayingID, AkSegmentInfo &out_segmentInfo, bool in_bExtrapolate = true);
void				AK_Comm_GetDefaultInitSettings(AkCommSettings &out_settings);
#else // ITF_USE_WWISE_WRAPPER
#define AK_SoundEngine_SetListenerPosition					AK::SoundEngine::SetListenerPosition
#define AK_SoundEngine_GetDefaultInitSettings				AK::SoundEngine::GetDefaultInitSettings
#define AK_SoundEngine_GetDefaultPlatformInitSettings		AK::SoundEngine::GetDefaultPlatformInitSettings
#define AK_SoundEngine_RegisterGameObj						AK::SoundEngine::RegisterGameObj
#define AK_SoundEngine_UnregisterGameObj					AK::SoundEngine::UnregisterGameObj
#define AK_SoundEngine_SetPosition							AK::SoundEngine::SetPosition
#define AK_SoundEngine_RegisterPlugin						AK::SoundEngine::RegisterPlugin
#define AK_SoundEngine_GetIDFromString						AK::SoundEngine::GetIDFromString
#define AK_SoundEngine_RenderAudio							AK::SoundEngine::RenderAudio
#define AK_SoundEngine_PostEvent							AK::SoundEngine::PostEvent
#define AK_SoundEngine_StopPlayingID						AK::SoundEngine::StopPlayingID
#define AK_SoundEngine_SetRTPCValue							AK::SoundEngine::SetRTPCValue
#define AK_SoundEngine_Query_GetRTPCValue					AK::SoundEngine::Query::GetRTPCValue
#define AK_SoundEngine_Query_GetGameObjectFromPlayingID		AK::SoundEngine::Query::GetGameObjectFromPlayingID
#define AK_SoundEngine_Query_GetActiveGameObjects			AK::SoundEngine::Query::GetActiveGameObjects
#define AK_SoundEngine_SetSwitch							AK::SoundEngine::SetSwitch
#define AK_SoundEngine_SetState								AK::SoundEngine::SetState
#define AK_SoundEngine_SetBusEffect							AK::SoundEngine::SetBusEffect
#define AK_SoundEngine_LoadBank								AK::SoundEngine::LoadBank
#define AK_SoundEngine_UnloadBank							AK::SoundEngine::UnloadBank
#define AK_SoundEngine_Query_GetPlayingIDsFromGameObject	AK::SoundEngine::Query::GetPlayingIDsFromGameObject
#define AK_SoundEngine_Init									AK::SoundEngine::Init
#define AK_SoundEngine_IsInitialized						AK::SoundEngine::IsInitialized
#define AK_SoundEngine_Term									AK::SoundEngine::Term
#define AK_SoundEngine_SetVolumeThreshold					AK::SoundEngine::SetVolumeThreshold
#define AK_StreamMgr_Create									AK::StreamMgr::Create
#define AK_StreamMgr_GetFileLocationResolver				AK::StreamMgr::GetFileLocationResolver
#define AK_StreamMgr_SetFileLocationResolver				AK::StreamMgr::SetFileLocationResolver
#define AK_StreamMgr_CreateDevice							AK::StreamMgr::CreateDevice
#define AK_StreamMgr_DestroyDevice							AK::StreamMgr::DestroyDevice
#define AK_StreamMgr_GetDefaultSettings						AK::StreamMgr::GetDefaultSettings
#define AK_StreamMgr_GetDefaultDeviceSettings				AK::StreamMgr::GetDefaultDeviceSettings
#define AK_IAkStreamMgr_Get									AK::IAkStreamMgr::Get
#define AK_StreamMgr_AddLanguageChangeObserver				AK::StreamMgr::AddLanguageChangeObserver
#define AK_StreamMgr_RemoveLanguageChangeObserver			AK::StreamMgr::RemoveLanguageChangeObserver
#define AK_MusicEngine_GetDefaultInitSettings				AK::MusicEngine::GetDefaultInitSettings
#define AK_MemoryMgr_Init									AK::MemoryMgr::Init
#define AK_MemoryMgr_CreatePool								AK::MemoryMgr::CreatePool
#define AK_MemoryMgr_DestroyPool							AK::MemoryMgr::DestroyPool
#define AK_MemoryMgr_GetBlock								AK::MemoryMgr::GetBlock
#define AK_MemoryMgr_ReleaseBlock							AK::MemoryMgr::ReleaseBlock
#define AK_StreamMgr_GetCurrentLanguage						AK::StreamMgr::GetCurrentLanguage
#define AK_MemoryMgr_GetPoolAttributes						AK::MemoryMgr::GetPoolAttributes
#define AK_MemoryMgr_CheckPoolId							AK::MemoryMgr::CheckPoolId
#define AK_MemoryMgr_GetBlockSize							AK::MemoryMgr::GetBlockSize
#define AK_Monitor_SetLocalOutput							AK::Monitor::SetLocalOutput
#define AK_StreamMgr_SetCurrentLanguage						AK::StreamMgr::SetCurrentLanguage
#define AK_MusicEngine_Init									AK::MusicEngine::Init
#define AK_MusicEngine_Term									AK::MusicEngine::Term
#define AK_MemoryMgr_IsInitialized							AK::MemoryMgr::IsInitialized
#define AK_MemoryMgr_Term									AK::MemoryMgr::Term
#define AK_MusicEngine_GetPlayingSegmentInfo				AK::MusicEngine::GetPlayingSegmentInfo
#define AK_SoundEngine_SetGameObjectAuxSendValues			AK::SoundEngine::SetGameObjectAuxSendValues
#define AK_SoundEngine_SetMultiplePositions					AK::SoundEngine::SetMultiplePositions
#define AK_SoundEngine_ExecuteActionOnEvent					AK::SoundEngine::ExecuteActionOnEvent
#define AK_SoundEngine_SeekOnEvent							AK::SoundEngine::SeekOnEvent
#define AK_SoundEngine_Query_GetPositioningInfo				AK::SoundEngine::Query::GetPositioningInfo
#define AK_SoundEngine_Query_QueryAudioObjectIDs			AK::SoundEngine::Query::QueryAudioObjectIDs
#define AK_SoundEngine_Query_GetEventIDFromPlayingID		AK::SoundEngine::Query::GetEventIDFromPlayingID
#define AK_SoundEngine_RegisterAllPlugins					AK::SoundEngine::RegisterAllPlugins
#define AK_SoundEngine_StartOutputCapture					AK::SoundEngine::StartOutputCapture
#define AK_SoundEngine_StopOutputCapture					AK::SoundEngine::StopOutputCapture
#define AK_SoundEngine_Query_GetState						AK::SoundEngine::Query::GetState
#define AK_SoundEngine_RegisterCodec                        AK::SoundEngine::RegisterCodec
#define AK_MotionEngine_RegisterMotionDevice                AK::MotionEngine::RegisterMotionDevice
#define AK_SoundEngine_GetSourcePlayPosition				AK::SoundEngine::GetSourcePlayPosition


#ifndef AK_OPTIMIZED
#define AK_Comm_Init(_a)
#define AK_Comm_Term()
#define AK_Comm_GetDefaultInitSettings(_a)
#endif
#endif // ITF_USE_WWISE_WRAPPER

//$4-*****************************************************************************************************************

#endif // #ifdef ITF_SUPPORT_WWISE
#endif // __WWISE_WRAPPER_H__
