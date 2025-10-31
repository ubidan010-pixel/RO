#ifndef _ITF_BUNDLEMANAGER_H_
#define _ITF_BUNDLEMANAGER_H_

#ifndef _ITF_SAFEARRAY_H_
#include "core/container/SafeArray.h"
#endif //_ITF_SAFEARRAY_H_

#ifndef _ITF_FILEPACKFORMAT_H_
#include "engine/file/FilePackFormat.h"
#endif //_ITF_FILEPACKFORMAT_H_

#ifndef _ITF_BUNDLEFILE_H_
#include "engine/file/BundleFile.h"
#endif //_ITF_BUNDLEFILE_H_

namespace ITF
{
class SharableBundleHeader;
class FilePackMaster;
class Filepack;
class BundleList;
class BundleProfileManager;
class BundleProfileEventScope;

class HeaderRegistry
{
private:
    typedef ITF_MAP< String8, SharableBundleHeader* > Registry;

public:
    HeaderRegistry();
    ~HeaderRegistry();

    // returned headers are always incremented
    SharableBundleHeader*   acquireHeader( const String8& bundleFilename );
    bbool                   add( const String8& bundleFilename, SharableBundleHeader*& header );
    void                    release( const String8& bundleFilename );

private:
    ITF_THREAD_CRITICAL_SECTION m_cs;
    Registry                    m_registry;
};

class BundleManager: public TemplateSingleton<BundleManager>
{
public:
    struct BundleInfo
    {
        BundleInfo(): refCount( 0 ), useCache( bfalse ), prio( FileManager::Lowest ) {}

        u32                             refCount;
        String8                         bundleFilename;
        String8                         keyWordName;
        String8                         tag;
        bbool                           useCache;
        FileManager::BundlePriority     prio;
    };

private:
    struct BundlePending
    {
        u32         threadId;
        BundleFile* pFile;
    };

public:
    BundleManager(const String& _szRoot);
    BundleManager();
    ~BundleManager();

    bbool loadSecureFat();
    bbool registerBundle( const String& _szFilename, bbool useCache = bfalse, FileManager::BundlePriority prio = FileManager::Lowest, const char* tag = NULL );
    bbool unregisterBundleTag( const char* tag );
    bbool unregisterBundle( const String& _szFilename );
    bbool isRegistered( const String& _szFilename ) const;

    u32   getRegistredBundleCount() { csAutoLock  lock( &m_cs ); return m_bundles.size();}

    u32     loadBundles();
    void    unloadBundles();
    void    synchronize();

    ////File accessors
    Filepack*           fileOpen(const String& _szFileName);
    bbool               fileExists(const String& _szFileName);
    u64                 fileGetSize(const String& _szFileName);
    u64                 fileGetLastTimeWriteAccess(const String& _szFileName);

    bbool                 getPositionInBundle(const String& _szFileName, FilesPosition& filePos);

    ITF_INLINE void     setDataVersion(u32 _dataversion)    {m_DataVersion = _dataversion;};
    u32                 getDataVersion() const              {return m_DataVersion;}

    void                lockAccess();
    void                unlockAccess();

    void                orderLoad(ITF_VECTOR<FilesPosition>& _orderedList);

    void                removePendings();

private:
    BundleFile* openBundle( const BundleInfo& info, u32 index );
    void        unloadBundles( BundleList& list );
    void        unloadBundle( BundleList& list, u32 index );
    bbool       unregisterBundle( u32 index );

    BundleFile* getBundle( const String& _szFileName );

    String                              m_Root;
    HeaderRegistry                      m_headers;
    mutable ITF_THREAD_CRITICAL_SECTION m_cs;
    u32                                 m_opId;
    ITF_VECTOR< BundleInfo >            m_bundles;
    ITF_THREAD_CRITICAL_SECTION         m_pendingsCs;
    ITF_VECTOR< BundlePending >         m_pendingDeletePool;
    static ThreadLocalStorage           m_TLSBundle;
    u32                                 m_DataVersion;
    GlobalFat*                          m_secureFat;

    ITF_THREAD_CRITICAL_SECTION         m_csLockAccess;//for testing

    BundleLists* getBundleList() const {return static_cast<BundleLists*>(m_TLSBundle.pGetPtrValue());}


};



}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#endif //_ITF_BUNDLEMANAGER_H_
