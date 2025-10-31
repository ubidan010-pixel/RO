#include "precompiled_engine.h"

#include "engine/boot/PrefetchFct.h"

#ifndef _ITF_FILEPATH_H_
#include "core/file/FilePath.h"
#endif //_ITF_FILEPATH_H_

#ifndef _ITF_COMPRESS_H_
#include "engine/compression/compress.h"
#endif //_ITF_COMPRESS_H_

#ifndef _ITF_FILESERVER_H_
#include "core/file/FileServer.h"
#endif //_ITF_FILESERVER_H_

#ifdef ITF_WII
#define ITF_USE_CACHED_ACT
#endif

namespace ITF
{

PrefetchFactory::Callbacks PrefetchFactory::m_callbacks;

bool PrefetchFactory::registerCallback( const String& ext, Callback cb )
{
    return m_callbacks.insert( Callbacks::value_type( ext, cb ) ).second;
}

bool PrefetchFactory::prefetch( const Path& path, Params& params )
{
    PathString_t filename;
    
    path.getString(filename);
    String                      ext = FilePath::getExtension( filename );
    Callbacks::const_iterator   it = m_callbacks.find( ext );

#ifdef ITF_USE_CACHED_ACT
#define CACHERANGE_COMPRESSION 150*1024

    if (ext == "act")
    {
        const String& nameCooked = FILESERVER->getCookedName(filename);
        u32 filesize = (u32) FILEMANAGER->getFileSize(nameCooked);

        if (filesize>CACHERANGE_COMPRESSION && !TEMPLATEDATABASE->hasCacheTemplate(path))
        {
            File* pFile = FILEMANAGER->openFile(nameCooked,ITF_FILE_ATTR_READ);

            u32 length = (u32) pFile->getLength();

            ArchiveMemory* pArchiveMemory = newAlloc(mId_Temporary, ArchiveMemory(length, length ,btrue));
            pFile->read(pArchiveMemory->getPtrForWrite(),length);
            pArchiveMemory->rewindForReading();
 
            u32 sizeRequired = Compress::computeSizeRequired(length);
            u8* pBuffer = new_array(u8,sizeRequired,mId_Temporary);

            u32 sizeCompressed = Compress::compressBuffer(pBuffer, sizeRequired, pArchiveMemory->getData(), length); 

            ITF_ASSERT_CRASH( params.hdl != NULL, "Invalid template client handler instance" );
            params.hdl->addUsedTemplate( path.getStringID() );
            TEMPLATEDATABASE->addToCache(params.hdl,path,length,sizeCompressed,pBuffer);
            SF_DEL(pArchiveMemory);
            FILEMANAGER->closeFile(pFile);
            SF_DEL_ARRAY(pBuffer);
            return  btrue;
        }
    }
#endif //ITF_USE_CACHED_ACT

    if ( it == m_callbacks.end() )
    {
        LOG( "[LOADING] Unable to prefetch file %s, file type not managed", path.getString8().cStr() );
        return false;
    }

    Callback ptr = it->second;

    return ptr( path, params );
}

}
