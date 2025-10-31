#ifndef _ITF_FILEMANAGER_ITF_H_
#define _ITF_FILEMANAGER_ITF_H_

#ifndef _ITF_FILEMANAGER_H_
#include "core/AdaptersInterfaces/FileManager.h"
#endif //_ITF_FILEMANAGER_H_

#include "core/system/Synchronize.h"

#ifndef ITF_FINAL
#ifndef _ITF_FILE_MANAGER_LOGGER_ITF_H_
#include "adapters/FileManager_ITF/FileManagerLogger_ITF.h"
#endif //_ITF_FILE_MANAGER_LOGGER_ITF_H_
#endif //ITF_FINAL

namespace ITF
{
    class File;
    class BundleList;

class FileManager_ITF : public FileManager
{
public:
    FileManager_ITF();
    virtual ~FileManager_ITF();

    virtual bbool   loadSecureFat();
    virtual bbool   registerBundle(const String& _szFilename, bbool useCache = bfalse, BundlePriority prio = Lowest, const char* tag = NULL);
    virtual bbool   unregisterBundle(const String& _szFilename);
    virtual bbool   unregisterBundleTag(const char* tag);
    virtual bbool   isBundleRegistered(const String& _szFilename) const;
    virtual void    loadBundles();
    virtual void    unloadBundles();
    virtual void    reloadBundles();
    virtual void    closePendingBundles();

    virtual void    mountFileDevice(const String& _szDeviceName,const String& _szDrivePath,u32 _priority,u32 mode = ITF_FILE_ATTR_READ);
    virtual void    getPlatformFilename(String& _szDst,const String& _szSrc);

#ifdef ENABLE_FILE_REGISTERING
    virtual void    registerFile(const String& _szFileName,uPtr _Handle);
    virtual void    unRegisterFile(uPtr _Handle);
    virtual void    dumpFilesOpen();
#endif // ENABLE_FILE_REGISTERING

    u64             getFileSize(const String& _szFileName);
    bbool           fileExists(const String& _szFileName);
    virtual File*   openFile(const String& _szFileName,u32 mode);
    File*           closeFile(File* _pFile);
    void            deleteFile(const String& _szFileName);
    u64             getLastTimeWriteAccess(const String& _szFileName);
    void            flushTimeWriteAccess(const String& _szFileName, const u64& _lastWrite);


    virtual bbool   fileChooser(bbool _save, String& _defaultAndResult, const wchar_t* _filter = NULL, const String& _workingPath = "")const;
    virtual bbool   TranslatePath(String& _translatedPath, const String& _szPathName, u32 mode = ITF_FILE_ATTR_READ)const;

    static bbool     m_bBundleSupported;

#ifndef ITF_FINAL
    void            checkMainThreadLoad(const String& filename);
    FileManagerLogger_ITF& getLogger() { return m_logger; }
#endif //ITF_FINAL
#if defined(ENABLE_FILE_REGISTERING) && defined(ITF_WINDOWS)
    void            setEnableTrackingRegistering(bbool _bTracking) {m_bEnableTrackingRegistering = _bTracking;}
    const auto & getFilesOpenedGlobal() {return map_FileopenGlobal;}
#endif // defined(ENABLE_FILE_REGISTERING) && defined(ITF_WINDOWS)

protected:


    ITF_VECTOR<FileDevice*> m_vFileDevice;

#ifdef ENABLE_FILE_REGISTERING
    ITF_THREAD_CRITICAL_SECTION m_csFileRegistering;
    ITF_MAP<uPtr, String_Uncached> map_FileopenPending;
    ITF_MAP<String_Uncached, u32> map_FileopenGlobal;
#ifdef ITF_WINDOWS
    bbool            m_bEnableTrackingRegistering;
#endif //ITF_WINDOWS
#endif // ENABLE_FILE_REGISTERING

    static bbool     m_bBundleUsageEnabled;

#ifndef ITF_FINAL
    FileManagerLogger_ITF m_logger;
#endif //ITF_FINAL


};

} //namespace ITF


#endif  //_ITF_FILEMANAGER_ITF_H_
