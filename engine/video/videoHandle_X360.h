#ifndef _ITF_VIDEOHANDLE_X360_H_
#define _ITF_VIDEOHANDLE_X360_H_

#include <xmedia2.h>

namespace ITF
{

class videoHandle : public videoHandleBase
{
public:

    struct videoContext
    {
    public:
        videoContext():m_pFile(NULL) {};
        void set(File* _pFile);
        File* getFile() {return m_pFile;}
        void close();

        ITF_THREAD_CRITICAL_SECTION  m_cs;
    private:

        File  *m_pFile;

    };

    videoHandle():m_dwTextureID(0),m_xmvPlayer(NULL),m_pVertexBuffer(NULL),m_pMemory(NULL),m_fileSize(U32_INVALID) {};

    ~videoHandle();

    void update();
    void play();
    void pause();
    void close();

    bbool open(const String& _filename);
    void stop();
    void render();

    bbool getCurrentTime(f32& _timeInSeconds);

protected:

    ResourceID  textureIDY[2];
    ResourceID  textureIDU[2];
    ResourceID  textureIDV[2];

    IXMedia2XmvPlayer* m_xmvPlayer;
    ITF_VertexBuffer* m_pVertexBuffer;
    XMEDIA_VIDEO_DESCRIPTOR m_VideoDescriptor;
    u32 m_dwTextureID;
    u8*    m_pMemory;
    u32    m_fileSize;

    videoContext m_context;
};

}

#endif //_ITF_VIDEOHANDLE_X360_H_