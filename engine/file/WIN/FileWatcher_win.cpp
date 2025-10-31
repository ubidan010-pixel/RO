#include "precompiled_engine.h"

#include "core/math/BitTweak.h"

#ifndef _ITF_FILEWATCHER_H_
#include "engine/file/FileWatcher.h"
#endif //_ITF_FILEWATCHER_H_

#ifndef _ITF_DIRECTORY_H_
#include "core/file/Directory.h"
#endif //_ITF_DIRECTORY_H_

#ifndef _ITF_FILEPATH_H_
#include "core/file/filepath.h"
#endif //_ITF_FILEPATH_H_

#ifndef _ITF_THREAD_H_
#include "core/system/Thread.h"
#endif //_ITF_THREAD_H_

#ifndef _ITF_FILESERVER_H_
#include "core/file/FileServer.h"
#endif //_ITF_FILESERVER_H_

#ifndef _ITF_DEBUGINFO_H_
#include "engine/debug/debugInfo.h"
#endif //_ITF_DEBUGINFO_H_

#ifndef _ITF_WATCHDIRECTORYLISTENER_H
#include "engine/file/WatchDirectoryListener.h"
#endif  //_ITF_WATCHDIRECTORYLISTENER_H

namespace ITF
{

#define HasOverlappedIoCompleted(myoverLap) (((DWORD)(myoverLap)->Internal) != STATUS_PENDING)
#define BUFFER_COUNT 1024

void vCheckAligned(void* _pUserAddress, u32 _align)
{
    ITF_ASSERT(uPtr(_pUserAddress) == BitTweak::GetAboveAlign(uPtr(_pUserAddress), uPtr(_align)));
}

//-----------------------------------------------------------------------------------------------------------------------

WatchDirectory::WatchDirectory(const String& _szPath,const String& _szFilter,WatchDirectoryListener* _pListener,u32 _index)
{
    Synchronize::createCriticalSection(&m_cs);

    m_szDirName = _szPath;
    

    _pListener->addFilter(_szFilter);
    m_pListeners.push_back(_pListener);

    m_hDirectory = CreateFile( (LPCWSTR)_szPath.cStr(), 
        FILE_LIST_DIRECTORY,              
        FILE_SHARE_READ|FILE_SHARE_WRITE|FILE_SHARE_DELETE, 
        NULL,                              
        OPEN_EXISTING,                    
        FILE_FLAG_BACKUP_SEMANTICS|FILE_FLAG_OVERLAPPED ,        
        NULL                              
        );

    ITF_MemSet(&m_OverLap, 0, sizeof(m_OverLap));

    DWORD BytesReturned = 0;
    m_dwflags_notification = FILE_NOTIFY_CHANGE_LAST_WRITE|FILE_NOTIFY_CHANGE_FILE_NAME;

    //TODO allocate aligned on 4
    pNotifyBuffer = new_array(FILE_NOTIFY_INFORMATION,BUFFER_COUNT,mId_System);

    //the buffer must aligned on a DWORD boundary
    vCheckAligned(pNotifyBuffer,4);

    char nameEvent[MAX_PATH];
    sprintf_s(nameEvent,MAX_PATH,"eventWatcher%d",_index);
    Synchronize::createEvent(&m_hEvent,nameEvent);
    m_OverLap.hEvent = m_hEvent;
    m_uBufferSize =  sizeof(FILE_NOTIFY_INFORMATION)*BUFFER_COUNT;
#ifdef ASSERT_ENABLED
    BOOL res  = 
#endif
        ReadDirectoryChangesW(m_hDirectory,pNotifyBuffer, m_uBufferSize,TRUE,m_dwflags_notification,&BytesReturned,&m_OverLap,NULL);
    ITF_ASSERT(res);
}

//-----------------------------------------------------------------------------------------------------------------------

WatchDirectory::~WatchDirectory()
{
    SF_DEL_ARRAY(pNotifyBuffer);
    Synchronize::destroyCriticalSection(&m_cs);
    Synchronize::destroyEvent(&m_hEvent);
    ::CloseHandle(m_hDirectory);
}

//-----------------------------------------------------------------------------------------------------------------------
    
void WatchDirectory::addListener(WatchDirectoryListener* _pListener)
{
    csAutoLock cs(m_cs);
    for (u32 i = 0; i < m_pListeners.size(); i++)
    {
        if (m_pListeners[i] == _pListener)
            return;
    }
    m_pListeners.push_back(_pListener);
}

//-----------------------------------------------------------------------------------------------------------------------

void WatchDirectory::addEvent(WatchFileEvent& _evt)
{
    ITF_VECTOR<WatchFileEvent>::iterator it = m_vWatchEventList.begin();
    for (; it != m_vWatchEventList.end(); ++it)
    {
        if ((*it).m_szFilename == _evt.m_szFilename)
        {
            (*it).m_timer = GetTickCount();
            return;
        }
    }
    _evt.m_timer = GetTickCount();
    m_vWatchEventList.push_back(_evt);
}

//-----------------------------------------------------------------------------------------------------------------------

void WatchDirectory::process()
{
    DWORD BytesReturned;

    DWORD dwNumberOfBytesTransferred;
    BOOL bChange=0;

    if (HasOverlappedIoCompleted(&m_OverLap))
    {
        bChange = GetOverlappedResult(m_hDirectory,&m_OverLap,&dwNumberOfBytesTransferred,TRUE);
    }


    if (bChange)
    {
        PFILE_NOTIFY_INFORMATION lpFileNotification =(PFILE_NOTIFY_INFORMATION) pNotifyBuffer;

        while (lpFileNotification)
        {
            //WCHAR to U16...
            u32 size = lpFileNotification->FileNameLength/2;
            u16* text= new u16[size+2];

            ITF_Memcpy(text,&lpFileNotification->FileName[0],lpFileNotification->FileNameLength);
            text[size] = 0;
            String filename(FilePath::normalizePath((u16*)text));

            SF_DEL_ARRAY(text);

            String fullPath = m_szDirName;
            bbool record = btrue;
            fullPath += filename;
            {
                csAutoLock cs(m_cs);
                //check already in the list.. 
                for (ITF_VECTOR<WatchFileEvent>::iterator iterFileEvent = m_vWatchEventList.begin();iterFileEvent!=m_vWatchEventList.end();++iterFileEvent)
                {
                    if ((*iterFileEvent).m_szFilename == fullPath)
                    {
                        record = bfalse;
                        break;
                    }        
                }

                if (record)
                {
#if ITF_DEBUG_LEVEL > 0
                    //LOG("onWatcher resource db for %S\n", fullPath.cStr());
#endif //ITF_DEBUG_LEVEL

                    if (!Directory::exists(fullPath))
                    {
                        WatchFileEvent evt(fullPath,lpFileNotification->Action);
                        addEvent(evt);
                        
                    }
#if ITF_DEBUG_LEVEL > 0
                    else
                    {
                         //LOG("onWatcher resource db for %S SKIPPED!!!!!!!!!\n", fullPath.cStr());
                    }
#endif //ITF_DEBUG_LEVEL
                }
            }


            if(lpFileNotification->NextEntryOffset != 0)
                lpFileNotification =  (PFILE_NOTIFY_INFORMATION) (((char *) lpFileNotification) + lpFileNotification->NextEntryOffset);
            else
                lpFileNotification = NULL;
        }
    }


    //Assign a new notification..
#ifdef ASSERT_ENABLED
    BOOL res  = 
#endif
        ReadDirectoryChangesW(m_hDirectory,pNotifyBuffer, m_uBufferSize,TRUE,m_dwflags_notification,&BytesReturned,&m_OverLap,NULL);
    ITF_ASSERT(res);
}

//-----------------------------------------------------------------------------------------------------------------------

void WatchDirectory::update()
{
    //TODO Should be nice to avoid this cs it s meanly here because resource loading is not async
    csAutoLock cs(m_cs);

    if (m_vWatchEventList.size())
    {
        for (ITF_VECTOR<WatchFileEvent>::iterator iterFileEvent = m_vWatchEventList.begin();iterFileEvent!=m_vWatchEventList.end();)
        {
            // using GetTickCount() because we are in W32 code and we dont want to depend on frame-rate
            if (GetTickCount() - (*iterFileEvent).m_timer < ITF_WatchFileEvent_defaultDelay)
            {
                ++iterFileEvent;
                continue;
            }
            String extension = FilePath::getExtension((*iterFileEvent).m_szFilename);
            u32 action = (*iterFileEvent).m_dwAction;
            for (ITF_VECTOR<WatchDirectoryListener*>::iterator iterListener = m_pListeners.begin();iterListener != m_pListeners.end(); ++iterListener)
            {
                WatchDirectoryListener* listener  = *iterListener;
                for (u32 iFilter=0; iFilter<listener->getFilterCount(); iFilter++)
                {
                    if (listener->getFilter(iFilter) == extension)
                    {
                        String abs_path = (*iterFileEvent).m_szFilename;
                        String rel_path = abs_path;
                        if(!FilePath::transformToRelative(rel_path))
                        {
                            ITF_ASSERT_MSG(0, "WatchDirectory get event outside the data root folder on file : '%ls'", abs_path.cStr());
                            return;
                        }

                        listener->addTracing(abs_path);

                        switch (action)
                        {
                        case FILE_ACTION_ADDED:
                            listener->onNewFile(abs_path);
                            break;

                        case FILE_ACTION_REMOVED:
                            listener->onDeleteFile(abs_path);
                            break;

                        case FILE_ACTION_MODIFIED:
                            listener->onModifiedFile(abs_path);
                            break;
                        }
                        break;
                    }
                }
           } 
            iterFileEvent  = m_vWatchEventList.erase(iterFileEvent);
        }
    }


}
//-----------------------------------------------------------------------------------------------------------------------

void FileWatcher::buildEvents()
{
    //build the final list..
    SF_DEL_ARRAY(m_Events);

    m_Events = new_array(ITF_THREAD_EVENT,m_vEventUsed.size(),mId_System);
        
    m_uEventCount = 0;
    for (ITF_VECTOR<ITF_THREAD_EVENT> ::iterator iterEvent = m_vEventUsed.begin();iterEvent!=m_vEventUsed.end();++iterEvent)
    {
        m_Events[m_uEventCount++] = *iterEvent;
    }
}

//-----------------------------------------------------------------------------------------------------------------------

void FileWatcher::wakeup()
{
    Synchronize::setEvent(&m_SpecialEvent);
}

//-----------------------------------------------------------------------------------------------------------------------

void FileWatcher::addEvents(WatchDirectory* _pWatchDir)
{   
    {
        csAutoLock cs(m_csEvent);
        m_vEventUsed.push_back(_pWatchDir->getEvent());
        buildEvents();
    }
    
    //wake up the thread to tell a new watcher is available...
    wakeup();
}

//-----------------------------------------------------------------------------------------------------------------------

u32 WINAPI Thread_Watcher (void* _lpParameter)
{   
    FileWatcher* pFileWatcher = (FileWatcher*) _lpParameter;
    pFileWatcher->process();
    return 0;
}

//-----------------------------------------------------------------------------------------------------------------------

WatchDirectory* FileWatcher::getWatchDirectories(const ITF_THREAD_EVENT _theEvent)
{
    for (ITF_VECTOR<WatchDirectory*> ::iterator iter = m_vWatchDirectory.begin();iter!=m_vWatchDirectory.end();++iter)
    {
        if (_theEvent == (*iter)->getEvent())
            return *iter;
    }

   return NULL;
}

//-----------------------------------------------------------------------------------------------------------------------

void FileWatcher::process()
{
    for(;;)
    {
        ThreadSettings::threadStartFrame();
        
        //make a copy
        u32 uEventCount = 0;
        HANDLE *events  = NULL;
       
        {
            csAutoLock cs(m_csEvent);
            uEventCount = m_uEventCount;
            events = new_array(ITF_THREAD_EVENT,m_uEventCount,mId_System);
            ITF_Memcpy(events,m_Events,sizeof(ITF_THREAD_EVENT)*uEventCount);
        }
      
        ITF_MemoryBarrier();
         
        DWORD dwWaitStatus = WaitForMultipleObjects(m_uEventCount, events, FALSE, INFINITE);

        //reset the corresponding events..

        //the insert event...
        if (dwWaitStatus == 0)
        {
            Synchronize::resetEvent(&events[0]);
            if (m_bExitReceiveThread)
            {
                SF_DEL_ARRAY(events);
                return;
            }
        }
        else
        if (dwWaitStatus>=1 && dwWaitStatus<m_uEventCount)
        {
            //find the corresponding watch directory..
            WatchDirectory* pWatchDirectory = getWatchDirectories(events[dwWaitStatus]);
            ITF_ASSERT(pWatchDirectory);
            pWatchDirectory->process();
        }
        else
        {
            GetLastError();
            ITF_ASSERT(0);
        }


        SF_DEL_ARRAY(events);
    }
}

//-----------------------------------------------------------------------------------------------------------------------

void FileWatcher::update()
{
    for ( ITF_VECTOR<WatchDirectory*>::iterator iter = m_vWatchDirectory.begin();iter!=m_vWatchDirectory.end();++iter)
    {
        (*iter)->update();
    }
}

//-----------------------------------------------------------------------------------------------------------------------

FileWatcher::~FileWatcher()
{
    destroy();
}

//-----------------------------------------------------------------------------------------------------------------------

void FileWatcher::destroy()
{
    if (m_bInit)
    {
        m_bExitReceiveThread = btrue;  
        ITF_MemoryBarrier();
        wakeup();
        //TODO wait the joining thread
        sleep(100);

        SF_DEL_ARRAY(m_Events);

        for (ITF_VECTOR<WatchDirectory*> ::iterator iter = m_vWatchDirectory.begin();iter!=m_vWatchDirectory.end();++iter)
        {
            SF_DEL(*iter);
        }

        Synchronize::destroyEvent(&m_SpecialEvent);
        Synchronize::destroyCriticalSection(&m_csEvent);
    }
}

//-----------------------------------------------------------------------------------------------------------------------

void FileWatcher::init()
{
    Synchronize::createCriticalSection(&m_csEvent);

    Synchronize::createEvent(&m_SpecialEvent,"eventInsertWatcher");
    m_vEventUsed.push_back(m_SpecialEvent);
    buildEvents();

    ITF_MemoryBarrier();
    //run the thread if not existing
    if (m_pThread==NULL)
    {
        m_pThread = THREADMANAGER->createThread(Thread_Watcher, this, ThreadSettings::m_settings[eThreadId_FileWatcher]);
    }

    m_bInit = btrue;
}

//-----------------------------------------------------------------------------------------------------------------------

void FileWatcher::registerWatch(const String& _szpath,WatchDirectoryListener* _pListener,const String& _szFilter)
{
    if (m_bDisable)
        return;

    //create a new watch directory if not present
    for (ITF_VECTOR<WatchDirectory*> ::iterator iterDir = m_vWatchDirectory.begin();iterDir!=m_vWatchDirectory.end();++iterDir)
    {
        if ((*iterDir)->getPath() == _szpath)
        {
            //add this listener
            (*iterDir)->addListener(_pListener);
            _pListener->addFilter(_szFilter);
            return;
        }
    }
    
    //not found make a new one..
    WatchDirectory* pWatchDir = newAlloc(mId_System, WatchDirectory(_szpath,_szFilter,_pListener,m_uEventIncrement++));
    addEvents(pWatchDir);
    m_vWatchDirectory.push_back(pWatchDir);
}


//-----------------------------------------------------------------------------------------------------------------------

}
