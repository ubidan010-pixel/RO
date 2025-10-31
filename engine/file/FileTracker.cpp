#include "precompiled_engine.h"

#ifdef ITF_WINDOWS

#ifndef _ITF_FILETRACKER_H_
#include "engine/file/FileTracker.h"
#endif //_ITF_FILETRACKER_H_

#ifndef _ITF_SYSTEMADAPTER_
#include "core/AdaptersInterfaces/SystemAdapter.h"
#endif //_ITF_SYSTEMADAPTER_

namespace ITF
{

// #define TRACK_TIMING 1


ThreadLocalStorage FileTracker::m_TLSInstance; //each ITF thread support 1 FileTrackerScope at time

FileTracker::FileTracker(const String& _szName)
{
    m_FileList = newAlloc(mId_System, ITF_VECTOR<String>());
    m_Hierarchy = 1;
    m_szName = _szName;


#ifdef TRACK_TIMING
    m_startTime = SYSTEM_ADAPTER->getTime();
#else // !TRACK_TIMING
    m_startTime = 0.0f;
#endif //TRACK_TIMING
}


FileTracker::~FileTracker()
{
    SF_DEL(m_FileList);

#ifdef TRACK_TIMING
    f32 timeTracked = static_cast<f32>(SYSTEM_ADAPTER->getTime()-m_startTime);
    LOG("Filetracker %s %0.02f ms", StringToUTF8(m_szName).get(), timeTracked*1000.0f);
#endif //TRACK_TIMING
}


void FileTracker::init(const String& _szName)
{
    //if the scope already exists ;we just increase the hierarchy
    FileTracker* pInstance = getInstance();
    if (pInstance)
    {
        pInstance->m_Hierarchy++;
        return; 
    }

    m_TLSInstance.setValue(pInstance);
}

void FileTracker::destroy()
{
    FileTracker* pInstance = getInstance();
    ITF_ASSERT(pInstance);
    pInstance->m_Hierarchy--;
    if (pInstance->m_Hierarchy == 0)
    {
        SF_DEL(pInstance);
        m_TLSInstance.setNULL();
    }
}

void FileTracker::recordFile(const String& _szfilename)
{
    FileTracker* pInstance = getInstance();
    if (!pInstance)
        return;

   pInstance->addFile(_szfilename);
}

void FileTracker::addFile(const String& _szFile)
{
    ITF_ASSERT(m_FileList);

    //To track multiple access in loading 
    /*ITF_VECTOR<String>::iterator iter  = find(m_FileList->begin(),m_FileList->end(),_szFile);
    if (iter != m_FileList->end())
    {
        String msg;
        msg.setTextFormat("This file is loading multiple time in the same thread please change this behavior %ls",*iter);
        StringConverter str(msg);
        ITF_ASSERT_MSG(0,str.getChar());
    }*/

    m_FileList->push_back(_szFile);
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

FileTrackerScope::FileTrackerScope(const String& _szName)
{
    FileTracker::init(_szName);
}

FileTrackerScope::~FileTrackerScope()
{
    FileTracker::destroy();
}

}

#endif //ITF_WINDOWS
