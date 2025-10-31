#include "precompiled_SoundAdapter_XAudio.h"
#ifdef ITF_SUPPORT_RAKI 

#ifdef ITF_WINDOWS

#ifndef _ITF_CAPTUREAPO_H_
#include "adapters/SoundAdapter_XAudio/CaptureWin32/CaptureAPO.h"
#endif //_ITF_CAPTUREAPO_H_

#include "SDKwavefile.h"

namespace ITF
{

CaptureAPO::CaptureAPO(): CXAudioXAPOBase<CaptureAPO, CaptureAPOParams>()
{
    m_Position = 0;
    m_BufferCopy = NULL;
    m_CaptureAPO_MaxSize = 0;
    m_BufferOverflow    = bfalse;
    m_bEnable           = bfalse;
}


void CaptureAPO::init(u32 channel,u32 samples)
{
    const u32 Bitsper           = 4;
    const u32 RecordTimeSeconds = 360;

    m_CaptureAPO_MaxSize = channel*samples*Bitsper*RecordTimeSeconds;  
    m_BufferCopy = newAlloc(mId_AudioEngine,unsigned  char[m_CaptureAPO_MaxSize]);
}


void CaptureAPO::stop()
{
    CWaveFile file;
    m_bEnable = bfalse;
    //wait !!
    WAVEFORMATEX wf = WaveFormat();
    wf.cbSize = 0;
    wf.wFormatTag = WAVE_FORMAT_IEEE_FLOAT;
    u32 bytesWritten;
    file.Open((LPWSTR) m_Filename.cStr(),&wf,WAVEFILE_WRITE);

    if (!m_BufferOverflow)
    {
        file.Write(m_Position,m_BufferCopy,&bytesWritten);
    }
    else
    {
        //copy end-position
        file.Write(m_CaptureAPO_MaxSize-m_Position,m_BufferCopy+m_Position,&bytesWritten);

        //copy 0-position
        file.Write(m_Position,m_BufferCopy,&bytesWritten);
    }
}

 
CaptureAPO::~CaptureAPO()
{
    SF_DEL_ARRAY(m_BufferCopy);
}


void CaptureAPO::start(String _Filename)
{
    m_bEnable           = btrue;
    m_Position          = 0; 
    m_Filename          = _Filename;
    m_BufferOverflow    = bfalse;
}


void CaptureAPO::DoProcess( const CaptureAPOParams& params, FLOAT32* __restrict pData, UINT32 cFrames, UINT32 cChannels )
{
    if(exists() && get().isEnable() && cFrames)
    {
        WAVEFORMATEX wf = WaveFormat();
        unsigned int size = cFrames * cChannels * (wf.wBitsPerSample >> 3) ;
        
        if (m_BufferCopy && m_Position+size < m_CaptureAPO_MaxSize)
        {
            ITF_Memcpy(m_BufferCopy+m_Position,pData,size);
            m_Position+=size;
        }
        else //overflow the buffer,restart to zero
        {
            m_Position = 0;
            m_BufferOverflow = btrue;
        }
    }
}


}

#endif
#endif //ITF_WINDOWS
