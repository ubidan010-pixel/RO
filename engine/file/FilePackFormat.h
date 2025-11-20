#ifndef _ITF_FILEPACKFORMAT_H_
#define _ITF_FILEPACKFORMAT_H_

#ifndef _ITF_PATH_H_
#include "core/file/Path.h"
#endif //_ITF_PATH_H_

#ifndef ITF_WINDOWS
# define ITF_NO_PATH_FILE
#endif

namespace ITF
{
    class File;
    class BundleFile;

    struct BundlePlatform
    {
        enum Enum
        {
            PC      = 0,
            X360    = 1,
            PS3     = 2,
            IPAD    = 4,
            _CTR    = 5,
            WII     = 6,
            PS5     = 7,
            NSwitch = 8, // aka. NX (but is a define)
            NSwitch2 = 9, // aka. Ounce
            XBOXSERIES = 10,
        };
    };

    struct BundleBootHeader
    {
        u32     uMagicNumber;
        u32     uVersion;
        u32     uPlatformSupported;
        u32     uFilesStart;
        u32     uFilesCount;
        bbool   bCompressed;//mixed platform supported
        bbool   bBinaryScene;
        bbool   bBinaryLogic;
        u32     uDataSignature;
        u32     uEngineSignature;
        u32     uEngineVersion;
        void serialize(ArchiveMemory& _Archive);
    };

    struct FileHeaderRuntime
    {
        typedef u64                     PosType;
        typedef ITF_VECTOR< PosType >   Positions;

        FileHeaderRuntime():m_Count(0),m_OriginalSize(0),m_CompressedSize(0),m_FlushTime(0),m_indexBundle(0) {};

        u32 m_Count;
        u32 m_OriginalSize;
        u32 m_CompressedSize;
        Positions               m_Positions;
        u64 m_FlushTime;
        u32 m_indexBundle;//not serialized
        
        u32 computeSize() const;
        void serialize(ArchiveMemory& _Archive);
    };

    class FilePackMaster
    {   
    public:
        typedef ITF_MAP< String, FileHeaderRuntime > Headers;

        static const u32 PackMagicNumber;

        void addFile(const String& filename, const FileHeaderRuntime& _filePackHeader);

        u32 computeHeaderSize() const;
        void serialize(ArchiveMemory& _Archive);

        FileHeaderRuntime&  getHeader( const String& filename );
        const Headers&      getFileHeaderBuild() const {return m_vHeader;}

    private:
        Headers             m_vHeader;
    };

    class GlobalFatBuilder
    {
    private:
        typedef ITF_SET< String8 >          BundleSet;
        typedef ITF_MAP< Path, BundleSet >  FileSet;

    public:
        void referenceFile( const Path& path, const String& bundleFilename );

        bbool save( const String& filename );

    private:
        FileSet m_files;
    };

    class GlobalFat
    {
    private:
        struct FileLight
        {
            String8 filename;
            u16     pathId;

            void serialize( ArchiveMemory& am );

            bool operator <( const FileLight& rhs ) const;
        };

        typedef ITF_MAP< u8, String8 >          BundleTOC;
        typedef ITF_VECTOR< u8 >                BundleIds;
#ifdef ITF_NO_PATH_FILE
        typedef ITF_MAP< StringID, BundleIds >  FileLinks;
#else
        typedef ITF_MAP< String8, u16 >         PathTOC;
        typedef ITF_MAP< FileLight, BundleIds > FileLinks;
#endif

#ifdef ITF_X360
        typedef ITF_MAP< String8, u32 >         SortKeyTOC;
#endif

    public:
        void serialize( ArchiveMemory& am );

        bbool load( const String& filename );

        bbool   isBundleRegistered( const String& bundleFilename ) const;
        String  findBundleContaining( const Path& filename ) const;

#ifdef ITF_X360
        u32     getSortKey( const String& bundleFilename ) const;
#endif

    private:
#ifndef ITF_NO_PATH_FILE
        PathTOC     m_pathTOC;
#endif
        BundleTOC   m_bundleTOC;
        FileLinks   m_files;
#ifdef ITF_X360
        SortKeyTOC  m_sortKeys;
#endif
    };

    class FileHeaderMap
    {
    public:
#ifdef ITF_NO_PATH_FILE
        typedef StringID Key;
#else
        typedef Path Key;
#endif

    private:
        typedef ITF_MAP< Key, FileHeaderRuntime > Map;

    public:
        typedef Map::iterator       iterator;
        typedef Map::const_iterator const_iterator;

        iterator        find( const Path& filename );
        const_iterator  find( const Path& filename ) const;

        iterator        begin();
        const_iterator  begin() const;

        iterator        end();
        const_iterator  end() const;

        FileHeaderRuntime& operator []( const Path& filename );

    private:
        Map m_files;
    };

    class SharableBundleHeader
    {
        friend class HeaderRegistry;
    public:

        SharableBundleHeader(const String& _Root):m_bReadOnly(bfalse),m_refCount(0),m_FilesStart(0),m_EngineSignature(0),m_DataSignature(0)
        {
            setRoot(_Root);
        };

        void build(const FilePackMaster::Headers& _vHeader,u32 _indexBundle);
        void setAsReadOnly()        {m_bReadOnly = btrue;}
        bbool isReadOnly() const    {return m_bReadOnly;}

        void setRoot(const String& _szRoot) {m_Root = _szRoot;m_Root.toLower();}

        i32 getRefCount() const     {return m_refCount;}

        bbool safeToDestroy() const {return (m_refCount == 0);}

        bbool   fileExists(const Path& _szFileName);
        u64     fileGetSize(const Path& _szFileName);
        u64     fileGetLastTimeWriteAccess(const Path& _szFileName);
        u64     fileGetPosition(const Path& _szFileName);
        u64     fileGetCompressedSize(const Path& _szFilename);
        void    getFilesFromRange( u64 start, u64 end, ITF_VECTOR< FileHeaderMap::Key >& range );
        
        const FileHeaderRuntime* getHeaderRuntime(const Path& _szFileName);

        ITF_INLINE u32  getFileStart() const { ITF_ASSERT( m_FilesStart ); return m_FilesStart; }
        ITF_INLINE void setFileStart( u32 value ) { ITF_ASSERT( !m_FilesStart ); m_FilesStart = value; }

        ITF_INLINE u32  getEngineSignature() const { ITF_ASSERT( m_EngineSignature ); return m_EngineSignature; }
        ITF_INLINE void setEngineSignature( u32 value ) { ITF_ASSERT( !m_EngineSignature ); m_EngineSignature = value; }

        ITF_INLINE u32  getDataSignature() const { return m_DataSignature; }
        ITF_INLINE void setDataSignature( u32 value ) { ITF_ASSERT( !m_DataSignature ); m_DataSignature = value; }

#ifdef ITF_WINDOWS
        void getFiles( ITF_VECTOR< FileHeaderMap::Key >& files ) const;
#endif //ITF_WINDOWS

    protected:
        void addRef()               {m_refCount++;}
        void removeRef()            {m_refCount--;ITF_ASSERT(m_refCount>=0);}

        FileHeaderMap   m_mapFiles;
        bbool           m_bReadOnly;
        i32             m_refCount;
        String          m_Root;
        u32             m_FilesStart;
        u32             m_DataSignature;
        u32             m_EngineSignature;
    };

} // namespace ITF


#endif // _ITF_FILEPACKFORMAT_H_
