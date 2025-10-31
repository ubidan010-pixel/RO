#include "precompiled_engine.h"

#ifndef ITF_FINAL
#ifndef _ITF_PROFILERMACRO_H_
#include "engine/profiler/profilerMacro.h"
#endif //_ITF_PROFILERMACRO_H_

#ifndef _ITF_BUNDLEPROFILER_H_
#include "engine/file/bundleProfiler.h"
#endif //_ITF_BUNDLEPROFILER_H_

#ifndef _ITF_BLOB_H_
#include "core/Blob.h"
#endif //_ITF_BLOB_H_

#ifndef _ITF_FILESERVER_H_
#include "core/file/FileServer.h"
#endif //_ITF_FILESERVER_H_

namespace ITF
{
//-----------------------------------------------------------------------------------------------------------------------------------------------//

BundleProfileEventScope::BundleProfileEventScope(bundleProfileEvent _eventType,u64 _offset,u32 _readbytes,u64 _filesize,const String& _filename,const String& _bundlename)
{
    if (!BundleProfileManager::get().isEnable())
        return;

    m_eventType  = _eventType;
    m_offset     = _offset;
    m_readbytes  = _readbytes;
    m_filename   = _filename;
    m_bundlename = _bundlename;
    m_filesize   = _filesize;
    m_startFrame = CURRENTFRAME;
    m_threadID   = Synchronize::getCurrentThreadId();

    PROFILER_QUERYPERFORMANCECOUNTER(m_highTimerStart);
    
}

//-----------------------------------------------------------------------------------------------------------------------------------------------//

BundleProfileEventScope::BundleProfileEventScope(bundleProfileEvent _eventType,f32 _time):m_eventType(_eventType)
{
    if (!BundleProfileManager::get().isEnable())
        return;

    m_startFrame = CURRENTFRAME;
    m_timeReadMs = _time*1000.0f;
}
//-----------------------------------------------------------------------------------------------------------------------------------------------//

BundleProfileEventScope::~BundleProfileEventScope()
{
    if (!BundleProfileManager::get().isEnable())
            return;

    if (m_eventType != type_endload)
    {
        PROFILER_TYPECOUNT  m_highTimerEnd;
        PROFILER_QUERYPERFORMANCECOUNTER(m_highTimerEnd);
        PROFILER_TYPECOUNT   _delta = PROFILER_DELTA(m_highTimerEnd,m_highTimerStart);
        f64 frequency = PROFILER_GETFREQUENCY();
        m_timeReadMs = static_cast<f32>(PROFILER_CONVERT_TOMS(_delta,frequency ));
        m_endFrame = CURRENTFRAME;
    }

    BundleProfileManager::get().addProfileEventScope(this);
}

//-----------------------------------------------------------------------------------------------------------------------------------------------//

void BundleProfileEventScope::fillBlob(Blob& _blob) const
{
    _blob.pushUInt32(m_eventType);
    _blob.pushUInt32(m_startFrame);
    _blob.pushFloat32(m_timeReadMs);

    if (m_eventType != BundleProfileEventScope::type_endload)
    {
        _blob.pushUInt64(m_offset);
        _blob.pushUInt32(m_readbytes);
        _blob.pushUInt64(m_filesize);

        _blob.pushUInt32(m_endFrame);
        _blob.pushString(m_filename);
        _blob.pushString(m_bundlename);
        _blob.pushUInt32((u32)m_threadID);
      
    }
}

//-----------------------------------------------------------------------------------------------------------------------------------------------//

BundleProfileManager::BundleProfileManager():m_bEnable(bfalse)
{
    Synchronize::createCriticalSection(&m_cs);
}

//-----------------------------------------------------------------------------------------------------------------------------------------------//

BundleProfileManager::~BundleProfileManager()
{
    Synchronize::destroyCriticalSection(&m_cs);

}


//----------------------------------- PROFILER -------------------------------------------------------------------------------------------------//
//-----------------------------------------------------------------------------------------------------------------------------------------------//

void BundleProfileManager::writeBlob(Blob& _blob)
{
    csAutoLock cs(m_cs);

    _blob.pushUInt32(m_arrayProfileEvent.size());
    _blob.pushString(FILESERVER->getPlatform());

    for ( ITF_VECTOR<BundleProfileEventScope*>::const_iterator iter = m_arrayProfileEvent.begin();iter!=m_arrayProfileEvent.end();iter++)
    {
        const BundleProfileEventScope* profileEvent = (*iter);
        profileEvent->fillBlob(_blob);
        SF_DEL(profileEvent);
    }

    m_arrayProfileEvent.clear();

}

//-----------------------------------------------------------------------------------------------------------------------------------------------//


void BundleProfileManager::addProfileEventScope(const BundleProfileEventScope* _scope)
{
    csAutoLock cs(m_cs);
    BundleProfileEventScope * pScope = new BundleProfileEventScope();
    *pScope = *_scope;
    m_arrayProfileEvent.push_back(pScope);
}



//-----------------------------------------------------------------------------------------------------------------------------------------------//


}
#endif //ITF_FINAL
