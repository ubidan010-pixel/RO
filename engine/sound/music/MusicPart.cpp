#include "precompiled_engine.h"


#ifndef _ITF_MUSICPART_H_
#include "engine/sound/Music/MusicPart.h"
#endif // _ITF_MUSICPART_H_


#ifndef _ITF_RAKIINCLUDES_
#include "engine/sound/raki/RakiIncludes.h"
#endif // _ITF_RAKIINCLUDES_

#ifndef RAKI_WAVEFILE_H
#include "RakiEngine/Data/WaveFile/WaveFile.h"
#endif // RAKI_WAVEFILE_H


#ifndef RAKI_MEMORYSTREAM_H
#include "RakiEngine/Data/Stream/MemoryStream/MemoryStream.h"
#endif // RAKI_MEMORYSTREAM_H

#ifndef RAKI_RIFFCHUNKMANAGER_H
#include "RakiEngine/Data/WaveFile/RiffChunkManager.h"
#endif // RAKI_RIFFCHUNKMANAGER_H

#ifndef RAKI_ENGINEINITIALIZER_H
#include "RakiEngine/Initializer/EngineInitializer.h"
#endif // RAKI_ENGINEINITIALIZER_H



#ifndef _ITF_SIMPLECACHEREADFILESTREAM_H_
#include "engine/sound/raki/SimpleCacheReadFileStream.h"
#endif // _ITF_SIMPLECACHEREADFILESTREAM_H_

#ifndef _ITF_READFILESTREAM_H_
#include "engine/sound/raki/ReadFileStream.h"
#endif //_ITF_READFILESTREAM_H_

#ifndef _ITF_RAKIINTERFACE_H_
#include "engine/sound/raki/RakiInterface.h"
#endif //_ITF_RAKIINTERFACE_H_

#ifndef _ITF_MUSICPARTTEMPLATE_H_
#include "engine/sound/Music/MusicPartTemplate.h"
#endif // _ITF_MUSICPARTTEMPLATE_H_

#ifndef _ITF_RESOURCEMANAGER_H_
#include "engine/resources/ResourceManager.h"
#endif //_ITF_RESOURCEMANAGER_H_

#ifndef _ITF_FILESERVER_H_
#include "core/file/FileServer.h"
#endif //_ITF_FILESERVER_H_

#include "engine/AdaptersInterfaces/AudioMiddlewareAdapter.h"



//#define MUSICPART_LOG_RAMSIZE



