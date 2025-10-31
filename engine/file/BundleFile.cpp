#include "precompiled_engine.h"

#ifndef _ITF_BUNDLEFILE_H_
#include "engine/file/bundleFile.h"
#endif //_ITF_BUNDLEFILE_H_

#ifndef _ITF_FILEPACK_H_
#include "engine/file/FilePack.h"
#endif //_ITF_FILEPACK_H_

#ifndef _ITF_FILEPACKFORMAT_H_
#include "engine/file/FilePackFormat.h"
#endif //_ITF_FILEPACKFORMAT_H_

#ifndef _ITF_COMPRESS_H_
#include "engine/compression/compress.h"
#endif //_ITF_COMPRESS_H_

#ifndef _ITF_SYSTEMADAPTER_
#include "core/AdaptersInterfaces/SystemAdapter.h"
#endif //_ITF_SYSTEMADAPTER_

#ifndef _ITF_ENGINESIGNATURE_H_
#include "engine/signature/engineSignature.h"
#endif //_ITF_ENGINESIGNATURE_H_

#ifndef _ITF_BUNDLEPROFILER_H_
#include "engine/file/bundleProfiler.h"
#endif //_ITF_BUNDLEPROFILER_H_

#ifndef _ITF_FILEREMOTE_H_
#include "core/file/FileRemote.h"
#endif //_ITF_FILEREMOTE_H_

#ifndef ITF_FINAL
//#define DISPLAY_BUNDLE_LOG 1 
#endif //ITF_FINAL

#define ITF_NO_BUNDLE_TRACKING

