#include "precompiled_SoundAdapter_XAudio.h"
#ifdef ITF_SUPPORT_RAKI


#ifndef _ITF_SINGLETONS_H_
#include "engine/singleton/Singletons.h"
#endif //_ITF_SINGLETONS_H_

#ifndef _ITF_SOUNDADAPTER_H_XAUDIO_
#include "adapters/SoundAdapter_XAudio/SoundAdapter_XAudio.h"
#endif //_ITF_SOUNDADAPTER_H_XAUDIO_

#ifndef _ITF_FILEMANAGER_H_
#include "core/AdaptersInterfaces/FileManager.h"
#endif //_ITF_FILEMANAGER_H_

#ifndef _ITF_ERRORHANDLER_H_
#include "core/error/ErrorHandler.h"
#endif //_ITF_ERRORHANDLER_H_

#ifndef _ITF_WARNINGMANAGER_H_
#include "engine/common/WarningManager.h"
#endif // _ITF_WARNINGMANAGER_H_

#ifndef _ITF_SOUND_H_
#include "engine/sound/Sound.h"
#endif //_ITF_SOUND_H_

#ifndef _ITF_GFX_ADAPTER_H_
#include "engine/AdaptersInterfaces/GFXAdapter.h"
#endif //_ITF_GFX_ADAPTER_H_

#ifndef _ITF_SLOTALLOCATORMANAGER_H_
#include "core/memory/slotallocatorManager.h"
#endif //_ITF_SLOTALLOCATORMANAGER_H_

#ifndef _ITF_VIEW_H_
#include "engine/display/View.h"
#endif //_ITF_VIEW_H_

#ifndef _ITF_CAMERA_H_
#include "engine/display/Camera.h"
#endif //_ITF_CAMERA_H_

#ifndef _ITF_XAUDIOAPOBASE_H_
#include "adapters/SoundAdapter_XAudio/CaptureWin32/XAudioAPOBase.h"
#endif //_ITF_XAUDIOAPOBASE_H_

#ifdef ITF_SUPPORT_VIDEOCAPTURE
    #ifndef _ITF_CAPTUREAPO_H_
        #include "adapters/SoundAdapter_XAudio/CaptureWin32/CaptureAPO.h"
    #endif //_ITF_CAPTUREAPO_H_
#endif

#ifndef _ITF_RAKIINTERFACE_H_
#include "engine/sound/raki/RakiInterface.h"
#endif //_ITF_RAKIINTERFACE_H_

#ifndef RAKI_FORMAT_H
#include "RakiEngine/Sounds/Format.h"
#endif // RAKI_FORMAT_H

#ifndef RAKI_MEMORYBLOCK_H
#include "RakiEngine/Data/MemoryBlock/MemoryBlock.h"
#endif // RAKI_MEMORYBLOCK_H

#ifndef RAKI_WAVEFILE_H
#include "RakiEngine/Data/WaveFile/WaveFile.h"
#endif // RAKI_WAVEFILE_H

#ifndef _ITF_INSTANCEMANAGER_H_
#include "engine/sound/InstanceManager.h"
#endif // _ITF_INSTANCEMANAGER_H_

#ifndef RAKI_BUFFERSOUNDNATIVE_XAUDIO2_H
#include "RakiEngine/Sounds/_XAudio2/BufferSoundNative.h"
#endif // RAKI_BUFFERSOUNDNATIVE_XAUDIO2_H

#ifndef RAKI_RIFFCHUNKMANAGER_H
#include "RakiEngine/Data/WaveFile/RiffChunkManager.h"
#endif // RAKI_RIFFCHUNKMANAGER_H

#ifndef _ITF_READFILESTREAM_H_
#include "engine/sound/raki/ReadFileStream.h"
#endif //_ITF_READFILESTREAM_H_

#ifndef _ITF_SOUNDCOMPONENT_H_
#include "engine/actors/components/SoundComponent.h"
#endif //_ITF_SOUNDCOMPONENT_H_

#ifndef _ITF_RESOURCEMANAGER_H_
#include "engine/resources/ResourceManager.h"
#endif //_ITF_RESOURCEMANAGER_H_

#ifndef _ITF_SIMPLECACHEREADFILESTREAM_H_
#include "engine/sound/raki/SimpleCacheReadFileStream.h"
#endif // _ITF_SIMPLECACHEREADFILESTREAM_H_

#include "engine/AdaptersInterfaces/AudioMiddlewareAdapter_Types.h"

#ifdef ITF_X360 //Big-Endian
#define fourccRIFF 'RIFF'
#define fourccDATA 'data'
#define fourccFMT 'fmt '
#define fourccWAVE 'WAVE'
#define fourccXWMA 'XWMA'
#define fourccDPDS 'dpds'
#endif

#ifndef ITF_X360 //Little-Endian
#define fourccRIFF 'FFIR'
#define fourccDATA 'atad'
#define fourccFMT ' tmf'
#define fourccWAVE 'EVAW'
#define fourccXWMA 'AMWX'
#define fourccDPDS 'sdpd'
#endif

#define NUM_CHANNELS 2
#define SAMPLING_RATE 48000

namespace ITF
{
    enum SourceFlags
    {
        SourceFlags_Free = 0x1,
        SourceFlags_InUse = 0x2,
        SourceFlags_ToBeDestroyed = 0x4,
        SourceFlags_Done = 0x8,
        SourceFlags_Paused= 0x10,
        ENUM_FORCE_SIZE_32(SourceFlags)
    };

    //************************************
    // @name	ITF::ReadChunkData
    // @public 
    // @param	File * _f
    // @param	void * buffer
    // @param	DWORD buffersize
    // @param	DWORD bufferoffset
    // @return	HRESULT
    //************************************
    HRESULT ReadChunkData(File * _f, void * buffer, DWORD buffersize, DWORD bufferoffset)
    {
        ITF_ASSERT(_f);
        _f->seek(bufferoffset,begin);
        u32 dwRead;
        return (_f->read(buffer, buffersize, &dwRead) ? S_OK : S_FALSE);
    }

