#ifndef _ITF_VIDEOHANDLE_WIN_H_
#define _ITF_VIDEOHANDLE_WIN_H_

#pragma once

#ifdef ITF_SUPPORT_BINK

#include "bink.h"

namespace ITF
{

class videoHandle : public videoHandleBase
{
public:
    videoHandle():m_Bink(0),m_pVertexBuffer(NULL) {};

    ~videoHandle(){};

    void update();
    void play();
    void pause();
    void close();

    bbool open(const String& _filename);
    void stop();
    void render();
    
    bbool getCurrentTime(f32& _timeInSeconds);

    void OnResetDevice();
    void OnLostDevice();

    // Draw Textures.
    ResourceID YtextureDraw;
    ResourceID cRtextureDraw;
    ResourceID cBtextureDraw;
    ResourceID AtextureDraw;
    
    ResourceID Ytexture[2];
    ResourceID cRtexture[2];
    ResourceID cBtexture[2];
    ResourceID Atexture[2];

    ITF_VertexBuffer* m_pVertexBuffer;

protected:

    HBINK  m_Bink;
};

}

#endif //ITF_SUPPORT_BINK

#endif //_ITF_VIDEOHANDLE_WIN_H_
