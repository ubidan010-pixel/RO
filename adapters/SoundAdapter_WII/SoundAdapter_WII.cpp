#include "precompiled_SoundAdapter_WII.h"

#ifndef _ITF_SOUNDADAPTER_H_WII_
#include "adapters/SoundAdapter_WII/SoundAdapter_WII.h"
#endif //_ITF_SOUNDADAPTER_H_WII_


#ifndef _ITF_FILEMANAGER_H_
#include "core/AdaptersInterfaces/FileManager.h"
#endif //_ITF_FILEMANAGER_H_

#ifndef _ITF_ERRORHANDLER_H_
#include "core/error/ErrorHandler.h"
#endif //_ITF_ERRORHANDLER_H_

#ifndef _ITF_SOUND_H_
#include "engine/sound/Sound.h"
#endif //_ITF_SOUND_H_

#ifndef _ITF_GFX_ADAPTER_H_
#include "engine/AdaptersInterfaces/GFXAdapter.h"
#endif //_ITF_GFX_ADAPTER_H_

#ifndef _ITF_SLOTALLOCATORMANAGER_H_
#include "core/memory/slotallocatorManager.h"
#endif //_ITF_SLOTALLOCATORMANAGER_H_

#ifndef _ITF_CAMERA_H_
#include "engine/display/Camera.h"
#endif //_ITF_CAMERA_H_

#ifndef _ITF_RAKIINTERFACE_H_
#include "engine/sound/raki/RakiInterface.h"
#endif //_ITF_RAKIINTERFACE_H_

#ifndef _ITF_READFILESTREAM_H_
#include "engine/sound/raki/ReadFileStream.h"
#endif //_ITF_READFILESTREAM_H_

#ifndef RAKI_WAVEFILE_H
#include "RakiEngine/Data/WaveFile/WaveFile.h"
#endif // RAKI_WAVEFILE_H

#ifndef _ITF_RAKIINTERFACE_H_
#include "engine/sound/raki/RakiInterface.h"
#endif //_ITF_RAKIINTERFACE_H_

#ifndef RAKI_FORMAT_H
#include "RakiEngine/Sounds/Format.h"
#endif // RAKI_FORMAT_H

#ifndef RAKI_MEMORYBLOCK_H
#include "RakiEngine/Data/MemoryBlock/MemoryBlock.h"
#endif // RAKI_MEMORYBLOCK_H

#ifndef RAKI_BUFFERSOUNDNATIVE_WII_H
#include "RakiEngine/Sounds/_Wii/BufferSoundNative_Wii.h"
#endif // RAKI_BUFFERSOUNDNATIVE_WII_H

#ifndef RAKI_AUDIOCALLBACKMANAGER_WII_H
#include "RakiEngine/Specific/_Wii/AudioCallbackManager_Wii.h"
#endif // RAKI_AUDIOCALLBACKMANAGER_WII_H

#ifndef _ITF_INSTANCEMANAGER_H_
#include "engine/sound/InstanceManager.h"
#endif // _ITF_INSTANCEMANAGER_H_

#ifndef RAKI_RIFFCHUNKMANAGER_H
#include "RakiEngine/Data/WaveFile/RiffChunkManager.h"
#endif // RAKI_RIFFCHUNKMANAGER_H

#ifndef _ITF_SOUNDCOMPONENT_H_
#include "engine/actors/components/SoundComponent.h"
#endif //_ITF_SOUNDCOMPONENT_H_

#ifndef _ITF_RESOURCEMANAGER_H_
#include "engine/resources/ResourceManager.h"
#endif //_ITF_RESOURCEMANAGER_H_

#ifndef _ITF_SIMPLECACHEREADFILESTREAM_H_
#include "engine/sound/raki/SimpleCacheReadFileStream.h"
#endif // _ITF_SIMPLECACHEREADFILESTREAM_H_