    //************************************
    // @name	ITF::FindChunk
    // @public 
    // @param	File * _f
    // @param	DWORD fourcc
    // @param	DWORD & dwChunkSize
    // @param	DWORD & dwChunkDataPosition
    // @return	HRESULT
    //************************************
    HRESULT FindChunk(File * _f, DWORD fourcc, DWORD & dwChunkSize, DWORD & dwChunkDataPosition)
    {

        ITF_ASSERT(_f);
        _f->seek(0,begin);

        DWORD dwChunkType;
        DWORD dwChunkDataSize;
        DWORD dwRIFFDataSize = 0;
        DWORD dwFileType;
        DWORD bytesRead = 0;
        DWORD dwOffset = 0;
        bbool result = btrue;
        while (result)
        {
            u32 dwRead;
            result = _f->read(&dwChunkType, sizeof(DWORD), &dwRead);
            if (!result)
                return S_FALSE;
            result = _f->read(&dwChunkDataSize, sizeof(DWORD), &dwRead);
            if (!result)
                return S_FALSE;

            switch (dwChunkType)
            {
            case fourccRIFF:
                dwRIFFDataSize = dwChunkDataSize;
                dwChunkDataSize = 4;
                result = _f->read(&dwFileType, sizeof(DWORD), &dwRead);
                if (!result)
                    return S_FALSE;
                break;

            default:
                _f->seek(dwChunkDataSize,current);
            }

            dwOffset += sizeof(DWORD) * 2;

            if (dwChunkType == fourcc)
            {
                dwChunkSize = dwChunkDataSize;
                dwChunkDataPosition = dwOffset;
                return S_OK;
            }

            dwOffset += dwChunkDataSize;

            if (bytesRead >= dwRIFFDataSize) return S_FALSE;

        }


        return S_OK;

    }    

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////

    struct SoundResourceData
    {
        union 
        {
            WAVEFORMATEX wfx;
            WAVEFORMATEXTENSIBLE wfxtensible;
            ADPCMWAVEFORMAT adpcm;
            XMA2WAVEFORMATEX xma2;
        } formatUnion;

        u32 size;
        void * data;
        XAUDIO2_BUFFER_WMA wma;
        SoundDataFormat format;
        ITF_INLINE SoundResourceData() : size(0),data(NULL),format(SoundDataFormat_Uncompressed)
        {
            ITF_MemSet(&formatUnion,0,sizeof(formatUnion));
            wma.pDecodedPacketCumulativeBytes  = NULL;
            wma.PacketCount = 0;
        }
        ITF_INLINE ~SoundResourceData()
        {
            if (wma.pDecodedPacketCumulativeBytes)
                free((void*)wma.pDecodedPacketCumulativeBytes);//TODO Neil : remove the free usage
        }
    };

    class SourceXAudio 
    {
    public:
        SourceXAudio() 
            : voice(NULL)
            , callback(NULL)
        {
            buffer.Flags = 0;
            buffer.AudioBytes = 0;
            buffer.pAudioData = NULL;
            buffer.PlayBegin = 0;
            buffer.PlayLength = 0;
            buffer.LoopBegin = 0;
            buffer.LoopLength = 0;
            buffer.LoopCount = 0;
            buffer.pContext = NULL;
        };
        virtual ~SourceXAudio(){};

        IXAudio2SourceVoice * voice;
        XAUDIO2_BUFFER buffer;
        IXAudio2VoiceCallback * callback;
        SourceInfo params;
    };

    Bus::~Bus()
    {
        m_inputs.clear();
		if (m_voice)
			m_voice->DestroyVoice();
    }

    //************************************
    // @name	ITF::SoundAdapter_XAudio::SoundAdapter_XAudio
    // @public 
    // @return	
    //************************************
    SoundAdapter_XAudio::SoundAdapter_XAudio() : 
        m_xAudio2(NULL),
        m_masterVoice(NULL),
        m_sources(NULL),
        m_perf(NULL),
        m_debug(false), 
        m_operationSet(1)
    {

#ifndef ITF_X360
        CoInitializeEx(NULL, COINIT_MULTITHREADED);
#endif
        m_xAudio2 = NULL;
        HRESULT hr;


// $STB removed => init is failing in debug mode
#if 0 // defined(_DEBUG)
        u32 flags = XAUDIO2_DEBUG_ENGINE;
#else 
        u32 flags = 0;
#endif // 

#ifndef _RETAIL
        m_perf = newAlloc(mId_AudioEngine,XAUDIO2_PERFORMANCE_DATA);
#endif //!_RETAIL

        if ( FAILED(hr = XAudio2Create( &m_xAudio2, flags, XAUDIO2_DEFAULT_PROCESSOR ) ) )
        {
            ITF_FATAL_ERROR("Could not create sound device - you may have to install latest DirectX drivers");
            LOG("<< SoundAdapter >> failed to create XAudio2 %x\n", hr);
            return;
        }
#ifdef _DEBUG
        XAUDIO2_DEBUG_CONFIGURATION debugConfig;
        debugConfig.TraceMask = XAUDIO2_LOG_ERRORS|XAUDIO2_LOG_WARNINGS ;//|XAUDIO2_LOG_API_CALLS;//|XAUDIO2_LOG_FUNC_CALLS;//| XAUDIO2_LOG_INFO ; //XAUDIO2_LOG_DETAIL ;//|
        //         XAUDIO2_LOG_API_CALLS | /*XAUDIO2_LOG_FUNC_CALLS  |*/ XAUDIO2_LOG_TIMING | /*XAUDIO2_LOG_LOCKS |*/ ;//XAUDIO2_LOG_MEMORY |
        //XAUDIO2_LOG_STREAMING;

        debugConfig.BreakMask = 0;//XAUDIO2_LOG_ERRORS;
        debugConfig.LogThreadID = TRUE;
        debugConfig.LogFileline = TRUE;
        debugConfig.LogFunctionName = TRUE;
        debugConfig.LogTiming = TRUE;
        m_xAudio2->SetDebugConfiguration(&debugConfig);
#endif

        printConfig();


        m_masterVoice = NULL;

#if defined(ITF_WINDOWS) && defined(ITF_SUPPORT_VIDEOCAPTURE)
       m_pCaptureAPO = NULL;
#endif //ITF_WINDOWS


       // RakiEngine initialization 

       RakiInterface::InitStruct initStruct; 

#if defined ( ITF_WINDOWS ) || defined ( ITF_X360 )
       initStruct.m_xaudio2 = m_xAudio2;
#endif //ITF_WINDOWS

       RAKIINTERFACE->initLowLevelAudio( &initStruct );
    }


    //************************************
    // @name	ITF::SoundAdapter_XAudio::~SoundAdapter_XAudio
    // @public 
    // @return	
    //************************************
    SoundAdapter_XAudio::~SoundAdapter_XAudio()
    {
        destroy();
    }

	//************************************
	// @name	ITF::SoundAdapter_XAudio::init
	// @public 
	// @return	ITF::bbool
	//************************************
    bbool SoundAdapter_XAudio::init(bbool _audioCaptureAllowed)
	{
        bbool res=SoundAdapter::init(_audioCaptureAllowed);
        if (res)
        {
            res = initInternal();
        }
        return res;
	}


	//************************************
	// @name	ITF::SoundAdapter_XAudio::isRunning
	// @public 
	// @return	ITF::bbool
	//************************************
	bbool SoundAdapter_XAudio::isRunning() const
	{
        return m_xAudio2 != NULL;
	}


	//************************************
	// @name	ITF::SoundAdapter_XAudio::shutdown
	// @public 
	// @return	void
	//************************************
	void SoundAdapter_XAudio::shutdown()
	{
        SoundAdapter::shutdown();
	}


