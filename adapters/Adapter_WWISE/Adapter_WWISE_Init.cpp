#include "precompiled_Adapter_WWISE.h"

#ifdef ITF_SUPPORT_WWISE

#include "adapters/Adapter_WWISE/Adapter_WWISE.h"
#include "core/AdaptersInterfaces/SystemAdapter.h"
#include "adapters/Adapter_WWISE/Configure/WWISE_Configure.h"
#include "AK/SoundEngine/Common/AkStreamMgrModule.h"
#include "AK/SoundEngine/Common/AkSoundEngine.h"
#include "AK/MusicEngine/Common/AkMusicEngine.h"
#include <AK/SoundEngine/Common/IAkStreamMgr.h>
#include <AK/SoundEngine/Common/AkMemoryMgr.h>		// Memory Manager
#include <AK/SoundEngine/Common/AkMemoryMgrModule.h>			// Default memory and stream managers
#include "adapters/Adapter_WWISE/WwiseHooks/Files/Common/AkFilePackageLowLevelIODeferred.h"
#include "adapters/Adapter_WWISE/WwiseHooks/Files/Common/AkJobWorkerMgr.h"
#include <AK/SpatialAudio/Common/AkSpatialAudio.h>	// Spatial Audio module
#ifndef AK_OPTIMIZED
#include "AK/Comm/AkCommunication.h"
#endif
#include "engine/sound/SoundConfigTemplate.h"
#include "core/error/ErrorHandler.h"
#include "core/AdaptersInterfaces/FileManager.h"
#include "adapters/Adapter_WWISE/Helper/WWISE_Helper.h"

#include "engine/events/Events.h"
#include "engine/events/EventManager.h"
#include "engine/TemplateManager/TemplateDatabase.h"
#include "adapters/Adapter_WWISE/Adapter_WWISE_MetronomeState.h"
#include "engine/localisation/LocalisationManager.h"


#include "adapters/Adapter_WWISE/WwiseHooks/WWISE_Hooks.h"
#include "adapters/AudioSerializedData/Wwise/WwiseEvents.h"
#include "adapters/Adapter_WWISE/Wrapper/WWISE_Wrapper.h"
#include "adapters/AudioSerializedData/Wwise/WwiseItem.h"
#include "adapters/AudioSerializedData/Wwise/WwiseEngineEvent.h"
#include <string.h>
#include "adapters/Adapter_WWISE/Adapter_WWISE_Listener.h"
#include "adapters/Adapter_WWISE/Helper/WWISE_AkGameObject.h"
#include "adapters/Adapter_WWISE/AudioSDK/error.h"
#include "adapters/Adapter_WWISE/WwiseHooks/Files/Common/AkJobWorkerMgr.h"


#ifndef _ITF_SOFTWAREBUS_H_
#include "engine/sound/routing/SoftwareBus.h"
#endif // _ITF_SOFTWAREBUS_H_

#ifndef _ITF_MUSICMANAGER_H_
#include "engine/gameplay/musicmanager.h"
#endif //_ITF_MUSICMANAGER_H_

#include "engine/sound/InstanceManager.h"

#include "AK/Plugin/AllPluginsFactories.h"
#define SOUND_ADAPTER_MAX_NB_VOICES 256
// !!! check all those constant has always the same value !!!
// AUDIO_STATIC_ASSERT(AK_INVALID_AUX_ID == ITF_INVALID_SOUND_BUS_ID, "both constants must be equal");
// AUDIO_STATIC_ASSERT(AK_INVALID_UNIQUE_ID == ITF_INVALID_SOUND_EFFECT_ID, "both constants must be equal");
// AUDIO_STATIC_ASSERT(AK_INVALID_UNIQUE_ID == ITF_INVALID_SOUND_EVENT_ID, "both constants must be equal");
// AUDIO_STATIC_ASSERT(AK_INVALID_RTPC_ID == ITF_INVALID_SOUND_RTPC_ID, "both constants must be equal");
// AUDIO_STATIC_ASSERT(AK_INVALID_PLAYING_ID == ITF::SoundHandle::c_invalidValue, "both constants must be equal");
// AUDIO_STATIC_ASSERT(AK_INVALID_BANK_ID == ITF_INVALID_SOUND_BANK_ID, "both constants must be equal");

namespace	ITF
{

