#include "precompiled_engine.h"
#ifdef ITF_SUPPORT_RAKI

#ifndef _ITF_PCMDATAPROVIDER_H_
#include "engine/sound/raki/PCMDataProvider.h"
#endif //_ITF_PCMDATAPROVIDER_H_


#ifndef _ITF_MUSICPART_H_
#include "engine/sound/Music/MusicPart.h"
#endif // _ITF_MUSICPART_H_


#ifndef RAKI_SOUNDS_SEQUENCER_H
#include "RakiEngine/Sounds/Sequencer/Sequencer.h"
#endif // RAKI_SOUNDS_SEQUENCER_H

#ifndef _ITF_FILESERVER_H_
#include "core/file/FileServer.h"
#endif //_ITF_FILESERVER_H_

#ifndef RAKI_WAVEFILE_H
#include "RakiEngine/Data/WaveFile/WaveFile.h"
#endif // RAKI_WAVEFILE_H

#ifndef _ITF_MUSICMANAGER_H_
#include "engine/gameplay/musicmanager.h"
#endif //_ITF_MUSICMANAGER_H_

#ifndef RAKI_MEMORYBLOCK_H
#include "RakiEngine/Data/MemoryBlock/MemoryBlock.h"
#endif // RAKI_MEMORYBLOCK_H

#ifndef _ITF_RAKIINTERFACE_H_
#include "engine/sound/raki/RakiInterface.h"
#endif //_ITF_RAKIINTERFACE_H_

#ifndef _ITF_MUSICINSTANCE_H_
#include "engine/sound/Music/MusicInstance.h"
#endif // _ITF_MUSICINSTANCE_H_

#ifndef RAKI_ENGINEINITIALIZER_H
#include "RakiEngine/Initializer/EngineInitializer.h"
#endif // RAKI_ENGINEINITIALIZER_H


#if defined ( RAKI_USING_XAUDIO2 )

#ifndef RAKI_PCMSTREAMSOUNDNATIVE_H
#include "RakiEngine/Sounds/_XAudio2/PCMStreamSoundNative.h"
#endif // RAKI_PCMSTREAMSOUNDNATIVE_H

#elif defined ( RAKI_PLATFORM_XBOX_SERIES )

#include "RakiEngine/Sounds/_XBoxSeries/PCMStreamSoundNative_xboxseries.h"

#elif defined ( RAKI_PLATFORM_PS3 )

#ifndef RAKI_PCMSTREAMSOUNDNATIVE_PS3_H
#include "RakiEngine/Sounds/_PS3/PCMStreamSoundNative_PS3.h"
#endif // RAKI_PCMSTREAMSOUNDNATIVE_PS3_H

#elif defined ( RAKI_PLATFORM_WII )

#ifndef RAKI_PCMSTREAMSOUNDNATIVE_WII_H
#include "RakiEngine/Sounds/_WII/PCMStreamSoundNative_WII.h"
#endif // RAKI_PCMSTREAMSOUNDNATIVE_WII_H

#elif defined ( RAKI_PLATFORM_PS5 )

#include "RakiEngine/Sounds/_PS5/PCMStreamSoundNative_PS5.h"

#elif defined ( RAKI_PLATFORM_NINTENDO )

#include "RakiEngine/Sounds/_Nintendo/PCMStreamSoundNative_Nintendo.h"

#endif 

#ifndef ITF_WII


//#define LOG_PCMDATAPROVIDER




