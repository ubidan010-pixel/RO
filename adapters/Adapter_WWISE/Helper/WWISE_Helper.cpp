#include "precompiled_Adapter_WWISE.h"
#ifdef ITF_SUPPORT_WWISE
#include "adapters/Adapter_WWISE/Helper/WWISE_Helper.h"
#include "AK/SoundEngine/Common/AkSoundEngine.h"
#include "core/file/FileServer.h"
#include "core/types.h"
#include "engine/actors/actor.h"
#include "adapters/Adapter_WWISE/AudioSDK/error.h"
#include "core/UnicodeTools.h"


namespace	ITF
{
	namespace	Wwise
{

	//
	// ===================================================================================================================
	// ===================================================================================================================
    //
    const AkOSChar *Helper::getWwiseLanguageName(Helper::Language _language)
    {
        switch(_language)
        {
        case Helper::ARABIC:				return AKTEXT("Arabic");
        case Helper::BULGARIAN:				return AKTEXT("Bulgarian");
        case Helper::CHINESE_HK:			return AKTEXT("Chinese(HK)");
        case Helper::CHINESE_PRC:			return AKTEXT("Chinese(PRC)");
        case Helper::CHINESE_TAIWAN:		return AKTEXT("Chinese(Taiwan)");
        case Helper::CZECH:					return AKTEXT("Czech");
        case Helper::DANISH:				return AKTEXT("Danish");
        case Helper::DUTCH:					return AKTEXT("Dutch");
        case Helper::ENGLISH_AUSTRALIA:		return AKTEXT("English(Australia)");
        case Helper::ENGLISH_INDIA:			return AKTEXT("English(India)");
        case Helper::ENGLISH_UK:			return AKTEXT("English(UK)");
        case Helper::ENGLISH_US:			return AKTEXT("English(US)");
        case Helper::FINNISH:				return AKTEXT("Finnish");
        case Helper::FRENCH_CANADA:			return AKTEXT("French(Canada)");
        case Helper::FRENCH_FRANCE:			return AKTEXT("French(France)");
        case Helper::GERMAN:				return AKTEXT("German");
        case Helper::GREEK:					return AKTEXT("Greek");
        case Helper::HEBREW:				return AKTEXT("Hebrew");
        case Helper::HUNGARIAN:				return AKTEXT("Hungarian");
        case Helper::INDONESIAN:			return AKTEXT("Indonesian");
        case Helper::ITALIAN:				return AKTEXT("Italian");
        case Helper::JAPANESE:				return AKTEXT("Japanese");
        case Helper::KOREAN:				return AKTEXT("Korean");
        case Helper::LATIN:					return AKTEXT("Latin");
        case Helper::NORWEGIAN:				return AKTEXT("Norwegian");
        case Helper::POLISH:				return AKTEXT("Polish");
        case Helper::PORTUGUESE_BRAZIL:		return AKTEXT("Portuguese(Brazil)");
        case Helper::PORTUGUESE_PORTUGAL:	return AKTEXT("Portuguese(Portugal)");
        case Helper::ROMANIAN:				return AKTEXT("Romanian");
        case Helper::RUSSIAN:				return AKTEXT("Russian");
        case Helper::SLOVENIAN:				return AKTEXT("Slovenian");
        case Helper::SPANISH_MEXICO:		return AKTEXT("Spanish(Mexico)");
        case Helper::SPANISH_SPAIN:			return AKTEXT("Spanish(Spain)");
        case Helper::SPANISH_US:			return AKTEXT("Spanish(US)");
        case Helper::SWEDISH:				return AKTEXT("Swedish");
        case Helper::TURKISH:				return AKTEXT("Turkish");
        case Helper::UKRAINIAN:				return AKTEXT("Ukrainian");
        case Helper::VIETNAMESE:			return AKTEXT("Vietnamese");
        default:							return AKTEXT("InvalidLanguage");
        }
    }