namespace ITF
{

#ifdef ITF_SUPPORT_CHEAT
BundleFile*     BundleFile::m_previousBundleFile = NULL;
#endif //ITF_SUPPORT_CHEAT

#ifdef ITF_X360
volatile long BundleFile::m_lastAccessSortKey = U32_INVALID;
#endif

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------

BundleCache::BundleCache( File* file, u32 cacheCapacity )
: m_file( file )
, m_capacity( cacheCapacity )
, m_absRead( 0 )
, m_absWrite( 0 )
{
    ITF_ASSERT_CRASH( file != NULL, "Invalid file pointer" );
    ITF_ASSERT_CRASH( m_capacity && !( m_capacity & ( m_capacity - 1 ) ), "Cache capacity must be power of 2" );
    m_pData = newAlloc(mId_System, u8[m_capacity] );
}

BundleCache::~BundleCache()  
{
    SF_DEL_ARRAY( m_pData );
};

u32 BundleCache::read( u8* buffer, u32 size )
{
    ITF_ASSERT( buffer != NULL );
    if ( buffer == NULL || !size )
        return 0;

    u32 read = 0;
    u32 cacheMask = m_capacity - 1;

    while ( size )
    {
        if ( m_absWrite == m_absRead )
        {
            u32 writeOff = m_absWrite & cacheMask;
            u32 readFromFile = 0;

            if ( !m_file->read( m_pData + writeOff, m_capacity - writeOff, &readFromFile ) )
                return U32_INVALID;
            if ( !readFromFile )
                return read;
            m_absWrite += readFromFile;
        }

        u32 cacheSize = getCacheSize();

        if ( cacheSize > size )
            cacheSize = size;
        ITF_Memcpy( buffer, m_pData + ( m_absRead & cacheMask ), cacheSize );
        buffer += cacheSize;
        m_absRead += cacheSize;
        size -= cacheSize;
        read += cacheSize;
    }
    return read;
}

u64 BundleCache::seek( u64 pos )
{
    if ( pos >= m_absRead && pos <= m_absWrite )
        m_absRead = pos;
    else
        m_absRead = m_absWrite = pos;
    return m_absRead;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------


    //------------------------------------------------------------------------------------------------------------------------------------
//BundleFile is owned by one thread and cannot be used by other thread
bbool BundleFile::openBundle(const String& _szFilename,SharableBundleHeader* _SharableHeader,u32 _indexBundle,bbool _getHeader,u32& _dataVersion,bbool useCache)
{
    m_szFilename = _szFilename;

    m_FileRead = newAlloc(mId_System, hwFile());

    String finalPath;
    FILEMANAGER->TranslatePath(finalPath,_szFilename);
    bbool openSuccess = m_FileRead->open(finalPath,ITF_FILE_ATTR_READ, ( useCache ? ITF_FILE_FLAGS_ATTR_SEQUENTIAL : ITF_FILE_FLAGS_ATTR_RANDOM ) | ITF_FILE_FLAGS_ATTR_READONLY );
    if (!openSuccess)
    {
        SF_DEL(m_FileRead);
        return bfalse;
    }

#ifdef ITF_X360
    m_sortKey = XGetFilePhysicalSortKey( m_FileRead->getHandle() );
    if ( m_sortKey == U32_INVALID )
    {
        LOG( "[BUNDLE] Warning: invalid sort key for bundle %ls", _szFilename.cStr() );
    }
#endif
    PROFILE_BUNDLEOPEN( _szFilename );

    m_SharableHeader = _SharableHeader;

  //get the fat if required
    if (_getHeader)
    {
#ifndef ITF_NO_BUNDLE_TRACKING
        LOG( "[BUNDLE][%s] Reading %s bundle headers", ThreadSettings::getCurrentThreadName(), StringToUTF8(_szFilename).get() );
#endif

        //Boot Header
        BundleBootHeader   bundleBootHeader;
        u8*                pHeader = new_array( u8, sizeof( BundleBootHeader ), mId_Temporary );

        if ( !m_FileRead->read( pHeader,sizeof( BundleBootHeader ) ) )
        {
            m_FileRead->close();
            SF_DEL( m_FileRead );
            SF_DEL_ARRAY( pHeader );
            return bfalse;
        }

        ArchiveMemory am( pHeader, sizeof( BundleBootHeader ) );

        bundleBootHeader.serialize( am );
        SF_DEL_ARRAY( pHeader );

        FilePackMaster  packMaster;
        u32             sizeFilesHeader = static_cast< u32 >( bundleBootHeader.uFilesStart - sizeof( BundleBootHeader ) );
        u8*             buffer = new_array( u8, sizeFilesHeader, mId_Temporary );

        if ( !m_FileRead->read( buffer, sizeFilesHeader ) )
        {
            m_FileRead->close();
            SF_DEL( m_FileRead );
            SF_DEL_ARRAY( buffer );
            return bfalse;
        }

        ArchiveMemory mw( buffer, sizeFilesHeader );

        packMaster.serialize( mw );
        SF_DEL_ARRAY( buffer );
        if ( !m_SharableHeader->isReadOnly() )
        {
            m_SharableHeader->build( packMaster.getFileHeaderBuild(), _indexBundle );
        }
        m_SharableHeader->setFileStart( bundleBootHeader.uFilesStart );
        m_SharableHeader->setEngineSignature( bundleBootHeader.uEngineSignature );
        m_SharableHeader->setDataSignature( bundleBootHeader.uDataSignature );
    }

    m_StartDataPos = m_SharableHeader->getFileStart();
    m_CurrentPosition = m_FileRead->seek( m_StartDataPos, FILE_BEGIN );
    m_engineSignature = m_SharableHeader->getEngineSignature();
    _dataVersion      = m_SharableHeader->getDataSignature();

#ifndef ITF_FINAL
    u32 globalEngineSignature = EngineSignature::getEngineSignature();

    if ( globalEngineSignature != 0 && globalEngineSignature != m_engineSignature )
    {
        ITF_FATAL_ERROR( "the bundle loaded %s doesnt match the signature", StringToUTF8(m_szFilename).get() );
    }
#endif //ITF_FINAL

    m_fileSize  = m_FileRead->getLength();
    if ( useCache )
    {
        m_dataCache = newAlloc(mId_System, BundleCache( m_FileRead, 128 * 1024 ));
        m_dataCache->seek( m_SharableHeader->getFileStart() );
    }

    return btrue;
}
//------------------------------------------------------------------------------------------------------------------------------------

BundleList::~BundleList()
{
    //for (ITF_VECTOR<BundleFile*> ::iterator iter = m_vBundleFile.begin();iter!=m_vBundleFile.end();iter++)
    //{
    //    SF_DEL(*iter);
    //}
}


//------------------------------------------------------------------------------------------------------------------------------------
void BundleFile::closeBundle()
{
    m_FileRead = FILEMANAGER->closeFile(m_FileRead);
}

//------------------------------------------------------------------------------------------------------------------------------------

u64   BundleFile::fileGetLastTimeWriteAccess(const String& _szFileName)
{
    ITF_ASSERT(m_SharableHeader);
    return m_SharableHeader->fileGetLastTimeWriteAccess(_szFileName);
}

void BundleFile::getFilesFromRange(u64 start, u64 end, ITF_VECTOR< FileHeaderMap::Key >& range)
{
    ITF_ASSERT(m_SharableHeader);
    return m_SharableHeader->getFilesFromRange(start - m_StartDataPos, end - m_StartDataPos, range);
}

//------------------------------------------------------------------------------------------------------------------------------------

bbool   BundleFile::fileExists(const String& _szFileName)
{
    ITF_ASSERT(m_SharableHeader);
    return m_SharableHeader->fileExists(_szFileName);
}

//------------------------------------------------------------------------------------------------------------------------------------

void   BundleFile::fileGetPosition(const String& _szFileName,FilesPosition& filePos)
{
    ITF_ASSERT(m_SharableHeader);

#ifdef ITF_X360
    filePos.m_bundleKey = m_sortKey;
#endif
    filePos.m_Position = m_StartDataPos + m_SharableHeader->fileGetPosition( _szFileName );
    filePos.m_fileSize = m_SharableHeader->fileGetCompressedSize( _szFileName );
    if ( !filePos.m_fileSize )
        filePos.m_fileSize = m_SharableHeader->fileGetSize( _szFileName );
}

//------------------------------------------------------------------------------------------------------------------------------------

u64     BundleFile::fileGetSize(const String& _szFileName)
{
    ITF_ASSERT(m_SharableHeader);
    return m_SharableHeader->fileGetSize(_szFileName);
}

//------------------------------------------------------------------------------------------------------------------------------------

BundleFile::BundleFile()
: m_SharableHeader( NULL )
, m_CurrentPosition( 0 )
, m_FileRead( NULL )
, m_engineSignature( 0 )
, m_dataCache( NULL )
, m_prio( FileManager::Lowest )
{
#ifdef ITF_X360
    m_sortKey = U32_INVALID;
#endif
}

BundleFile::~BundleFile()
{
    SF_DEL(m_dataCache);
    SF_DEL(m_FileRead);
}

//------------------------------------------------------------------------------------------------------------------------------------

bbool BundleFile::fileRead(u64 _globalposition,void* _buffer,u32 _bytesToRead,u32& _readBytes)
{

/****** DEBUG KEEP IT TO TRACK AN ISSUE***/
    u32 readBytesLocal = 0;
    u8* buffer = static_cast< u8* >( _buffer );
    
    _readBytes = 0;
    if ( m_dataCache != NULL )
    {
        if ( _globalposition != m_dataCache->tell() )
        {
            m_dataCache->seek( _globalposition );
            if ( !m_dataCache->getCacheSize() )
            {

#ifdef ITF_SUPPORT_CHEAT
                if (m_previousBundleFile != this)
                {
                    SIMULATE_TIME
                }
#endif //ITF_SUPPORT_CHEAT
                m_CurrentPosition = m_FileRead->seek( _globalposition, FILE_BEGIN );

#ifdef ITF_SUPPORT_CHEAT
                m_previousBundleFile = this;
#endif //ITF_SUPPORT_CHEAT
            }
        }
        if ( _bytesToRead <= m_dataCache->getCacheCapacity() )
        {
            readBytesLocal = m_dataCache->read( buffer, _bytesToRead );
            m_CurrentPosition = m_dataCache->tell() + m_dataCache->getCacheSize();
            _readBytes = readBytesLocal;
#ifdef ITF_X360
            if ( m_sortKey != U32_INVALID )
                InterlockedExchange( &m_lastAccessSortKey, m_sortKey );
#endif
            return btrue;
        }

        if ( _globalposition >= m_dataCache->tell() && _globalposition < m_dataCache->tell() + m_dataCache->getCacheSize() )
        {
            readBytesLocal = m_dataCache->read( buffer, m_dataCache->getCacheSize() );
            buffer += readBytesLocal;
            _bytesToRead -= readBytesLocal;
            _readBytes += readBytesLocal;
            m_CurrentPosition = m_dataCache->tell();
        }

        if ( _bytesToRead )
        {
            m_FileRead->read( buffer, _bytesToRead, &readBytesLocal );
            m_CurrentPosition += readBytesLocal;
            m_dataCache->seek(m_CurrentPosition);
            _readBytes += readBytesLocal;
        }
#ifdef ITF_X360
        if ( m_sortKey != U32_INVALID )
            InterlockedExchange( &m_lastAccessSortKey, m_sortKey );
#endif
        return btrue;
    }

    if ( _globalposition != m_CurrentPosition )
    {
        //add a tracker
        m_CurrentPosition = m_FileRead->seek( _globalposition, FILE_BEGIN );
    }

    m_FileRead->read( buffer, _bytesToRead, &readBytesLocal );
    m_CurrentPosition += readBytesLocal;
    _readBytes = readBytesLocal;

   // u32 allowedHardwareToRead = (u32)((m_CurrentPosition+_bytesToRead) > m_fileSize ? (m_CurrentPosition-_bytesToRead) : m_fileSize-m_CurrentPosition);
   /*
    BundleCache::CacheHit hit = m_dataCache->getHit(_globalposition,_bytesToRead);
    u32 readBytesLocal = 0;
    bbool useCache = bfalse;
    const u32 cacheCapacity = m_dataCache->getCapacity();
    switch (hit)
    {
        case BundleCache::eCacheNoneHit:
            {
                //if the bytes to read is more that the cache capacity,we read directly
                //if the bytes is less that the cache,we read the cache capacity
                if (_globalposition != m_CurrentPosition)
                {
                    //add a tracker
                    m_CurrentPosition = m_FileRead->seek(_globalposition,FILE_BEGIN);
                }

                if (_bytesToRead>cacheCapacity)
                {
                    m_FileRead->read(_buffer,_bytesToRead,&readBytesLocal);

                    m_CurrentPosition+=readBytesLocal;
                    _readBytes = readBytesLocal;
                }
                else
                {
                    u8* pBufferCache = m_dataCache->getBuffer();
                    m_FileRead->read(pBufferCache,cacheCapacity,&readBytesLocal);
                    m_dataCache->set(_globalposition,readBytesLocal);

                    u32 copyBytes = Min(_bytesToRead,readBytesLocal);
                    ITF_Memcpy(_buffer,pBufferCache,copyBytes);

                    m_CurrentPosition+=readBytesLocal;
                    _readBytes = copyBytes;
                }

            }
        break;

        case BundleCache::eCacheFullHit:
            {
                ITF_Memcpy(_buffer,m_dataCache->getFullHitBuffer(_globalposition),_bytesToRead);
                _readBytes = _bytesToRead;
                useCache = btrue;
            }
        break;

        case BundleCache::eCachePartiallyHit:
            {
                m_FileRead->read(_buffer,_bytesToRead,&readBytesLocal);
                m_CurrentPosition+=readBytesLocal;
                _readBytes = readBytesLocal;
            }
        break;

        default:
            {
                ITF_ASSERT(0);
            }
            break;
    }*/

    //LOG("CACHE READ : usecache :%d //%d was supposed to read %d // %ld",useCache,_bytesToRead,readBytesLocal,_globalposition);
    //fill next cache
    //m_FileRead->read(_buffer,m_dataCache->getCapacity(),&_readBytes);
    //m_dataCache->set(_buffer,);

#ifdef ITF_X360
    if ( m_sortKey != U32_INVALID )
        InterlockedExchange( &m_lastAccessSortKey, m_sortKey );
#endif
    return btrue;
}

//------------------------------------------------------------------------------------------------------------------------------------

Filepack*   BundleFile::fileOpen(const String& _szFileName)
{
    const FileHeaderRuntime* pHeaderRuntime = m_SharableHeader->getHeaderRuntime(_szFileName);

    if (!pHeaderRuntime)
        return nullptr;

#if !defined(ITF_WII) || defined(_DEBUG)
    //if (ISDEBUGGER_PRESENT)
    {
        //i64 deltaseek = pHeaderRuntime->m_Position-lastposition;
    //    LOG("READ FILE %ls DELTA:%li FRAME:%d COMPRESSED:%d",_szFileName.cStr(),deltaseek,CURRENTFRAME,pHeaderRuntime->m_CompressedSize!=0,loadsize);
    }
#endif // !defined(ITF_WII) || defined(_DEBUG)

    ITF_ASSERT_CRASH( !pHeaderRuntime->m_Positions.empty(), "Invalid header, should at least contains one offset" );
#ifdef ASSERT_ENABLED
    u64 lastPos = pHeaderRuntime->m_Positions[0];

    for ( FileHeaderRuntime::Positions::size_type i = 1; i < pHeaderRuntime->m_Positions.size(); ++i )
    {
        ITF_ASSERT_MSG( pHeaderRuntime->m_Positions[i] > lastPos, "File position are not sorted" );
        lastPos = pHeaderRuntime->m_Positions[i];
    }
#endif // ASSERT_ENABLED

    u64 filePos = 0;

    if ( pHeaderRuntime->m_Positions.size() == 1 )
        filePos = pHeaderRuntime->m_Positions[0];
    else if ( m_CurrentPosition <= pHeaderRuntime->m_Positions.front() )
        filePos = pHeaderRuntime->m_Positions.front();
    else if ( m_CurrentPosition >= pHeaderRuntime->m_Positions.back() )
        filePos = pHeaderRuntime->m_Positions.back();
    else
        for ( FileHeaderRuntime::Positions::size_type i = 0; i + 1 < pHeaderRuntime->m_Positions.size(); ++i )
            if ( m_CurrentPosition >= pHeaderRuntime->m_Positions[i] && m_CurrentPosition <= pHeaderRuntime->m_Positions[i + 1] )
            {
                if ( m_CurrentPosition - pHeaderRuntime->m_Positions[i] < pHeaderRuntime->m_Positions[i + 1] - m_CurrentPosition )
                    filePos = pHeaderRuntime->m_Positions[i];
                else
                    filePos = pHeaderRuntime->m_Positions[i + 1];
                break;
            }

    ITF_ASSERT(pHeaderRuntime->m_OriginalSize);

#ifdef DISPLAY_BUNDLE_LOG
    f64 startTime = SYSTEM_ADAPTER->getTime();
#endif //DISPLAY_BUNDLE_LOG
    
    Filepack* pFile = newAlloc(mId_System, Filepack(this,filePos+m_StartDataPos,pHeaderRuntime->m_OriginalSize,pHeaderRuntime->m_CompressedSize, _szFileName));

#ifndef ITF_CTR
    if (pFile->isCompressed())
    {
        u32 originalSize    = pHeaderRuntime->m_OriginalSize;
        u8* compressed      = new_array(u8, pHeaderRuntime->m_CompressedSize, mId_Temporary);
        u8* pBufferData     = NULL;

        pFile->readFromCompressed(compressed,pHeaderRuntime->m_CompressedSize,pHeaderRuntime->m_CompressedSize);
        {
            pBufferData     = new_array(u8, originalSize, mId_System);
            PROFILE_BUNDLECOMPRESSION(pFile->getGlobalPositionOnfile(),pHeaderRuntime->m_OriginalSize,pHeaderRuntime->m_OriginalSize,_szFileName,pFile->getBundleParent()->getFilename());
            Compress::uncompressBuffer(pBufferData,originalSize,compressed,pHeaderRuntime->m_CompressedSize);
        }
        //restart the pack file at the beginning
        pFile->seek(0,0);
        pFile->setBuffer(pBufferData);
        SF_DEL_ARRAY(compressed);
    }
#else
    ITF_ASSERT_MSG(bfalse, "not implemented");
#endif

#ifdef DISPLAY_BUNDLE_LOG
    f32 timeToLoad = static_cast<f32>(SYSTEM_ADAPTER->getTime()-startTime);
    LOG("open Filepack %s %0.02f ms", StringToUTF8(_szFileName).get(), timeToLoad*1000.0f);
#endif //DISPLAY_BUNDLE_LOG
    //pFile->waitTobeReady();

//#ifndef ITF_NO_BUNDLE_TRACKING
//    if ( pFile != NULL )
//        LOG( "[Layout OPEN][%s] fileName: %ls fileSize: %d currentpos: %ld openedAt: %ld", ThreadSettings::getCurrentThreadName(), _szFileName.cStr(), pFile->getLength(), m_CurrentPosition, filePos );
//#endif

    return pFile;
}

//------------------------------------------------------------------------------------------------------------------------------------

//#ifndef ITF_FINAL
void BundleFile::TrackReference( Filepack* file )
{
    ITF_VERIFY(m_trackedFiles.insert( file ).second);
}

void BundleFile::UntrackReference( Filepack* file )
{
    m_trackedFiles.erase( file );
}
//#endif //ITF_FINAL

}
