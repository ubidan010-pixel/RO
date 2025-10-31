#include "precompiled_engine.h"

#ifdef ITF_SUPPORT_RAKI
#ifndef _ITF_ADPCMDATAPROVIDER_WII_H_
#include "engine/sound/raki/ADPCMDataProvider_Wii.h"
#endif // _ITF_ADPCMDATAPROVIDER_WII_H_


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

#ifndef RAKI_STREAMSOUNDNATIVE_WII_H
#include "RakiEngine/Sounds/_Wii/StreamSoundNative_Wii.h"
#endif // RAKI_STREAMSOUNDNATIVE_WII_H





//#define LOG_ADPCMDATAPROVIDER




namespace ITF
{


ADPCMDataProvider::ADPCMDataProvider( const u32 _nbChannels )
    : WiiAdpcmSoundStreamDataProvider( _nbChannels )
    , m_currentPart( NULL )
    , m_nextPart( NULL )
    , m_musicInstance( NULL )
    , m_sound( NULL )
    , m_currentAsyncStreamIndex( 0 )
    , m_musicInstanceHasRequestedNewPlay( bfalse )
    , m_dspadpcmL( NULL )
    , m_dspadpcmR( NULL )
{
    m_asyncStream.allocateBuffers( raki::EngineInitializer::singleton().getStreamBufferSize( WAVE_FORMAT_ADPCM, _nbChannels ) );
}


ADPCMDataProvider::~ADPCMDataProvider()
{
}


void ADPCMDataProvider::reinit()
{
    m_asyncStream.reinit();

    m_currentPart = NULL;
    m_nextPart = NULL;
}


void ADPCMDataProvider::init( MusicInstance * _musicInstance, const MusicPart * _part, raki::StreamSoundNative * _sound )
{
    ITF_ASSERT_MSG( _part->getWaveFile(), "Resource has not yet been loaded by loading thread..." );

    m_musicInstanceHasRequestedNewPlay = bfalse;

    m_musicInstance = _musicInstance;
    m_sound = _sound;

    reinit();

    ITF_ASSERT_MSG( ( ( MusicPart* )_part )->isPhysicallyLoaded(), "Resource has not yet been loaded by loading thread..." );

    setNextPart( _part );

    setNextStreamNow();
}


void ADPCMDataProvider::update( bool _launchNewReadRequests )
{
    m_asyncStream.update( _launchNewReadRequests );
}


void ADPCMDataProvider::setNextPart( const MusicPart * _part )
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

            const char datS[] = "datS"; // interlaced stereo
            const char datL[] = "datL"; // mono 

            const char * dataTag = m_nextPart->getWaveFile()->getChunkSize( datS ) ? datS : datL;

            const u32 chunkOffset = m_nextPart->getWaveFile()->getChunkOffsetInStream( dataTag );
            const u32 chunkSize = m_nextPart->getWaveFile()->getChunkSize( dataTag );

            m_dspadpcmL = m_nextPart->getWaveFile()->getChunkData( "dspL" );
            m_dspadpcmR = m_nextPart->getWaveFile()->getChunkData( "dspR" );

#ifndef ITF_FINAL
            if ( m_nextPart->getWaveFile()->getChunkSize( datS ) )
            {
                ITF_ASSERT_CRASH( m_dspadpcmL, "DSPADPCM data consistency problem");
                ITF_ASSERT_CRASH( m_dspadpcmR, "DSPADPCM data consistency problem");
            }
            else
            {
                ITF_ASSERT_CRASH( m_nextPart->getWaveFile()->getChunkSize( datL ), "DSPADPCM data consistency problem");
                ITF_ASSERT_CRASH( m_dspadpcmL, "DSPADPCM data consistency problem");
                ITF_ASSERT_CRASH( !m_dspadpcmR, "DSPADPCM data consistency problem");
            }
#endif // ITF_FINAL

            m_partStream.set( &m_fileStream, chunkOffset, chunkSize );

            const raki::MemoryBlock * prefetchData = m_nextPart->getPrefetchData();
            if ( prefetchData )
                m_asyncStream.setStreamData( prefetchData->getBuffer(), prefetchData->getSize(), &m_partStream, (void*)m_nextPart );
            else
                m_asyncStream.setStreamData( NULL, 0, &m_partStream, (void*)m_nextPart );
        }
        else
        {
            LOG( "MusicPart <%s> data unavailable", m_currentPart->getPath().getString8().cStr() );
            ITF_ASSERT_MSG( 0, "MusicPart data unavailable" );
        }

        m_asyncStream.setStreamingThread( MUSICMANAGER->getStreamingThread() );
    }
    else 
    {
        m_asyncStream.reinit();
    }
}

const u32 ADPCMDataProvider::getSizeInTicks() const
{
    if ( m_currentPart )
        return m_currentPart->getSizeInBars() * m_currentPart->getBeatsPerBar() * RAKI_MUSICALTIME_NB_SUBDIVISIONS_IN_BEAT;

    return 0;
}