	//
	// ===================================================================================================================
	// ===================================================================================================================
	//
	Adapter_WWISE::Adapter_WWISE() :
		m_pLowLevelIO(NULL),
		m_isRunning(bfalse),
		m_config(NULL),
		m_usePackage(true),
		m_defaultPackageID(0),
		m_defaultLocalizedPackageID(0),
        m_headPhonesPlugged(false),
		m_isInDebugMode(false)
#ifdef ITF_SUPPORT_EDITOR
        ,m_ignorePauseDuringBankGenerations(false)
#endif
	{

        Synchronize::createCriticalSection(&m_cs);

        m_pLowLevelIO = WWISE_NEW(CAkFilePackageLowLevelIODeferred());

		for(u32 i = 0; i < AUDIO_ENGEVT_COUNT; ++i) m_engineEventIDs[i] = ITF_INVALID_SOUND_EVENT_ID;


		for(u32 i = 0; i < METRONOME_TYPE_COUNT; ++i)
		{
			m_metronomeState[i] = WWISE_NEW(MetronomeState) ((MetronomeType) i);
		}

		m_listenersState = WWISE_NEW(ListenerPosition[Wwise::Configure::LISTENER_COUNT]);

	}

	//
	// ===================================================================================================================
	// ===================================================================================================================
	//
	Adapter_WWISE::~Adapter_WWISE()
	{

		WWISE_DELETE(m_pLowLevelIO);

		for(u32 i = 0; i < METRONOME_TYPE_COUNT; ++i)
		{
			WWISE_DELETE(m_metronomeState[i]);
		}

		WWISE_DELETE_ARRAY(m_listenersState);

        Synchronize::destroyCriticalSection(&m_cs);
	}

	//
	// ===================================================================================================================
	// ===================================================================================================================
	//
	ITF::bbool Adapter_WWISE::init(bbool  _enableGameOutputCapture)
	{
#if defined(USE_PAD_HAPTICS)
	    initMotionLib();
#endif
		// init engine
		m_isRunning = initEngine();
		if(m_isRunning)
		{
#if !defined(ITF_FINAL) && defined(ITF_SUPPORT_EDITOR)
			// only if editor + no bundle : auto switch "bank" or "package" mode for loading
            m_usePackage = false;//switchToBankMode() == false;
#endif

            m_usePackage = false;// for wwise bank mode

			// load common packages SFX + localized
	   if(m_usePackage) loadDefaultPackages();

            // init some singletons
            Wwise::AkGameObjectFactory::s_createSingleton();

            // register global gao
            Wwise::AkGameObjectFactory::s_getSingleton()->create(ITF_INVALID_OBJREF);


			// load config
			loadConfig();

            m_instanceManager = newAlloc(mId_AudioEngine, InstanceManager)(SOUND_ADAPTER_MAX_NB_VOICES);


            InitUAF();

			// register events
// 			EVENTMANAGER_REGISTER_EVENT_LISTENER(EventLoadWwiseBank_CRC, this);
// 			EVENTMANAGER_REGISTER_EVENT_LISTENER(EventUnloadWwiseBank_CRC, this);
// 			EVENTMANAGER_REGISTER_EVENT_LISTENER(EventSetWwiseState_CRC, this);
// 			EVENTMANAGER_REGISTER_EVENT_LISTENER(EventPlayWwiseEvent_CRC, this);
// 			EVENTMANAGER_REGISTER_EVENT_LISTENER(SendMetronomedEvent_CRC, this);
// 			EVENTMANAGER_REGISTER_EVENT_LISTENER(EventSetWwiseAuxBusEffect_CRC, this);
// 			EVENTMANAGER_REGISTER_EVENT_LISTENER(EventResetWwiseAuxBusEffect_CRC, this);

		}
		else
		{
			AUDIO_ERROR("Failed to initialize Adapter_WWISE !");
		}




		return m_isRunning;
	}

