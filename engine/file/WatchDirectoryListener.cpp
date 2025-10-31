#include "precompiled_engine.h"

#ifndef _ITF_WATCHDIRECTORYLISTENER_H
#include "engine/file/WatchDirectoryListener.h"
#endif  //_ITF_WATCHDIRECTORYLISTENER_H

#ifndef _ITF_FILEPATH_H_
#include "core/file/filepath.h"
#endif //_ITF_FILEPATH_H_

#ifndef _ITF_DEBUGINFO_H_
#include "engine/debug/debugInfo.h"
#endif //_ITF_DEBUGINFO_H_


namespace ITF
{


//------------------------------------------------------------------------------------------------------------------------------------------------------------

void WatchDirectoryListener::addTracing(const String& _filename)
{
    String tmp,relativeFilename(_filename);
    FilePath::transformToRelative(relativeFilename);
    tmp.setStringFormat("File changed: : %ls",relativeFilename.cStr());
#ifdef ITF_SUPPORT_DEBUGFEATURE
    if (DEBUGINFO)
        DEBUGINFO->addDebugInfoDisplay(tmp,5.0f);
#endif // ITF_SUPPORT_DEBUGFEATURE
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------

void WatchDirectoryListener::onDeleteFile(const String& _absoluteFilename)
{
    String relativeFile = _absoluteFilename;
    FilePath::getExtension(relativeFile);
    FilePath::transformToRelative(relativeFile);

    // Some softwares deletes a file and write a new instead of overwriting the existing file
    // So if the file exists then condider this event as 'onModifiedFIle'
    if (FILEMANAGER->fileExists(relativeFile))
    {
        onModifiedFile(_absoluteFilename);
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------

}