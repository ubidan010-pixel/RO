#ifndef _ITF_CAPTUREAPO_H_
#define _ITF_CAPTUREAPO_H_

#ifndef _ITF_TEMPLATESINGLETON_H_
#include "core/templateSingleton.h"
#endif //_ITF_TEMPLATESINGLETON_H_

#ifndef _ITF_AUDIOCAPTURE_H_
#include "engine/sound/AudioCapture.h"
#endif //_ITF_AUDIOCAPTURE_H_

#ifndef _ITF_XAUDIOAPOBASE_H_
#include "adapters/SoundAdapter_XAudio/CaptureWin32/XAudioAPOBase.h"
#endif //_ITF_XAUDIOAPOBASE_H_


namespace ITF
{

struct CaptureAPOParams
{
    
};

class _declspec( uuid("{A4945B8A-EB14-4c96-8067-DF726B528091}")) 
CaptureAPO: public CXAudioXAPOBase<CaptureAPO, CaptureAPOParams>,public AudioCapture
{
public:
    CaptureAPO();
    ~CaptureAPO();

    void start(String _Filename);
    void stop();

    bbool isEnable() {return m_bEnable;}
    void init(u32 channel,u32 samples);

    void DoProcess( const CaptureAPOParams&, FLOAT32* __restrict pData, UINT32 cFrames, UINT32 cChannels );

protected:
    bbool m_bEnable;
    String m_Filename;

    u8* m_BufferCopy;
    u32 m_Position;
    u32 m_CaptureAPO_MaxSize;

    bbool m_BufferOverflow;
    

};

};

#endif //_ITF_CAPTUREAPO_H_
