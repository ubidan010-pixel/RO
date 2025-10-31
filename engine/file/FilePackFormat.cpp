#include "precompiled_engine.h"

#ifndef _ITF_FILEPACKFORMAT_H_
#include "engine/file/FilePackFormat.h"
#endif //_ITF_FILEPACKFORMAT_H_

#ifndef _ITF_FILEPATH_H_
#include "core/file/filepath.h"
#endif //_ITF_FILEPATH_H_

#ifndef _ITF_BUNDLEFILE_H_
#include "engine/file/BundleFile.h"
#endif //_ITF_BUNDLEFILE_H_

namespace ITF
{
    //------------------------------------------------------------------------------------------------------------------------

    const u32 FilePackMaster::PackMagicNumber = 0x50EC12BA;

    void BundleBootHeader::serialize(ArchiveMemory& _Archive)
    {
        _Archive.serialize(uMagicNumber);
        _Archive.serialize(uVersion);
        _Archive.serialize(uPlatformSupported);
        _Archive.serialize(uFilesStart);
        _Archive.serialize(uFilesCount);
        _Archive.serialize(bCompressed);
        _Archive.serialize(bBinaryScene);
        _Archive.serialize(bBinaryLogic);
        _Archive.serialize(uDataSignature);
        _Archive.serialize(uEngineSignature);
        _Archive.serialize(uEngineVersion);
    }

    //------------------------------------------------------------------------------------------------------------------------

    u32 FileHeaderRuntime::computeSize() const
    {
        u32 size =  sizeof(m_Count) + sizeof(m_OriginalSize) + sizeof(m_CompressedSize) + sizeof(m_FlushTime);

        size += m_Count * sizeof( PosType );
        return size;
        
    }

    //------------------------------------------------------------------------------------------------------------------------

    void FileHeaderRuntime::serialize(ArchiveMemory& _Archive)
    {
        if ( !_Archive.isReading() )
            m_Count = m_Positions.size();
        _Archive.serialize(m_Count);
        _Archive.serialize(m_OriginalSize);
        _Archive.serialize(m_CompressedSize);
        _Archive.serialize(m_FlushTime);
        if ( _Archive.isReading() )
            m_Positions.resize( m_Count );
        for (ux i = 0; i < m_Positions.size(); ++i)
            _Archive.serialize( m_Positions[i] );
    }

    //------------------------------------------------------------------------------------------------------------------------

    void FilePackMaster::addFile(const String& filename, const FileHeaderRuntime& _filePackHeader)
    {
        ITF_ASSERT_CRASH( FILEMANAGER->fileExists( filename ), "File not found: %s", StringToUTF8(filename).get() );

            std::pair< Headers::iterator, bool > res = m_vHeader.insert( Headers::value_type( filename, _filePackHeader ) );

            if ( res.second )
                res.first->second.m_Count = 1;
            else
                res.first->second.m_Count += 1;
        }

    //------------------------------------------------------------------------------------------------------------------------

    void FilePackMaster::serialize(ArchiveMemory& _Archive)
    {
        u32 size;

        if ( _Archive.isReading() )
        {
            FileHeaderRuntime   header;
            String              filename;

            _Archive.serialize( size );
            while ( size-- )
            {
                header.serialize( _Archive );
                filename.serialize( _Archive );
                m_vHeader[filename] = header;
            }
        }
        else
        {
            Headers::iterator end = m_vHeader.end();

            size = m_vHeader.size();
            _Archive.serialize( size );
            for ( Headers::iterator it = m_vHeader.begin(); it != end; ++it )
            {
                it->second.serialize( _Archive );
                it->first.serialize( _Archive );
            }
        }
    }

    FileHeaderRuntime& FilePackMaster::getHeader( const String& filename )
    {
        Headers::iterator it = m_vHeader.find( filename );

        ITF_ASSERT_CRASH( it != m_vHeader.end(), "File not found: %s", StringToUTF8(filename).get() );
        return it->second;
    }

    //------------------------------------------------------------------------------------------------------------------------

    u32  FilePackMaster::computeHeaderSize() const
    {
        u32                     size = 0;
        Headers::const_iterator end = m_vHeader.end();
        String                  key;

        size += sizeof(u32);
        for ( Headers::const_iterator it = m_vHeader.begin(); it != end; ++it )
        {
            key = it->first;
            size +=  it->second.computeSize() + key.getSerializeSize();
        }
        return size;
    }

    void GlobalFatBuilder::referenceFile( const Path& path, const String& bundleFilename )
    {
        PathString_t lowerPath;

        path.getString(lowerPath);
        String8 lowerBundle = bundleFilename.cStr();

        lowerPath.toLower();
        lowerBundle.toLower();
        m_files[lowerPath].insert( lowerBundle );
    }

