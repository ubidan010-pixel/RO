#include "precompiled_engine.h"

#ifdef ITF_WINDOWS

#ifndef _ITF_BUNDLEBUILDER_H_
#include "engine/file/BundleBuilder.h"
#endif //_ITF_BUNDLEBUILDER_H_

#ifndef _ITF_FILEPACKFORMAT_H_
#include "engine/file/FilePackFormat.h"
#endif //_ITF_FILEPACKFORMAT_H_

#ifndef _ITF_FILEPATH_H_
#include "core/file/filepath.h"
#endif //_ITF_FILEPATH_H_

#ifndef _ITF_BUNDLEFILE_H_
#include "engine/file/BundleFile.h"
#endif //_ITF_BUNDLEFILE_H_

#ifndef _ITF_VERSIONING_H_
#include "core/versioning.h"
#endif //_ITF_VERSIONING_H_

#ifndef _ITF_COMPRESS_H_
#include "engine/compression/compress.h"
#endif //_ITF_COMPRESS_H_

#ifndef _ITF_ENGINESIGNATURE_H_
#include "engine/signature/engineSignature.h"
#endif //_ITF_ENGINESIGNATURE_H_

namespace ITF
{
    BundleBuilder::BundleBuilder(const String& _szRoot)
    {
        m_Header = new FilePackMaster;
        m_Root = _szRoot + "\\";
        m_Root.toLower();

        m_Root = FilePath::normalizePath(m_Root);

//under test
/*
        //open listed order;
        String filename = "E:/dvdtracking/FileList.bin";
       
        File* pFile = FILEMANAGER->openFile(filename,ITF_FILE_ATTR_READ);
        if (pFile)
        {
            u32 lenght = (u32) pFile->getLength();
            u8* pBuffer = newAlloc(mId_Temporary,u8[lenght]);

            pFile->read(pBuffer,lenght);
            FILEMANAGER->closeFile(pFile);
            pFile = NULL;

            ArchiveMemory rd(pBuffer,lenght);
            SF_DEL_ARRAY(pBuffer);

            u32 fileList;
            rd.serialize(fileList);

            String depot = m_Root;
            String file;
            String tmp,normalized;

            for (u32 index = 0;index<fileList;++index)
            {
                file.serialize(rd);
                addFile(file);
                m_FilePresent[file] = true;
                
            }


        }
*/

    }

    BundleBuilder::~BundleBuilder()
    {
        SF_DEL(m_Header);
    }

    void BundleBuilder::addFile(const String& _szFilename)
    {
        FileHeaderRuntime header;
        //Translate it in a relative directory..;
        i32 index;

        String tmp = _szFilename;
        tmp.toLower();
        u16* match = tmp.strstr(m_Root.cStr(), bfalse, &index);
        String filename;

        if ( match )
        {
            u32 dataLength = m_Root.getLen();
            u32 totalLength = tmp.getLen();
            filename.setText(match+dataLength,totalLength-dataLength);
        }
        else
        {
            filename = _szFilename;
        }

        if ( !FILEMANAGER->fileExists( _szFilename ) )
            return;

        u32 length = static_cast<u32> (FILEMANAGER->getFileSize(_szFilename));

        header.m_FlushTime      = FILEMANAGER->getLastTimeWriteAccess(_szFilename);
        header.m_OriginalSize   = length;
        m_Header->addFile(filename, header);
        m_FileStack.push_back(filename);
    }

    void _stdcall displayProgressBuild(f32 _percent)
    {
#ifndef ITF_CTR
        #undef printf
        printf("\rPacking: %0.02f%%",_percent);
        fflush(stdout);
#endif
    }