	//
	// ===================================================================================================================
	// ===================================================================================================================
	//
	bbool Adapter_WWISE::initEngine()
	{


		// get default settings
		AkStreamMgrSettings		stmSettings;
		AkDeviceSettings		deviceSettings;
		AkInitSettings			initSettings;
		AkPlatformInitSettings	platformInitSettings;
		AkMusicSettings			musicInit;
        AkMemSettings           memSettings;
        AK::JobWorkerMgr::InitSettings jobWorkerSettings;

		Wwise::Configure::getDefaultSettings(memSettings, stmSettings, deviceSettings, initSettings, platformInitSettings, musicInit, jobWorkerSettings);

		// Initialize Wwise engine
		if(!initWwise(memSettings,stmSettings, deviceSettings, initSettings, platformInitSettings, musicInit, jobWorkerSettings))
		{
			termWwise();
			return bfalse;
		}


		// Set the path to the SoundBank Files. root
		// set bank path
		AkOSChar	root[512];

        AudioSDK::safeStringCopy(root, Wwise::Helper::getUAFDataRoot());
		AudioSDK::safeStringCat(root, Wwise::Configure::c_bankRootPath);
		AudioSDK::safeStringCat(root, Wwise::Helper::getWwisePlatformName(Wwise::Helper::CURRENT_PLATFORM));
		AudioSDK::safeStringCat(root, AKTEXT("/"));
#if defined( ITF_PS5) || defined( ITF_NX) || defined( ITF_OUNCE)
        String finalPath;
        FILEMANAGER->TranslatePath(finalPath, root);
        const AkOSChar* consoleroot = { AKTEXT(finalPath.getCharCopy()) };
        m_pLowLevelIO->SetBasePath(consoleroot);
#else

        m_pLowLevelIO->SetBasePath(root);
#endif




		//
		// Set global language. Low-level I/O devices can use this string to find
		// language-specific assets.
		//
		const ITF_LANGUAGE				language = LOCALISATIONMANAGER->getCurrentLanguage();
		const Wwise::Helper::Language	wwiseLanguage = Wwise::Helper::staticCast(language);

		if(AK_StreamMgr_SetCurrentLanguage(Wwise::Helper::getWwiseLanguageName(wwiseLanguage)) != AK_Success)
		{
			termWwise();
			return bfalse;
		}

        SetupListeners();
        AK::SoundEngine::RegisterGlobalCallback(&Adapter_WWISE::WwiseAudioThreadCallbackFunc,
            AkGlobalCallbackLocation_BeginRender,
            this);

        //




        // override Wwise log
        AK_Monitor_SetLocalOutput(AK::Monitor::ErrorLevel_Error, ITF::Wwise::Hooks::logHook);


		return btrue;
	}

    SoftwareBus* Adapter_WWISE::getBus(const StringID _name)
    {
        ITF_MAP<StringID, SoftwareBus*>::iterator it = m_buses.find(_name);

        if (it != m_buses.end())
            return it->second;

        return NULL;
    }

    void Adapter_WWISE::ClearUAF()
    {
        ITF_MAP<StringID, SoftwareBus*>::iterator it;

        for (it = m_buses.begin(); it != m_buses.end(); ++it)
        {
            SoftwareBus* bus = it->second;

            delete bus;
        }

        m_buses.clear();

        delete m_instanceManager;
        m_instanceManager = NULL;
    }
    void Adapter_WWISE::InitUAF()
    {
        u32 numBuses = m_config->getBusDefs().size();
        u32 u;
        for (u = 0; u < numBuses; ++u)
        {
            ITF_ASSERT_CRASH(m_buses.find(m_config->getBusDefs()[u].m_name) == m_buses.end(), "Bus <%s> defined twice in sound config", m_config->getBusDefs()[u].m_name.getDebugString());

            SoftwareBus* bus = newAlloc(mId_AudioEngine, SoftwareBus(&m_config->getBusDefs()[u]));
            m_buses[bus->getBusDef()->m_name] = bus;
        }

        ITF_MAP<StringID, SoftwareBus*>::const_iterator it = m_buses.find(SOUND_BUS_MASTER);
        ITF_ASSERT_CRASH(it != m_buses.end(), "You must define the master bus in your soundconfig.isg");

        for (it = m_buses.begin(); it != m_buses.end(); ++it)
        {
            SoftwareBus* bus = it->second;
            u32 numOutputs = bus->getBusDef()->m_outputs.size();
            ITF_UNUSED(numOutputs);

            ITF_ASSERT_CRASH((numOutputs == 0 && bus->getBusDef()->m_name == SOUND_BUS_MASTER) || (numOutputs > 0),
                "Bus %s has no output defined", bus->getBusDef()->m_name.getDebugString());

            ITF_ASSERT_CRASH(bus->getBusDef()->m_outputs.size() <= 1, "Bus %s has more than one output",
                bus->getBusDef()->m_name.getDebugString());

            if (bus->getBusDef()->m_outputs.size())
            {
                ITF_ASSERT_CRASH(m_buses.find(bus->getBusDef()->m_outputs[0]) != m_buses.end(), "Bus %s has invalid output",
                    bus->getBusDef()->m_name.getDebugString());

                bus->setParentGroup(m_buses[bus->getBusDef()->m_outputs[0]]);
            }
        }

        // check that there is no circular dependency
        for (it = m_buses.begin(); it != m_buses.end(); ++it)
        {
            SoftwareBus* bus = it->second;
            SoftwareBus* parent = (SoftwareBus*)bus->getParentGroup();

            while (parent)
            {
                ITF_ASSERT_CRASH(parent != bus, "Bus %s has circular dependency", bus->getBusDef()->m_name.getDebugString());

                parent = (SoftwareBus*)parent->getParentGroup();
            }
        }
    }

