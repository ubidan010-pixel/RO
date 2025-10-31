#include "precompiled_engine.h"

#ifndef _ITF_SINGLETONS_H_
#include "engine/singleton/Singletons.h"
#endif //_ITF_SINGLETONS_H_

#if defined(ITF_PS3)
#include "EdgeUncompress_PS3.h"
#endif

#ifndef ITF_CTR

#ifndef _ITF_COMPRESS_H_
#include "engine/compression/compress.h"
#endif //_ITF_COMPRESS_H_

#ifndef ZLIB_H
#include "ZLib/zlib.h"
#endif //ZLIB_H

#ifndef ZUTIL_H
#include "ZLib/zutil.h"
#endif //ZUTIL_H

#ifndef _ITF_FILE_H_
#include "core/file/File.h"
#endif //_ITF_FILE_H_

#ifndef _ITF_FILEPACKFORMAT_H_
#include "engine/file/FilePackFormat.h"
#endif //_ITF_FILEPACKFORMAT_H_

namespace ITF
{
#define COMPRESSION_SIZE 32768


    u32 Compress::computeSizeRequired(u32 _sourceSize)
    {   
        //Upon entry, destLen is the total size of the destination buffer,
        //which must be at least 0.1% larger than sourceLen plus 12 bytes
        u32 targetSize = (u32)(_sourceSize*1.1f+12+1); //+1 roundup
        return targetSize;
    }

    //----------------------------------------------------------------------------------------------------

    bbool Compress::uncompressFile(const String& _szSrcFilename,const String& _szDstFilename)
    {
        File* readFile;
        File* writeFile;
        readFile = FILEMANAGER->openFile(_szSrcFilename,ITF_FILE_ATTR_READ);
        u32 filesize = (u32)readFile->getLength();

        writeFile = FILEMANAGER->openFile(_szDstFilename,ITF_FILE_ATTR_WRITE|ITF_FILE_ATTR_CREATE_NEW);

        u32 targetSize = computeSizeRequired(COMPRESSION_SIZE);
        u8* destination     = new u8[targetSize];
        u8* source          = new u8[COMPRESSION_SIZE];

        
        u32 bytesToRead = filesize > COMPRESSION_SIZE ? COMPRESSION_SIZE : filesize;
        u32 bytesRead = 0;

        u32 bytesRemaining = filesize;
        u32 compressedSize = 0; 
        u32 written = 0;

        while (bytesRemaining!=0)
        {
            readFile->read(&compressedSize,sizeof(compressedSize),&bytesRead);
            bytesRemaining-=bytesRead;

            readFile->read(source,compressedSize,&bytesRead);
            if (bytesRead!=0)
                compressedSize = uncompressBuffer(destination,targetSize,source,bytesRead);

            bytesRemaining-=bytesRead;

            bytesToRead = bytesToRead>COMPRESSION_SIZE ? COMPRESSION_SIZE : bytesToRead;

            writeFile->write(destination,compressedSize,&written);
        }

        SF_DEL_ARRAY(destination);
        SF_DEL_ARRAY(source);

        FILEMANAGER->closeFile(writeFile);
        FILEMANAGER->closeFile(readFile);
        
        return btrue;
    }

    //----------------------------------------------------------------------------------------------------
    //compress in our own format,external zip are not supported
    bbool Compress::compressFile(const String& _szSrcFilename,const String& _szDstFilename)
    {
        
        File* readFile = FILEMANAGER->openFile(_szSrcFilename,ITF_FILE_ATTR_READ);
        u32 filesize = (u32) readFile->getLength();

        File* writeFile = FILEMANAGER->openFile(_szDstFilename,ITF_FILE_ATTR_WRITE|ITF_FILE_ATTR_CREATE_NEW);
    
        u32 targetSize = computeSizeRequired(COMPRESSION_SIZE);

        u8* destination     = new u8[targetSize];
        u8* source          = new u8[COMPRESSION_SIZE];

        u32 bytesToRead = filesize>COMPRESSION_SIZE ? COMPRESSION_SIZE : filesize;
        u32 bytesRead = 0;

        u32 bytesRemaining = filesize;
        u32 compressedSize = 0; 
        u32 written = 0;

        while (bytesRemaining!=0)
        {
            readFile->read(source,bytesToRead,&bytesRead);

            if (bytesRead!=0)
                compressedSize = compressBuffer(destination,targetSize,source,bytesRead);

            bytesRemaining-=bytesRead;
            
            bytesToRead = bytesToRead>COMPRESSION_SIZE ? COMPRESSION_SIZE : bytesToRead;

            writeFile->write(&compressedSize,sizeof(compressedSize),&written);
            writeFile->write(destination,compressedSize,&written);
        }

        SF_DEL_ARRAY(destination);
        SF_DEL_ARRAY(source);

        FILEMANAGER->closeFile(writeFile);
        FILEMANAGER->closeFile(readFile);
        return btrue;
    }


    //----------------------------------------------------------------------------------------------------
    bbool Compress::compressFiles(const ITF_VECTOR<String>& _vFilenames,ITF_VECTOR<u64>& _vCompressedSize)
    {
        //TODO MUST BE IMPLEMENTED
        ITF_ASSERT(0);
        return btrue;
    }

    //----------------------------------------------------------------------------------------------------
    i32 Compress::compressBuffer(u8 *target, u32 target_size,const u8 *source, u32 source_size) 
    {
        uLongf dest_len = (uLongf)target_size;

        int zerr = compress(target, &dest_len, source, source_size);
        switch(zerr) {
            case Z_MEM_ERROR:	// not enough memory
            case Z_BUF_ERROR:	// not enough room in the output buffer
                    ITF_ASSERT(bfalse);
                return 0;
            case Z_OK:
                return dest_len;
        }

        return 0;
    }

    //----------------------------------------------------------------------------------------------------
    i32 Compress::uncompressBuffer(u8 *target, u32 target_size, const u8 *source, u32 source_size) 
    {
#if !defined(ITF_PS3)
        uLongf dest_len = (uLongf)target_size;

        int zerr = uncompress(target, &dest_len, source, source_size);
        switch(zerr) {
            case Z_MEM_ERROR:	// not enough memory
            case Z_BUF_ERROR:	// not enough room in the output buffer
            case Z_DATA_ERROR:	// input data was corrupted
                //Raise an error here
                ITF_ASSERT(bfalse);
                return 0;
            case Z_OK:
                return dest_len;
        }
        return 0;
#else
        return EdgeUnCompress::uncompressBuffer(target, target_size, source, source_size);
#endif
    }

} // namespace ITF
#endif