	//************************************
	// @name	ITF::SoundAdapter_XAudio::destroy
	// @public 
	// @return	void
	//************************************
    void SoundAdapter_XAudio::destroy()
    {
        if (m_xAudio2)
        {
            if (m_masterVoice)
            {
                m_masterVoice->DestroyVoice();
                m_masterVoice = NULL;
            }

            m_xAudio2->Release();
            m_xAudio2 = NULL;
    #ifndef ITF_X360
            CoUninitialize();
    #endif //ITF_X360
        }

    #ifndef _RETAIL
        if (m_perf)
            delete m_perf;
        m_perf = NULL;
    #endif //!_RETAIL


    #if defined (ITF_WINDOWS) && defined(ITF_SUPPORT_VIDEOCAPTURE)
        SF_DEL(m_pCaptureAPO);
    #endif //
    }


	//************************************
	// @name	ITF::SoundAdapter_XAudio::loadSound
	// @public 
	// @param	Sound * _sound
	// @param	const char * _filename
	// @return	ITF::bbool
	//************************************
	bbool SoundAdapter_XAudio::loadSound(Sound * _sound, const char * _path )
	{
        raki::RiffChunkManager::singleton().setShouldReadDataBlockForWaveData( true );

        SimpleCacheReadFileStream file;
        if ( !file.open( _path ) )
        {
            ITF_WARNING( NULL, bfalse, "<< SoundAdapter >> Unable to create file" );
            return bfalse;
        }

        raki::WaveFile * waveFile = newAlloc( mId_AudioEngine, raki::WaveFile );
        raki::Serializer serializer( &file );

        waveFile->serialize( serializer );

        char fmtTag[] = "fmt ";
        char dataTag[] = "data";

        if ( !waveFile->getChunkData( fmtTag ) || !waveFile->getChunkData( dataTag ) )
        {
            ITF_WARNING( NULL, bfalse, "<< SoundAdapter >> File has invalid format >> %s", _path );
            delete waveFile;
            return bfalse;
        }

        _sound->m_adapterImplementationData = waveFile;

#ifndef ITF_FINAL
        _sound->setMemoryUsage( waveFile->getChunkSize( dataTag ) );
#endif

        RESOURCE_MANAGER->addResourceAsLoaded( _sound );

        return btrue;

	}

	//************************************
	// @name	ITF::SoundAdapter_XAudio::unloadSound
	// @public 
	// @param	Sound * _sound
	// @return	void
	//************************************
	void SoundAdapter_XAudio::unloadSound(Sound * _sound)
	{
        ITF_ASSERT(_sound);
        raki::WaveFile * waveFile = (raki::WaveFile*)_sound->m_adapterImplementationData;
        if (!waveFile)
            return;

        delete waveFile;

        _sound->m_adapterImplementationData = NULL;
	}

    f32 SoundAdapter_XAudio::getSoundDuration( const Sound * _sound )
    {
        if (!_sound)
            return 0.0f;
#ifndef ITF_FINAL
        SoundResourceData * resourceData = (SoundResourceData*)_sound->m_adapterImplementationData;
        if (!resourceData)
            return 0.0f;
        const u32 bitsPerSample = resourceData->formatUnion.wfx.wBitsPerSample * resourceData->formatUnion.wfx.nChannels;
        const u32 samplesPerSecond = resourceData->formatUnion.wfx.nSamplesPerSec;
        const u32 bitsPerSecond = bitsPerSample * samplesPerSecond;
        const u32 sizeInBits = _sound->getMemoryUsage() << 3;
        if (bitsPerSecond == 0)
        {
            ITF_WARNING( NULL, bfalse, "<< SoundAdapter >> Sound data are incorrect" );
            return 0.0f;
        }
        return (f32) (sizeInBits / bitsPerSecond);
#else
        return 0.0f;
#endif
    }

#ifndef ITF_FINAL
    void SoundAdapter_XAudio::getPerformanceDebugInfo( String &debugInfo, char paramDelimiter /*= '\n'*/, char valueDelimiter /*= ':'*/ )
    {
        ITF_ASSERT(m_perf);
        m_xAudio2->GetPerformanceData(m_perf);
        debugInfo.setTextFormat(
            "AudioCyclesSinceLastQuery: %I64u\nTotalCyclesSinceLastQuery: %I64u\nMinimumCyclesPerQuantum: %u\nMaximumCyclesPerQuantum: %u\nMemoryUsageInBytes: %u\nCurrentLatencyInSamples: %u\nGlitchesSinceEngineStarted: %u\nActiveSourceVoiceCount: %u\nTotalSourceVoiceCount: %u\nActiveSubmixVoiceCount: %u\nActiveResamplerCount: %u\nActiveMatrixMixCount: %u\nActiveXmaSourceVoices: %u\nActiveXmaStreams: %u\nActiveXmaSourceVoices: %u\nActiveXmaStreams: %u",
            m_perf->AudioCyclesSinceLastQuery,
            m_perf->TotalCyclesSinceLastQuery,
            m_perf->MinimumCyclesPerQuantum,
            m_perf->MaximumCyclesPerQuantum,
            m_perf->MemoryUsageInBytes,
            m_perf->CurrentLatencyInSamples,
            m_perf->GlitchesSinceEngineStarted,
            m_perf->ActiveSourceVoiceCount,
            m_perf->TotalSourceVoiceCount,
            m_perf->ActiveSubmixVoiceCount,
            m_perf->ActiveResamplerCount,
            m_perf->ActiveMatrixMixCount,
            m_perf->ActiveXmaSourceVoices,
            m_perf->ActiveXmaStreams,
            m_perf->ActiveXmaSourceVoices,
            m_perf->ActiveXmaStreams);
    }
#endif

	//************************************
	// @name	ITF::SoundAdapter_XAudio::createSource
	// @virtual private 
	// @param	const SourceInfo * _params
	// @return	ITF::SourceHandle
	//************************************
	SourceHandle SoundAdapter_XAudio::createSource(SourceInfo * _params)
	{
		return createSource(_params,NULL);
	}

    //************************************
    // @name	ITF::SoundAdapter_XAudio::toggleDebug
    // @public 
    // @return	void
    //************************************
    void SoundAdapter_XAudio::toggleDebug()
    {
        m_debug = !m_debug;
    }
 
    //************************************
    // @name	ITF::SoundAdapter_XAudio::isDebug
    // @public 
    // @return	ITF::bbool
    //************************************
    bbool SoundAdapter_XAudio::isDebug()
    {
        return m_debug;
    }

    //************************************
    // @name	ITF::SoundAdapter_XAudio::setMasterVolume
    // @public 
    // @param	f32 _volume
    // @return	void
    //************************************
    void SoundAdapter_XAudio::setMasterVolume(f32 _volume)
    {
        m_masterVoice->SetVolume(_volume,m_operationSet);
    }

