#include "precompiled_SoundAdapter_PS3.h"
#ifdef ITF_SUPPORT_RAKI
#include <cell/sysmodule.h>
#include <sys/spu_initialize.h>

#ifndef _ITF_WARNINGMANAGER_H_
#include "engine/common/WarningManager.h"
#endif // _ITF_WARNINGMANAGER_H_

#ifndef _ITF_SOUNDADAPTER_H_PS3_
#include "adapters/SoundAdapter_PS3/SoundAdapter_PS3.h"
#endif //_ITF_SOUNDADAPTER_H_PS3_

#include "adapters/SystemAdapter_PS3/SystemAdapter_PS3.h"

#ifndef _ITF_RAKIINTERFACE_H_
#include "engine/sound/raki/RakiInterface.h"
#endif //_ITF_RAKIINTERFACE_H_

#ifndef RAKI_SPECIFIC_PS3_H
#include "RakiEngine/Specific/_PS3/Specific_PS3.h"
#endif // RAKI_SPECIFIC_PS3_H

#ifndef _ITF_READFILESTREAM_H_
#include "engine/sound/raki/ReadFileStream.h"
#endif //_ITF_READFILESTREAM_H_

#ifndef RAKI_WAVEFILE_H
#include "RakiEngine/Data/WaveFile/WaveFile.h"
#endif // RAKI_WAVEFILE_H

#ifndef _ITF_SOUND_H_
#include "engine/sound/Sound.h"
#endif //_ITF_SOUND_H_

#ifndef RAKI_BUFFERSOUNDNATIVE_PS3_H
#include "RakiEngine/Sounds/_PS3/BufferSoundNative_PS3.h"
#endif // RAKI_BUFFERSOUNDNATIVE_PS3_H

#ifndef RAKI_FORMAT_H
#include "RakiEngine/Sounds/Format.h"
#endif // RAKI_FORMAT_H

#ifndef RAKI_MEMORYBLOCK_H
#include "RakiEngine/Data/MemoryBlock/MemoryBlock.h"
#endif // RAKI_MEMORYBLOCK_H

#ifndef _ITF_SOUNDCOMPONENT_H_
#include "engine/actors/components/SoundComponent.h"
#endif //_ITF_SOUNDCOMPONENT_H_

#ifndef _ITF_INSTANCEMANAGER_H_
#include "engine/sound/InstanceManager.h"
#endif // _ITF_INSTANCEMANAGER_H_

#ifndef _ITF_MODULEMANAGER_PS3_H_
#include "core/system/PS3/ModuleManager_ps3.h"
#endif //_ITF_MODULEMANAGER_PS3_H_

#ifndef RAKI_RIFFCHUNKMANAGER_H
#include "RakiEngine/Data/WaveFile/RiffChunkManager.h"
#endif // RAKI_RIFFCHUNKMANAGER_H

#ifndef _ITF_RESOURCEMANAGER_H_
#include "engine/resources/ResourceManager.h"
#endif //_ITF_RESOURCEMANAGER_H_

#ifndef _ITF_SIMPLECACHEREADFILESTREAM_H_
#include "engine/sound/raki/SimpleCacheReadFileStream.h"
#endif // _ITF_SIMPLECACHEREADFILESTREAM_H_



namespace ITF
{

    SoundAdapter_PS3::SoundAdapter_PS3()
        : m_audioPortNumber( (u32)-1 )
        , m_multistreamMemory( NULL )
        , m_mp3Memory( NULL )
        , m_multiStreamUpdateThread( NULL )
        , m_spurs(NULL)
    {
        ITF_MemSet( &m_portConfig, 0, sizeof( m_portConfig ) );
        ITF_MemSet( &m_audioPortParam, 0, sizeof( m_audioPortParam ) );

        MODULE_MANAGER->loadModule( CELL_SYSMODULE_AUDIO );
    }


    SoundAdapter_PS3::~SoundAdapter_PS3()
    {
        MODULE_MANAGER->unloadModule( CELL_SYSMODULE_AUDIO );
    }