    //
    // ===================================================================================================================
    // ===================================================================================================================
    //
    /*
    Wwise::Helper::Language Helper::getLanguageFromWwiseLanguageName(const AkOSChar* _languageName)
    {
        const StringID::StringIdValueType arabic = ITF_GET_STRINGID_CRC(Arabic,750492424);
        const StringID::StringIdValueType bulgarian = ITF_GET_STRINGID_CRC(Bulgarian,678039110);
        const StringID::StringIdValueType chinese_hk = ITF_GET_STRINGID_CRC(Chinese(HK),3384596678);
        const StringID::StringIdValueType chinese_prc = ITF_GET_STRINGID_CRC(Chinese(PRC),1077180983);
        const StringID::StringIdValueType chinese_taiwan = ITF_GET_STRINGID_CRC(Chinese(Taiwan),1361453367);
        const StringID::StringIdValueType czech = ITF_GET_STRINGID_CRC(Czech,511282798);
        const StringID::StringIdValueType danish = ITF_GET_STRINGID_CRC(Danish,2084010556);
        const StringID::StringIdValueType dutch = ITF_GET_STRINGID_CRC(Dutch,2500043916);
        const StringID::StringIdValueType english_australia = ITF_GET_STRINGID_CRC(English(Australia),1475274447);
        const StringID::StringIdValueType english_india = ITF_GET_STRINGID_CRC(English(India),1144358671);
        const StringID::StringIdValueType english_uk = ITF_GET_STRINGID_CRC(English(UK),644600623);
        const StringID::StringIdValueType english_us = ITF_GET_STRINGID_CRC(English(US),1762460918);
        const StringID::StringIdValueType finnish = ITF_GET_STRINGID_CRC(Finnish,2717534333);
        const StringID::StringIdValueType french_canada = ITF_GET_STRINGID_CRC(French(Canada),1673527912);
        const StringID::StringIdValueType french_france = ITF_GET_STRINGID_CRC(French(France),2604986370);
        const StringID::StringIdValueType german = ITF_GET_STRINGID_CRC(German,3813463945);
        const StringID::StringIdValueType greek = ITF_GET_STRINGID_CRC(Greek,2241839105);
        const StringID::StringIdValueType hebrew = ITF_GET_STRINGID_CRC(Hebrew,1669994801);
        const StringID::StringIdValueType hungarian = ITF_GET_STRINGID_CRC(Hungarian,1472365794);
        const StringID::StringIdValueType indonesian = ITF_GET_STRINGID_CRC(Indonesian,978882953);
        const StringID::StringIdValueType italian = ITF_GET_STRINGID_CRC(Italian,3756319546);
        const StringID::StringIdValueType japanese = ITF_GET_STRINGID_CRC(Japanese,3598488061);
        const StringID::StringIdValueType korean = ITF_GET_STRINGID_CRC(Korean,3106368350);
        const StringID::StringIdValueType latin = ITF_GET_STRINGID_CRC(Latin,581554270);
        const StringID::StringIdValueType norwegian = ITF_GET_STRINGID_CRC(Norwegian,2364800222);
        const StringID::StringIdValueType polish = ITF_GET_STRINGID_CRC(Polish,651888564);
        const StringID::StringIdValueType portuguese_brazil = ITF_GET_STRINGID_CRC(Portuguese(Brazil),2605146527);
        const StringID::StringIdValueType portuguese_portugal = ITF_GET_STRINGID_CRC(Portuguese(Portugal),3004749759);
        const StringID::StringIdValueType romanian = ITF_GET_STRINGID_CRC(Romanian,4030204911);
        const StringID::StringIdValueType russian = ITF_GET_STRINGID_CRC(Russian,264541497);
        const StringID::StringIdValueType slovenian = ITF_GET_STRINGID_CRC(Slovenian,2559195175);
        const StringID::StringIdValueType spanish_mexico = ITF_GET_STRINGID_CRC(Spanish(Mexico),1687200194);
        const StringID::StringIdValueType spanish_spain = ITF_GET_STRINGID_CRC(Spanish(Spain),2615208843);
        const StringID::StringIdValueType spanish_us = ITF_GET_STRINGID_CRC(Spanish(US),3799294108);
        const StringID::StringIdValueType swedish = ITF_GET_STRINGID_CRC(Swedish,186480402);
        const StringID::StringIdValueType turkish = ITF_GET_STRINGID_CRC(Turkish,3537418047);
        const StringID::StringIdValueType ukrainian = ITF_GET_STRINGID_CRC(Ukrainian,3019353372);
        const StringID::StringIdValueType vietnamese = ITF_GET_STRINGID_CRC(Vietnamese,2683857602);
        switch (StringID(String8(AK_StreamMgr_GetCurrentLanguage())).GetValue())
        {
        case arabic: return Helper::ARABIC;
        case bulgarian: return Helper::BULGARIAN;
        case chinese_hk: return Helper::CHINESE_HK;
        case chinese_prc: return Helper::CHINESE_PRC;
        case chinese_taiwan: return Helper::CHINESE_TAIWAN;
        case czech: return Helper::CZECH;
        case danish: return Helper::DANISH;
        case dutch: return Helper::DUTCH;
        case english_australia: return Helper::ENGLISH_AUSTRALIA;
        case english_india: return Helper::ENGLISH_INDIA;
        case english_uk: return Helper::ENGLISH_UK;
        case english_us: return Helper::ENGLISH_US;
        case finnish: return Helper::FINNISH;
        case french_canada: return Helper::FRENCH_CANADA;
        case french_france: return Helper::FRENCH_FRANCE;
        case german: return Helper::GERMAN;
        case greek: return Helper::GREEK;
        case hebrew: return Helper::HEBREW;
        case hungarian: return Helper::HUNGARIAN;
        case indonesian: return Helper::INDONESIAN;
        case italian: return Helper::ITALIAN;
        case japanese: return Helper::JAPANESE;
        case korean: return Helper::KOREAN;
        case latin: return Helper::LATIN;
        case norwegian: return Helper::NORWEGIAN;
        case polish: return Helper::POLISH;
        case portuguese_brazil: return Helper::PORTUGUESE_BRAZIL;
        case portuguese_portugal: return Helper::PORTUGUESE_PORTUGAL;
        case romanian: return Helper::ROMANIAN;
        case russian: return Helper::RUSSIAN;
        case slovenian: return Helper::SLOVENIAN;
        case spanish_mexico: return Helper::SPANISH_MEXICO;
        case spanish_spain: return Helper::SPANISH_SPAIN;
        case spanish_us: return Helper::SPANISH_US;
        case swedish: return Helper::SWEDISH;
        case turkish: return Helper::TURKISH;
        case ukrainian: return Helper::UKRAINIAN;
        case vietnamese: return Helper::VIETNAMESE;
        default: return Helper::INVALID_LANGUAGE;
        }
    }
    */
	//
	// ===================================================================================================================
	// ===================================================================================================================
	//
	const AkOSChar *Helper::getWwisePlatformName(Helper::Platform _platform)
	{
		switch(_platform)
		{
		case Helper::WINDOWS:		return AKTEXT("Windows");
		case Helper::MAC:			return AKTEXT("Mac");
		case Helper::IOS:			return AKTEXT("iOS");
		case Helper::XBOX360:		return AKTEXT("XBox360");
		case Helper::XBOXONE:		return AKTEXT("XBoxOne");
		case Helper::PS3:			return AKTEXT("PS3");
		case Helper::PS4:			return AKTEXT("PS4");
		case Helper::WII:			return AKTEXT("Wii");
		case Helper::WIIU:			return AKTEXT("WiiU");
		case Helper::ANDROIDS:		return AKTEXT("Android");
		case Helper::VITASW:		return AKTEXT("VitaSW");
		case Helper::VITAHW:		return AKTEXT("VitaHW");
		case Helper::NINTENDO3DS:	return AKTEXT("Nintendo3DS");
        case Helper::PS5:	        return AKTEXT("PS5");
        case Helper::NX:	        return AKTEXT("Switch");
        case Helper::OUNCE:	        return AKTEXT("Switch2");
        case Helper::XBOXSX:	    return AKTEXT("XboxSeriesX");
		default:					return AKTEXT("InvalidPlatform");
		}
	}