    //************************************
    // @name	ITF::SoundAdapter_XAudio::createSource
    // @virtual private 
    // @param	const SourceInfo * _params
    // @param	IXAudio2VoiceCallback * _callback
    // @return	ITF::SourceHandle
    //************************************
    SourceHandle SoundAdapter_XAudio::createSource(SourceInfo * _params, IXAudio2VoiceCallback * _callback)
    {
        if (m_instanceManager == NULL || _params == NULL || _params->m_sound == NULL)
            return 0xFFFFFFFF;

        ITF_ASSERT_MSG( ((Sound*)_params->m_sound)->isPhysicallyLoaded(), "Trying to play sound before it is finished loading" );

        if ( !((Sound*)_params->m_sound)->isPhysicallyLoaded() )
            return 0xFFFFFFFF;

        const bool isMusic = _params->m_bus == ITF_GET_STRINGID_CRC(MUSIC,863906348);
        const bool isLooping = _params->m_loop ? true : false;
        raki::Format format( isMusic, isLooping );

        raki::WaveFile * waveFile = (raki::WaveFile*)_params->m_sound->m_adapterImplementationData;

        char dataChunk[] = "data";
        raki::MemoryBlock dataBlock( waveFile->getChunkData( dataChunk ), waveFile->getChunkSize( dataChunk ) );

        char fmtChunk[] = "fmt ";
        format.setData( (WAVEFORMATEX*) waveFile->getChunkData( fmtChunk ), waveFile->getChunkSize( fmtChunk ), &dataBlock );

#if ITF_X360
        u32 * seekTable = (u32*)waveFile->getChunkData( "seek" );
        u32 nbSeekBlocks = waveFile->getChunkSize( "seek" ) / sizeof( u32 );
        u32 sampleSize = seekTable[ nbSeekBlocks - 1 ];
        if ( sampleSize > 768 )
            sampleSize -= 768;
        format.setLoopSampleSize( sampleSize );
#endif // ITF_X360

        return m_instanceManager->createSource( &format, _params->m_bus );
    }


    //************************************
    // @name	ITF::SoundAdapter_XAudio::initInternal
    // @public 
    // @return	ITF::bbool
    //************************************
    ITF::bbool SoundAdapter_XAudio::initInternal()
    {
        if (!isRunning())
            return bfalse;


#ifdef ITF_WINDOWS
        u32 preferredDevice = 0;
        u32 deviceCount = 0;
        m_xAudio2->GetDeviceCount(&deviceCount);
        XAUDIO2_DEVICE_DETAILS details;
        for (u32 i = 0; i < deviceCount; ++i)
        {
            m_xAudio2->GetDeviceDetails(i,&details);
            if (details.Role == GlobalDefaultDevice)
            {
                char deviceId[MAX_PATH];
                char deviceName[MAX_PATH];
                size_t num;
                wcstombs_s(
                    &num,
                    deviceId,
                    sizeof(deviceId),
                    details.DeviceID,
                    sizeof(details.DeviceID) 
                    );
                wcstombs_s(
                    &num,
                    deviceName,
                    sizeof(deviceName),
                    details.DisplayName,
                    sizeof(details.DisplayName) 
                    );
                LOG("<<<< Sound using device %s %s >>>>",deviceId,deviceName);
                preferredDevice = i;
                //                    m_numChannels = details.OutputFormat.Format.nChannels;

            }
        }
#endif

        u32 effectCount = 0;

        IUnknown * pVolumeMeterAPO;
        XAudio2CreateVolumeMeter(&pVolumeMeterAPO);
        XAUDIO2_EFFECT_DESCRIPTOR descriptor[3];
        IUnknown * pReverbAPO;
        XAudio2CreateReverb(&pReverbAPO);

        descriptor[0].InitialState = false;
        descriptor[0].OutputChannels = NUM_CHANNELS;
        descriptor[0].pEffect = pReverbAPO;
        effectCount++;

        descriptor[1].InitialState = true;
        descriptor[1].OutputChannels = NUM_CHANNELS;
        descriptor[1].pEffect = pVolumeMeterAPO;
        effectCount++;


#if defined (ITF_WINDOWS) && defined(ITF_SUPPORT_VIDEOCAPTURE)
        if (m_bAudioCaptureAllowed)
        {
            CaptureAPO::CreateInstance( NULL, 0, &m_pCaptureAPO );
            m_pCaptureAPO->init(NUM_CHANNELS,SAMPLING_RATE);

            descriptor[2].InitialState = true;
            descriptor[2].OutputChannels = 2;
            descriptor[2].pEffect = static_cast<IXAPO*>(m_pCaptureAPO);
            effectCount++;
        }
#endif // ITF_WINDOWS && ITF_SUPPORT_VIDEOCAPTURE

        XAUDIO2_EFFECT_CHAIN chain;
        chain.EffectCount = effectCount++;;
        chain.pEffectDescriptors = descriptor;


        //
        HRESULT hr;
        if ( FAILED(hr = m_xAudio2->CreateMasteringVoice( &m_masterVoice, NUM_CHANNELS,SAMPLING_RATE, 0, 0, &chain ) ) )
        {
            ITF_ERROR("Unable to create sound master voice");
            m_xAudio2->Release();
            m_xAudio2 = NULL;
            LOG("<< SoundAdapter >> failed to create master voice");
            return false;
        }
        //m_busses[masterBus]->setVoice(m_masterVoice);


/*    
    initReverbPresets();

        

        const SoundConfig_Template * config = SOUND_ADAPTER->getConfig();
        u32 numBusses = config->getBusDefs().size();
        for (u32 i = 0; i < numBusses; ++i)
        {
            Bus * bus = newAlloc(mId_Audio,Bus(&config->getBusDefs()[i]));
            m_busses[bus->getName()] = bus;
        }


        ITF_MAP<StringID,Bus*>::iterator it = m_busses.find(masterBus);
        ITF_ASSERT_CRASH(it != m_busses.end(),"You must define the master bus in your soundconfig.isg");

        //resolve inputs
        for (ITF_MAP<StringID,Bus*>::const_iterator it = m_busses.begin(); it != m_busses.end(); ++it)
        {
            Bus * bus = it->second;
            u32 numOutputs = bus->getOutputs().size();

            ITF_ASSERT_CRASH((numOutputs == 0 && bus->getName() == masterBus) || (numOutputs > 0),
                "Bus %s has no output defined",bus->getName().getDebugString());

            for (u32 i = 0; i < numOutputs; ++i)
            {
                //tell its output it has it as an input
                if (bus->getOutputs()[i].isValid())
                    m_busses[bus->getOutputs()[i]]->addInput(it->first);
            }
        }

        // calculate processing stage
        for (ITF_MAP<StringID,Bus*>::const_iterator it = m_busses.begin(); it != m_busses.end(); ++it)
        {
            Bus * bus = it->second;
            bus->setProcessingStage(getProcessingStage(bus,1000));
        }

        //create master voice first
        createSubmixVoice(m_busses[masterBus]);
        if (!isRunning())
            return bfalse;

        // create voices
        for (ITF_MAP<StringID,Bus*>::const_iterator it = m_busses.begin(); it != m_busses.end(); ++it)
        {
            Bus * bus = it->second;
            if (bus->getName() != masterBus)
                createSubmixVoice(bus);
        }

        // create XAudio2 voice pool 

        const int nbPoolVoices = MAX_VOICES + 16; // add 8 for music 
        IXAudio2SourceVoice * poolVoiceArray[ nbPoolVoices ];

        WAVEFORMATEX tempPoolFormat;
        tempPoolFormat.wFormatTag = WAVE_FORMAT_PCM;
        tempPoolFormat.nChannels = 2;
        tempPoolFormat.nSamplesPerSec = 48000;
        tempPoolFormat.wBitsPerSample = 16;
        tempPoolFormat.nBlockAlign = ( tempPoolFormat.wBitsPerSample / 8 ) * tempPoolFormat.nChannels;
        tempPoolFormat.nAvgBytesPerSec = tempPoolFormat.nBlockAlign * tempPoolFormat.nSamplesPerSec; 
        tempPoolFormat.cbSize = 0;

        int i;
        // allocate voices in pool 
        for ( i = 0 ; i < nbPoolVoices ; ++i )
        {
            poolVoiceArray[ i ] = NULL;
            if FAILED( m_xAudio2->CreateSourceVoice( &poolVoiceArray[ i ], &tempPoolFormat ) )
                poolVoiceArray[ i ] = NULL;
        }

        // destroy voices in pool 
        for ( i = 0 ; i < nbPoolVoices ; ++i )
        {
            if ( poolVoiceArray[ i ] )
                poolVoiceArray[ i ]->DestroyVoice();
            poolVoiceArray[ i ] = NULL;
        }
*/

        return btrue;
    }

