#ifndef _ITF_DATAPROVIDER_H_
#define _ITF_DATAPROVIDER_H_

namespace ITF 
{


class MusicPart;
class MusicInstance;



class DummyDataProvider 
{

public:
    DummyDataProvider();

    ~DummyDataProvider() {}


    void reinit();

    void init( MusicInstance * _musicInstance, const MusicPart * _part, void* _sound );

     void update( bool _launchNewReadRequests );

     const u32 getSizeInTicks() const;
     const u32 getSizeInBars() const;
     const u32 getNbBeatsPerBar() const;
/*     const f64 getSamplesPerTicks() const;*/

    // SoundStreamDataProvider 
//     const u64 pullData( void * _buffer, const u64 _sizeToRead );
//     const u64 getRemainingByteSize(); 
//     const u64 getEntireByteSize(); 
//     const u64 getCurrentSamplePosition(); 
    const void * getClientData();
/*    const u64 getEntireSampleSize();*/
    void prefetchNextStream();
    void setNextStreamNow();

    void setMusicInstance( MusicInstance * _musicInstance );

    const bbool canReinit();

    const bbool isStopping();

    void musicInstanceHasRequestedNewPlay();

    ITF_INLINE const MusicPart * getCurrentPart(); 

    ITF_INLINE const bbool hasValidStream() { return true; }

private:

    void setNextPart( const MusicPart * _part );


    const MusicPart * m_currentPart;
    const MusicPart * m_nextPart;
    MusicInstance * m_musicInstance;

    void* m_sound;

    int m_currentAsyncStreamIndex;

    // this flag is used to know if a play has been requested after prefetch has been requested on a part:
    // 1) some time before end of part -> prefetch is requested on next part 
    // 2) client requests new play - data requested must have prefetch data 
    // 3) just before end of part : new data with prefetch is played 
    bbool m_musicInstanceHasRequestedNewPlay;
};




} // namespace ITF 


#endif 