    void Adapter_WWISE::SetupListeners()
    {
        // there's nothing to do for the default listener, only the center speaker one needs to be setup

        // for the center listener, mute all speakers except the center one
        AkChannelConfig channelConfig;
        channelConfig.SetStandard(AK_SPEAKER_SETUP_7_1);

        AkUInt32 uVectorSize = AK::SpeakerVolumes::Vector::GetRequiredSize(channelConfig.uNumChannels);
        AK::SpeakerVolumes::VectorPtr volumes = (AK::SpeakerVolumes::VectorPtr)new_array(u32,uVectorSize, mId_AudioEngine);
        AK::SpeakerVolumes::Vector::Zero(volumes, channelConfig.uNumChannels);

        //create gameobject for listeners

        AK_SoundEngine_RegisterGameObj(static_cast<AkGameObjectID>(ListenerID::eDefaultListener), "Listener_Default");
        AK_SoundEngine_RegisterGameObj(static_cast<AkGameObjectID>(ListenerID::eCenterSpeakerListener), "Listener_Center_Speaker");


        AkGameObjectID listenerIds[static_cast<u32>(ListenerID::eListenerCount)];
        for (AkGameObjectID i = 0; i < static_cast<u64>(ListenerID::eListenerCount); ++i)
        {
            listenerIds[i] = i;
        }
        // Make sure all listeners are created in Wwise (to prevent issues when calling e.g. SetListenerSpatialization).
        AK::SoundEngine::SetDefaultListeners(listenerIds, static_cast<u32>(ListenerID::eListenerCount));

        // Then register the default listener again, so that eDefaultListener is the only default listener.
        AkGameObjectID defaultListenerIds[] = { static_cast<AkGameObjectID>(ListenerID::eDefaultListener) };
        AK::SoundEngine::SetDefaultListeners(defaultListenerIds, sizeof(defaultListenerIds));
        AkVector orientFront, orientTop;
        orientFront.Zero();
        orientTop.Zero();
        orientFront.Z = 1.f;
        orientTop.Y = 1.f;
        AkSoundPosition InitPos;
        AkVector64 Pos{ 0.0, 0.0,0.0 };

        InitPos.Set(Pos, orientFront, orientTop);;

        AK_SoundEngine_SetPosition(static_cast<AkGameObjectID>(ITF::ListenerID::eDefaultListener), InitPos);
        AK_SoundEngine_SetPosition(static_cast<AkGameObjectID>(ITF::ListenerID::eCenterSpeakerListener), InitPos);


       // SetListenerPosition(ubiVector4::GetZero(), ListenerID::eCenterSpeakerListener);



    }