    //************************************
    // @name	ITF::SoundAdapter_XAudio::destroySubmixVoice
    // @private 
    // @param	Bus * _bus
    // @return	void
    //************************************
    void SoundAdapter_XAudio::destroySubmixVoice(Bus * _bus)
    {
/*        for (u32 i = 0; i < _bus->getInputs().size(); ++i)
        {
            destroySubmixVoice(m_busses[_bus->getInputs()[i]]);
        }*/
        delete _bus;
    }


    //************************************
    // @name	ITF::SoundAdapter_XAudio::createSubmixVoice
    // @private 
    // @param	Bus * _bus
    // @param	u32 _processingStage
    // @return	void
    //************************************
    void SoundAdapter_XAudio::createSubmixVoice(Bus * _bus)
    {
/*        if (_bus->getName() != masterBus)
        {

            u32 flags = _bus->getFilterType() != FilterType_None?XAUDIO2_VOICE_USEFILTER:0;

            const XAUDIO2_EFFECT_CHAIN * effectChain = NULL;

            XAUDIO2_EFFECT_DESCRIPTOR descriptor[1];
            XAUDIO2_EFFECT_CHAIN chain;

            if (_bus->getHasReverb())
            {

                chain.EffectCount = 1;
                chain.pEffectDescriptors = descriptor;

                IUnknown * pReverbAPO;
                XAudio2CreateReverb(&pReverbAPO);

                descriptor[0].InitialState = _bus->getReverbActive();
                descriptor[0].OutputChannels = NUM_CHANNELS;
                descriptor[0].pEffect = pReverbAPO;

                effectChain = &chain;
            }

            IXAudio2Voice * voice = NULL;
            m_xAudio2->CreateSubmixVoice((IXAudio2SubmixVoice**)(&voice),
                NUM_CHANNELS,
                SAMPLING_RATE,
                flags,
                _bus->getProcessingStage(),
                NULL,
                effectChain);
            _bus->setVoice(voice);

        }
        else
        {
 
#ifdef ITF_WINDOWS
            u32 preferredDevice = 0;
            u32 deviceCount = 0;
            m_xAudio2->GetDeviceCount(&deviceCount);
            XAUDIO2_DEVICE_DETAILS details;
            for (u32 i = 0; i < deviceCount; ++i)
            {
                m_xAudio2->GetDeviceDetails(i,&details);
                if (details.Role == GlobalDefaultDevice)
                {
                    char deviceId[MAX_PATH];
                    char deviceName[MAX_PATH];
                    size_t num;
                    wcstombs_s(
                        &num,
                        deviceId,
                        sizeof(deviceId),
                        details.DeviceID,
                        sizeof(details.DeviceID) 
                        );
                    wcstombs_s(
                        &num,
                        deviceName,
                        sizeof(deviceName),
                        details.DisplayName,
                        sizeof(details.DisplayName) 
                        );
                    LOG("<<<< Sound using device %s %s >>>>",deviceId,deviceName);
                    preferredDevice = i;
//                    m_numChannels = details.OutputFormat.Format.nChannels;

                }
            }
#endif

            u32 effectCount = 0;

            IUnknown * pVolumeMeterAPO;
            XAudio2CreateVolumeMeter(&pVolumeMeterAPO);
            XAUDIO2_EFFECT_DESCRIPTOR descriptor[3];
            IUnknown * pReverbAPO;
            XAudio2CreateReverb(&pReverbAPO);

            descriptor[0].InitialState = false;
            descriptor[0].OutputChannels = NUM_CHANNELS;
            descriptor[0].pEffect = pReverbAPO;
            effectCount++;

            descriptor[1].InitialState = true;
            descriptor[1].OutputChannels = NUM_CHANNELS;
            descriptor[1].pEffect = pVolumeMeterAPO;
            effectCount++;


#if defined (ITF_WINDOWS) && !defined (ITF_FINAL)
            if (m_bAudioCaptureAllowed)
            {
                CaptureAPO::CreateInstance( NULL, 0, &m_pCaptureAPO );
                m_pCaptureAPO->init(NUM_CHANNELS,SAMPLING_RATE);

                descriptor[2].InitialState = true;
                descriptor[2].OutputChannels = 2;
                descriptor[2].pEffect = static_cast<IXAPO*>(m_pCaptureAPO);
                effectCount++;
            }
#endif //ITF_WINDOWS && !ITF_FINAL

            XAUDIO2_EFFECT_CHAIN chain;
            chain.EffectCount = effectCount++;;
            chain.pEffectDescriptors = descriptor;


            //
            HRESULT hr;
            if ( FAILED(hr = m_xAudio2->CreateMasteringVoice( &m_masterVoice, NUM_CHANNELS,SAMPLING_RATE, 0, 0, &chain ) ) )
            {
                ITF_ERROR("Unable to create sound master voice");
                m_xAudio2->Release();
                m_xAudio2 = NULL;
                LOG("<< SoundAdapter >> failed to create master voice");
                return;
            }
            m_busses[masterBus]->setVoice(m_masterVoice);
        }
        */
    }