	//
	// ===================================================================================================================
	// ===================================================================================================================
	//
	Wwise::Helper::Language Helper::staticCast(ITF_LANGUAGE _itfLang)
	{
		switch(_itfLang)
		{
		case ITF_LANGUAGE_ENGLISH:				return Wwise::Helper::ENGLISH_US;
		case ITF_LANGUAGE_FRENCH:				return Wwise::Helper::FRENCH_FRANCE;
		case ITF_LANGUAGE_JAPANESE:				return Wwise::Helper::ENGLISH_US;
		case ITF_LANGUAGE_GERMAN:				return Wwise::Helper::GERMAN;
		case ITF_LANGUAGE_SPANISH:				return Wwise::Helper::SPANISH_SPAIN;
		case ITF_LANGUAGE_ITALIAN:				return Wwise::Helper::ITALIAN;
		case ITF_LANGUAGE_KOREAN:				return Wwise::Helper::ENGLISH_US;
		case ITF_LANGUAGE_TRADITIONALCHINESE:	return Wwise::Helper::ENGLISH_US;
		case ITF_LANGUAGE_PORTUGUESE:			return Wwise::Helper::ENGLISH_US;
		case ITF_LANGUAGE_SIMPLIFIEDCHINESE:	return Wwise::Helper::ENGLISH_US;
		case ITF_LANGUAGE_POLISH:				return Wwise::Helper::ENGLISH_US;
		case ITF_LANGUAGE_RUSSIAN:				return Wwise::Helper::RUSSIAN;
		case ITF_LANGUAGE_DUTCH:				return Wwise::Helper::ENGLISH_US;
		default:								return Wwise::Helper::ENGLISH_US;
		}
	}