	//
	// ===================================================================================================================
	// ===================================================================================================================
	//
	bbool Adapter_WWISE::initWwise
						(
                            AkMemSettings           & in_memSettings,
							AkStreamMgrSettings		&in_stmSettings,
							AkDeviceSettings		&in_deviceSettings,
							AkInitSettings			&in_initSettings,
							AkPlatformInitSettings	&in_platformInitSettings,
							AkMusicSettings			&in_musicInit,
                            AK::JobWorkerMgr::InitSettings& _jobWorkerSettings

						)
	{

#if  defined( ITF_NX) || defined( ITF_OUNCE)
        AK::JobWorkerMgr::InitSettings& jws = m_jobWorkerSettings;
        if (jws.uNumWorkerThreads > 0)
        {
            ITF_ASSERT(jws.uNumWorkerThreads <= AK_MAX_WORKER_THREADS);
            jws.arThreadWorkerProperties = &Wwise::Configure::arThreadProperties[0];
            for (int uWorkerIdx = 0; uWorkerIdx < jws.uNumWorkerThreads; uWorkerIdx++)
            {
                AkThreadProperties prop;
                AKPLATFORM::AkGetDefaultThreadProperties(prop);
                prop.nPriority = AK_THREAD_PRIORITY_ABOVE_NORMAL;
                int iIdealCore = (uWorkerIdx + 1) % 3;
                prop.affinityMask = AK_THREAD_AFFINITY_ALL;
                //prop.affinityMask = 1 << iIdealCore; // may want to uncomment if thread-migration gets in the way of profiling
                prop.iIdealCore = iIdealCore;
                jws.arThreadWorkerProperties[uWorkerIdx] = prop;
            }
        }
#elif defined (ITF_OUNCE)
 //       once
//         AkThreadProperties prop;
//         AKPLATFORM::AkGetDefaultThreadProperties(prop);
//         prop.nPriority = AK_THREAD_PRIORITY_ABOVE_NORMAL;
//         int iIdealCore = (uWorkerIdx + 1) % 6;
//         //prop.affinityMask = AK_THREAD_AFFINITY_ALL;
//         prop.affinityMask = 1 << iIdealCore; // may want to uncomment if thread-migration gets in the way of profiling
//         prop.iIdealCore = iIdealCore;
//         jws.arThreadWorkerProperties[uWorkerIdx] = prop;

#elif defined (ITF_PS5)
//         AK::JobWorkerMgr::InitSettings& jws = m_jobWorkerSettings;
//         if (jws.uNumWorkerThreads > 0)
        //
//             AKASSERT(jws.uNumWorkerThreads <= AK_MAX_WORKER_THREADS);
//             jws.arThreadWorkerProperties = &Wwise::Configure::arThreadProperties[0];
//             for (int uWorkerIdx = 0; uWorkerIdx < jws.uNumWorkerThreads; uWorkerIdx++)
//             {
//                 AkThreadProperties prop;
//                 AKPLATFORM::AkGetDefaultThreadProperties(prop);
//                 prop.nPriority = AK_THREAD_PRIORITY_ABOVE_NORMAL;
//
//                 // fill up CCX0 first, with threads able to float across pairs of hw threads, and prefer to run threads "Solo" on a core, eg, 0x0003, 0x000C, 0x0030, 0x00C0, 0x0003, 0x000C, 0x0030, 0x00C0, 0x0300, 0x0C00...
//                 if (uWorkerIdx < 8)
//                 {
//                     prop.dwAffinityMask = 0x3 << ((uWorkerIdx % 4) * 2);
//                 }
//                 else if (uWorkerIdx < 12)
//                 {
//                     prop.dwAffinityMask = (0x3 << ((uWorkerIdx % 2) * 2)) << 8;
//                 }
//                 else
//                 {
//                     prop.dwAffinityMask = AK_THREAD_AFFINITY_ALL;
//                 }
//
//                 jws.arThreadWorkerProperties[uWorkerIdx] = prop;
//             }
//         }
#endif
    //
    // Create and initialize an instance of the default memory manager.
    //


        if (AK::MemoryMgr::Init(&in_memSettings) != AK_Success)
        {
            return false;
        }



		//
		// Create and initialize an instance of the default streaming manager. Note that
		// you can override the default streaming manager with your own. Refer to the SDK
		// documentation for more information. ;
		// Customize the Stream Manager settings here.
		//

		if(!AK_StreamMgr_Create(in_stmSettings))
		{
			return false;
		}

		//
		// Create a streaming device with blocking low-level I/O handshaking. Note that
		// you can override the default low-level I/O module with your own. Refer to the
		// SDK documentation for more information. ;
		// CAkFilePackageLowLevelIOBlocking::Init() creates a streaming device in the
		// Stream Manager, and registers itself as the File Location Resolver.
        //

#if defined( ITF_NX) || defined( ITF_OUNCE)
        in_deviceSettings.bUseStreamCache = true;
#endif
        AKRESULT res = m_pLowLevelIO->Init(in_deviceSettings);
		if(res != AK_Success)
		{
			return false;
		}



        if (m_jobWorkerSettings.uNumWorkerThreads > 0)
        {
            res = AK::JobWorkerMgr::InitWorkers(m_jobWorkerSettings);
            if (res != AK_Success)
            {
                return false;
            }
        }

		// Create the Sound Engine Using default initialization parameters
		res = AK_SoundEngine_Init(&in_initSettings, &in_platformInitSettings);
		if(res != AK_Success)
		{
			return false;
		}

		// Initialize the music engine Using default initialization parameters
		res = AK_MusicEngine_Init(&in_musicInit);
		if(res != AK_Success)
		{
			return false;
		}

#ifndef AK_OPTIMIZED
		// Initialize communications (not in release build!)
		AkCommSettings	commSettings;
		AK_Comm_GetDefaultInitSettings(commSettings);
        AK_Comm_Init(commSettings);
#endif // AK_OPTIMIZED

        AkSpatialAudioInitSettings settings;

        res = AK::SpatialAudio::Init(settings);
        if (res != AK_Success)
        {
            return false;
        }
        AK::SoundEngine::RegisterResourceMonitorCallback(ResourceMonitorDataCallback);

        AK::Monitor::SetLocalOutput(AK::Monitor::ErrorLevel_All, LocalErrorCallback);


		return true;
	}