const u32 ADPCMDataProvider::getSizeInBars() const
{
    return m_currentPart ? m_currentPart->getSizeInBars() : 0;
}


const u32 ADPCMDataProvider::getNbBeatsPerBar() const
{
    return m_currentPart ? m_currentPart->getBeatsPerBar() : 0;
}



const f64 ADPCMDataProvider::getSamplesPerTicks() const
{
    if ( m_currentPart )
    {
        u32 sizeInTicks = getSizeInTicks();

        if ( sizeInTicks )
            return (f64)m_currentPart->getSampleSize() / (f64)sizeInTicks;
    }

    return 0.0;
}



const u64 ADPCMDataProvider::getEntireSampleSize() 
{
    return m_currentPart ? m_currentPart->getSampleSize() : 0;
}


const u64 ADPCMDataProvider::pullData( void * _buffer, const u64 _sizeToRead )
{
    return m_asyncStream.stream( _buffer, _sizeToRead );
}


const u64 ADPCMDataProvider::getRemainingByteSize()  
{
    const u64 size = m_asyncStream.getSize();
    const u64 position = m_asyncStream.getCurrentPos();

    ITF_ASSERT( position <= size );

    return ( size - position );
}


void ADPCMDataProvider::prefetchNextStream()
{
    if ( m_musicInstance )
    {
        const MusicPart * nextPart = m_musicInstance->getNextPart();

        if ( getClientData() != nextPart )
        {
#ifdef LOG_ADPCMDATAPROVIDER
            RAKI_OUTPUT( "0x%x ADPCMDataProvider::prefetchNextStream setPart nextPart 0x%x\n", this, nextPart );
#endif // LOG_ADPCMDATAPROVIDER

            // set part on next async stream 
            setNextPart( nextPart );
        }
        else
        {
#ifdef LOG_ADPCMDATAPROVIDER
            RAKI_OUTPUT( "0x%x ADPCMDataProvider::prefetchNextStream end of part\n", this );
#endif // LOG_ADPCMDATAPROVIDER

            m_nextPart = m_currentPart; 
        }
    }

    m_asyncStream.resetWritePos();

#ifdef LOG_ADPCMDATAPROVIDER
    RAKI_OUTPUT("0x%x ADPCMDataProvider::prefetchNextStream setting m_musicInstanceHasRequestedNewPlay %s -> false m_nextPart 0x%x\n", 
        this, m_musicInstanceHasRequestedNewPlay ? "true" : "false", m_nextPart );
#endif // LOG_ADPCMDATAPROVIDER

    m_musicInstanceHasRequestedNewPlay = bfalse;
}


void ADPCMDataProvider::setNextStreamNow()
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


#ifdef LOG_ADPCMDATAPROVIDER
    RAKI_OUTPUT("0x%x ADPCMDataProvider::setNextStreamNow m_currentPart 0x%x m_nextPart 0x%x setting m_musicInstanceHasRequestedNewPlay %s -> false\n", 
        this, m_currentPart, m_nextPart, m_musicInstanceHasRequestedNewPlay ? "true" : "false" );
#endif // LOG_ADPCMDATAPROVIDER

    m_asyncStream.seekBeginning();

    m_currentPart = m_nextPart;
    m_nextPart = NULL;

    m_musicInstanceHasRequestedNewPlay = bfalse;
}



const u64 ADPCMDataProvider::getEntireByteSize() 
{
    return m_currentPart ? m_currentPart->getByteSize() : 0;
}


const u64 ADPCMDataProvider::getCurrentSamplePosition() 
{
    if ( m_currentPart )
    {
        const u64 position = m_asyncStream.getCurrentPos();

        if ( m_dspadpcmR )
            return 14 * position / 16;
        else 
            return 14 * position / 8;
    }

    return 0;
}


const void * ADPCMDataProvider::getClientData()  
{
    return m_currentPart;
}


void ADPCMDataProvider::setMusicInstance( MusicInstance * _musicInstance )
{
    m_musicInstance = _musicInstance;
}


const bbool ADPCMDataProvider::canReinit()
{
    return m_asyncStream.canReinit() ? btrue : bfalse;
}


const bbool ADPCMDataProvider::isStopping()
{
    return ( m_sound ? m_sound->isStopping() : false ) ? btrue : bfalse;
}


void ADPCMDataProvider::musicInstanceHasRequestedNewPlay()
{
#ifdef LOG_ADPCMDATAPROVIDER
    RAKI_OUTPUT("0x%x ADPCMDataProvider::musicInstanceHasRequestedNewPlay setting m_musicInstanceHasRequestedNewPlay %s -> TRUE\n", 
        this, m_musicInstanceHasRequestedNewPlay ? "true" : "false" );
#endif // LOG_ADPCMDATAPROVIDER

    m_musicInstanceHasRequestedNewPlay = btrue;
}


void * ADPCMDataProvider::getDspadpcmL()
{
    return m_dspadpcmL;
}


void * ADPCMDataProvider::getDspadpcmR()
{
    return m_dspadpcmR;
}




} // namespace ITF 

#endif
