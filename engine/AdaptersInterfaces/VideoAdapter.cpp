#include "precompiled_engine.h"

#include "engine/video/videoHandle.h"

#ifndef _ITF_VIDEOADAPTER_H_
#include "engine/AdaptersInterfaces/VideoAdapter.h"
#endif //_ITF_VIDEOADAPTER_H_


namespace ITF
{

    videoHandle* VideoAdapter::open(const String& _filename,bbool _bPlayFromMemory)
    {
        videoHandle* pVideoHandle = new videoHandle();
        pVideoHandle->setPlayedFromMemory(_bPlayFromMemory);
		if( _filename == "fresco" || _filename == "fresco_rayman")
			m_haveSubtitle = btrue;

        if ( pVideoHandle->open(_filename) )
        {
			m_isPlaying = true;
            m_pendingHandle.push_back(pVideoHandle);
            return pVideoHandle;
        } else
        {
            SF_DEL(pVideoHandle);
            return NULL;
        }
    }

    void VideoAdapter::destroy()
    {
        for (ITF_VECTOR<videoHandle*>::iterator iter = m_pendingHandle.begin();iter!=m_pendingHandle.end();++iter)
        {
            videoHandle* pVideoHandle = (*iter);
            pVideoHandle->close();
            SF_DEL(pVideoHandle);
        }

        m_pendingHandle.clear();
    }

    void VideoAdapter::update(f32 _dt)
    {
        for (ITF_VECTOR<videoHandle*>::iterator iter = m_pendingHandle.begin();iter!=m_pendingHandle.end();++iter)
        {
            videoHandle* pVideoHandle = (*iter);
            pVideoHandle->update();
        }
    }

    void VideoAdapter::render()
    {

        for (ITF_VECTOR<videoHandle*>::iterator iter = m_pendingHandle.begin();iter!=m_pendingHandle.end();++iter)
        {
            videoHandle* pVideoHandle = (*iter);
            pVideoHandle->render();
        }

    }

    void VideoAdapter::close(videoHandle* _pHandle)
    {
		m_isPlaying = bfalse;
		m_haveSubtitle = bfalse;
		m_isRendering = false;
        ITF_VECTOR<videoHandle*>::iterator iter = find(m_pendingHandle.begin(),m_pendingHandle.end(),_pHandle);
        if (iter!=m_pendingHandle.end())
        {
            _pHandle->close();
            SF_DEL(_pHandle);
            m_pendingHandle.erase(iter);
        }

    }

    videoHandle* VideoAdapter::getHandleAt(u32 _n)
    {
        if (_n <  m_pendingHandle.size())
            return m_pendingHandle[_n];
        else
            return NULL;
    }

}// namespace ITF