    //************************************
    // @name	ITF::SoundAdapter_XAudio::getReverbParams
    // @private 
    // @param	ReverbPreset _preset
    // @return	XAUDIO2FX_REVERB_PARAMETERS *
    //************************************
/*    XAUDIO2FX_REVERB_PARAMETERS * SoundAdapter_XAudio::getReverbParams( ReverbPreset _preset )
    {
        if (_preset < ReverbPreset_COUNT)
            return &m_reverbParams[_preset];
        else
            return NULL;
    }

    //************************************
    // @name	ITF::SoundAdapter_XAudio::initReverbPresets
    // @public 
    // @return	void
    //************************************
    void SoundAdapter_XAudio::initReverbPresets()
    {
        for (u32 i = 0; i < ReverbPreset_COUNT; ++i)
        {
            switch((ReverbPreset)i)
            {
            case ReverbPreset_DEFAULT:
                {
                    XAUDIO2FX_REVERB_I3DL2_PARAMETERS params = XAUDIO2FX_I3DL2_PRESET_DEFAULT;
                    ReverbConvertI3DL2ToNative(&params,&m_reverbParams[i]);
                }
                break;
            case ReverbPreset_GENERIC:
                {
                    XAUDIO2FX_REVERB_I3DL2_PARAMETERS params = XAUDIO2FX_I3DL2_PRESET_GENERIC;
                    ReverbConvertI3DL2ToNative(&params,&m_reverbParams[i]);
                }
                break;
            case ReverbPreset_PADDEDCELL:
                {
                    XAUDIO2FX_REVERB_I3DL2_PARAMETERS params = XAUDIO2FX_I3DL2_PRESET_PADDEDCELL;
                    ReverbConvertI3DL2ToNative(&params,&m_reverbParams[i]);
                }
                break;
            case ReverbPreset_ROOM:
                {
                    XAUDIO2FX_REVERB_I3DL2_PARAMETERS params = XAUDIO2FX_I3DL2_PRESET_ROOM;
                    ReverbConvertI3DL2ToNative(&params,&m_reverbParams[i]);
                }
                break;
            case ReverbPreset_BATHROOM:
                {
                    XAUDIO2FX_REVERB_I3DL2_PARAMETERS params = XAUDIO2FX_I3DL2_PRESET_BATHROOM;
                    ReverbConvertI3DL2ToNative(&params,&m_reverbParams[i]);
                }
                break;
            case ReverbPreset_LIVINGROOM:
                {
                    XAUDIO2FX_REVERB_I3DL2_PARAMETERS params = XAUDIO2FX_I3DL2_PRESET_LIVINGROOM;
                    ReverbConvertI3DL2ToNative(&params,&m_reverbParams[i]);
                }
                break;
            case ReverbPreset_STONEROOM:
                {
                    XAUDIO2FX_REVERB_I3DL2_PARAMETERS params = XAUDIO2FX_I3DL2_PRESET_STONEROOM;
                    ReverbConvertI3DL2ToNative(&params,&m_reverbParams[i]);
                }
                break;
            case ReverbPreset_AUDITORIUM:
                {
                    XAUDIO2FX_REVERB_I3DL2_PARAMETERS params = XAUDIO2FX_I3DL2_PRESET_AUDITORIUM;
                    ReverbConvertI3DL2ToNative(&params,&m_reverbParams[i]);
                }
                break;
            case ReverbPreset_CONCERTHALL:
                {
                    XAUDIO2FX_REVERB_I3DL2_PARAMETERS params = XAUDIO2FX_I3DL2_PRESET_CONCERTHALL;
                    ReverbConvertI3DL2ToNative(&params,&m_reverbParams[i]);
                }
                break;
            case ReverbPreset_CAVE:
                {
                    XAUDIO2FX_REVERB_I3DL2_PARAMETERS params = XAUDIO2FX_I3DL2_PRESET_CAVE;
                    ReverbConvertI3DL2ToNative(&params,&m_reverbParams[i]);
                }
                break;
            case ReverbPreset_ARENA:
                {
                    XAUDIO2FX_REVERB_I3DL2_PARAMETERS params = XAUDIO2FX_I3DL2_PRESET_ARENA;
                    ReverbConvertI3DL2ToNative(&params,&m_reverbParams[i]);
                }
                break;
            case ReverbPreset_HANGAR:
                {
                    XAUDIO2FX_REVERB_I3DL2_PARAMETERS params = XAUDIO2FX_I3DL2_PRESET_HANGAR;
                    ReverbConvertI3DL2ToNative(&params,&m_reverbParams[i]);
                }
                break;
            case ReverbPreset_CARPETEDHALLWAY:
                {
                    XAUDIO2FX_REVERB_I3DL2_PARAMETERS params = XAUDIO2FX_I3DL2_PRESET_CARPETEDHALLWAY;
                    ReverbConvertI3DL2ToNative(&params,&m_reverbParams[i]);
                }
                break;
            case ReverbPreset_HALLWAY:
                {
                    XAUDIO2FX_REVERB_I3DL2_PARAMETERS params = XAUDIO2FX_I3DL2_PRESET_HALLWAY;
                    ReverbConvertI3DL2ToNative(&params,&m_reverbParams[i]);
                }
                break;
            case ReverbPreset_STONECORRIDOR:
                {
                    XAUDIO2FX_REVERB_I3DL2_PARAMETERS params = XAUDIO2FX_I3DL2_PRESET_STONECORRIDOR;
                    ReverbConvertI3DL2ToNative(&params,&m_reverbParams[i]);
                }
                break;
            case ReverbPreset_ALLEY:
                {
                    XAUDIO2FX_REVERB_I3DL2_PARAMETERS params = XAUDIO2FX_I3DL2_PRESET_ALLEY;
                    ReverbConvertI3DL2ToNative(&params,&m_reverbParams[i]);
                }
                break;
            case ReverbPreset_FOREST:
                {
                    XAUDIO2FX_REVERB_I3DL2_PARAMETERS params = XAUDIO2FX_I3DL2_PRESET_FOREST;
                    ReverbConvertI3DL2ToNative(&params,&m_reverbParams[i]);
                }
                break;
            case ReverbPreset_CITY:
                {
                    XAUDIO2FX_REVERB_I3DL2_PARAMETERS params = XAUDIO2FX_I3DL2_PRESET_CITY;
                    ReverbConvertI3DL2ToNative(&params,&m_reverbParams[i]);
                }
                break;
            case ReverbPreset_MOUNTAINS:
                {
                    XAUDIO2FX_REVERB_I3DL2_PARAMETERS params = XAUDIO2FX_I3DL2_PRESET_MOUNTAINS;
                    ReverbConvertI3DL2ToNative(&params,&m_reverbParams[i]);
                }
                break;
            case ReverbPreset_QUARRY:
                {
                    XAUDIO2FX_REVERB_I3DL2_PARAMETERS params = XAUDIO2FX_I3DL2_PRESET_QUARRY;
                    ReverbConvertI3DL2ToNative(&params,&m_reverbParams[i]);
                }
                break;
            case ReverbPreset_PLAIN:
                {
                    XAUDIO2FX_REVERB_I3DL2_PARAMETERS params = XAUDIO2FX_I3DL2_PRESET_PLAIN;
                    ReverbConvertI3DL2ToNative(&params,&m_reverbParams[i]);
                }
                break;
            case ReverbPreset_PARKINGLOT:
                {
                    XAUDIO2FX_REVERB_I3DL2_PARAMETERS params = XAUDIO2FX_I3DL2_PRESET_PARKINGLOT;
                    ReverbConvertI3DL2ToNative(&params,&m_reverbParams[i]);
                }
                break;
            case ReverbPreset_SEWERPIPE:
                {
                    XAUDIO2FX_REVERB_I3DL2_PARAMETERS params = XAUDIO2FX_I3DL2_PRESET_SEWERPIPE;
                    ReverbConvertI3DL2ToNative(&params,&m_reverbParams[i]);
                }
                break;
            case ReverbPreset_UNDERWATER:
                {
                    XAUDIO2FX_REVERB_I3DL2_PARAMETERS params = XAUDIO2FX_I3DL2_PRESET_UNDERWATER;
                    ReverbConvertI3DL2ToNative(&params,&m_reverbParams[i]);
                }
                break;
            case ReverbPreset_SMALLROOM:
                {
                    XAUDIO2FX_REVERB_I3DL2_PARAMETERS params = XAUDIO2FX_I3DL2_PRESET_SMALLROOM;
                    ReverbConvertI3DL2ToNative(&params,&m_reverbParams[i]);
                }
                break;
            case ReverbPreset_MEDIUMROOM:
                {
                    XAUDIO2FX_REVERB_I3DL2_PARAMETERS params = XAUDIO2FX_I3DL2_PRESET_MEDIUMROOM;
                    ReverbConvertI3DL2ToNative(&params,&m_reverbParams[i]);
                }
                break;
            case ReverbPreset_LARGEROOM:
                {
                    XAUDIO2FX_REVERB_I3DL2_PARAMETERS params = XAUDIO2FX_I3DL2_PRESET_LARGEROOM;
                    ReverbConvertI3DL2ToNative(&params,&m_reverbParams[i]);
                }
                break;
            case ReverbPreset_MEDIUMHALL:
                {
                    XAUDIO2FX_REVERB_I3DL2_PARAMETERS params = XAUDIO2FX_I3DL2_PRESET_MEDIUMHALL;
                    ReverbConvertI3DL2ToNative(&params,&m_reverbParams[i]);
                }
                break;
            case ReverbPreset_LARGEHALL:
                {
                    XAUDIO2FX_REVERB_I3DL2_PARAMETERS params = XAUDIO2FX_I3DL2_PRESET_LARGEHALL;
                    ReverbConvertI3DL2ToNative(&params,&m_reverbParams[i]);
                }
                break;
            case ReverbPreset_PLATE:
                {
                    XAUDIO2FX_REVERB_I3DL2_PARAMETERS params = XAUDIO2FX_I3DL2_PRESET_PLATE;
                    ReverbConvertI3DL2ToNative(&params,&m_reverbParams[i]);
                }
                break;
            default:
                ITF_ASSERT_MSG(0,"Invalid reverb preset");
                break;
            }
        }
    }
*/
    //************************************
    // @name	ITF::SoundAdapter_XAudio::initBus
    // @private 
    // @param	Bus * _bus
    // @return	void
    //************************************
    void SoundAdapter_XAudio::initBus( Bus * _bus )
    {
/*
        if (_bus->getName() != masterBus)
        {
            u32 numSends = _bus->getOutputs().size();
            ITF_ASSERT(numSends > 0);
            XAUDIO2_SEND_DESCRIPTOR  * sends = newAlloc(mId_Audio,XAUDIO2_SEND_DESCRIPTOR[numSends]);

            for (u32 i = 0; i < numSends; ++i)
            {
                sends[i].Flags = 0; //XAUDIO2_SEND_USEFILTER
                sends[i].pOutputVoice = m_busses[_bus->getOutputs()[i]]->getVoice();
            }

            XAUDIO2_VOICE_SENDS sendList;
            sendList.SendCount = numSends;
            sendList.pSends = sends;

            if ( _bus->getVoice() )
                _bus->getVoice()->SetOutputVoices(&sendList);

            delete[] sends;

            //SET OUTPUT VOLUMES 
            f32 volumes[] = 
            {
                1.0f,0.0f,
                0.0f,1.0f,
            };
            for (u32 i = 0; i < numSends; ++i)
            {
                if ( _bus->getVoice() )
                    _bus->getVoice()->SetOutputMatrix(m_busses[_bus->getOutputs()[i]]->getVoice(),NUM_CHANNELS,NUM_CHANNELS,volumes,m_operationSet);
            }
        }

        //SET REVERB
        if (_bus->getHasReverb())
        {
            XAUDIO2FX_REVERB_PARAMETERS *reverbParams = getReverbParams(_bus->getReverbPreset());
            if ( reverbParams && _bus->getVoice() )
                _bus->getVoice()->SetEffectParameters(0,reverbParams,sizeof(XAUDIO2FX_REVERB_PARAMETERS),m_operationSet);
        }

        //SET VOLUME
        if ( _bus->getVoice() )
            _bus->getVoice()->SetVolume(_bus->getVolume().ratio(),m_operationSet);

        //SET FILTER
        if (_bus->getFilterType() != FilterType_None)
        {
            FilterType filterType = _bus->getFilterType();
            f32 filterFrequency = _bus->getFilterFrequency();
            f32 filterQ = _bus->getFilterQ();
            //setVoiceFilterParameters(_bus->getVoice(), filterType, filterFrequency, filterQ);
            _bus->setFilterType(filterType);
            _bus->setFilterFrequency(filterFrequency);
            _bus->setFilterQ(filterQ);
        }
*/
    }


