#ifndef _ITF_VIDEOADAPTER_H_
#define _ITF_VIDEOADAPTER_H_

#ifndef _ITF_TEMPLATESINGLETON_H_
#include "core/templateSingleton.h"
#endif //_ITF_TEMPLATESINGLETON_H_

namespace  ITF
{
class videoHandle;

class VideoAdapter:public TemplateSingleton<VideoAdapter>
{
public:
    VideoAdapter()
	{
		m_isPlaying = false;
		m_haveSubtitle = false;
		m_isRendering = false;
	};

    static void create() {newAlloc(mId_System, VideoAdapter());}
    void destroy();

    videoHandle* open(const String& _filename,bbool _bPlayFromMemory);
    void close(videoHandle* _pHandle);
    void render();
    void update(f32 _dt);

    u32 getNumberHandle() {return m_pendingHandle.size();}
    videoHandle* getHandleAt(u32 _n);

	bool isPlaying(){ return m_isPlaying; };

	bool isRendering(){ return m_isRendering; };
	void setRendering( bool _val) { m_isRendering = _val; };

	bool haveSubtitle(){ return m_haveSubtitle; };
	void SetHaveSubtitle(bool val) { m_haveSubtitle = val; }

protected:

	bool m_isPlaying;
	bool m_isRendering;
	bool m_haveSubtitle;


    ITF_VECTOR<videoHandle*> m_pendingHandle;
};

#define VIDEO_ADAPTER VideoAdapter::getptr()

} // namespace ITF



#endif //_ITF_VIDEOADAPTER_H_
