#ifndef _ITF_BUNDLEPROFILER_H_
#define _ITF_BUNDLEPROFILER_H_

#ifndef ITF_FINAL

#ifndef _ITF_PROFILERMACRO_H_
#include "engine/profiler/profilerMacro.h"
#endif //_ITF_PROFILERMACRO_H_

namespace ITF
{

    class Blob;

class BundleProfileEventScope
{

public:

    enum bundleProfileEvent
    {
        type_endload = 0,
        type_read = 1,
        type_compression = 2,
        type_open = 3,
    };

    BundleProfileEventScope() {};
    BundleProfileEventScope(bundleProfileEvent _eventType,f32 _time);

    BundleProfileEventScope(bundleProfileEvent _eventType, u64 _offset,u32 _readbytes,u64 _filesize,const String& _filename,const String& _bundlename);
    ~BundleProfileEventScope();

    void fillBlob(Blob& _blob) const;


protected:
    ITF_THREAD_ID  m_threadID;
    bundleProfileEvent m_eventType;
    u64 m_filesize;
    u32 m_startFrame;
    u32 m_endFrame;

    u64 m_offset;
    u32 m_readbytes;
    PROFILER_TYPECOUNT  m_highTimerStart;
    f32                 m_timeReadMs;

    String m_filename;
    String m_bundlename;

};

#define PROFILE_BUNDLEOPEN(__bundlename) BundleProfileEventScope  pev(BundleProfileEventScope::type_open,0,0,0,String::emptyString,__bundlename);
#define PROFILE_BUNDLE(__offset,__readbytes,__filesize,__filename,__bundlename) BundleProfileEventScope  pev(BundleProfileEventScope::type_read,__offset,__readbytes,__filesize,__filename,__bundlename);
#define PROFILE_BUNDLECOMPRESSION(__offset,__readbytes,__filesize,__filename,__bundlename) BundleProfileEventScope  pev(BundleProfileEventScope::type_compression,__offset,__readbytes,__filesize,__filename,__bundlename);
#define PROFILE_ENDLOAD(__timeload) BundleProfileEventScope  pev(BundleProfileEventScope::type_endload,__timeload);


//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

class BundleProfileManager:public TemplateSingleton<BundleProfileManager>
{

public:

    BundleProfileManager();
    ~BundleProfileManager();

    void addProfileEventScope(const BundleProfileEventScope* _scope);
    void writeBlob(Blob& _blob);
    ITF_INLINE bbool isEnable() const {return m_bEnable;}
    void setEnable(bbool _bEnable) {m_bEnable = _bEnable;}
 
protected:

    ITF_VECTOR<BundleProfileEventScope*> m_arrayProfileEvent;
    ITF_THREAD_CRITICAL_SECTION m_cs;
    bbool m_bEnable;
};

}
#else

#define PROFILE_BUNDLEOPEN(__bundlename) {}
#define PROFILE_BUNDLE(__offset,__readbytes,__filesize,__filename,__bundlename) {}
#define PROFILE_BUNDLECOMPRESSION(__offset,__readbytes,__filesize,__filename,__bundlename) {}
#define PROFILE_ENDLOAD(__timeload) {}

#endif //ITF_FINAL

#endif //_ITF_BUNDLEPROFILER_H_