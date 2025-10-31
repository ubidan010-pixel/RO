#ifndef __ADAPTER_WWISE_H__
#define __ADAPTER_WWISE_H__

#ifdef ITF_SUPPORT_WWISE
#include "engine/AdaptersInterfaces/AudioMiddlewareAdapter.h"
#include "engine/AdaptersInterfaces/AudioMiddlewareAdapter_Types.h"
#include "adapters/Adapter_WWISE/AudioSDK/RingBuffer.h"
#include "engine/events/IEventListener.h"
#include "core/math/vec3d.h"
#include "core/file/Path.h"
#include "engine/sound/SoundConfigTemplate.h"
#ifndef _ITF_SAFEARRAY_H_
#include "core/container/SafeArray.h"
#endif //_ITF_SAFEARRAY_H_


struct AkMemSettings;
struct AkStreamMgrSettings;
struct AkDeviceSettings;
struct AkInitSettings;
struct AkPlatformInitSettings;
struct AkMusicSettings;
struct AkCallbackInfo;
struct AkMusicSyncCallbackInfo;
class AkAudioBuffer;
struct AkAudioFormat;
class WWiseSoundBank;
class SoundConfig_Template;

#include "adapters/Adapter_WWISE/WwiseHooks/Files/Common/AkFilePackageLowLevelIODeferred.h" //todoisap
#include "adapters/Adapter_WWISE/WwiseHooks/Files/Common/AkJobWorkerMgr.h"

namespace AudioSDK
{
	class	VuMeter;
}



namespace	ITF
{
    class InstanceManager;

	namespace	Wwise
    {
	    class	GameObject;
    }

    union BankLoaderRef
    {
        const SoundConfig_Template* soundConfigTemplate;
        const SoundComponent_Template* soundComponentTemplate;
    };

    class BankLoader
    {
        public:
        BankLoader() {};
        BankLoader(const BankLoader&) = default;
        BankLoader( BankLoader&) = default;
        BankLoader(BankLoader&&) = default;
        BankLoader& operator=(const BankLoader&) = default;
        BankLoader& operator=(BankLoader&&) = default;
        enum type
        {
            SOUND_CONFIG = 0,
            SOUND_COMPONENT,
            EVENT,
            GLOBAL,
            RESOURCE
        };
        type                                        BankType = GLOBAL;
        BankLoaderRef								ref;
        u32											BankId = 0;
    };