	//
	// ===================================================================================================================
	// ===================================================================================================================
	//
	ITF_LANGUAGE Helper::staticCast(Wwise::Helper::Language _wwiseLang)
	{
		switch(_wwiseLang)
		{
		case Wwise::Helper::ENGLISH_AUSTRALIA:
		case Wwise::Helper::ENGLISH_INDIA:
		case Wwise::Helper::ENGLISH_UK:
		case Wwise::Helper::ENGLISH_US:
			return ITF_LANGUAGE_ENGLISH;

		case Wwise::Helper::CHINESE_HK:
		case Wwise::Helper::CHINESE_PRC:
		case Wwise::Helper::CHINESE_TAIWAN:
			return ITF_LANGUAGE_TRADITIONALCHINESE;

		case Wwise::Helper::FRENCH_CANADA:
		case Wwise::Helper::FRENCH_FRANCE:
			return ITF_LANGUAGE_FRENCH;

		case Wwise::Helper::JAPANESE:
			return ITF_LANGUAGE_JAPANESE;

		case Wwise::Helper::GERMAN:
			return ITF_LANGUAGE_GERMAN;

		case Wwise::Helper::SPANISH_MEXICO:
		case Wwise::Helper::SPANISH_SPAIN:
		case Wwise::Helper::SPANISH_US:
			return ITF_LANGUAGE_SPANISH;

		case Wwise::Helper::ITALIAN:
			return ITF_LANGUAGE_ITALIAN;

		case Wwise::Helper::KOREAN:
			return ITF_LANGUAGE_KOREAN;

		case Wwise::Helper::PORTUGUESE_BRAZIL:
		case Wwise::Helper::PORTUGUESE_PORTUGAL:
			return ITF_LANGUAGE_PORTUGUESE;

		case Wwise::Helper::POLISH:
			return ITF_LANGUAGE_POLISH;

		case Wwise::Helper::RUSSIAN:
			return ITF_LANGUAGE_RUSSIAN;

		case Wwise::Helper::DUTCH:
			return ITF_LANGUAGE_DUTCH;


		default:
			return ITF_LANGUAGE_UNKNOWN;
		}
	}

	//
	// ===================================================================================================================
	// ===================================================================================================================
	//
	const AkOSChar *Helper::getUAFDataRoot()
	{
		// root
        static AkOSChar s_root[512] = AKTEXT("");
        String  file = FILESERVER->getDataDepot();
        if(!file.isEmpty())
            AudioSDK::safeStringCopy(s_root, StringToCharType<AkOSChar>(file).get());
        return s_root;
	}

	//
	// ===================================================================================================================
	// ===================================================================================================================
	//
	const AkOSChar *Helper::getTempDir()
	{

		// root
		static AkOSChar s_root[512];
		AudioSDK::safeStringCopy(s_root, StringToCharType<AkOSChar>(FILESERVER->getDataDepot()).get());
		return s_root;

	}