	//
	// ===================================================================================================================
	// ===================================================================================================================
	//
	void Adapter_WWISE::destroy()
	{
		fastDestroy();
	}

	//
	// ===================================================================================================================
	// ===================================================================================================================
	//
	void Adapter_WWISE::fastDestroy()
	{
#ifndef ITF_FINAL
		traceBankLeaks();
#endif
		if(EVENTMANAGER && m_isRunning)
		{
// 			EVENTMANAGER->unregisterEvent(EventLoadWwiseBank_CRC, this);
// 			EVENTMANAGER->unregisterEvent(EventUnloadWwiseBank_CRC, this);
// 			EVENTMANAGER->unregisterEvent(EventSetWwiseState_CRC, this);
// 			EVENTMANAGER->unregisterEvent(EventPlayWwiseEvent_CRC, this);
// 			EVENTMANAGER->unregisterEvent(SendMetronomedEvent_CRC, this);
// 			EVENTMANAGER->unregisterEvent(EventSetWwiseAuxBusEffect_CRC, this);
// 			EVENTMANAGER->unregisterEvent(EventResetWwiseAuxBusEffect_CRC, this);
		}

        treatEndOfEvent();

        ClearUAF();

        //unregister mic gao

        AK_SoundEngine_UnregisterGameObj(static_cast<AkGameObjectID>(ListenerID::eDefaultListener));
        AK_SoundEngine_UnregisterGameObj(static_cast<AkGameObjectID>(ListenerID::eCenterSpeakerListener));


        // unregister global gao
        if (Wwise::AkGameObjectFactory::s_getSingleton())
        {
            Wwise::AkGameObjectFactory::s_getSingleton()->destroy(Wwise::AkGameObject::s_getAkGameObjectID(ITF_INVALID_OBJREF));
            Wwise::AkGameObjectFactory::s_destroySingleton();
        }

		unloadGlobalBank(); //to be done before destroy config

		if(m_config)
		{
			m_config->unloadConfigBank();

			const Path	tmpPath = m_config->getFile();
			TEMPLATEDATABASE->removeFromCache(tmpPath);
			m_config = NULL;
		}

		termWwise();
	}

	//
	// ===================================================================================================================
	// ===================================================================================================================
	//
	void Adapter_WWISE::termWwise()
	{
        AK::SoundEngine::UnregisterResourceMonitorCallback(ResourceMonitorDataCallback);

#ifndef AK_OPTIMIZED
		// Terminate communications between Wwise and the game
		AK_Comm_Term();
#endif // AK_OPTIMIZED

		// Terminate the music engine
		AK_MusicEngine_Term();

		// Terminate the sound engine
		if(AK_SoundEngine_IsInitialized())
		{
			AK_SoundEngine_Term();
		}
        // Stop and clean up the worker threads
        if (m_jobWorkerSettings.uNumWorkerThreads > 0)
        {
            AK::JobWorkerMgr::TermWorkers();
        }
		//
		// Terminate the streaming device and streaming manager
		// CAkFilePackageLowLevelIOBlocking::Term() destroys its associated streaming
		// device that lives in the Stream Manager, and unregisters itself as the File
		// Location Resolver.
		//
		if(AK_IAkStreamMgr_Get())
		{
			m_pLowLevelIO->UnloadAllFilePackages();
 			m_defaultPackageID = 0;
 			m_defaultLocalizedPackageID = 0;

			m_pLowLevelIO->Term();
			AK_IAkStreamMgr_Get()->Destroy();
		}

		// Terminate the Memory Manager
		if(AK_MemoryMgr_IsInitialized())
		{
			AK_MemoryMgr_Term();
		}



	}