	class Adapter_WWISE :
		public Adapter_AudioMiddleware
	{
	//
	// -------------------------------------------------------------------------------------------------------------------
	//    public interface
	// -------------------------------------------------------------------------------------------------------------------
	//


	public:
			Adapter_WWISE();
			virtual			~Adapter_WWISE();

#ifdef ITF_SUPPORT_PLUGIN
            virtual void            registerPlugins() const ;
#endif
#ifdef ITF_SUPPORT_EDITOR
;
            virtual void            visitAllPlayingInstances( void (*_visitor)(void*,const AudioPlayRequest &), void*_userParam ) const ;
			virtual void			debugEffects();
#endif


			virtual bbool	init(bbool  _enableGameOutputCapture=false);
			virtual void	destroy();
			virtual void	fastDestroy();
			virtual void	update(f32 _dt = 0);
			virtual bbool isRunning() const
			{
				return m_isRunning;
			}

			virtual SoundEventID	getIDFromGUID(const StringID &_guid) const;
            virtual f32             getDurationFromGUID(const StringID& _guid) const;

            virtual f32             getLocalizationVolumeFromLocId( u32 _locId) const;
            virtual SoundEventID        getLocalizationWWiseIdFromLocId( u32 _locId) const;
            virtual StringID            getLocalizationWWiseGUIdFromLocId( u32 _locId) const;


#ifdef ITF_DEBUG_STRINGID
            virtual const char*     getWwiseNameFromGUID(const StringID &_guid) const;
            virtual const char*     getWwiseNameFromID(SoundEventID _id) const;
#endif
            virtual SoundEventID    getEventIDFromPlayingID(SoundHandle _playingID) const;
            virtual bbool           getPlayRequestFromPlayingID(SoundHandle _playingID, AudioPlayRequest& request) const;

			virtual void			pauseImmediate(AudioPauseID _pauselevel);
			virtual void			pause(AudioPauseID _pauselevel);
			virtual void			resume(AudioPauseID _pauselevel);
			virtual void			muteDevice(AudioDeviceID _device);
			virtual void			unmuteDevice(AudioDeviceID _device);

			virtual void			mute(bbool _isMute);

			virtual void			loadBank(const char *_path, const SoundComponent_Template *_tpl);
			virtual void			unloadBank(const char *_path, const SoundComponent_Template *_tpl);
			virtual void			loadBank(const char *_path, const SoundConfig_Template *_tpl);
			virtual void			unloadBank(const char *_path, const SoundConfig_Template *_tpl);
            virtual void            prepareBank(const char* _path);
            virtual void            unprepareBank(const char* _path);

			virtual void			loadGlobalBank();
			virtual void			unloadGlobalBank();
			virtual bbool			isGlobalBankLoaded();

			virtual void			loadPackage(u32 _episode);
			virtual void			unloadPackage(u32 _episode);

			virtual void			signal(AudioSignal _signal);

			virtual void			setBusEffect(SoundBusID _busID, AudioBusSlotID _slotID, SoundEffectID _effectID);
			virtual void			resetBusEffect(SoundBusID _busID, AudioBusSlotID _slotID);
			virtual void			reInitBusEffect(SoundBusID _busID, AudioBusSlotID _slotID);

			virtual void			setListenerPosition(const Vec3d &_position, int _listenerIndex = 0);
			virtual void			setListenerOrientation(const Vec3d &_front, const Vec3d &_top, int _listenerIndex = 0);
			virtual void			getListenerPosition(Vec3d *_position, int _listenerIndex = 0) const;
			virtual void			getListenerOrientation(Vec3d *_front, Vec3d *_top, int _listenerIndex = 0) const;
            virtual void            setActorPosition(ObjectRef _objectRef, const ITF_VECTOR<Vec3d>&_positions, AudioMultiPositionMode _mode) ;

			virtual SoundHandle	    play(const AudioPlayRequest &_playRequest);
            virtual bbool           prepareEvent(const  SoundEventID _soundEventID);
            virtual bbool           unPrepareEvent(const  SoundEventID _soundEventID);
			virtual void			stop(SoundHandle _SoundHandle);
			virtual bbool			isPlaying(SoundHandle _SoundHandle) const;
			virtual void			seek(SoundHandle _SoundHandle, f32 _seekMsec);
			virtual void			pause(SoundHandle _SoundHandle);
			virtual void			resume(SoundHandle _SoundHandle);
			virtual void			unregisterObjectRef(ObjectRef _objRef);

			virtual	i32				getSourcePlayPosition(SoundHandle _SoundHandle);

			virtual void			setRtpc(SoundRtpcID _inputName, f32 _inputValue, ObjectRef _objectRef);
			virtual void			setRtpc(SoundRtpcID _inputName, SoundRtpcID _inputValue, ObjectRef _objectRef);
			virtual bool			getRtpc(SoundRtpcID _inputName, f32 &_inputValue, ObjectRef _objectRef);
            virtual bool            getState(SoundRtpcID _inputName, SoundRtpcID &_value);
            virtual void            setState(SoundRtpcID _inputName, SoundRtpcID _value);

            virtual void            setVolume(const ObjectRef _objectRef, const f32 _volume);
            virtual void            setPan(const ObjectRef _objectRef, const f32 _pan) ;
            virtual void            setPitch(const ObjectRef _objectRef, f32 _pitch) ;
            virtual ObjectRef      createWwiseActor(const char* _name = NULL);

            virtual SourceHandle    createSource(SourceInfo* _params, SoundInstance* _inst);
            virtual  void           getPauseSensitivePlayingVoices(SafeArray<SourceHandle>& _playingVoices);
            virtual InstanceManager* getInstanceManager() { return m_instanceManager; }
            virtual const SoundConfig_Template* getConfig() const { return m_config; }
            virtual void setBusVolume(StringID _bus, Volume _volume, f32 _time);
            virtual void setMasterVolume( Volume _volume);

            virtual SoftwareBus* getBus(const StringID _name);



#if defined ITF_X360 || defined ITF_DURANGO
            virtual ::IXAudio2*     getXAudio2Interface() ;
#endif

			// from IEventListener
			virtual void			onEvent(Event *_event);
            virtual ITF::ITF_LANGUAGE    getCurrentLanguage();

			virtual bbool			playInstanceAfterDestroy(SoundHandle _SoundHandle);


	//
	// -------------------------------------------------------------------------------------------------------------------
	//    public member
	// -------------------------------------------------------------------------------------------------------------------
	//


	public:
			class					MetronomeState;


			static Adapter_WWISE	*s_getSingleton()	{ return (Adapter_WWISE *)Adapter_WWISE::getptr(); }
			void					onEndOfEvent(SoundHandle _SoundHandle);
			void					onMusicEvent(const AkMusicSyncCallbackInfo *);
                        void                                    onDurationEvent(const AkDurationCallbackInfo* _akInfo, SoundHandle _SoundHandle);

			virtual bool				isInDebugMode() {return m_isInDebugMode;}
			virtual void				setInDebugMode(bool _inDebugMode){ m_isInDebugMode = _inDebugMode;}
#if !defined(ITF_FINAL) 
			void						updateFailedBank();
			void						addFailedBank(u32 _bankId);
#endif
			void						updateLoadingBank(u32 _bankID,u32 _LoadResult);
			void						setGlobalBankLoaded(bbool _bLoaded) {m_globalBankIsLoaded = _bLoaded;}
			u32							m_globalBankId;





	//
	// -------------------------------------------------------------------------------------------------------------------
	//    private attribute
	// -------------------------------------------------------------------------------------------------------------------
	//


	private:
			struct ListenerPosition;

// 			struct CaptureFormat
// 			{
// 				u32 channels;
// 				u32 sampleRate;
// 				u32 vumeterDecayRate;
// 				f32 vumeterRmsBlend;
// 			};
// 
// 			struct WavToolboxSettings
// 			{
// 				f32 windowDuration_sec;
// 				f32 level_dB;
// 				f32 fadeDuration_sec;
// 			};



	
			typedef ITF_VECTOR<BankLoader>								BankLoaderList;
			typedef ITF_MAP<Path, BankLoaderList>						LoadedBankMap;
			typedef ITF_MAP<u32, AudioPlayRequest>						PlayingIDs;
			typedef ITF_MAP<StringID, WwiseItem >	GUIDMap;
            typedef ITF_MAP<u32, SoundDialogItem  >	LocIdMap;
            typedef std::pair<SoundBusID, std::pair<AudioBusSlotID,SoundEffectID> >	ActiveAuxBus;
            typedef ITF_VECTOR<WwiseEnvironmentComponent*> EnvironmentComponentVector;
            typedef ITF_MAP<StringID, StringID>	                        WwiseGUIDNameMap;
            typedef ITF_MAP<SoundEventID, StringID>	                    WwiseIDNameMap;
            typedef ITF_VECTOR<String8>                                 DelayedUnloadBank;
            typedef std::pair<SoundRtpcID, SoundRtpcID>                  StateValue;
            

	private:

            CAkFilePackageLowLevelIODeferred*           m_pLowLevelIO;
			const SoundConfig_Template				    *m_config;		////< config
			PlayingIDs									m_playingIDs;	////< playing ID map
			GUIDMap										m_guidToID;		////< GUID -> (ID, type)
            LocIdMap									m_locIdToSound;		
            RtpcConfig                                  m_guidRtpc;
			u32											m_engineEventIDs[AUDIO_ENGEVT_COUNT];	////< engevt ID
			AudioSDK::RingBuffer<SoundHandle, 256>	m_endedEvents;	////< end of event stack
			MetronomeState								*m_metronomeState[METRONOME_TYPE_COUNT];	////< metronomes
			bbool										m_isRunning;
			bool										m_usePackage;
			u32											m_defaultPackageID; 
			u32											m_defaultLocalizedPackageID;	


			bbool										m_globalBankIsLoaded;
			ITF_VECTOR<ActiveAuxBus>					m_activeAuxBuses;
			ListenerPosition							*m_listenersState;

            InstanceManager*                            m_instanceManager = nullptr;
            ITF_MAP<StringID, SoftwareBus*>             m_buses;

#ifdef ITF_SUPPORT_EDITOR
			ITF_VECTOR<AudioPlayRequest>				m_playingEventsBeforeGenerateBanks;			///< only for reloading bank
            ITF_VECTOR<StateValue>                      m_stateBeforeGenerateBanks;
            bool                                        m_ignorePauseDuringBankGenerations;
			ITF_VECTOR<ActiveAuxBus>					m_auxEffectsBeforeGenerateBanks;			///< only for reloading bank
#endif
#ifdef ITF_DEBUG_STRINGID
            WwiseGUIDNameMap                            m_wwiseGUID2NameMap;
            WwiseIDNameMap                              m_wwiseID2NameMap;
#endif

			LoadedBankMap								m_loadedBank;
			ITF::Mutex									m_loadedBankMutex;
            DelayedUnloadBank                           m_delayedUnloadBank;
			bool										m_headPhonesPlugged;

            ITF_THREAD_CRITICAL_SECTION                 m_cs;

			bool										m_isInDebugMode;
#if !defined(ITF_FINAL) 
            SafeArray<const char*>							m_failedBank;
			ITF_VECTOR<u32>								m_failedBankId;
#endif

	//
	// -------------------------------------------------------------------------------------------------------------------
	//    private member
	// -------------------------------------------------------------------------------------------------------------------
	//


	private:
			bbool					initEngine();
            void                    SetupListeners();
			void					loadConfig();
			void					loadDefaultPackages();
#if !defined(ITF_FINAL) && defined(ITF_SUPPORT_EDITOR)
			bool					switchToBankMode() const;
#endif
            bbool	initWwise(AkMemSettings& in_memSettings,
                AkStreamMgrSettings& in_stmSettings,
                AkDeviceSettings& in_deviceSettings,
                AkInitSettings& in_initSettings,
                AkPlatformInitSettings& in_platformInitSettings,
                AkMusicSettings& in_musicInit,
                AK::JobWorkerMgr::InitSettings& _jobWorkerSettings);
			void	termWwise();
            void                    InitUAF();
            void                    ClearUAF();
            void                    UpdateUAF(f32 _deltaTime);
			
			SoundHandle	            priv_play(const AudioPlayRequest &_audioPlayRequest);
			void					treatEndOfEvent();
			void					registerActiveAuxBus(SoundBusID _busID, AudioBusSlotID _slotID, SoundEffectID _effectID);
			void					unregisterActiveAuxBus(SoundBusID _busID, AudioBusSlotID _slotID);
            void                    forceActiveAuxBus();
			void					resetAllActiveAuxBus();
			void					updatePositions();
			void 					initDefaultAuxBusSettings();

             static void ResourceMonitorDataCallback(const AkResourceMonitorDataSummary* in_pdataSummary);
             static AkResourceMonitorDataSummary ResourceDataSummary;
            static void LocalErrorCallback(
                AK::Monitor::ErrorCode in_eErrorCode,	///< Error code number value
                const AkOSChar* in_pszError,	///< Message or error string to be displayed
                AK::Monitor::ErrorLevel in_eErrorLevel,	///< Specifies whether it should be displayed as a message or an error
                AkPlayingID in_playingID,   ///< Related Playing ID if applicable, AK_INVALID_PLAYING_ID otherwise
                AkGameObjectID in_gameObjID ///< Related Game Object ID if applicable, AK_INVALID_GAME_OBJECT otherwise
            );
            AK::JobWorkerMgr::InitSettings m_jobWorkerSettings;
            static void WwiseAudioThreadCallbackFunc(AK::IAkGlobalPluginContext* in_pContext, AkGlobalCallbackLocation in_eLocation, void* in_pCookie);

            AKRESULT AK_SoundEngine_PrepareBank(const char* in_pszString);
            AKRESULT AK_SoundEngine_UnPrepareBank(const char* in_pszString);
            AKRESULT AK_SoundEngine_PrepareEvent(AkUniqueID				 in_eventID);
            AKRESULT AK_SoundEngine_UnPrepareEvent(AkUniqueID				 in_eventID);

#ifndef ITF_FINAL
			// bank reloading for editor
			void	unloadAllBanks();
			void	reloadAllBanks();
			void	traceBankLeaks();
#endif
           
	};
}

//$4-*****************************************************************************************************************

#endif // ITF_SUPPORT_WWISE
#endif // __ADAPTER_WWISE_H__