namespace ITF
{


#ifdef MUSICPART_LOG_RAMSIZE
    static u32 MusicPart_mem = 0;
#endif // MUSICPART_LOG_RAMSIZE


MusicPart::MusicPart( const Path & _path )
    : Resource( _path )
    , m_waveFile( NULL )
    , m_prefetchData( NULL )
    , m_sampleSize( 0 )
    , m_bUseTemplate(bfalse)
#ifdef ITF_WII
    , m_forcePrefetch(bfalse)
#endif
    , m_name(StringID::Invalid)
{
}


MusicPart::~MusicPart()
{
    flushPhysicalData();
}


bbool MusicPart::tryLoadFile()
{
#ifdef ITF_SUPPORT_RAKI

#if defined ( ITF_CTR ) 
    RESOURCE_MANAGER->addResourceAsLoaded(this);
    return btrue;
#endif

    ITF_ASSERT_MSG(!isPhysicallyLoaded(), "duplicate resource loading can lead to crashes");

    PathString_t pathName;

    getPath().getString(pathName);
    const String filenameCooked = FILESERVER->getCookedName(pathName);

    SimpleCacheReadFileStream fileStream;

    raki::RiffChunkManager::singleton().setShouldReadDataBlockForWaveData( false );

    bool opened = fileStream.open( filenameCooked );

    if ( !opened )
    {
        LOG( "MusicPart::tryLoadFile: Unable to open <%s>", StringToUTF8(filenameCooked).get() );
        return bfalse;
    }

    raki::Serializer serializer( &fileStream );

    m_waveFile = newAlloc( mId_AudioEngine, raki::WaveFile );

    m_waveFile->read( serializer );

    if ( serializer.encounteredError() )
    {
        LOG( "MusicPart::tryLoadFile: error reading <%s>", StringToUTF8(filenameCooked).get());
        return bfalse;
    }

    // calc some stuff
    const WAVEFORMATEX * waveformat = (const WAVEFORMATEX*)m_waveFile->getChunkData( "fmt " );
    ITF_ASSERT( waveformat ); 

#if defined ( ITF_WII )

    ITF_ASSERT_MSG( waveformat->wFormatTag == WAVE_FORMAT_ADPCM, "MusicPartResource::init: error reading <%s> - invalid format", StringToUTF8(filenameCooked).get());

    if ( waveformat->wFormatTag != WAVE_FORMAT_ADPCM )
        return bfalse;

    const char datSChunkName[] = "datS"; // for stereo interlaced adpcm 
    const char datLChunkName[] = "datL"; // for mono adpcm 

    const char * dataWiiChunkName = m_waveFile->getChunkSize( datSChunkName ) ? datSChunkName : datLChunkName;

    m_byteSize = m_waveFile->getChunkSize( dataWiiChunkName );

    if ( m_waveFile->getChunkSize( datSChunkName ) )
    {
        m_bytesPerSample = (u32) waveformat->nBlockAlign;
        m_sampleSize = 14 * m_byteSize / 16;
    }
    else
    {
        m_bytesPerSample = (u32) waveformat->nBlockAlign;
        m_sampleSize = 14 * m_byteSize / 8;
    }

#elif defined ( ITF_X360 )

    const XMA2WAVEFORMATEX * xma2Waveformat = (const XMA2WAVEFORMATEX*)waveformat;

    ITF_ASSERT_MSG( waveformat->wFormatTag == WAVE_FORMAT_XMA2, "MusicPartResource::init: error reading <%s> - invalid format", StringToUTF8(filenameCooked).get());
    ITF_ASSERT_MSG( m_waveFile->getChunkSize( "seek" ), "MusicPartResource::init: error reading <%s> - invalid format -> needs recook?", StringToUTF8(filenameCooked).get());
    ITF_ASSERT( m_waveFile->getChunkSize( "seek" ) ); 

    if ( waveformat->wFormatTag != WAVE_FORMAT_XMA2 )
        return bfalse;

    const char dataChunkName[] = "data";

    m_byteSize = m_waveFile->getChunkSize( dataChunkName );
    m_bytesPerSample = (u32) waveformat->nBlockAlign;
    m_sampleSize = xma2Waveformat->SamplesEncoded;

#elif defined ( ITF_PS3 )

    ITF_ASSERT_MSG( waveformat->wFormatTag == WAVE_FORMAT_MPEGLAYER3, "MusicPartResource::init: error reading <%s> - invalid format", StringToUTF8(filenameCooked).get());

    if ( waveformat->wFormatTag != WAVE_FORMAT_MPEGLAYER3 )
        return bfalse;

    const char dataChunkName[] = "msf ";

    m_byteSize = m_waveFile->getChunkSize( dataChunkName );
    m_bytesPerSample = (u32) waveformat->nBlockAlign;

    const u64 nbBlocks = m_byteSize / 576;
    // first block is silence for MP3
    m_sampleSize = nbBlocks ? ( nbBlocks - 1 )* 1152 : 0;


//    RAKI_OUTPUT( "0x%x MusicPart::tryLoadFile m_byteSize 0x%x nbBlocks 0x%llx m_sampleSize 0x%llx", this, m_byteSize, nbBlocks, m_sampleSize );

#else

    ITF_ASSERT_MSG( waveformat->wFormatTag == WAVE_FORMAT_PCM, "MusicPartResource::init: error reading <%s> - invalid format", StringToUTF8(filenameCooked).get());

    if ( waveformat->wFormatTag != WAVE_FORMAT_PCM )
        return bfalse;

    const char dataChunkName[] = "data";

    m_byteSize = m_waveFile->getChunkSize( dataChunkName );
    m_bytesPerSample = (u32) waveformat->nBlockAlign;
    m_sampleSize = m_byteSize / m_bytesPerSample;

#endif 

#if defined ( ITF_WII )
    // Wii: Don't prefetch when there is no template -> don't prefetch voices 
    if ( m_forcePrefetch || (m_bUseTemplate && m_template.getPrefetch()) )
#else
    // Assuming here that if there is no template defined, then data is automatically prefetched
    if ( !m_bUseTemplate || m_template.getPrefetch() )
#endif 
    {

        // load all data if byte size is smaller than prefetch size, if not just read projectPrefetchSize
#if defined ( ITF_WII )
        const u64 projectPrefetchSize = raki::EngineInitializer::singleton().getStreamBufferSize( WAVE_FORMAT_ADPCM, 2 );
        const u32 dataChunkSize = m_waveFile->getChunkSize( dataWiiChunkName );
        u64 prefetchSize = (u64)dataChunkSize;
#elif defined ( ITF_X360 )
        const u64 projectPrefetchSize = raki::EngineInitializer::singleton().getStreamBufferSize( WAVE_FORMAT_XMA2, 2 );
        const u32 dataChunkSize = m_waveFile->getChunkSize( dataChunkName );
        u64 prefetchSize = (u64)dataChunkSize;
#elif defined ( ITF_PS3 ) 
        const u64 projectPrefetchSize = raki::EngineInitializer::singleton().getStreamBufferSize( WAVE_FORMAT_MPEGLAYER3, 2 );
        const u32 dataChunkSize = m_waveFile->getChunkSize( dataChunkName );
        u64 prefetchSize = (u64)dataChunkSize;
#else 
        const u64 projectPrefetchSize = raki::EngineInitializer::singleton().getStreamBufferSize( WAVE_FORMAT_PCM, 2 );
        const u32 dataChunkSize = m_waveFile->getChunkSize( dataChunkName );
        u64 prefetchSize = (u64)dataChunkSize;
#endif

        ITF_ASSERT( prefetchSize );

        if ( prefetchSize > projectPrefetchSize )
            prefetchSize = projectPrefetchSize;

        // read whole file is file is small enough 
        if ( ( prefetchSize < dataChunkSize ) && ( ( dataChunkSize - prefetchSize ) < projectPrefetchSize ) )
            prefetchSize = dataChunkSize;

        m_prefetchData = newAlloc( mId_AudioEngine, raki::MemoryBlock );

#if defined ( RAKI_PLATFORM_XB360 )
        const raki::MemoryBlock::MemoryType memoryType = raki::MemoryBlock::xma2_aligned2048;
#elif defined ( RAKI_PLATFORM_PS3 )
        const raki::MemoryBlock::MemoryType memoryType = raki::MemoryBlock::ps3_aligned128;
#elif defined ( RAKI_PLATFORM_WII )
        const raki::MemoryBlock::MemoryType memoryType = raki::MemoryBlock::wii_mem2_aligned32;
#else
        const raki::MemoryBlock::MemoryType memoryType = raki::MemoryBlock::standard;
#endif 
        m_prefetchData->allocateBuffer( prefetchSize, memoryType, raki::Memory::data );

        if ( !m_prefetchData->getBuffer() )
        {
            LOG( "MusicPart::tryLoadFile: Unable to allocate prefetch buffer" );
            return false;
        }

#ifndef ITF_FINAL
        setMemoryUsage( (u32) m_prefetchData->getSize() );

#ifdef MUSICPART_LOG_RAMSIZE
        MusicPart_mem += (u32)m_prefetchData->getSize();
        RAKI_OUTPUT("0x%x MusicPart <%s> %d o - new total %d ko = %.3f Mo",
            this, StringToUTF8(filenameCooked).get(),
            (u32)m_prefetchData->getSize(), MusicPart_mem / 1024, (f32)MusicPart_mem / 1048576.f );
#endif // MUSICPART_LOG_RAMSIZE

#endif //ITF_FINAL

#if defined ( ITF_WII )
        const u64 offsetOfData = m_waveFile->getChunkOffsetInStream( dataWiiChunkName );
#else
        const u64 offsetOfData = m_waveFile->getChunkOffsetInStream( dataChunkName );
#endif 
        fileStream.seek( offsetOfData, raki::SeekableStream::fromBeginning );

        ITF_ASSERT( fileStream.getCurrentPos() == offsetOfData );

        serializer.serialize( m_prefetchData->getBuffer(), m_prefetchData->getSize() );

        if ( serializer.encounteredError() )
        {
            LOG( "MusicPart::tryLoadFile: error reading prefetch buffer for <%s>", StringToUTF8(filenameCooked).get());
            return bfalse;
        }

    }
#else
StringID  Id = m_template.getWwiseEventGuid();
if (Id.isValid())
{
    f32 waveduration = AUDIO_ADAPTER->getDurationFromGUID(Id);
    setWaveDuration(waveduration);
    setSampleSize((u32)(waveduration * DEFAULT_SAMPLING_RATE));
}
#endif
    RESOURCE_MANAGER->addResourceAsLoaded(this);
   

    return btrue;
}


void MusicPart::flushPhysicalData()
{
    if ( m_waveFile )
    {
        delete m_waveFile;
    }
    m_waveFile = NULL;

    if ( m_prefetchData )
    {
        delete m_prefetchData;
#ifndef ITF_FINAL

#ifdef MUSICPART_LOG_RAMSIZE
        MusicPart_mem -= getMemoryUsage();
#endif // MUSICPART_LOG_RAMSIZE

        setMemoryUsage( 0 );
#endif //ITF_FINAL
    }
    m_prefetchData = NULL;
}


Resource::ResourceType MusicPart::getType()const
{
    return Resource::ResourceType_MusicPart;
}




} // namespace ITF