    bbool SoundAdapter_PS3::init( bbool _audioCaptureAllowed)
    {
        SoundAdapter::init(_audioCaptureAllowed);

        //	cellMSSystemConfigureSysUtilEx returns the following data:
        //	Bits 0-3:	Number of available output channels
        //	Bit    4:	Dolby On status
        //	Bit    5:	DTS On status
        u32 retSysUtil = cellMSSystemConfigureSysUtilEx( CELL_MS_AUDIOMODESELECT_SUPPORTSLPCM );
        u32 numChans = (retSysUtil & 0x0000000F);
        u32 dolbyOn = (retSysUtil & 0x00000010) >> 4;
        u32 dtsOn = (retSysUtil & 0x00000020) >> 5;
        LOG("Number Of Channels: %u", numChans);
        LOG("Dolby enabled: %u", dolbyOn);
        LOG("DTS enabled: %u", dtsOn);

        int ret = cellAudioInit();
        if ( ret != CELL_OK )
            return bfalse;

        m_audioPortParam.nChannel = CELL_AUDIO_PORT_8CH;
        m_audioPortParam.nBlock = CELL_AUDIO_BLOCK_8;

        ret = cellAudioPortOpen( &m_audioPortParam, &m_audioPortNumber );
        if ( ret != CELL_OK )
            return bfalse;


        // get port config.
        ret = cellAudioGetPortConfig(m_audioPortNumber, &m_portConfig);
        if ( ret != CELL_OK )
            return bfalse;


        ret = cellMSSystemConfigureLibAudio(&m_audioPortParam, &m_portConfig);
        if ( ret != CELL_OK )
            return bfalse;



        // Initialise MultiStream

        const int maxNumberOfStreams = 256;

        CellMSSystemConfig multistreamSystemConfig = { 0 };
        multistreamSystemConfig.channelCount = maxNumberOfStreams;
        multistreamSystemConfig.subCount = 31;
        multistreamSystemConfig.dspPageCount = 5;
        multistreamSystemConfig.flags = CELL_MS_DISABLE_SPU_PRINTF_SERVER;
        int nMemoryNeeded = cellMSSystemGetNeededMemorySize( &multistreamSystemConfig );

        m_multistreamMemory = raki::Memory::mallocAligned( nMemoryNeeded, 128, raki::Memory::system );
        if ( !m_multistreamMemory )
            return bfalse;


        m_spurs = static_cast<SystemAdapter_PS3 *>(SystemAdapter::getptr())->getMainSpurs();
        if (!m_spurs)
            return bfalse;

        // Initialise SPURS MultiStream version
        uint8_t prios[8] = {1, 0, 0, 0, 0, 0, 0, 0};

        if ( cellMSSystemInitSPURS( m_multistreamMemory, &multistreamSystemConfig, m_spurs, &prios[0]) )
            return bfalse;


        // Initialise MP3
        int mp3SizeNeeded = cellMSMP3GetNeededMemorySize( maxNumberOfStreams );
        m_mp3Memory = (int*)Memory::malloc( mp3SizeNeeded );
        if ( !m_mp3Memory )
            return bfalse;

        ret = cellMSMP3Init( maxNumberOfStreams, m_mp3Memory ); 
        if ( ret )
            return bfalse;

        // Setup the volumes on sub buss 1. By default all sub busses route to the master bus
        int SUBNUM = 1;
        int PLAYSUB = SUBNUM | CELL_MS_BUS_FLAG;
        // bus volumes. (Maximum volume for each speaker)
        float fBusVols[64] = {1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f};
        cellMSCoreSetVolume64( PLAYSUB, CELL_MS_WET, fBusVols );

        // Setup the volumes on the master buss
        cellMSCoreSetVolume64( CELL_MS_MASTER_BUS, CELL_MS_DRY, fBusVols);


        ITF_ASSERT( !m_multiStreamUpdateThread );
        m_multiStreamUpdateThread = newAlloc( mId_AudioEngine, raki::MultiStreamUpdateThread )( m_audioPortNumber );

        m_multiStreamUpdateThread->startThread();


        RakiInterface::InitStruct initStruct; 
        RakiInterface::instance()->initLowLevelAudio( &initStruct );


        return btrue;
    }


    bbool SoundAdapter_PS3::isRunning() const
    {
        return m_multiStreamUpdateThread!=NULL;
    }


