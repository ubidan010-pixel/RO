#ifndef _ITF_VIDEOHANDLE_WII_H_
#define _ITF_VIDEOHANDLE_WII_H_

#include "bink.h"

#ifndef BINKTEXTURESH
#include "engine/video/binktextures.h"
#endif //BINKTEXTURESH

#define USE_ITF_IO

namespace ITF
{

class videoHandle : public videoHandleBase
{
public:
    struct BINKSET
    {
        BINKTEXTURESET  m_TextureSet;
        renderTarget*   m_RT;
        bbool           mb_UseAFXBfr;
    };

public:
    videoHandle():
          m_Bink(0),
          m_isPaused(bfalse),
          m_pMemory(NULL),
          m_wasInterruptedFromAnIOError(bfalse),
          m_lastFrameNumAfterIOError(0),
          m_oneFramePlayed(bfalse),
          m_SaveScreenProtectionState(btrue)
    {
#ifdef USE_ITF_IO
        m_BinkFile = NULL;
#endif // USE_ITF_IO
        ITF_MemSet(&m_Set.m_TextureSet, 0, sizeof(m_Set.m_TextureSet));
        m_Set.m_RT          = NULL;
        m_Set.mb_UseAFXBfr  = bfalse;
    };
    ~videoHandle() {destroy();};

    void            destroy();

    bbool           open(const String& _filename);
    void            close();

    bbool           update(bbool _ignoreIOError = bfalse);
    void            render();
    void            pause();
    void            resume();

    bbool           getCurrentTime(f32& _timeInSeconds);
    void            onIOError();
    bbool           isPaused() const { return m_isPaused; }

    ITF_INLINE void setReadError(u32 _error) { ITF_ASSERT(m_Bink); m_Bink->ReadError = _error; }
#ifdef USE_ITF_IO
public:
    File*           m_BinkFile;
    static videoHandle*    getVideoHandle();
    static bbool           isVideoPlaying();
#endif // USE_ITF_IO

protected:
    bbool           handleError();

    HBINK           m_Bink;
    u32             m_lastFrameNumAfterIOError;
    u32             m_wasInterruptedFromAnIOError:1,
                    m_isPaused:1,
                    m_oneFramePlayed:1,
                    m_SaveScreenProtectionState:1,
                    m_Padding:28;
    String          m_lastVideoOpened;

    BINKSET         m_Set;

    u8              *m_pMemory;
};

}

#endif //_ITF_VIDEOHANDLE_WII_H_