	//
	// ===================================================================================================================
	// ===================================================================================================================
	//
	const char *Helper::getAkErrorCodeString(AK::Monitor::ErrorCode _errorCode)
	{
		switch(_errorCode)
		{
		case AK::Monitor::ErrorCode_NoError:								return "";
		case AK::Monitor::ErrorCode_FileNotFound:							return "File Not Found";
		case AK::Monitor::ErrorCode_CannotOpenFile:							return "Cannot Open File";
		case AK::Monitor::ErrorCode_CannotStartStreamNoMemory:				return "Cannot Start Stream No Memory";
		case AK::Monitor::ErrorCode_IODevice:								return "IO Device";
		case AK::Monitor::ErrorCode_IncompatibleIOSettings:					return "Incompatible IO Settings";
		case AK::Monitor::ErrorCode_PluginUnsupportedChannelConfiguration:	return "Plugin Unsupported Channel Configuration";
		case AK::Monitor::ErrorCode_PluginMediaUnavailable:					return "Plugin Media Unavailable";
		case AK::Monitor::ErrorCode_PluginInitialisationFailed:				return "Plugin Initialisation Failed";
		case AK::Monitor::ErrorCode_PluginProcessingFailed:					return "Plugin Processing Failed";
		case AK::Monitor::ErrorCode_PluginExecutionInvalid:					return "Plugin Execution Invalid";
		case AK::Monitor::ErrorCode_PluginAllocationFailed:					return "Plugin Allocation Failed";
		case AK::Monitor::ErrorCode_VorbisDecodeError:						return "Vorbis Decode Error";
		case AK::Monitor::ErrorCode_InvalidAudioFileHeader:					return "Invalid Audio File Header";
	    case AK::Monitor::ErrorCode_TransitionNotAccurateChannel:			return "Transition Not Accurate Channel";
		case AK::Monitor::ErrorCode_TransitionNotAccurateStarvation:		return "Transition Not Accurate Starvation";
		case AK::Monitor::ErrorCode_NothingToPlay:							return "Nothing To Play";
		case AK::Monitor::ErrorCode_PlayFailed:								return "Play Failed";
		case AK::Monitor::ErrorCode_StingerCouldNotBeScheduled:				return "Stinger Could Not Be Scheduled";
		case AK::Monitor::ErrorCode_TooLongSegmentLookAhead:				return "Too Long Segment Look Ahead";
		case AK::Monitor::ErrorCode_CannotScheduleMusicSwitch:				return "Cannot Schedule Music Switch";
		case AK::Monitor::ErrorCode_TooManySimultaneousMusicSegments:		return "Too Many Simultaneous Music Segments";
		case AK::Monitor::ErrorCode_PlaylistStoppedForEditing:				return "Playlist Stopped For Editing";
		case AK::Monitor::ErrorCode_MusicClipsRescheduledAfterTrackEdit:	return "Music Clips Rescheduled After Track Edit";
		case AK::Monitor::ErrorCode_CannotPlaySource_Create:				return "Cannot Play Source_Create";
		case AK::Monitor::ErrorCode_CannotPlaySource_VirtualOff:			return "Cannot Play Source_VirtualOff";
		case AK::Monitor::ErrorCode_CannotPlaySource_TimeSkip:				return "Cannot Play Source_TimeSkip";
		case AK::Monitor::ErrorCode_CannotPlaySource_InconsistentState:		return "Cannot Play Source_InconsistentState";
		case AK::Monitor::ErrorCode_MediaNotLoaded:							return "Media Not Loaded";
		case AK::Monitor::ErrorCode_VoiceStarving:							return "Voice Starving";
		case AK::Monitor::ErrorCode_StreamingSourceStarving:				return "Streaming Source Starving";
		case AK::Monitor::ErrorCode_PluginNotRegistered:					return "Plugin Not Registered";
		case AK::Monitor::ErrorCode_CodecNotRegistered:						return "Codec Not Registered";
		case AK::Monitor::ErrorCode_EventIDNotFound:						return "Event ID Not Found";
		case AK::Monitor::ErrorCode_InvalidGroupID:							return "Invalid Group ID";
		case AK::Monitor::ErrorCode_SelectedNodeNotAvailable:				return "Selected Node Not Available";
		case AK::Monitor::ErrorCode_SelectedMediaNotAvailable:				return "Selected Media Not Available";
		case AK::Monitor::ErrorCode_NoValidSwitch:							return "No Valid Switch";
		case AK::Monitor::ErrorCode_BankLoadFailed:							return "Bank Load Failed";
		case AK::Monitor::ErrorCode_ErrorWhileLoadingBank:					return "Error While Loading Bank";
		case AK::Monitor::ErrorCode_InsufficientSpaceToLoadBank:			return "Insufficient Space To Load Bank";
		case AK::Monitor::ErrorCode_LowerEngineCommandListFull:				return "Lower Engine Command List Full";
		case AK::Monitor::ErrorCode_SeekNoMarker:							return "Seek No Marker";
		case AK::Monitor::ErrorCode_CannotSeekContinuous:					return "Cannot Seek Continuous";
		case AK::Monitor::ErrorCode_SeekAfterEof:							return "Seek After Eof";
		case AK::Monitor::ErrorCode_UnknownGameObject:						return "Unknown Game Object";
		case AK::Monitor::ErrorCode_ExternalSourceNotResolved:				return "External Source Not Resolved";
		case AK::Monitor::ErrorCode_FileFormatMismatch:						return "File Format Mismatch";
		case AK::Monitor::ErrorCode_CommandQueueFull:						return "Command Queue Full";
		case AK::Monitor::ErrorCode_CommandTooLarge:						return "Command Too Large";
		default:															return "unknown error code";
		}
	}