    void SoundAdapter_PS3::shutdown()
    {
        m_multiStreamUpdateThread->requestEnd();
        while ( m_multiStreamUpdateThread->isRunning() )
            raki::Thread::sleep( 20 );
        delete m_multiStreamUpdateThread;
        m_multiStreamUpdateThread = NULL;

        SoundAdapter::shutdown();
        // Initialized by cellMSSystemInitSPURS
        cellMSSystemClose();
    }


    bbool SoundAdapter_PS3::loadSound( Sound * _sound, const char * _path )
    {
        raki::RiffChunkManager::singleton().setShouldReadDataBlockForWaveData( true );

        SimpleCacheReadFileStream file;
        if ( !file.open( _path) )
        {
            ITF_WARNING( NULL, bfalse, "<< SoundAdapter >> Unable to create file" );
            return bfalse;
        }

        raki::WaveFile * waveFile = newAlloc( mId_AudioEngine, raki::WaveFile );
        raki::Serializer serializer( &file );

        waveFile->serialize( serializer );

        char fmtTag[] = "fmt ";
        char msfTag[] = "msf ";

        if ( !waveFile->getChunkData( fmtTag ) || !waveFile->getChunkData( msfTag ) )
        {
            ITF_WARNING( NULL, bfalse, "<< SoundAdapter >> File has invalid format >> %s", _path );
            delete waveFile;
            return bfalse;
        }

        _sound->m_adapterImplementationData = waveFile;

#ifndef ITF_FINAL
        _sound->setMemoryUsage( waveFile->getChunkSize( msfTag ) );
#endif

        RESOURCE_MANAGER->addResourceAsLoaded( _sound );

        return btrue;
    }


    void SoundAdapter_PS3::unloadSound( Sound * _sound )
    {
        ITF_ASSERT(_sound);
        raki::WaveFile * waveFile = (raki::WaveFile*)_sound->m_adapterImplementationData;
        if (!waveFile)
            return;

        delete waveFile;

#ifndef ITF_FINAL
        _sound->setMemoryUsage( 0 );
#endif

        _sound->m_adapterImplementationData = NULL;
    }


    SourceHandle SoundAdapter_PS3::createSource( SourceInfo * _params )
    {
        if ( m_instanceManager == NULL || _params == NULL || _params->m_sound == NULL )
            return SoundComponent::InvalidSoundHandle;

        ITF_ASSERT_MSG( ((Sound*)_params->m_sound)->isPhysicallyLoaded(), "Trying to play sound before it is finished loading" );

        if ( !((Sound*)_params->m_sound)->isPhysicallyLoaded() )
            return SoundComponent::InvalidSoundHandle;

        const bool isMusic = false;
        const bool isLooping = _params->m_loop ? true : false;
        raki::Format format( isMusic, isLooping );

        raki::WaveFile * waveFile = (raki::WaveFile*)_params->m_sound->m_adapterImplementationData;

        char msfChunk[] = "msf ";
        raki::MemoryBlock dataBlock( waveFile->getChunkData( msfChunk ), waveFile->getChunkSize( msfChunk ) );

        char fmtChunk[] = "fmt ";
        format.setData( (WAVEFORMATEX*) waveFile->getChunkData( fmtChunk ), waveFile->getChunkSize( fmtChunk ), &dataBlock );

        return m_instanceManager->createSource( &format, _params->m_bus );
    }


    f32 SoundAdapter_PS3::getSoundDuration( const Sound * _sound )
    {
        return 1.f;
    }


    CellSpurs * SoundAdapter_PS3::getSpurs()
    {
        return m_spurs;
    }


    CellAudioPortConfig * SoundAdapter_PS3::getPortConfig()
    {
        return &m_portConfig;
    }


    CellAudioPortParam * SoundAdapter_PS3::getAudioPortParam()
    {
        return &m_audioPortParam;
    }

#ifndef ITF_FINAL
    void SoundAdapter_PS3::getPerformanceDebugInfo( String &debugInfo, char paramDelimiter /*= '\n'*/, char valueDelimiter /*= ':'*/ )
    {

    }
#endif



} // namespace ITF
#endif