#ifndef _ITF_XMA2DATAPROVIDER_X360_H_
#define _ITF_XMA2DATAPROVIDER_X360_H_



#ifndef _ITF_RAKIINCLUDES_
#include "engine/sound/raki/RakiIncludes.h"
#endif // _ITF_RAKIINCLUDES_


#ifndef RAKI_SOUNDSTREAMDATABUFFER_H
#include "RakiEngine/Sounds/SoundStreamDataBuffer.h"
#endif // RAKI_SOUNDSTREAMDATABUFFER_H

#ifndef _ITF_READFILESTREAM_H_
#include "engine/sound/raki/ReadFileStream.h"
#endif //_ITF_READFILESTREAM_H_

#ifndef RAKI_ASYNCREADSTREAMER_H
#include "RakiEngine/Data/Stream/AsyncReadStreamer.h"
#endif // RAKI_ASYNCREADSTREAMER_H

#ifndef RAKI_SEEKABLESTREAMPART_H
#include "RakiEngine/Data/Stream/SeekableStreamPart.h"
#endif // RAKI_SEEKABLESTREAMPART_H

#ifndef RAKI_ASYNCREADSTREAMER_H
#include "RakiEngine/Data/Stream/AsyncReadStreamer.h"
#endif // RAKI_ASYNCREADSTREAMER_H




namespace raki
{
    class XMA2StreamSoundNative;
}



namespace ITF 
{


class MusicPart;
class MusicInstance;



class XMA2DataProvider 
    : public raki::XMA2SoundStreamDataProvider
{
public:

    XMA2DataProvider( const u32 _nbChannels );

    virtual ~XMA2DataProvider();

    void reinit();

    void init( MusicInstance * _musicInstance, const MusicPart * _part, raki::XMA2StreamSoundNative * _sound );

    void update( bool _launchNewReadRequests );

    virtual const u32 getSizeInTicks() const;
    virtual const u32 getSizeInBars() const;
    virtual const u32 getNbBeatsPerBar() const;
    virtual const f64 getSamplesPerTicks() const;

    // SoundStreamDataProvider 
    virtual const u64 pullData( void * _buffer, const u64 _sizeToRead );
    virtual const u64 getRemainingByteSize(); 
    virtual const u64 getEntireByteSize(); 
    virtual const u64 getCurrentSamplePosition(); 
    virtual const void * getClientData();
    virtual const u64 getEntireSampleSize();
    virtual void prefetchNextStream();
    virtual void setNextStreamNow();

    void setMusicInstance( MusicInstance * _musicInstance );

    const bbool canReinit();

    const bbool isStopping();

    void musicInstanceHasRequestedNewPlay();

    ITF_INLINE const MusicPart * getCurrentPart(); 

    ITF_INLINE const bbool hasValidStream();

private:

    void setNextPart( const MusicPart * _part );

    raki::AsyncReadStreamer m_asyncStream;

    const MusicPart * m_currentPart;
    const MusicPart * m_nextPart;

    MusicInstance * m_musicInstance;

    ReadFileStream m_fileStream;
    raki::SeekableStreamPart m_partStream;

    raki::XMA2StreamSoundNative * m_sound;

    int m_currentAsyncStreamIndex;

    // this flag is used to know if a play has been requested after prefetch has been requested on a part:
    // 1) some time before end of part -> prefetch is requested on next part 
    // 2) client requests new play - data requested must have prefetch data 
    // 3) just before end of part : new data with prefetch is played 
    bbool m_musicInstanceHasRequestedNewPlay;
};



// inline implementation 


const MusicPart * XMA2DataProvider::getCurrentPart()
{
    return m_currentPart;
}


const bbool XMA2DataProvider::hasValidStream()
{
    return m_asyncStream.hasValidStream() ? btrue : bfalse;
}





} // namespace ITF 


#endif // _ITF_XMA2DATAPROVIDER_X360_H_