    bbool GlobalFatBuilder::save( const String& filename )
    {
        hwFile* file = newAlloc(mId_Temporary, hwFile());

        if ( !file->open( filename, ITF_FILE_ATTR_CREATE_NEW | ITF_FILE_ATTR_WRITE ) )
        {
            SF_DEL( file );
            return bfalse;
        }

        typedef ITF_MAP< String8, u8 >  TOC8;
        typedef ITF_MAP< String8, u16 > TOC16;

        u8                          bundleId = 0;
        TOC8                        bundleTOC;
        u16                         pathId = 0;
        TOC16                       pathTOC;
        ITF_VECTOR< u8 >            bundleIds;
        String8                     currentPath;
        ArchiveMemory               am;
        FileSet::const_iterator     fileEnd = m_files.end();
        u32                         size = m_files.size();

        // FileTOC
        //  Count
        //  Path id
        //  Filename (without path)
        //  Bundle count
        //  Bundle ids
        // BundleTOC
        //  Count
        //  Bundle id
        //  Bundle filename
        // PathTOC
        //  Count
        //  Path id
        //  Path filename

        am.serialize( size );

        PathString_t fileName;

        for ( FileSet::const_iterator file = m_files.begin(); file != fileEnd; ++file )
        {
            {
                TOC16::const_iterator   it;
                u16                     currentId;

                file->first.getString(fileName);

                currentPath = FilePath::getDirectory( fileName ).cStr();
                it = pathTOC.find( currentPath );
                if ( it == pathTOC.end() )
                {
                    ITF_ASSERT_CRASH( pathId != static_cast< u16 >( -1 ), "Integer overflow" );
                    currentId = pathId++;
                    pathTOC[currentPath] = currentId;
                }
                else
                    currentId = it->second;
                currentPath = FilePath::getFilename( fileName ).cStr();
                am.serialize( currentId );
                currentPath.serialize( am );
            }

            BundleSet::const_iterator bundleEnd = file->second.end();

            for ( BundleSet::const_iterator bundleIt = file->second.begin(); bundleIt != bundleEnd; ++bundleIt )
            {
                TOC8::const_iterator    it;
                u8                      currentId;

                it = bundleTOC.find( *bundleIt );
                if ( it == bundleTOC.end() )
                {
                    ITF_ASSERT_CRASH( pathId != static_cast< u8 >( -1 ), "Integer overflow" );
                    currentId = bundleId++;
                    bundleTOC[*bundleIt] = currentId;
                }
                else
                    currentId = it->second;
                bundleIds.push_back( currentId );
            }

            size = bundleIds.size();
            am.serialize( size );
            for ( u32 i = 0; i < bundleIds.size(); ++i )
                am.serialize( bundleIds[i] );
            bundleIds.clear();
        }

        size = bundleTOC.size();
        am.serialize( size );
        for ( TOC8::iterator it = bundleTOC.begin(); it != bundleTOC.end(); ++it )
        {
            am.serialize( it->second );
            it->first.serialize( am );
        }

        size = pathTOC.size();
        am.serialize( size );
        for ( TOC16::iterator it = pathTOC.begin(); it != pathTOC.end(); ++it )
        {
            it->first.serialize( am );
            am.serialize( it->second );
        }

        bbool success;

        success = file->write( am.getData(), am.getSize() );
        SF_DEL( file );
        return success;
    }

    bool GlobalFat::FileLight::operator <( const FileLight& rhs ) const
    {
        if ( pathId != rhs.pathId )
            return pathId < rhs.pathId;
        return filename < rhs.filename;
    }

    void GlobalFat::serialize( ArchiveMemory& am )
    {
        if ( !am.isReading() )
            return;

        ITF_VECTOR< u8 >    bundleIds;
        u32                 size;
        String8             str;
        u16                 int16;
        u8                  int8;

#ifdef ITF_NO_PATH_FILE
        ITF_MAP< u16, String8 >         pathTOC;
        ITF_MAP< FileLight, BundleIds > files;
#endif

        am.serialize( size );
        while ( size-- )
        {
            FileLight file;

            am.serialize( file.pathId );
            file.filename.serialize( am );

            u32 bundles;

            am.serialize( bundles );
            bundleIds.resize( bundles );
            for ( u32 i = 0; i < bundles; ++i )
            {
                am.serialize( int8 );
                bundleIds[i] = int8;
            }
#ifdef ITF_NO_PATH_FILE
            files[file] = bundleIds;
#else
            m_files[file] = bundleIds;
#endif
            bundleIds.clear();
        }

        am.serialize( size );
        while ( size-- )
        {
            am.serialize( int8 );
            str.serialize( am );
            m_bundleTOC[int8] = str;
        }

        am.serialize( size );
        while ( size-- )
        {
            str.serialize( am );
            am.serialize( int16 );
#ifdef ITF_NO_PATH_FILE
            pathTOC[int16] = str;
#else
            m_pathTOC[str] = int16;
#endif
        }

#ifdef ITF_NO_PATH_FILE
        String8 fullpath;

        for ( ITF_MAP< FileLight, BundleIds >::const_iterator it = files.begin(); it != files.end(); ++it )
        {
            ITF_MAP< u16, String8 >::const_iterator pathIt = pathTOC.find( it->first.pathId );

            fullpath.clear();
            ITF_ASSERT( pathIt != pathTOC.end() );
            if ( pathIt != pathTOC.end() )
                fullpath = pathIt->second;
            fullpath += it->first.filename;
            m_files[StringID::StrToCRC(fullpath.cStr())] = it->second;
        }
#endif
    }