	//
	// ===================================================================================================================
	// ===================================================================================================================
	//
	const char *Helper::getAkResultString(AKRESULT _akResult)
	{
		switch(_akResult)
		{
		case AK_Success:						return NULL;
		case AK_NotImplemented:					return "This feature is not implemented";
		case AK_Fail:							return "The operation failed.";
		case AK_PartialSuccess:					return "The operation succeeded partially.";
		case AK_NotCompatible:					return "Incompatible formats";
		case AK_AlreadyConnected:				return "The stream is already connected to another node.";
		case AK_InvalidFile:					return "An unexpected value causes the file to be invalid.";
		case AK_AudioFileHeaderTooLarge:		return "The file header is too large.";
		case AK_MaxReached:						return "The maximum was reached.";
		case AK_InvalidID:						return "The ID is invalid.";
		case AK_IDNotFound:						return "The ID was not found.";
		case AK_InvalidInstanceID:				return "The InstanceID is invalid.";
		case AK_NoMoreData:						return "No more data is available from the source.";
		case AK_InvalidStateGroup:				return "The StateGroup is not a valid channel.";
		case AK_ChildAlreadyHasAParent:			return "The child already has a parent.";
		case AK_InvalidLanguage:				return "The language is invalid (applies to the Low-Level I/O).";
		case AK_InvalidParameter:				return "Something is not within bounds.";
		case AK_ElementAlreadyInList:			return "The item could not be added because it was already in the list.";
		case AK_PathNotFound:					return "This path is not known.";
		case AK_PathNoVertices:					return "Stuff in vertices before trying to start it";
		case AK_PathNotRunning:					return "Only a running path can be paused.";
		case AK_PathNotPaused:					return "Only a paused path can be resumed.";
		case AK_PathNodeAlreadyInList:			return "This path is already there.";
		case AK_PathNodeNotInList:				return "This path is not there.";
		case AK_DataNeeded:						return "The consumer needs more.";
		case AK_NoDataNeeded:					return "The consumer does not need more.";
		case AK_DataReady:						return "The provider has available data.";
		case AK_NoDataReady:					return "The provider does not have available data.";
		case AK_InsufficientMemory:				return "Memory error.";
		case AK_Cancelled:						return "The requested action was cancelled (not an error).";
		case AK_UnknownBankID:					return "Trying to load a bank using an ID which is not defined.";
		case AK_BankReadError:					return "Error while reading a bank.";
		case AK_InvalidSwitchType:				return "Invalid switch type (used with the switch container)";
		case AK_FormatNotReady:					return "Source format not known yet.";
		case AK_WrongBankVersion:				return "The bank version is not compatible with the current bank reader.";
		case AK_FileNotFound:					return "File not found.";
		case AK_DeviceNotReady:					return "IO device not ready (may be because the tray is open)";
		case AK_BankAlreadyLoaded:				return "The bank load failed because the bank is already loaded.";
		case AK_RenderedFX:						return "The effect on the node is rendered.";
		case AK_ProcessNeeded:					return "A routine needs to be executed on some CPU.";
		case AK_ProcessDone:					return "The executed routine has finished its execution.";
		case AK_MemManagerNotInitialized:		return "The memory manager should have been initialized at this point.";
		case AK_StreamMgrNotInitialized:		return "The stream manager should have been initialized at this point.";
		case AK_SSEInstructionsNotSupported:	return "The machine does not support SSE instructions (required on PC).";
		case AK_Busy:							return "The system is busy and could not process the request.";
		case AK_UnsupportedChannelConfig:		return "Channel configuration is not supported in the current execution context.";
		case AK_PluginMediaNotAvailable:		return "Plugin media is not available for effect.";
		case AK_MustBeVirtualized:				return "Sound was Not Allowed to play.";
		case AK_CommandTooLarge:				return "SDK command is too large to fit in the command queue.";
		default:								return "unknown AKRESULT value";
		}
	}
}	// namespace ITF{
}		// namespace Wwise{
#endif // ITF_SUPPORT_WWISE
