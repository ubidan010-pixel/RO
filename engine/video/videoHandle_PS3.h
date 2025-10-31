#ifndef ITF_VIDEOHANDLE_PS3_H_
#define ITF_VIDEOHANDLE_PS3_H_

struct VideoPlayerInit;

namespace cell
{
    namespace Sail
    {
        class hlPlayer;
        class memallocator;
        class CustomSource;
    }
}

namespace ITF
{

class videoHandle : public videoHandleBase
{
public:
    videoHandle();
    ~videoHandle();
    void destroy();

    void update();
    
    void play();
    void pause();
    void stop();
    
    bbool open(const String& _filename);
    void close();

    void render();

    bbool getCurrentTime(f32& _timeInSeconds);

protected:
    static void onFlipStatic(u64 _tHis);
    void onFlip();

    VideoPlayerInit * m_playerInit;
    cell::Sail::hlPlayer * m_player;
    cell::Sail::memallocator * m_memAllocator;
    cell::Sail::CustomSource * m_customSrc;
    
    ITF_VertexBuffer * m_pVertexBuffer;

    bool m_isStopping;
    bool m_firstFrameDisplayed;
    bool m_clearScreen;
};

}

#endif //ITF_VIDEOHANDLE_PS3_H_
