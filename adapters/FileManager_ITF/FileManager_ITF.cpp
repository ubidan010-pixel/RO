#include "precompiled_FileManager_ITF.h"

#include <algorithm>

#ifndef _ITF_SINGLETONS_H_
#include "engine/singleton/Singletons.h"
#endif //_ITF_SINGLETONS_H_

#ifndef _ITF_FILEMANAGER_ITF_H_
#include "adapters/FileManager_ITF/FileManager_ITF.h"
#endif //_ITF_FILEMANAGER_ITF_H_

#ifndef _ITF_FILEPATH_H_
#include "core/file/filepath.h"
#endif //_ITF_FILEPATH_H_

#ifndef _ITF_FILE_H_
#include "core/file/File.h"
#endif //_ITF_FILE_H_

#ifndef _ITF_ERRORHANDLER_H_
#include "core/error/ErrorHandler.h"
#endif //_ITF_ERRORHANDLER_H_

#ifndef _ITF_SYSTEMADAPTER_
#include "core/AdaptersInterfaces/SystemAdapter.h"
#endif //_ITF_SYSTEMADAPTER_

#ifndef _ITF_FILESERVER_H_
#include "core/file/FileServer.h"
#endif //_ITF_FILESERVER_H_

#ifndef _ITF_FILEPACKFORMAT_H_
#include "engine/file/FilePackFormat.h"
#endif //_ITF_FILEPACKFORMAT_H_

#ifndef _ITF_FILEPACK_H_
#include "engine/file/FilePack.h"
#endif //_ITF_FILEPACK_H_

#ifndef _ITF_BUNDLEFILE_H_
#include "engine/file/BundleFile.h"
#endif //_ITF_BUNDLEFILE_H_

#ifndef _ITF_BUNDLEMANAGER_H_
#include "engine/file/BundleManager.h"
#endif //_ITF_BUNDLEMANAGER_H_

#ifndef _ITF_FILETRACKER_H_
#include "engine/file/FileTracker.h"
#endif //_ITF_FILETRACKER_H_

#ifndef _ITF_WARNINGMANAGER_H_
#include "engine/common/WarningManager.h"
#endif // _ITF_WARNINGMANAGER_H_

#ifdef ITF_IPAD
#include <algorithm>
#endif