	//
	// ===================================================================================================================
	// ===================================================================================================================
	//
	void Adapter_WWISE::loadConfig()
	{
		static const Path	soundConfig(GETPATH_ALIAS(Wwise::Configure::c_configFileName));
		if(!soundConfig.isEmpty())
		{

            TEMPLATEDATABASE->addTemplateClient(soundConfig.getStringID(), this);
            m_config = TEMPLATEDATABASE->getTemplate<SoundConfig_Template>(this, soundConfig);

            if (!m_config)
            {
                ITF_FATAL_ERROR("Couldn't load sound config: %ls", soundConfig.getString().cStr());
            }
			else
			{
				// create the GUI <-> ID map
				const ITF_VECTOR<WwiseItem> &ItemTable = m_config->getWwiseItemTable();
				for(ITF_VECTOR<WwiseItem>::const_iterator it = ItemTable.begin(), itEnd = ItemTable.end(); it != itEnd; ++it)
				{
					m_guidToID[it->getGUID()] = *it;
#ifdef ITF_DEBUG_STRINGID
                    m_wwiseGUID2NameMap[it->getGUID()] = it->getName();
                    m_wwiseID2NameMap[it->getID()] = it->getName();
#endif
				}

                m_guidRtpc = m_config->getWwiseRtpc();

                //create the localization map
                const ITF_VECTOR<SoundDialogItem>& DialogItemTable = m_config->getSoundDialogItemTable();
                for (ITF_VECTOR<SoundDialogItem>::const_iterator it = DialogItemTable.begin(), itEnd = DialogItemTable.end(); it != itEnd; ++it)
                {
                    m_locIdToSound[it->getLineID()] = *it;
                }


				// create the engine event ID table
				const ITF_VECTOR<WwiseEngineEvent>	&engineEventList = m_config->getWwiseEngineEventList();
				for(ITF_VECTOR<WwiseEngineEvent>::const_iterator it = engineEventList.begin(), itEnd = engineEventList.end(); it != itEnd; ++it)
				{
					const int	audioEngineEvent = static_cast<int>(it->getEngineEventType());
                //
					if((audioEngineEvent >= 0) && (audioEngineEvent < AUDIO_ENGEVT_COUNT))
					{
						m_engineEventIDs[audioEngineEvent] = getIDFromGUID(it->getGUID());
						if(m_engineEventIDs[audioEngineEvent] == ITF_INVALID_SOUND_EVENT_ID)
						{
							AUDIO_WARNING("\"Engine Event\" %d is not found. %s is unknown GUID", audioEngineEvent, it->getGUID().getDebugString());
						}
					}
				}

				// load common banks
				m_config->loadConfigBank();
			}
		}
	}



	//
	// ===================================================================================================================
	// ===================================================================================================================
	//
	void Adapter_WWISE::loadPackage(u32 _episode)
	{
		if(isRunning() == false)
			return;
#ifdef ITF_PROJECT_WW1
		AkOSChar	root[512];
		AkUInt32	packageID;
		AkOSChar	pck[512];

#if !defined(ITF_FINAL)
		m_failedBank.clear();
#endif

		if(m_usePackage == false)
			return;

		if(m_currentEpisode == _episode)
			return;

		m_currentEpisode = _episode;
		AudioSDK::safeStringCopy(root, Wwise::Configure::c_packageRootPath);
		AudioSDK::safeStringCat(root, Wwise::Helper::getWwisePlatformName(Wwise::Helper::CURRENT_PLATFORM));
		AudioSDK::safeStringCat(root, AKTEXT("/"));


		if( m_episodePackageID[1] == 0 ) //Ep1.pck + common.pck ( needed even for e2/e3 or e4)
		{
			AudioSDK::safeStringCopy(pck, root);
			AudioSDK::safeStringCat(pck, AKTEXT("Common"));
			AudioSDK::safeStringCat(pck, Wwise::Configure::c_packageExt);
#ifdef ITF_X360
			AudioSDK::stringReplace(pck, AKTEXT('/'), AKTEXT('\\'));
#endif
			if(m_pLowLevelIO->LoadFilePackage(pck, packageID) != AK_Success)
			{
				AUDIO_WARNING("Loading WWISE package : Common.pck \"%s\" has failed.", root);
			}
			priv_loadPackage(1);
		}

		if(m_currentEpisode!=1)
		{
			if(m_episodePackageID[0] == 0 )
			{
				AudioSDK::safeStringCopy(pck, root);
				AudioSDK::safeStringCat(pck, AKTEXT("Others")); //allsounds common to 2/3 or4 but not in 1
				AudioSDK::safeStringCat(pck, Wwise::Configure::c_packageExt);
#ifdef ITF_X360
				AudioSDK::stringReplace(pck, AKTEXT('/'), AKTEXT('\\'));
#endif
				if(m_pLowLevelIO->LoadFilePackage(pck, packageID) != AK_Success)
				{
					AUDIO_WARNING("Loading WWISE package: Others.pck \"%s\" has failed.", root);
				}
				m_episodePackageID[0] = packageID;
			}
			priv_loadPackage(m_currentEpisode);
		}
#endif
	}

#ifdef ITF_PROJECT_WW1
	void Adapter_WWISE::priv_loadPackage(u32 _Episode)
	{

		if( m_episodePackageID[_Episode] == 0 )  //PACKAGE has NOT been loaded yet
		{
			AkOSChar	root[512];
			AkOSChar	pck[512];
			AkUInt32	packageID;
			AkOSChar	szEpisodeName[20];

			AudioSDK::safeStringCopy(root, Wwise::Configure::c_packageRootPath);
			AudioSDK::safeStringCat(root, Wwise::Helper::getWwisePlatformName(Wwise::Helper::CURRENT_PLATFORM));
			AudioSDK::safeStringCat(root, AKTEXT("/"));
			AK_OSPRINTF(szEpisodeName, 20, AKTEXT("Ep%d"), _Episode);
			AudioSDK::safeStringCat(root, szEpisodeName);

			AudioSDK::safeStringCopy(pck, root);

			AudioSDK::safeStringCat(pck, Wwise::Configure::c_packageExt);
#ifdef ITF_X360
			AudioSDK::stringReplace(pck, AKTEXT('/'), AKTEXT('\\'));
#endif
			//get the package of the map
			if(m_pLowLevelIO->LoadFilePackage(pck, packageID) == AK_Success)
			{
				m_episodePackageID[_Episode] = packageID;
			}
			else
			{
				AUDIO_WARNING("Loading WWISE package \"%s\" has failed." , pck);

			}
			AudioSDK::safeStringCopy(pck, root);
			AudioSDK::safeStringCat(pck, AKTEXT("_"));
			AudioSDK::safeStringCat(pck, AK_StreamMgr_GetCurrentLanguage());
			AudioSDK::safeStringCat(pck, Wwise::Configure::c_packageExt);
#ifdef ITF_X360
			AudioSDK::stringReplace(pck, AKTEXT('/'), AKTEXT('\\'));
#endif
			if(m_pLowLevelIO->LoadFilePackage(pck, packageID) != AK_Success)
			{
				AUDIO_WARNING("Loading WWISE package \"%s\" has failed." , pck);
			}

		}

	}

#endif