#include <revolution.h>
#include <revolution/ai.h>
#include <revolution/ax.h>
#include <revolution/mix.h>


//#define SOUNDADAPTERWII_LOG_RAMSIZE



namespace ITF
{

#ifdef SOUNDADAPTERWII_LOG_RAMSIZE
    static u32 SoundAdapter_WII_mem = 0;
#endif // SOUNDADAPTERWII_LOG_RAMSIZE


    //************************************
    // @name	ITF::SoundAdapter_WII::SoundAdapter_WII
    // @public 
    // @return	
    //************************************
    SoundAdapter_WII::SoundAdapter_WII()
        : m_axBuffer( NULL )
    {
        AIInit(0);
        
        m_axBuffer = raki::Memory::mallocMem2Aligned32( AXGetMemorySize( AX_MAX_VOICES ), raki::Memory::system );

        AIInit(NULL);
        AXInitSpecifyMem( AX_MAX_VOICES, m_axBuffer );
        AXRegisterCallback( &raki::AudioCallbackManager::staticAudioCallback );

        //MIXInit();
        AXSetCompressor(AX_COMPRESSOR_OFF);
    }

    //************************************
    // @name	ITF::SoundAdapter_WII::~SoundAdapter_WII
    // @public 
    // @return	
    //************************************
    SoundAdapter_WII::~SoundAdapter_WII()
    {
        if (isRunning())
            shutdown();

        //Reset callback
        AIRegisterDMACallback(0);
        AXRegisterAuxACallback(0, 0);
        AXRegisterAuxBCallback(0, 0);
        AXRegisterAuxCCallback(0, 0);

        //MIXQuit();
        // temp removal ? to avoid deadlock in AXQuit();

        AXRegisterCallback(0);

        raki::Memory::freeMem2Aligned32( m_axBuffer );
        m_axBuffer = NULL;
    }

	//************************************
	// @name	ITF::SoundAdapter_WII::init
	// @public 
	// @return	ITF::bbool
	//************************************
	bbool SoundAdapter_WII::init(bbool _audioCaptureAllowed)
    {
        bbool res = SoundAdapter::init( _audioCaptureAllowed );
     
        if (res)
        {
            res = initInternal();
        }
        return res;
	}

	//************************************
	// @name	ITF::SoundAdapter_WII::isRunning
	// @public 
	// @return	ITF::bbool
	//************************************
	bbool SoundAdapter_WII::isRunning() const
	{
        return btrue;
	}

	//************************************
	// @name	ITF::SoundAdapter_WII::shutdown
	// @public 
	// @return	void
	//************************************
	void SoundAdapter_WII::shutdown()
	{
        SoundAdapter::shutdown();
    }

	//************************************
	// @name	ITF::SoundAdapter_WII::loadSound
	// @public 
	// @param	Sound * _sound
	// @param	const char * _filename
	// @return	ITF::bbool
	//************************************