namespace ITF
{


PCMDataProvider::PCMDataProvider( const u32 _nbChannels )
    : SoundStreamDataProvider( _nbChannels )
    , m_currentPart( NULL )
    , m_nextPart( NULL )
    , m_musicInstance( NULL )
    , m_sound( NULL )
    , m_currentAsyncStreamIndex( 0 )
    , m_musicInstanceHasRequestedNewPlay( bfalse )
{
    ITF_UNUSED(m_currentAsyncStreamIndex);
    m_asyncStream.allocateBuffers( raki::EngineInitializer::singleton().getStreamBufferSize( WAVE_FORMAT_PCM, _nbChannels ) );
}


PCMDataProvider::~PCMDataProvider()
{
}


void PCMDataProvider::reinit()
{
    m_asyncStream.reinit();

    m_currentPart = NULL;
    m_nextPart = NULL;
}


void PCMDataProvider::init( MusicInstance * _musicInstance, const MusicPart * _part, raki::PCMStreamSoundNative * _sound )
{
    ITF_ASSERT_MSG( _part->getWaveFile(), "Resource has not yet been loaded by loading thread..." );

    m_musicInstanceHasRequestedNewPlay = bfalse;

    m_musicInstance = _musicInstance;
    m_sound = _sound;

    reinit();

    ITF_ASSERT_MSG( ( ( MusicPart* )_part )->isPhysicallyLoaded(), "Resource has not yet been loaded by loading thread..." );
    ITF_ASSERT_MSG( _part->getPrefetchData(), "First part must always have prefetch data");
    ITF_WARNING_CATEGORY(Sound,NULL,_part->getPrefetchData(),"First part must always have prefetch data : please change this on %s",_part->getPartName().getDebugString());

    setNextPart( _part );

    setNextStreamNow();
}


void PCMDataProvider::update( bool _launchNewReadRequests )
{
    m_asyncStream.update( _launchNewReadRequests );
}


void PCMDataProvider::setNextPart( const MusicPart * _part )
{
    m_nextPart = _part;

    if ( m_nextPart )
    {
        if ( m_nextPart->getWaveFile() )
        {
            PathString_t pathName;
            m_nextPart->getPath().getString(pathName);
            const String filenameCooked = FILESERVER->getCookedName( pathName );

            m_fileStream.openInStream( filenameCooked );

            const char data[] = "data";
            const u32 chunkOffset = m_nextPart->getWaveFile()->getChunkOffsetInStream( data );
            const u32 chunkSize = m_nextPart->getWaveFile()->getChunkSize( data );

            m_partStream.set( &m_fileStream, chunkOffset, chunkSize );

            const raki::MemoryBlock * prefetchData = m_nextPart->getPrefetchData();
            if ( prefetchData )
                m_asyncStream.setStreamData( prefetchData->getBuffer(), prefetchData->getSize(), &m_partStream, (void*)m_nextPart );
            else
                m_asyncStream.setStreamData( NULL, 0, &m_partStream, (void*)m_nextPart );

#ifdef LOG_PCMDATAPROVIDER
            RAKI_OUTPUT("0x%x PCMDataProvider::setNextPart <%ls> prefetch 0x%x size 0x%x", 
                this, m_nextPart->getPath().getString().cStr(), prefetchData ? prefetchData->getBuffer() : NULL, prefetchData ? (u32)prefetchData->getSize() : 0 );
#endif // LOG_PCMDATAPROVIDER
        }
        else
        {
            LOG( "MusicPart <%s> data unavailable", m_nextPart->getPath().getString8().cStr() );
            ITF_ASSERT_MSG( 0, "MusicPart <%s> data unavailable", m_nextPart->getPath().getString8().cStr() );
        }

        m_asyncStream.setStreamingThread( MUSICMANAGER->getStreamingThread() );
    }
    else 
    {
        m_asyncStream.reinit();
    }
}

const u32 PCMDataProvider::getSizeInTicks() const
{
    if ( m_currentPart )
        return m_currentPart->getSizeInBars() * m_currentPart->getBeatsPerBar() * RAKI_MUSICALTIME_NB_SUBDIVISIONS_IN_BEAT;

    return 0;
}


const u32 PCMDataProvider::getSizeInBars() const
{
    return m_currentPart ? m_currentPart->getSizeInBars() : 0;
}


const u32 PCMDataProvider::getNbBeatsPerBar() const
{
    return m_currentPart ? m_currentPart->getBeatsPerBar() : 0;
}



const f64 PCMDataProvider::getSamplesPerTicks() const
{
    if ( m_currentPart )
    {
        u32 sizeInTicks = getSizeInTicks();

        if ( sizeInTicks )
            return (f64)m_currentPart->getSampleSize() / (f64)sizeInTicks;
    }

    return 0.0;
}



const u64 PCMDataProvider::getEntireSampleSize() 
{
    return m_currentPart ? m_currentPart->getSampleSize() : 0;
}


const u64 PCMDataProvider::pullData( void * _buffer, const u64 _sizeToRead )
{
    return m_asyncStream.stream( _buffer, _sizeToRead );
}


const u64 PCMDataProvider::getRemainingByteSize()  
{
    const u64 size = m_asyncStream.getSize();
    const u64 position = m_asyncStream.getCurrentPos();

    ITF_ASSERT( position <= size );

    return ( size - position );
}


void PCMDataProvider::prefetchNextStream()
{
    if ( m_musicInstance )
    {
        const MusicPart * nextPart = m_musicInstance->getNextPart();

        if ( getClientData() != nextPart )
        {
#ifdef LOG_PCMDATAPROVIDER
            RAKI_OUTPUT( "0x%x PCMDataProvider::prefetchNextStream setPart\n", this );
#endif // LOG_PCMDATAPROVIDER

            // set part on next async stream 
            setNextPart( nextPart );
        }
        else
        {
#ifdef LOG_PCMDATAPROVIDER
            RAKI_OUTPUT( "0x%x PCMDataProvider::prefetchNextStream end of part\n", this );
#endif // LOG_PCMDATAPROVIDER

            m_nextPart = m_currentPart;
        }
    }

    m_asyncStream.resetWritePos();

#ifdef LOG_PCMDATAPROVIDER
    RAKI_OUTPUT("0x%x PCMDataProvider::prefetchNextStream setting m_musicInstanceHasRequestedNewPlay %s -> false\n", 
        this, m_musicInstanceHasRequestedNewPlay ? "true" : "false" );
#endif // LOG_PCMDATAPROVIDER

    m_musicInstanceHasRequestedNewPlay = bfalse;
}


void PCMDataProvider::setNextStreamNow()
{
    if ( m_musicInstanceHasRequestedNewPlay && m_musicInstance && m_musicInstance->canReinit() )
    {
        // new play has been requested

        const MusicPart * nextPart = m_musicInstance->getNextPart();

        if ( nextPart )
        {
            if ( getClientData() != nextPart )
            {
                ITF_ASSERT_MSG( nextPart->getPrefetchData(), "Music: requested part <%s> does not have prefetch buffer...", nextPart->getName().getDebugString() );

                // safety...
                if ( nextPart->getPrefetchData() )
                {
                    m_musicInstance->reinit();

                    // set part on next async stream 
                    setNextPart( nextPart );
                }
            }
        }
        else
        {
            if ( m_sound )
                m_sound->requestEndAfterSubmittedBuffers();
        }
    }


#ifdef LOG_PCMDATAPROVIDER
    RAKI_OUTPUT("0x%x PCMDataProvider::setNextStreamNow m_currentPart 0x%x m_nextPart 0x%x setting m_musicInstanceHasRequestedNewPlay %s -> false\n", 
        this, m_currentPart, m_nextPart, m_musicInstanceHasRequestedNewPlay ? "true" : "false" );
#endif // LOG_PCMDATAPROVIDER

    m_asyncStream.seekBeginning();

    m_currentPart = m_nextPart;
    m_nextPart = NULL;

    m_musicInstanceHasRequestedNewPlay = bfalse;
}



const u64 PCMDataProvider::getEntireByteSize() 
{
    return m_currentPart ? m_currentPart->getByteSize() : 0;
}


const u64 PCMDataProvider::getCurrentSamplePosition() 
{
    if ( m_currentPart )
    {
        const u64 position = m_asyncStream.getCurrentPos();
        const u64 bytesPerSample = (u64)m_currentPart->getBytesPerSample();

        if ( bytesPerSample )
            return position / bytesPerSample;
    }

    return 0;
}


const void * PCMDataProvider::getClientData()  
{
    return m_currentPart;
}


void PCMDataProvider::setMusicInstance( MusicInstance * _musicInstance )
{
    m_musicInstance = _musicInstance;
}


const bbool PCMDataProvider::canReinit()
{
    return m_asyncStream.canReinit() ? btrue : bfalse;
}


const bbool PCMDataProvider::isStopping()
{
    return ( m_sound ? m_sound->isStopping() : false ) ? btrue : bfalse;
}


void PCMDataProvider::musicInstanceHasRequestedNewPlay()
{
#ifdef LOG_PCMDATAPROVIDER
    RAKI_OUTPUT("0x%x PCMDataProvider::musicInstanceHasRequestedNewPlay setting m_musicInstanceHasRequestedNewPlay %s -> TRUE\n", 
        this, m_musicInstanceHasRequestedNewPlay ? "true" : "false" );
#endif // LOG_PCMDATAPROVIDER

    m_musicInstanceHasRequestedNewPlay = btrue;
}





} // namespace ITF 


#endif // !ITF_WII
#endif