	//
	// ===================================================================================================================
	// ===================================================================================================================
	//
	void Adapter_WWISE::unloadPackage(u32 _episode)
	{
			//TODO? for the moment all packages are closed when desinit engine
	}

	//
	// ===================================================================================================================
	// ===================================================================================================================
	//
	void Adapter_WWISE::loadDefaultPackages()
	{
		AkOSChar	root[512];
		AkUInt32	packageID;

		// root = "<c_packageRootPath>/<platform>/<c_defaultPackageName>.<c_packageExt>
		AudioSDK::safeStringCopy(root, Wwise::Configure::c_packageRootPath);
		AudioSDK::safeStringCat(root, Wwise::Helper::getWwisePlatformName(Wwise::Helper::CURRENT_PLATFORM));
		AudioSDK::safeStringCat(root, AKTEXT("/"));
		AudioSDK::safeStringCat(root, Wwise::Configure::c_defaultPackageName);
		AudioSDK::safeStringCat(root, Wwise::Configure::c_packageExt);

#ifdef ITF_X360
		AudioSDK::stringReplace(root, AKTEXT('/'), AKTEXT('\\'));
#endif

		if(m_pLowLevelIO->LoadFilePackage(root, packageID) == AK_Success)
		{
			m_defaultPackageID = packageID;
		}
		else
		{
			AUDIO_WARNING("Loading WWISE package \"%s\" has failed. Continue with bank loading mode", root);
		}

#ifndef ITF_PROJECT_WW1
		//
		// root =
		// "<c_packageRootPath>/<platform>/<AK_StreamMgr_GetCurrentLanguage>.<c_packageExt>"
		//
		AudioSDK::safeStringCopy(root, Wwise::Configure::c_packageRootPath);
		AudioSDK::safeStringCat(root, Wwise::Helper::getWwisePlatformName(Wwise::Helper::CURRENT_PLATFORM));
		AudioSDK::safeStringCat(root, AKTEXT("/"));
		AudioSDK::safeStringCat(root, AK_StreamMgr_GetCurrentLanguage());
		AudioSDK::safeStringCat(root, Wwise::Configure::c_packageExt);

#ifdef ITF_X360
		AudioSDK::stringReplace(root, AKTEXT('/'), AKTEXT('\\'));
#endif

		if(m_pLowLevelIO->LoadFilePackage(root, packageID) == AK_Success)
		{
			m_defaultLocalizedPackageID = packageID;
		}
// #else
// 		loadPackage(1); //temp load episode 1 at init
#endif
	}

}
#endif // ITF_SUPPORT_WWISE