	bbool SoundAdapter_WII::loadSound(Sound * _sound, const char * _path )
	{
        SimpleCacheReadFileStream file;

        raki::RiffChunkManager::singleton().setShouldReadDataBlockForWaveData( true );

        if ( !file.open( _path ) )
        {
            LOG("<< SoundAdapter >> Unable to create file");
            return bfalse;
        }

        raki::WaveFile * waveFile = newAlloc( mId_AudioEngine, raki::WaveFile );
        raki::Serializer serializer( &file );

        waveFile->serialize( serializer );

        const char fmtTag[] = "fmt ";
        const char datLTag[] = "datL";
        const char datRTag[] = "datR";
        const char dataTag[] = "data";

        if ( !waveFile->getChunkData( fmtTag ) 
            || ( !waveFile->getChunkData( datLTag ) && !waveFile->getChunkData( dataTag ) ) )
        {
            LOG("<< SoundAdapter >> Unable to load file >> %s", _path);
            delete waveFile;
            return bfalse;
        }

        _sound->m_adapterImplementationData = waveFile;

        if ( waveFile->getChunkData( datLTag ) )
            DCFlushRange( waveFile->getChunkData( datLTag ), waveFile->getChunkSize( datLTag ) );

        if ( waveFile->getChunkData( datRTag ) )
            DCFlushRange( waveFile->getChunkData( datRTag ), waveFile->getChunkSize( datRTag ) );

#ifndef ITF_FINAL
        u32 dataSize = waveFile->getChunkSize( datLTag );
        dataSize += waveFile->getChunkSize( datRTag );
        dataSize += waveFile->getChunkSize( dataTag );

        _sound->setMemoryUsage( dataSize );

#ifdef SOUNDADAPTERWII_LOG_RAMSIZE
        SoundAdapter_WII_mem += dataSize;
        RAKI_OUTPUT("Buffer <%s> %d o - new total %d ko = %.3f Mo",
            _path, dataSize, SoundAdapter_WII_mem / 1024, (f32)SoundAdapter_WII_mem / 1048576.f );
#endif // SOUNDADAPTERWII_LOG_RAMSIZE

#endif

        RESOURCE_MANAGER->addResourceAsLoaded( _sound );

        return btrue;
	}

	//************************************
	// @name	ITF::SoundAdapter_WII::unloadSound
	// @public 
	// @param	Sound * _sound
	// @return	void
	//************************************
	void SoundAdapter_WII::unloadSound(Sound * _sound)
	{
        ITF_ASSERT(_sound);
        raki::WaveFile * waveFile = (raki::WaveFile*)_sound->m_adapterImplementationData;
        if (!waveFile)
            return;

        delete waveFile;

#ifndef ITF_FINAL

#ifdef SOUNDADAPTERWII_LOG_RAMSIZE
        SoundAdapter_WII_mem -= _sound->getMemoryUsage();
#endif // SOUNDADAPTERWII_LOG_RAMSIZE

        _sound->setMemoryUsage( 0 );
#endif

        _sound->m_adapterImplementationData = NULL;
	}

	//************************************
	// @name	ITF::SoundAdapter_WII::createSource
	// @virtual private 
	// @param	const SourceInfo * _params
	// @return	ITF::SourceHandle
	//************************************
	SourceHandle SoundAdapter_WII::createSource( SourceInfo * _params)
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
        
        WAVEFORMATEX * formatEx = (WAVEFORMATEX*)waveFile->getChunkData( "fmt " );

        format.setFormat( formatEx );
        format.setDataL( waveFile->getChunkData( "dspL"), waveFile->getChunkData( "datL") );
        format.setDataR( waveFile->getChunkData( "dspR"), waveFile->getChunkData( "datR") );

        return m_instanceManager->createSource( &format, _params->m_bus );
	}

    //************************************
    // @name	ITF::SoundAdapter_WII::toggleDebug
    // @public 
    // @return	void
    //************************************
    void SoundAdapter_WII::toggleDebug()
    {
    }
 
    //************************************
    // @name	ITF::SoundAdapter_WII::isDebug
    // @public 
    // @return	ITF::bbool
    //************************************
    bbool SoundAdapter_WII::isDebug()
    {
        return bfalse;
    }



    //************************************
    // @name	ITF::SoundAdapter_W>II::initInternal
    // @public 
    // @return	ITF::bbool
    //************************************
    ITF::bbool SoundAdapter_WII::initInternal()
    {
        if (!isRunning())
            return bfalse;

        // init Raki Engine

        RakiInterface::InitStruct initStruct; 
        initStruct.m_axBuffer = m_axBuffer;

        RakiInterface::instance()->initLowLevelAudio( &initStruct );

        return btrue;
    }


#ifndef ITF_FINAL
    void SoundAdapter_WII::getPerformanceDebugInfo(String &debugInfo, char paramDelimiter, char valueDelimiter)
    {

    }
#endif
} // namespace ITF