    bbool GlobalFat::load( const String& filename )
    {
        String  translatedPath;
        hwFile* file = newAlloc(mId_Temporary, hwFile());

        if ( !FILEMANAGER->TranslatePath( translatedPath, filename ) || !file->open( translatedPath, ITF_FILE_ATTR_READ ) )
        {
            SF_DEL( file );
            return bfalse;
        }

        u32 length = static_cast< u32 >( file->getLength() );
        u8* buffer = new_array(u8, length, mId_Temporary);

        if ( !file->read( buffer, length ) )
        {
            SF_DEL_ARRAY( buffer );
            SF_DEL( file );
            return bfalse;
        }

        ArchiveMemory am( buffer, length );

        serialize( am );
        SF_DEL_ARRAY( buffer );
        SF_DEL( file );

#ifdef ITF_X360
        String                      bundleFile;
        BundleTOC::const_iterator   end = m_bundleTOC.end();

        for ( BundleTOC::const_iterator it = m_bundleTOC.begin(); it != end; ++it )
        {
            u32 sortKey = U32_INVALID;

            bundleFile.setTextFormat( "%s_x360.ipk", it->second.cStr() );
            if ( FILEMANAGER->TranslatePath( translatedPath, bundleFile ) )
            {
                FILE_HANDLE handle = CreateFile( ( LPCSTR )StringConverter( translatedPath.cStr() ).getChar(), GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_FLAG_NO_BUFFERING, NULL );

                if ( handle != INVALID_HANDLE_VALUE )
                {
                    sortKey = XGetFilePhysicalSortKey( handle );
                    CloseHandle( handle );
                }
            }
            m_sortKeys[bundleFile.cStr()] = sortKey;
        }
#endif
        return btrue;
    }

    bbool GlobalFat::isBundleRegistered( const String& bundleFilename ) const
    {
        String8                     lower = bundleFilename.cStr();
        BundleTOC::const_iterator   end = m_bundleTOC.end();

        lower.toLower();
        for ( BundleTOC::const_iterator it = m_bundleTOC.begin(); it != end; ++it )
            if ( it->second == lower )
                return btrue;
        return bfalse;
    }

    String GlobalFat::findBundleContaining( const Path& filename ) const
    {
#ifdef ITF_NO_PATH_FILE
        StringID file = filename.getStringID();
#else
        PathString_t strFilename;
        
        filename.getString(strFilename);
        PathTOC::const_iterator it = m_pathTOC.find( FilePath::getDirectory( strFilename ).cStr() );

        if ( it == m_pathTOC.end() )
            return String::emptyString;

        FileLight file;

        file.filename = FilePath::getFilename( strFilename ).cStr();
        file.pathId = it->second;
#endif

        FileLinks::const_iterator it2 = m_files.find( file );

        if ( it2 == m_files.end() )
            return String::emptyString;

        const ITF_VECTOR< u8 >& bundles = it2->second;

        ITF_ASSERT_CRASH( !bundles.empty(), "File path %s referenced in secure fat but not contained in any bundle", filename.getString8().cStr() );
        for ( u32 i = 0; i < bundles.size(); ++i )
        {
            BundleTOC::const_iterator it3 = m_bundleTOC.find( bundles[i] );

            ITF_ASSERT_MSG( it3 != m_bundleTOC.end(), "Invalid secure fat bundle id" );
            if ( it3 != m_bundleTOC.end() )
                return it3->second;
        }
        return String::emptyString;
    }

#ifdef ITF_X360
    u32 GlobalFat::getSortKey( const String& bundleFilename ) const
    {
        String8                     lower = bundleFilename.cStr();
        SortKeyTOC::const_iterator  it;

        lower.toLower();
        it = m_sortKeys.find( lower );
        ITF_ASSERT_MSG( it != m_sortKeys.end(), "Unregistered bundle %s", lower.cStr() );
        return it != m_sortKeys.end() ? it->second : U32_INVALID;
    }
#endif

#ifdef ITF_NO_PATH_FILE
# define ACCESS_PATH_KEY( path ) path.getStringID()
#else
# define ACCESS_PATH_KEY( path ) path
#endif

