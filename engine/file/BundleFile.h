#ifndef _ITF_BUNDLEFILE_H_
#define _ITF_BUNDLEFILE_H_

#ifndef _ITF_RESOURCE_H_
#include "engine/resources/Resource.h"
#endif //_ITF_RESOURCE_H_

#ifndef _ITF_FILEMANAGER_H_
#include "core/AdaptersInterfaces/FileManager.h"
#endif //_ITF_FILEMANAGER_H_

#ifndef _ITF_FILEPACKFORMAT_H_
#include "engine/file/FilePackFormat.h"
#endif //_ITF_FILEPACKFORMAT_H_

namespace ITF
{

class BundleFile;

struct FilesPosition
{
    FilesPosition():m_file(NULL),m_bundleKey(0), m_Position(0), m_fileSize(0) {}

    BundleFile*     m_file;
    u32             m_bundleKey;
    u64             m_Position;
    ResourceID      m_resourceID;
    u64             m_fileSize;

    bool operator <( const FilesPosition& rhs )
    {
        if ( m_bundleKey == rhs.m_bundleKey )
            return m_Position < rhs.m_Position;
        return m_bundleKey > rhs.m_bundleKey;
    }
};

class SharableBundleHeader;
class Filepack;

class BundleCache
{
public:
    BundleCache( File* file, u32 cacheCapacity );
    ~BundleCache();

    u32 read( u8* buffer, u32 size );
    u64 seek( u64 pos );

    ITF_INLINE u32 getCacheCapacity() const { return m_capacity; }
    ITF_INLINE u32 getCacheSize() const
    {
        ITF_ASSERT(m_absWrite - m_absRead < u64(std::numeric_limits<u32>::max()));
        return static_cast<u32>(m_absWrite - m_absRead);
    }
    ITF_INLINE u64 tell() const { return m_absRead; }
    ITF_INLINE void invalidateCache() { m_absRead = m_absWrite = 0; }

private:
    File*   m_file;
    u32     m_capacity;
    u8*     m_pData;
    u64     m_absRead;
    u64     m_absWrite;
};

class BundleFile
{
    friend class BundleManager;
    friend class FilePacker;
    BundleFile();
    virtual ~BundleFile();
    
    bbool openBundle(const String &_szFilename,SharableBundleHeader* _SharableHeader,u32 _indexBundle,bbool _getHeader,u32& _dataVersion,bbool useCache);
    void closeBundle();

    ////File accessors
    Filepack*           fileOpen(const String& _szFileName);
    bbool               fileExists(const String& _szFileName);
    u64                 fileGetSize(const String& _szFileName);
    void                fileGetPosition(const String& _szFileName,FilesPosition& filePos);
    u64                 fileGetLastTimeWriteAccess(const String& _szFileName);
    void                getFilesFromRange(u64 start, u64 end, ITF_VECTOR< FileHeaderMap::Key >& range);

    u32                         getEngineSignature() {return m_engineSignature;}
    void                        setPriority( FileManager::BundlePriority value ) { ITF_ASSERT( m_prio >= FileManager::Lowest && m_prio <= FileManager::Highest ); m_prio = value; }
    FileManager::BundlePriority getPriority() const { return m_prio; }

    const String& getFilename() const { return m_szFilename; }

    void                        TrackReference( Filepack* file );
    void                        UntrackReference( Filepack* file );
    const ITF_SET< Filepack* >& GetTrackedReferences() const { return m_trackedFiles; }

    void overrideHeader( SharableBundleHeader* header ) { m_SharableHeader = header; }

    friend class Filepack;

    File*                       m_FileRead;
    u64                         m_StartDataPos;
    u64                         m_CurrentPosition;
    bbool                       m_bCompressedBundle;
    String                      m_szFilename;
    SharableBundleHeader*       m_SharableHeader;
    u32                         m_engineSignature;
    u64                         m_fileSize;
    BundleCache*                m_dataCache;
#ifdef ITF_X360
    u32                         m_sortKey;
    static volatile long        m_lastAccessSortKey;
#endif
    FileManager::BundlePriority m_prio;
    //threadid        //record it to track people that used outside this thread

    bbool                   fileRead(u64 _globalposition,void* _buffer,u32 _bytesToRead,u32& _readBytes);

#ifdef ITF_SUPPORT_CHEAT
    static BundleFile*      m_previousBundleFile;
#endif //ITF_SUPPORT_CHEAT

//#ifndef ITF_FINAL
    ITF_SET< Filepack* >    m_trackedFiles;
//#endif //ITF_FINAL
};

//---------------------------------------------------------------------------------------------------------------------------------

class BundleList
{
public:
    BundleList() {}
    ~BundleList();

    void addFile( BundleFile* _pfile ) { m_vBundleFile.push_back( _pfile ); }
    void removeFile( u32 _index ) { m_vBundleFile.erase( m_vBundleFile.begin() + _index ); }
    void insertFile( BundleFile* _pfile, u32 _index ) { m_vBundleFile.insert( m_vBundleFile.begin() + _index, _pfile ); }
    void clear() { m_vBundleFile.clear(); }

    BundleFile* getBundleFile(u32 _index)   {return m_vBundleFile[_index];}
    u32         getSize() const {return m_vBundleFile.size();}

protected:
    ITF_VECTOR< BundleFile* > m_vBundleFile;
};

struct BundleLists
{
    BundleList  m_registered;
    BundleList  m_secure;
    u32         m_opId;
};

}

#endif //_ITF_BUNDLEFILE_H_