    void SoundAdapter_XAudio::printConfig()
    {
#if defined(ITF_WINDOWS) && !defined(ITF_FINAL)
        LOG("<<<< Sound Config >>>>");
        u32 deviceCount = 0;
        m_xAudio2->GetDeviceCount(&deviceCount);

        XAUDIO2_DEVICE_DETAILS details;
        for (u32 i = 0; i < deviceCount; ++i)
        {
            LOG("DEVICE %d",i);
            m_xAudio2->GetDeviceDetails(i,&details);
            char temp[MAX_PATH];
            size_t num;
            wcstombs_s(
                &num,
                temp,
                sizeof(temp),
                details.DeviceID,
                sizeof(details.DeviceID) 
                );
            LOG("%s",temp);
            wcstombs_s(
                &num,
                temp,
                sizeof(temp),
                details.DisplayName,
                sizeof(details.DisplayName) 
                );
            LOG("%s",temp);
            switch(details.Role)
            {
            case NotDefaultDevice:
                LOG("Role: NotDefaultDevice");
                break;
            case DefaultConsoleDevice:
                LOG("Role: DefaultConsoleDevice");
                break;
            case DefaultMultimediaDevice:
                LOG("Role: DefaultMultimediaDevice");
                break;
            case DefaultCommunicationsDevice:
                LOG("Role: DefaultCommunicationsDevice");
                break;
            case DefaultGameDevice:
                LOG("Role: DefaultGameDevice");
                break;
            case GlobalDefaultDevice:
                LOG("Role: GlobalDefaultDevice");
                break;
            default:
                LOG("Role: Invalid");
                break;
            }
            WAVEFORMATEXTENSIBLE wfx = details.OutputFormat;
            switch(wfx.Format.wFormatTag)
            {
            case WAVE_FORMAT_PCM:
                LOG("wFormatTag: WAVE_FORMAT_PCM");
                break;
            case WAVE_FORMAT_IEEE_FLOAT:
                LOG("wFormatTag: WAVE_FORMAT_IEEE_FLOAT");
                break;
            case WAVE_FORMAT_EXTENSIBLE:
                LOG("wFormatTag: WAVE_FORMAT_EXTENSIBLE");
                break;
            case WAVE_FORMAT_ADPCM:
                LOG("wFormatTag: WAVE_FORMAT_ADPCM");
                break;
            case WAVE_FORMAT_XMA2:
                LOG("wFormatTag: WAVE_FORMAT_XMA2");
                break;
            case WAVE_FORMAT_WMAUDIO2:
                LOG("wFormatTag: WAVE_FORMAT_WMAUDIO2");
                break;
            case WAVE_FORMAT_WMAUDIO3:
                LOG("wFormatTag: WAVE_FORMAT_WMAUDIO3");
                break;
            default:
                LOG("wFormatTag: %d",wfx.Format.wFormatTag);
                break;
            }
            LOG("nChannels: %d",wfx.Format.nChannels);
            LOG("nSamplesPerSec: %d",wfx.Format.nSamplesPerSec);
            LOG("nAvgBytesPerSec: %d",wfx.Format.nAvgBytesPerSec);
            LOG("nBlockAlign: %d",wfx.Format.nBlockAlign);
            LOG("wBitsPerSample: %d",wfx.Format.wBitsPerSample);
            LOG("wValidBitsPerSample: %d",wfx.Samples.wValidBitsPerSample);
            LOG("wSamplesPerBlock: %d",wfx.Samples.wSamplesPerBlock);
            LOG("wReserved: %d",wfx.Samples.wReserved);
            LOG("dwChannelMask : %d",wfx.dwChannelMask );

            if (wfx.dwChannelMask & SPEAKER_FRONT_LEFT) LOG("SPEAKER_FRONT_LEFT");
            if (wfx.dwChannelMask & SPEAKER_FRONT_RIGHT) LOG("SPEAKER_FRONT_RIGHT");
            if (wfx.dwChannelMask & SPEAKER_FRONT_CENTER) LOG("SPEAKER_FRONT_CENTER");
            if (wfx.dwChannelMask & SPEAKER_LOW_FREQUENCY) LOG("SPEAKER_LOW_FREQUENCY");
            if (wfx.dwChannelMask & SPEAKER_BACK_LEFT) LOG("SPEAKER_BACK_LEFT");
            if (wfx.dwChannelMask & SPEAKER_BACK_RIGHT) LOG("SPEAKER_BACK_RIGHT");
            if (wfx.dwChannelMask & SPEAKER_FRONT_LEFT_OF_CENTER) LOG("SPEAKER_FRONT_LEFT_OF_CENTER");
            if (wfx.dwChannelMask & SPEAKER_FRONT_RIGHT_OF_CENTER) LOG("SPEAKER_FRONT_RIGHT_OF_CENTER");
            if (wfx.dwChannelMask & SPEAKER_BACK_CENTER) LOG("SPEAKER_BACK_CENTER");
            if (wfx.dwChannelMask & SPEAKER_SIDE_LEFT) LOG("SPEAKER_SIDE_LEFT");
            if (wfx.dwChannelMask & SPEAKER_SIDE_RIGHT) LOG("SPEAKER_SIDE_RIGHT");
            if (wfx.dwChannelMask & SPEAKER_TOP_CENTER) LOG("SPEAKER_TOP_CENTER");
            if (wfx.dwChannelMask & SPEAKER_TOP_FRONT_LEFT) LOG("SPEAKER_TOP_FRONT_LEFT");
            if (wfx.dwChannelMask & SPEAKER_TOP_FRONT_CENTER) LOG("SPEAKER_TOP_FRONT_CENTER");
            if (wfx.dwChannelMask & SPEAKER_TOP_FRONT_RIGHT) LOG("SPEAKER_TOP_FRONT_RIGHT");
            if (wfx.dwChannelMask & SPEAKER_TOP_BACK_LEFT) LOG("SPEAKER_TOP_BACK_LEFT");
            if (wfx.dwChannelMask & SPEAKER_TOP_BACK_CENTER) LOG("SPEAKER_TOP_BACK_CENTER");
            if (wfx.dwChannelMask & SPEAKER_TOP_BACK_RIGHT) LOG("SPEAKER_TOP_BACK_RIGHT");
        }
        LOG("<<<< Sound Config >>>>");
#endif
    }

    void SoundAdapter_XAudio::printBusInfo( Bus * _bus, u32 _numTabs, String & _txt )
    {
        f32 volume = 0.0f;
        if ( _bus->getVoice() )
            _bus->getVoice()->GetVolume(&volume);
        f32 dBVolume = XAudio2AmplitudeRatioToDecibels (volume);

        String tabs = "";
        for (u32 i = 0; i < _numTabs; ++i)
        {
            tabs += "    ";
        }

        String txt;
        txt.setTextFormat("%s%s : %.2f\n",StringConverter(tabs).getChar(),_bus->getName().getDebugString(),dBVolume);

        _txt += txt;
        for (u32 i = 0 ; i < _bus->getInputs().size(); ++i )
        {
//            printBusInfo(m_busses[_bus->getInputs()[i]],_numTabs+1,_txt);
        }
    }


} // namespace ITF
#endif
