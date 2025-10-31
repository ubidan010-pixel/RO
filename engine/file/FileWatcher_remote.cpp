#include "precompiled_engine.h"

#ifndef _ITF_FILEWATCHER_REMOTE_H_
#include "engine/file/FileWatcher_remote.h"
#endif  //_ITF_FILEWATCHER_REMOTE_H_


#ifndef _ITF_WATCHDIRECTORYLISTENER_H
#include "engine/file/WatchDirectoryListener.h"
#endif  //_ITF_WATCHDIRECTORYLISTENER_H

#ifndef _ITF_FILEPATH_H_
#include "core/file/filepath.h"
#endif //_ITF_FILEPATH_H_

namespace ITF
{
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
FileWatcher_Remote::FileWatcher_Remote()
{



}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

FileWatcher_Remote::~FileWatcher_Remote()
{



}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void FileWatcher_Remote::update()
{
    for (ITF_VECTOR<String> ::iterator iter = m_FilesChanged.begin();iter!=m_FilesChanged.end();iter++)
    {
        const String& file = *iter;
        const String& extension = FilePath::getExtension(file);

        for (ITF_VECTOR<RemoteFileEventFilter*> ::const_iterator iterEvent = m_vRemoteFileEventFilter.begin();iterEvent!=m_vRemoteFileEventFilter.end();iterEvent++)
        {
            const RemoteFileEventFilter* pRemoteFileEvent = *iterEvent;
            
            pRemoteFileEvent->executeIfMatch(extension,file);
        }

    }

    m_FilesChanged.clear();
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------


void FileWatcher_Remote::addFileChange(const String& _file)
{
    m_FilesChanged.push_back(_file);
}


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void RemoteFileEventFilter::executeIfMatch(const String& _szFilter,const String& _szFile) const
{
    if (_szFilter == m_szFilter)
    {
        m_pListener->onModifiedFile(_szFile);
    }
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void FileWatcher_Remote::registerWatch(WatchDirectoryListener* _pListener,const String& _szFilter)
{
    RemoteFileEventFilter *eventFilter = newAlloc(mId_System, RemoteFileEventFilter(_pListener,_szFilter));
    m_vRemoteFileEventFilter.push_back(eventFilter);
}

}