    //------------------------------------------------------------------------------------------------------------------------
    FileHeaderMap::iterator FileHeaderMap::find( const Path& filename )
    {
        return m_files.find( ACCESS_PATH_KEY( filename ) );
    }

    FileHeaderMap::const_iterator FileHeaderMap::find( const Path& filename ) const
    {
        return m_files.find( ACCESS_PATH_KEY( filename ) );
    }

    FileHeaderMap::iterator FileHeaderMap::begin()
    {
        return m_files.begin();
    }

    FileHeaderMap::const_iterator FileHeaderMap::begin() const
    {
        return m_files.begin();
    }

    FileHeaderMap::iterator FileHeaderMap::end()
    {
        return m_files.end();
    }

    FileHeaderMap::const_iterator FileHeaderMap::end() const
    {
        return m_files.end();
    }

    FileHeaderRuntime& FileHeaderMap::operator []( const Path& filename )
    {
        return m_files[ACCESS_PATH_KEY( filename )];
    }

    //------------------------------------------------------------------------------------------------------------------------

    void SharableBundleHeader::build(const FilePackMaster::Headers& _vHeader,u32 _indexBundle)
    {

        for (FilePackMaster::Headers::const_iterator iter = _vHeader.begin();iter!=_vHeader.end();++iter)
        {
            FileHeaderRuntime headerRuntime = (*iter).second;
            headerRuntime.m_indexBundle = _indexBundle;
            m_mapFiles[(*iter).first] = headerRuntime;
        }

    }


    //------------------------------------------------------------------------------------------------------------------------

    const FileHeaderRuntime* SharableBundleHeader::getHeaderRuntime(const Path& _szFileName)
    {
        FileHeaderMap::const_iterator iter = m_mapFiles.find(_szFileName);

        if (iter != m_mapFiles.end())
            return &((*iter).second);

        return NULL;
    }

#ifdef ITF_WINDOWS
    void SharableBundleHeader::getFiles( ITF_VECTOR< FileHeaderMap::Key >& files ) const
    {
        files.clear();
        for ( FileHeaderMap::const_iterator it = m_mapFiles.begin(); it != m_mapFiles.end(); ++it )
            files.push_back( it->first );
    }
#endif //ITF_WINDOWS

    //------------------------------------------------------------------------------------------------------------------------


    bbool   SharableBundleHeader::fileExists(const Path& _szFileName)
    {
        FileHeaderMap::const_iterator iter = m_mapFiles.find(_szFileName);
        return (iter != m_mapFiles.end());
    }        

    //------------------------------------------------------------------------------------------------------------------------
    u64     SharableBundleHeader::fileGetPosition(const Path& _szFileName)
    {
        FileHeaderMap::const_iterator iter = m_mapFiles.find(_szFileName);
        if (iter != m_mapFiles.end())
            return ((*iter).second.m_Positions[0]);

        return U64_INVALID;
    }

    u64     SharableBundleHeader::fileGetCompressedSize(const Path& _szFileName)
    {
        FileHeaderMap::const_iterator iter = m_mapFiles.find(_szFileName);
        if (iter != m_mapFiles.end())
            return ((*iter).second.m_CompressedSize);

        return U64_INVALID;
    }

    void SharableBundleHeader::getFilesFromRange( u64 start, u64 end, ITF_VECTOR< FileHeaderMap::Key >& range )
    {
        ITF_ASSERT( start < end );
        range.clear();
        for ( FileHeaderMap::const_iterator it = m_mapFiles.begin(); it != m_mapFiles.end(); ++it )
            if ( it->second.m_Positions[0] >= start && it->second.m_Positions[0] < end )
            {
                ITF_ASSERT( ( it->second.m_Positions[0] + ( it->second.m_CompressedSize ? it->second.m_CompressedSize : it->second.m_OriginalSize ) ) <= end );
                range.push_back( it->first );
            }
    }

    //------------------------------------------------------------------------------------------------------------------------

    u64     SharableBundleHeader::fileGetLastTimeWriteAccess(const Path& _szFileName)
    {
        FileHeaderMap::const_iterator iter = m_mapFiles.find(_szFileName);
        if (iter != m_mapFiles.end())
            return ((*iter).second.m_FlushTime);

        return 0;
    }

    //------------------------------------------------------------------------------------------------------------------------

    u64     SharableBundleHeader::fileGetSize(const Path& _szFileName)
    {
        FileHeaderMap::const_iterator iter = m_mapFiles.find(_szFileName);

        if (iter != m_mapFiles.end())
            return ((*iter).second.m_OriginalSize);

        return 0;
    }

    //------------------------------------------------------------------------------------------------------------------------

} // namespace ITF
