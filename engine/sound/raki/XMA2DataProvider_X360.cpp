#include "precompiled_engine.h"

#ifdef ITF_SUPPORT_RAKI
#ifndef _ITF_XMA2DATAPROVIDER_X360_H_
#include "engine/sound/raki/XMA2DataProvider_X360.h"
#endif //_ITF_XMA2DATAPROVIDER_X360_H_



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

#ifndef RAKI_XMA2STREAMSOUNDNATIVE_H
#include "RakiEngine/Sounds/_XAudio2/XMA2StreamSoundNative_X360.h"
#endif // RAKI_XMA2STREAMSOUNDNATIVE_H





//#define LOG_XMA2DATAPROVIDER




namespace ITF
{


XMA2DataProvider::XMA2DataProvider( const u32 _nbChannels )
    : XMA2SoundStreamDataProvider( _nbChannels )
    , m_currentPart( NULL )
    , m_nextPart( NULL )
    , m_musicInstance( NULL )
    , m_sound( NULL )
    , m_currentAsyncStreamIndex( 0 )
    , m_musicInstanceHasRequestedNewPlay( bfalse )
{
    m_asyncStream.allocateBuffers( raki::EngineInitializer::singleton().getStreamBufferSize( WAVE_FORMAT_XMA2, _nbChannels ) );
}


XMA2DataProvider::~XMA2DataProvider()
{
}


void XMA2DataProvider::reinit()
{
    m_asyncStream.reinit();

    m_currentPart = NULL;
    m_nextPart = NULL;
}


void XMA2DataProvider::init( MusicInstance * _musicInstance, const MusicPart * _part, raki::XMA2StreamSoundNative * _sound )
{
    ITF_ASSERT_MSG( _part->getWaveFile(), "Resource has not yet been loaded by loading thread..." );

    m_musicInstanceHasRequestedNewPlay = bfalse;

    m_musicInstance = _musicInstance;
    m_sound = _sound;

    reinit();

    ITF_ASSERT_MSG( ( ( MusicPart* )_part )->isPhysicallyLoaded(), "Resource has not yet been loaded by loading thread..." );
    ITF_ASSERT_MSG( _part->getPrefetchData(), "First part must always have prefetch data");

    setNextPart( _part );

    setNextStreamNow();
}


void XMA2DataProvider::update( bool _launchNewReadRequests )
{
    m_asyncStream.update( _launchNewReadRequests );
}


void XMA2DataProvider::setNextPart( const MusicPart * _part )
{
    m_nextPart = _part;

    if ( m_nextPart )
    {
        if ( m_nextPart->getWaveFile() )
        {
            String pathName;
            
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

#ifdef LOG_XMA2DATAPROVIDER
            RAKI_OUTPUT("0x%x XMA2DataProvider::setNextPart <%ls> prefetch 0x%x size 0x%x", 
                this, m_nextPart->getPath().getString().cStr(), prefetchData ? prefetchData->getBuffer() : NULL, prefetchData ? (u32)prefetchData->getSize() : 0 );
#endif // LOG_XMA2DATAPROVIDER
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

const u32 XMA2DataProvider::getSizeInTicks() const
{
    if ( m_currentPart )
        return m_currentPart->getSizeInBars() * m_currentPart->getBeatsPerBar() * RAKI_MUSICALTIME_NB_SUBDIVISIONS_IN_BEAT;

    return 0;
}


const u32 XMA2DataProvider::getSizeInBars() const
{
    return m_currentPart ? m_currentPart->getSizeInBars() : 0;
}


const u32 XMA2DataProvider::getNbBeatsPerBar() const
{
    return m_currentPart ? m_currentPart->getBeatsPerBar() : 0;
}



const f64 XMA2DataProvider::getSamplesPerTicks() const
{
    if ( m_currentPart )
    {
        u32 sizeInTicks = getSizeInTicks();

        if ( sizeInTicks )
            return (f64)m_currentPart->getSampleSize() / (f64)sizeInTicks;
    }

    return 0.0;
}



const u64 XMA2DataProvider::getEntireSampleSize() 
{
    return m_currentPart ? m_currentPart->getSampleSize() : 0;
}


const u64 XMA2DataProvider::pullData( void * _buffer, const u64 _sizeToRead )
{
    return m_asyncStream.stream( _buffer, _sizeToRead );
}


const u64 XMA2DataProvider::getRemainingByteSize()  
{
    const u64 size = m_asyncStream.getSize();
    const u64 position = m_asyncStream.getCurrentPos();

    ITF_ASSERT( position <= size );

    return ( size - position );
}


void XMA2DataProvider::prefetchNextStream()
{
    if ( m_musicInstance )
    {
        const MusicPart * nextPart = m_musicInstance->getNextPart();

        if ( getClientData() != nextPart )
        {
#ifdef LOG_XMA2DATAPROVIDER
            RAKI_OUTPUT( "0x%x XMA2DataProvider::prefetchNextStream setPart\n", this );
#endif // LOG_XMA2DATAPROVIDER

            // set part on next async stream 
            setNextPart( nextPart );
        }
        else
        {
#ifdef LOG_XMA2DATAPROVIDER
            RAKI_OUTPUT( "0x%x XMA2DataProvider::prefetchNextStream end of part\n", this );
#endif // LOG_XMA2DATAPROVIDER

            m_nextPart = m_currentPart;
        }
    }

    m_asyncStream.resetWritePos();

#ifdef LOG_XMA2DATAPROVIDER
    RAKI_OUTPUT("0x%x XMA2DataProvider::prefetchNextStream setting m_musicInstanceHasRequestedNewPlay %s -> false\n", 
        this, m_musicInstanceHasRequestedNewPlay ? "true" : "false" );
#endif // LOG_XMA2DATAPROVIDER

    m_musicInstanceHasRequestedNewPlay = bfalse;
}


void XMA2DataProvider::setNextStreamNow()
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


#ifdef LOG_XMA2DATAPROVIDER
    RAKI_OUTPUT("0x%x XMA2DataProvider::setNextStreamNow m_currentPart 0x%x m_nextPart 0x%x setting m_musicInstanceHasRequestedNewPlay %s -> false\n", 
        this, m_currentPart, m_nextPart, m_musicInstanceHasRequestedNewPlay ? "true" : "false" );
#endif // LOG_XMA2DATAPROVIDER

    m_asyncStream.seekBeginning();

    m_currentPart = m_nextPart;
    m_nextPart = NULL;

    m_musicInstanceHasRequestedNewPlay = bfalse;
}



const u64 XMA2DataProvider::getEntireByteSize() 
{
    return m_currentPart ? m_currentPart->getByteSize() : 0;
}


const u64 XMA2DataProvider::getCurrentSamplePosition() 
{
    if ( m_currentPart )
    {
        const u64 bytesAlreadySubmitted = m_asyncStream.getCurrentPos();;

        RAKI_ASSERT( bytesAlreadySubmitted <= m_asyncStream.getSize() );

        if ( !bytesAlreadySubmitted )
            return 0;

        if ( bytesAlreadySubmitted == m_asyncStream.getSize() )
            return getEntireSampleSize();

        XMA2WAVEFORMATEX * xma2format = (XMA2WAVEFORMATEX*)m_currentPart->getWaveFile()->getChunkData( "fmt " );
        u32 * seekTable = (u32*)m_currentPart->getWaveFile()->getChunkData( "seek" );
        u32 nbSeekBlocks = m_currentPart->getWaveFile()->getChunkSize( "seek" ) / sizeof( u32 );

        RAKI_ASSERT( ( bytesAlreadySubmitted % xma2format->BytesPerBlock ) == 0 );

        u64 nbBlocksSubmitted = bytesAlreadySubmitted / (u64)xma2format->BytesPerBlock;
        RAKI_ASSERT( nbBlocksSubmitted );
        RAKI_ASSERT( nbBlocksSubmitted < nbSeekBlocks );

        return seekTable[ nbBlocksSubmitted - 1 ];
    }

    return 0;
}


const void * XMA2DataProvider::getClientData()  
{
    return m_currentPart;
}


void XMA2DataProvider::setMusicInstance( MusicInstance * _musicInstance )
{
    m_musicInstance = _musicInstance;
}


const bbool XMA2DataProvider::canReinit()
{
    return m_asyncStream.canReinit() ? btrue : bfalse;
}


const bbool XMA2DataProvider::isStopping()
{
    return ( m_sound ? m_sound->isStopping() : false ) ? btrue : bfalse;
}


void XMA2DataProvider::musicInstanceHasRequestedNewPlay()
{
#ifdef LOG_XMA2DATAPROVIDER
    RAKI_OUTPUT("0x%x XMA2DataProvider::musicInstanceHasRequestedNewPlay setting m_musicInstanceHasRequestedNewPlay %s -> TRUE\n", 
        this, m_musicInstanceHasRequestedNewPlay ? "true" : "false" );
#endif // LOG_XMA2DATAPROVIDER

    m_musicInstanceHasRequestedNewPlay = btrue;
}





} // namespace ITF 

#endif
