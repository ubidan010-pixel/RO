#ifndef _ITF_FILEMANAGER_H_
#define _ITF_FILEMANAGER_H_

#ifndef _ITF_TEMPLATESINGLETON_H_
#include "core/templateSingleton.h"
#endif //_ITF_TEMPLATESINGLETON_H_

#ifndef _ITF_FILE_H_
#include "core/file/File.h"
#endif //_ITF_FILE_H_

namespace ITF
{
    class File;

    struct ePriorityFileDevice
    {
        enum Enum : u32
        {
            ePriorityFileDevice_Patch = 50,
            ePriorityFileDevice_DL = 70,
            ePriorityFileDevice_HDDCache = 90,
            ePriorityFileDevice_HDD = 110,
            ePriorityFileDevice_DVD = 130,
            ENUM_FORCE_SIZE_32(0)
        };
    };

#ifndef ITF_FINAL
    class FileManagerLogger_ITF;
#endif

    class FileDevice
    {
    public:

        FileDevice(const String& _szDeviceName,const String& _szDrivePath,u32 _priority,u32 _mode):
                   m_szDeviceName(_szDeviceName),m_szDrivePath(_szDrivePath),
                   m_PriorityFileDevice(_priority),m_Mode(0)
        {

        }

        const String&   getDeviceName()     const {return m_szDeviceName;} 
        const String&   getDrivePath()      const {return m_szDrivePath;}
        const u32       getPriority()       const {return m_PriorityFileDevice;}
        const u32       getMode()           const {return m_Mode;}

    protected:

        String  m_szDeviceName;
        String  m_szDrivePath;
        u32     m_PriorityFileDevice;
        u32     m_Mode;
    };

    class FileManager : public TemplateSingleton<FileManager>
    {
    public:
        enum BundlePriority
        {
            Lowest = 0,
            Highest,
        };

        FileManager()
#ifndef ITF_FINAL
            :m_enableCheckLoadFromMainThread(bfalse) 
#endif //!ITF_FINAL
        {};
        virtual ~FileManager() {};

        virtual bbool   loadSecureFat() = 0;
        virtual bbool   registerBundle(const String& _szFilename, bbool useCache = bfalse, BundlePriority prio = Lowest, const char* tag = NULL) = 0;
        virtual bbool   unregisterBundle(const String& _szFilename) = 0;
        virtual bbool   unregisterBundleTag(const char* tag) = 0;
        virtual bbool   isBundleRegistered(const String& _szFilename) const = 0;
        virtual void    loadBundles() = 0;
        virtual void    unloadBundles() = 0;
        virtual void    reloadBundles() = 0;
        virtual void    closePendingBundles() = 0;

        virtual void    mountFileDevice(const String& _szDeviceName,const String& _szDrivePath,u32 _priority,u32 _mode = ITF_FILE_ATTR_READ) = 0;
        virtual void    getPlatformFilename(String& _szDst,const String& _szSrc) = 0;

#ifdef ENABLE_FILE_REGISTERING
        virtual void    registerFile(const String& _szFileName, uPtr _Handle) = 0;
        virtual void    unRegisterFile(uPtr _Handle) = 0;
        virtual void    dumpFilesOpen() = 0;
#endif // ENABLE_FILE_REGISTERING

        virtual bbool   fileExists(const String& _szFileName) = 0;
        virtual File*   openFile(const String& _szFileName,u32 mode) = 0;
        virtual File*   closeFile(File* _pFile) = 0;
        virtual u64     getLastTimeWriteAccess(const String& _szFileName) = 0;
        virtual void    flushTimeWriteAccess(const String& szExistingFilename, const u64& _lastWrite) = 0;
        virtual u64     getFileSize(const String& _szFileName) = 0;
        virtual void    deleteFile(const String& _szFileName) = 0;

        virtual bbool   fileChooser(bbool _save, String& _defaultAndResult, const wchar_t* _filter = NULL, const String& _workingPath = "")const = 0;
        virtual bbool   folderChooser(const String& _title, const String& startPath, String& _result)const {return bfalse;}
        virtual bbool   TranslatePath(String& _translatedPath, const String& _szPathName, u32 mode = ITF_FILE_ATTR_READ)const = 0;

        virtual bbool   remoteCook(const String& _szFileName) {return bfalse;}
        virtual bbool   isRemote()                            {return bfalse;}

#ifndef ITF_FINAL
        void            enableCheckLoadFromMainThread()       {m_enableCheckLoadFromMainThread = btrue;}
        virtual FileManagerLogger_ITF& getLogger() = 0;
#endif //ITF_FINAL
     

    protected:
#ifndef ITF_FINAL
        bbool   m_enableCheckLoadFromMainThread;
#endif //
        
    };

#define FILEMANAGER            FileManager::getptr()

} //namespace ITF


#endif  //_ITF_FILEMANAGER_H_