    void BundleBuilder::generate(const String& _szFilename,u32 _uPlatform,bbool _bCompressedBundle,callbackProgressBuild _func,int _dataversion)
    {
        u32 sizeHeaderFiles = m_Header->computeHeaderSize();

        File* writeFile = FILEMANAGER->openFile(_szFilename,ITF_FILE_ATTR_READ|ITF_FILE_ATTR_WRITE|ITF_FILE_ATTR_CREATE_NEW);

        if (writeFile)
        {
            //BEGIN for the creation of file of sizeHeader
            ArchiveMemory archiveWrite;
            BundleBootHeader bundleBootHeader;

            bundleBootHeader.uMagicNumber       = FilePackMaster::PackMagicNumber;
            bundleBootHeader.uPlatformSupported = _uPlatform;
            bundleBootHeader.bCompressed        = _bCompressedBundle;
            bundleBootHeader.uVersion           = Versioning::bundleVer();
            bundleBootHeader.uFilesStart        = sizeof(BundleBootHeader)+sizeHeaderFiles;
            bundleBootHeader.uFilesCount        = m_FileStack.size();
            bundleBootHeader.bBinaryScene       = btrue;
            bundleBootHeader.bBinaryLogic       = btrue;
            bundleBootHeader.uDataSignature     = _dataversion;
            bundleBootHeader.uEngineSignature   = EngineSignature::getEngineSignature();
            bundleBootHeader.uEngineVersion     = 0;
            bundleBootHeader.serialize(archiveWrite);
            ITF_ASSERT(archiveWrite.getSize()==sizeof(BundleBootHeader));

            writeFile->seek( bundleBootHeader.uFilesStart,FILE_CURRENT);
            //END

            //parse all the files and add them
            if (_func == NULL)
                _func = displayProgressBuild;

            writeAllFiles(m_Root,_bCompressedBundle,writeFile,_func, _uPlatform);

            writeFile->seek(0,FILE_BEGIN);

            //write the header information; 
            m_Header->serialize(archiveWrite);

            ITF_ASSERT( archiveWrite.getSize() == static_cast< i32 >( sizeof( BundleBootHeader ) + m_Header->computeHeaderSize() ) );
            ITF_ASSERT( bundleBootHeader.uFilesStart == static_cast< i32 >( sizeof( BundleBootHeader ) + m_Header->computeHeaderSize() ) );
            writeFile->write(archiveWrite.getData(),archiveWrite.getSize());
            
            writeFile->close();
        }
    }

    void BundleBuilder::writeAllFiles(const String& _szRoot,bbool _bundleCompressed,File* _fileWrite,callbackProgressBuild _cbProgress,u32 _uPlatform)
    {
        u64 positionRelative = 0;

        FileStack::size_type count = 0;
        FileStack::size_type countSize = m_FileStack.size();

        for (FileStack::iterator iter = m_FileStack.begin();iter!=m_FileStack.end();++iter)
        {
            FileHeaderRuntime& header = m_Header->getHeader( *iter );
            File* readFile = NULL;

            readFile = FILEMANAGER->openFile(*iter,ITF_FILE_ATTR_READ);

            //open the source brutally
            ITF_ASSERT(readFile);
            if (readFile)
            {
                bbool Compressed = _bundleCompressed;

                u32 lenghtSource = (u32) readFile->getLength();
                u8* pBufferSource = new u8[lenghtSource];
                u8* bufferToSave = NULL;
                u32 lenghtDestination = 0;
                u32 lenghtCompressed = 0;

                if (_uPlatform != BundlePlatform::_CTR)
                {
                    if (iter->strstr((u16*)L".tga.ckd") || iter->strstr((u16*)L".png.ckd") || iter->strstr((u16*)L".anm.ckd"))
                    {
                        Compressed = btrue;
                    }
                }


                if (lenghtSource<32*1024)
                    Compressed = bfalse;

                // On PS5 and Nintendo the compression is automatic through the packaging system, so disable it.
                // PS5 use pass through oodle compression.
                // Nintendo use LZ4 compression with a probably more noticeable performance impact, but still better than zip.
                if (_uPlatform == BundlePlatform::PS5 || _uPlatform == BundlePlatform::NSwitch2 || _uPlatform == BundlePlatform::NSwitch)
                {
                    Compressed = bfalse;
                }

                // LOG("ADDFILE:%ls LENGHT:%d",filename.cStr(),lenghtSource);

                readFile->read(pBufferSource,lenghtSource);
                FILEMANAGER->closeFile(readFile);

                if (Compressed)
                {
#ifndef ITF_CTR
                    u32 sizeRequired = Compress::computeSizeRequired(lenghtSource);
                    u8* compressionBuffer = new u8[sizeRequired];

                    u32 outlen = Compress::compressBuffer(compressionBuffer,sizeRequired,pBufferSource,lenghtSource);
                    if (!outlen)
                    {
                        ITF_FATAL_ERROR("Failed to compress %ls ",iter->cStr());
                        return ;
                    }

                    bufferToSave = compressionBuffer;
                    SF_DEL_ARRAY(pBufferSource);
                    lenghtDestination = outlen;
                    lenghtCompressed = outlen;
#else
                    ITF_ASSERT_MSG(bfalse, "not implemented");
#endif
                }
                else
                {
                    bufferToSave = pBufferSource;
                    lenghtDestination = lenghtSource;
                }

                //compressed
                _fileWrite->write(bufferToSave,lenghtDestination);

                ITF_ASSERT( header.m_Count > 0 );
                header.m_Count -= 1;
                header.m_CompressedSize = lenghtCompressed;
                header.m_OriginalSize = lenghtSource;
                header.m_Positions.push_back( positionRelative );
                positionRelative += lenghtDestination;

                SF_DEL_ARRAY(bufferToSave);
            }

            if (_cbProgress)
            {
                count++;
                _cbProgress((count*100.0f)/countSize);
            }
        }
    }
}

#endif //ITF_WINDOWS