namespace ITF
{
bbool           FileManager_ITF::m_bBundleUsageEnabled  = bfalse;
bbool           FileManager_ITF::m_bBundleSupported     = btrue;
//-----------------------------------------------------------------------------------------------------------------------------------------------

FileManager_ITF::FileManager_ITF()
{
#ifdef ENABLE_FILE_REGISTERING
    Synchronize::createCriticalSection(&m_csFileRegistering);
    #ifdef ITF_WINDOWS
    m_bEnableTrackingRegistering = bfalse;
    #endif // ITF_WINDOWS
#endif // ENABLE_FILE_REGISTERING
}

//-----------------------------------------------------------------------------------------------------------------------------------------------
bbool FileManager_ITF::loadSecureFat()
{
#ifndef ITF_SUPPORT_EDITOR
    return BUNDLEMANAGER->loadSecureFat();
#else
    return btrue;
#endif
}

//-----------------------------------------------------------------------------------------------------------------------------------------------

bbool FileManager_ITF::registerBundle(const String& _szFilename, bbool useCache /* = bfalse */, BundlePriority prio /* = Lowest */, const char* tag /* = NULL */)
{
#ifndef ITF_SUPPORT_EDITOR
    return BUNDLEMANAGER->registerBundle( _szFilename, useCache, prio, tag );
#else
    return btrue;
#endif
}

//-----------------------------------------------------------------------------------------------------------------------------------------------

bbool FileManager_ITF::unregisterBundle(const String& _szFilename)
{
#ifndef ITF_SUPPORT_EDITOR
    return BUNDLEMANAGER->unregisterBundle( _szFilename );
#else
    return btrue;
#endif
}

//-----------------------------------------------------------------------------------------------------------------------------------------------

bbool FileManager_ITF::unregisterBundleTag(const char* tag)
{
#ifndef ITF_SUPPORT_EDITOR
    return BUNDLEMANAGER->unregisterBundleTag( tag );
#else
    return btrue;
#endif
}

//-----------------------------------------------------------------------------------------------------------------------------------------------

bbool FileManager_ITF::isBundleRegistered(const String& _szFilename) const
{
    return BUNDLEMANAGER->isRegistered( _szFilename );
}

//-----------------------------------------------------------------------------------------------------------------------------------------------

void FileManager_ITF::loadBundles()
{
#ifndef ITF_SUPPORT_EDITOR
    if ( !m_bBundleUsageEnabled )
        m_bBundleUsageEnabled = BUNDLEMANAGER->getRegistredBundleCount() != 0;
    BUNDLEMANAGER->loadBundles();
#endif
}

//-----------------------------------------------------------------------------------------------------------------------------------------------

void FileManager_ITF::unloadBundles()
{
#ifndef ITF_SUPPORT_EDITOR
    BUNDLEMANAGER->unloadBundles();
#endif
}

//-----------------------------------------------------------------------------------------------------------------------------------------------

void FileManager_ITF::reloadBundles()
{
#ifndef ITF_SUPPORT_EDITOR
    BUNDLEMANAGER->synchronize();
#endif
}

//-----------------------------------------------------------------------------------------------------------------------------------------------

void FileManager_ITF::closePendingBundles()
{
#ifndef ITF_SUPPORT_EDITOR
    BUNDLEMANAGER->removePendings();
#endif
}

//-----------------------------------------------------------------------------------------------------------------------------------------------

FileManager_ITF::~FileManager_ITF()
{
#ifdef ENABLE_FILE_REGISTERING
    //uncomment to dump a trace log file
    //dumpFilesOpen();

    ITF_ASSERT(!map_FileopenPending.size());
    for (auto iter = map_FileopenPending.begin(); iter!=map_FileopenPending.end(); ++iter)
    {
        const String_Uncached& name = iter->second;
        LOG("LEAK HANDLE FILE - %s", StringToUTF8(name).get());
    }

    Synchronize::destroyCriticalSection(&m_csFileRegistering);
#endif // ENABLE_FILE_REGISTERING

    for (ITF_VECTOR<FileDevice*>::const_iterator iter = m_vFileDevice.begin();iter!=m_vFileDevice.end();++iter)
    {
        delete (*iter);
    }
}

//-----------------------------------------------------------------------------------------------------------------------------------------------

void  FileManager_ITF::getPlatformFilename(String& _szDst,const String& _szSrc)
{
    String suffix = SYSTEM_ADAPTER->getPlatformName();

#ifdef _DEBUG
    suffix += "_dbg";
#else 
    suffix += "_rls";
#endif //_DEBUG

    _szDst = _szSrc+"_"+suffix;
}

//-----------------------------------------------------------------------------------------------------------------------------------------------

bbool FileManager_ITF::TranslatePath(String& _TranslatedPath, const String& _szPathName,u32 _mode) const
{
    String workingPath;
    
    //TODO demo/change to a more comfortable function
#ifdef ITF_X360
    const u16* pathPtr = _szPathName.cStr();
    u16* ptr = (u16*)wcsstr((wchar_t*) pathPtr,L"./");
    if (ptr)
    {
        String tmp;
        workingPath.setText(pathPtr,(u16*)ptr-pathPtr);
        tmp.setText(ptr+2);
        workingPath+=tmp;
    }
    else
#endif //ITF_X360
    {
        workingPath = _szPathName;
    }

    //it s already a translated Path ?
    //return it ...;
    String szFilename;
#if defined(ITF_POSIX_FILE_SYSTEM)
    if (!workingPath.isEmpty() && workingPath.cStr()[0] == L'/')
#else
    if(workingPath.strstr(':'))
#endif
    {
        _TranslatedPath = workingPath;
        _TranslatedPath.replace('/', '\\');

        return btrue;
    }

    
    //On final,we will store the result a map ,and use a virtual map files to avoid fat access
    //Parse the mounted device and find the best    

    u32 DeviceCount     = m_vFileDevice.size();
    u32 CurrentDevice   = 1;

    for (ITF_VECTOR<FileDevice*>::const_iterator iter = m_vFileDevice.begin();iter!=m_vFileDevice.end();++iter)
    {
        FileDevice* pFileDevice = (*iter);
        const String& szDrivePath = pFileDevice->getDrivePath();
        szFilename = szDrivePath+workingPath;

#if defined(ITF_MICROSOFT)
        szFilename.replace('/', '\\');
#else
        szFilename.replace('\\', '/');
#endif // ITF_MICROSOFT

        //file exits ?
        bbool bexist = hwFile::exists(szFilename);
        if (bexist || CurrentDevice == DeviceCount)
        {
           _TranslatedPath = szFilename;
           return btrue;
        }
        
        CurrentDevice++;
    }
    
    //Device not found..
    ITF_ASSERT(0);
    return bfalse;
}
//-----------------------------------------------------------------------------------------------------------------------------------------------

bool functionDeviceSort (FileDevice* a,FileDevice* b)
{
    return (a->getPriority()<b->getPriority()); 
}
//-----------------------------------------------------------------------------------------------------------------------------------------------

void FileManager_ITF::mountFileDevice(const String& _szDeviceName,const String& _szDrivePath,u32 _priority,u32 _mode)
{
    FileDevice* pFileDevice = newAlloc(mId_System,FileDevice(_szDeviceName,_szDrivePath,_priority,_mode));
    m_vFileDevice.push_back(pFileDevice);

    //sort by priority;
    std::sort(m_vFileDevice.begin(),m_vFileDevice.end(),functionDeviceSort);
}

//-----------------------------------------------------------------------------------------------------------------------------------------------

#ifdef ENABLE_FILE_REGISTERING
void FileManager_ITF::registerFile(const String& _szFileName, uPtr _Handle)
{
#ifdef ITF_SUPPORT_EDITOR
    if (!m_bEnableTrackingRegistering)
        return;
#endif // ITF_SUPPORT_EDITOR

    csAutoLock cs(m_csFileRegistering);
    
    //already registered ?
    ITF_ASSERT(map_FileopenPending.find(_Handle) == map_FileopenPending.end());

    map_FileopenPending[_Handle] = _szFileName;

    auto iter = map_FileopenGlobal.find(_szFileName);

    if (iter == map_FileopenGlobal.end())
        map_FileopenGlobal[_szFileName] = 1;
    else
        (*iter).second = (*iter).second+1;
}

//-----------------------------------------------------------------------------------------------------------------------------------------------

void FileManager_ITF::unRegisterFile(uPtr _Handle)
{
#ifdef ITF_WINDOWS
    if (!m_bEnableTrackingRegistering)
        return;
#endif // ITF_WINDOWS

    csAutoLock cs(m_csFileRegistering);

    //Try to unregister a not open file ?
    auto iter = map_FileopenPending.find(_Handle);
    if (iter != map_FileopenPending.end())
    {
        map_FileopenPending.erase(iter);
    }
}

//-----------------------------------------------------------------------------------------------------------------------------------------------

void FileManager_ITF::dumpFilesOpen()
{
    for (auto iter = map_FileopenGlobal.begin(); iter!=map_FileopenGlobal.end(); ++iter)
    {
        const String&   name    = (*iter).first;
        const u32       count   = (*iter).second;
        LOG("OPEN FILE - %s == %d", StringToUTF8(name).get(), count);
    }
}
#endif // ENABLE_FILE_REGISTERING

//-----------------------------------------------------------------------------------------------------------------------------------------------

bbool    FileManager_ITF::fileExists(const String& _szFileName)
{
#ifndef ITF_FINAL
    checkMainThreadLoad(_szFileName);
#endif //!ITF_FINAL

    if (m_bBundleSupported && m_bBundleUsageEnabled && BUNDLEMANAGER->fileExists(_szFileName))
        return btrue;

    //TRY on the hardware
    String finalPath;
    if(TranslatePath(finalPath, _szFileName))
        return hwFile::exists(finalPath);
    return bfalse;
}
//-----------------------------------------------------------------------------------------------------------------------------------------------

File*    FileManager_ITF::openFile(const String& _szFileName, u32 mode)
{
#ifndef ITF_FINAL
    checkMainThreadLoad(_szFileName);
#endif //!ITF_FINAL

    if (m_bBundleUsageEnabled)
    {
        Filepack* pFile = BUNDLEMANAGER->fileOpen(_szFileName);

        if ( pFile != NULL )
        {
#ifndef ITF_FINAL
            m_logger.LogOpenedFile( _szFileName );
#endif
            return pFile;
        }
    }

    hwFile* pFile = newAlloc(mId_System,hwFile());
    RECORDFILE(_szFileName);

    String finalPath;
    if(TranslatePath(finalPath, _szFileName))
        if (pFile->open(finalPath, mode))
        {
#ifndef ITF_FINAL
            m_logger.LogOpenedFile( _szFileName );
#endif
            return pFile;
        }

        SF_DEL(pFile);
        return NULL;
}
//-----------------------------------------------------------------------------------------------------------------------------------------------

u64      FileManager_ITF::getFileSize(const String& _szFileName)
{
    if (m_bBundleUsageEnabled)
    {
        return BUNDLEMANAGER->fileGetSize(_szFileName);
    }
    else
    {
        String fullpath;
        FILEMANAGER->TranslatePath(fullpath, _szFileName);
        return hwFile::getFileSize(fullpath);
    }
}

//-----------------------------------------------------------------------------------------------------------------------------------------------
File*    FileManager_ITF::closeFile(File* _pFile)
{
    SF_DEL(_pFile);
    return NULL;
}
//-----------------------------------------------------------------------------------------------------------------------------------------------


u64      FileManager_ITF::getLastTimeWriteAccess(const String& _szFileName)
{
    if (m_bBundleUsageEnabled)
    {
        u64 flushTime = BUNDLEMANAGER->fileGetLastTimeWriteAccess(_szFileName);
        if (flushTime != 0)
        {
            return flushTime;
        }
    }

    String fullpath;
    FILEMANAGER->TranslatePath(fullpath, _szFileName);
    return  hwFile::getLastTimeWriteAccess(fullpath);
}
//-----------------------------------------------------------------------------------------------------------------------------------------------

void    FileManager_ITF::flushTimeWriteAccess(const String& _szFileName,const u64& _lastWrite)
{
    String fullpath;
    FILEMANAGER->TranslatePath(fullpath, _szFileName);
    hwFile::flushTimeWriteAccess(fullpath,_lastWrite);
}

//-----------------------------------------------------------------------------------------------------------------------------------------------
bbool FileManager_ITF::fileChooser( bbool /*_save*/, String& /*_defaultAndResult*/, const wchar_t* /*_filter*//* = NULL*/, const String& /*_workingPath*/ /* = ""*/ ) const
{
    ITF_ASSERT_MSG(0, "fileChooser not implemented");
    return bfalse;
}

//-----------------------------------------------------------------------------------------------------------------------------------------------
void   FileManager_ITF::deleteFile(const String& _szFileName)
{
    String fullpath;
    FILEMANAGER->TranslatePath(fullpath, _szFileName);
    hwFile::deleteFile(fullpath);
}
//-----------------------------------------------------------------------------------------------------------------------------------------------

#ifndef ITF_FINAL
void   FileManager_ITF::checkMainThreadLoad(const String& filename)
 {
#ifdef ITF_CONSOLE
     if (m_enableCheckLoadFromMainThread && Synchronize::getCurrentThreadId() == ThreadSettings::m_settings[eThreadId_mainThread].m_threadID)
     {
         String8 error;
         error.setTextFormat("File loaded from main thread : '%s'", StringToUTF8(filename).get());
         ITF_WARNING_CATEGORY(GPP,NULL, bfalse, error.cStr());
     }
#endif //ITF_CONSOLE
 }
#endif //ITF_FINAL
}
