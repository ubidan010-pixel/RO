#include "precompiled_engine.h"


#include "engine/sound/common/DummyDataProvider.h"



#ifndef _ITF_MUSICPART_H_
#include "engine/sound/Music/MusicPart.h"
#endif // _ITF_MUSICPART_H_

#ifndef _ITF_FILESERVER_H_
#include "core/file/FileServer.h"
#endif //_ITF_FILESERVER_H_


#ifndef _ITF_MUSICMANAGER_H_
#include "engine/gameplay/musicmanager.h"
#endif //_ITF_MUSICMANAGER_H_


#ifndef _ITF_MUSICINSTANCE_H_
#include "engine/sound/Music/MusicInstance.h"
#endif // _ITF_MUSICINSTANCE_H_




//#define LOG_PCMDATAPROVIDER




namespace ITF
{

DummyDataProvider::DummyDataProvider( ):
     m_currentPart( NULL )
    , m_nextPart( NULL )
    , m_musicInstance( NULL )
    , m_sound( NULL )
    , m_currentAsyncStreamIndex( 0 )
    , m_musicInstanceHasRequestedNewPlay( bfalse )
{
}




void DummyDataProvider::reinit()
{
  //  m_asyncStream.reinit();

    m_currentPart = NULL;
    m_nextPart = NULL;
}


void DummyDataProvider::init( MusicInstance * _musicInstance, const MusicPart * _part, void* _sound )
{

    m_musicInstanceHasRequestedNewPlay = bfalse;

    m_musicInstance = _musicInstance;
    m_sound = _sound;

    reinit();

    ITF_ASSERT_MSG( ( ( MusicPart* )_part )->isPhysicallyLoaded(), "Resource has not yet been loaded by loading thread..." );

    setNextPart( _part );

    setNextStreamNow();
}


void DummyDataProvider::update( bool _launchNewReadRequests )
{
  //  m_asyncStream.update( _launchNewReadRequests );
}


void DummyDataProvider::setNextPart( const MusicPart * _part )
{
    m_nextPart = _part;

    if ( m_nextPart )
    {
        if (m_nextPart->getWwiseEventGuid().isValid())
        {


#ifdef LOG_PCMDATAPROVIDER
            RAKI_OUTPUT("0x%x PCMDataProvider::setNextPart <%ls> ", 
                this, m_nextPart->getPath().getString().cStr() );
#endif // LOG_PCMDATAPROVIDER
        }
        else
        {
            LOG( "MusicPart <%s> data unavailable", m_nextPart->getPath().getString8().cStr() );
          //  ITF_ASSERT_MSG( 0, "MusicPart <%s> data unavailable", m_nextPart->getPath().getString8().cStr() );
        }

    }

}

#define RAKI_MUSICALTIME_NB_SUBDIVISIONS_IN_BEAT        24
const u32 DummyDataProvider::getSizeInTicks() const
{
    if (m_currentPart )
        return m_currentPart->getSizeInBars() * m_currentPart->getBeatsPerBar() * RAKI_MUSICALTIME_NB_SUBDIVISIONS_IN_BEAT;

    return 0;
}


const u32 DummyDataProvider::getSizeInBars() const
{
    return m_currentPart ? m_currentPart->getSizeInBars() : 0;
}


const u32 DummyDataProvider::getNbBeatsPerBar() const
{
    return m_currentPart ? m_currentPart->getBeatsPerBar() : 0;
}



// const f64 DummyDataProvider::getSamplesPerTicks() const
// {
//     if ( m_currentPart )
//     {
//         u32 sizeInTicks = getSizeInTicks();
// 
//         if ( sizeInTicks )
//             return (f64)m_currentPart->getSampleSize() / (f64)sizeInTicks;
//     }
// 
//     return 0.0;
// }
// 
// 
// 
// const u64 DummyDataProvider::getEntireSampleSize()
// {
//     return m_currentPart ? m_currentPart->getSampleSize() : 0;
// }





// const u64 DummyDataProvider::getRemainingByteSize()
// {
//     const u64 size = m_asyncStream.getSize();
//     const u64 position = m_asyncStream.getCurrentPos();
// 
//     ITF_ASSERT( position <= size );
// 
//     return ( size - position );
// }


void DummyDataProvider::prefetchNextStream()
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


#ifdef LOG_PCMDATAPROVIDER
    RAKI_OUTPUT("0x%x PCMDataProvider::prefetchNextStream setting m_musicInstanceHasRequestedNewPlay %s -> false\n", 
        this, m_musicInstanceHasRequestedNewPlay ? "true" : "false" );
#endif // LOG_PCMDATAPROVIDER

    m_musicInstanceHasRequestedNewPlay = bfalse;
}


const void* DummyDataProvider::getClientData()
{
    return m_currentPart;
}


void DummyDataProvider::setNextStreamNow()
{
    if ( m_musicInstanceHasRequestedNewPlay && m_musicInstance && m_musicInstance->canReinit() )
    {
        // new play has been requested

        const MusicPart * nextPart = m_musicInstance->getNextPart();

        if ( nextPart )
        {
            if ( getClientData() != nextPart )
            {
                    // set part on next async stream 
                    setNextPart( nextPart );

            }
        }
        else
        {
//             if ( m_sound )
//                 m_sound->requestEndAfterSubmittedBuffers();
        }
    }


#ifdef LOG_PCMDATAPROVIDER
    RAKI_OUTPUT("0x%x PCMDataProvider::setNextStreamNow m_currentPart 0x%x m_nextPart 0x%x setting m_musicInstanceHasRequestedNewPlay %s -> false\n", 
        this, m_currentPart, m_nextPart, m_musicInstanceHasRequestedNewPlay ? "true" : "false" );
#endif // LOG_PCMDATAPROVIDER


    m_currentPart = m_nextPart;
    m_nextPart = NULL;

    m_musicInstanceHasRequestedNewPlay = bfalse;
}



// const u64 DummyDataProvider::getEntireByteSize()
// {
//     return m_currentPart ? m_currentPart->getByteSize() : 0;
// }


// const u64 DummyDataProvider::getCurrentSamplePosition()
// {
//     if ( m_currentPart )
//     {
//         const u64 position = m_asyncStream.getCurrentPos();
//         const u64 bytesPerSample = (u64)m_currentPart->getBytesPerSample();
// 
//         if ( bytesPerSample )
//             return position / bytesPerSample;
//     }
// 
//     return 0;
// }




void DummyDataProvider::setMusicInstance( MusicInstance * _musicInstance )
{
    m_musicInstance = _musicInstance;
}


const bbool DummyDataProvider::canReinit()
{
    return btrue;
}


const bbool DummyDataProvider::isStopping()
{
    return false;
    //return ( m_sound ? m_sound->isStopping() : false ) ? btrue : bfalse;
}


void DummyDataProvider::musicInstanceHasRequestedNewPlay()
{
#ifdef LOG_PCMDATAPROVIDER
    RAKI_OUTPUT("0x%x PCMDataProvider::musicInstanceHasRequestedNewPlay setting m_musicInstanceHasRequestedNewPlay %s -> TRUE\n", 
        this, m_musicInstanceHasRequestedNewPlay ? "true" : "false" );
#endif // LOG_PCMDATAPROVIDER

    m_musicInstanceHasRequestedNewPlay = btrue;
}





} // namespace ITF 


