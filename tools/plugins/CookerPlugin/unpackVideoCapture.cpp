#include "core/enginecommon.h"

#ifndef _ITF_DIRECTORY_H_
#include "core/file/Directory.h"
#endif //_ITF_DIRECTORY_H_

#ifndef _ITF_FILEPATH_H_
#include "core/file/filepath.h"
#endif //_ITF_FILEPATH_H_

#ifndef _ITF_FILEMANAGER_ITF_H_
#include "adapters/FileManager_ITF/FileManager_ITF.h"
#endif //_ITF_FILEMANAGER_ITF_H_

#ifndef _ITF_UNPACKVIDEOCAPTURE_H_
#include "unpackVideoCapture.h"
#endif //_ITF_UNPACKVIDEOCAPTURE_H_

#include "../extern/CaptureJpeg/Source/INTERFCE.h"

#ifndef __MINILZO_H
#include "engine/lzo/minilzo.h"
#endif //__MINILZO_H

using namespace ITF;


void unpackVideoCapture::restore(const ITF::String& _srData,const ITF::String& _szDestination,u32 _quality)//quality 0 = no jpg output
{
    #undef printf
    ITF_VECTOR<String> vFiles;
    String tmp;

    String srcDir = FilePath::normalizePath(_srData)+"\\";
    Directory::searchFiles(vFiles,srcDir,"*.*",btrue);
    String FinalDestination = FilePath::normalizePath(_szDestination) +"\\";

    Directory::create(_szDestination);
    printf("Restore destination ==> %S\n", _szDestination.wcharCStr());

    Directory::create(_szDestination+"\\HD\\");
    Directory::create(_szDestination+"\\LD\\");
    lzo_init();

    u32 count = 0;
    u32 fileCount = vFiles.size();
    for (ITF_VECTOR<ITF::String>::const_iterator iterFiles = vFiles.begin();iterFiles!=vFiles.end();++iterFiles)
    {
        count++;
        String filenameSrc      = FilePath::normalizePath(*iterFiles);
        ITF::File*  binaryFile  = FILEMANAGER->openFile(filenameSrc ,ITF_FILE_ATTR_READ);

        if (!binaryFile)
        {
            printf("failed to open the kao file\n");
            return;
        }

        printf("restore ==> %S %0.02f%%\n", filenameSrc.wcharCStr(), (count*100.0f)/fileCount);
        fflush(stdout);

        u32 lenght = (u32) binaryFile->getLength();
        u8* pBuffer = new u8[lenght];


        u32 width,height;
        binaryFile->read(&width,sizeof(width));
        binaryFile->read(&height,sizeof(height));

        binaryFile->read(pBuffer,lenght);
        FILEMANAGER->closeFile(binaryFile);

        u32 targetsize = width*height*3;
        u8* bufferUncompression = new u8[targetsize];

        BITMAPFILEHEADER infoHeader;

        // Bitmap info header 
        BITMAPINFOHEADER	t_BitmapInfo;
        ITF_MemSet(&t_BitmapInfo, 0, sizeof(BITMAPINFOHEADER));
        t_BitmapInfo.biWidth = width;
        t_BitmapInfo.biHeight = height;
        t_BitmapInfo.biPlanes = 1;
        t_BitmapInfo.biBitCount = (WORD) 24;
        t_BitmapInfo.biSize = sizeof(BITMAPINFOHEADER);
        t_BitmapInfo.biCompression = BI_RGB;
        t_BitmapInfo.biSizeImage = 0;
        t_BitmapInfo.biXPelsPerMeter = 0;
        t_BitmapInfo.biYPelsPerMeter = 0;
        t_BitmapInfo.biClrUsed = 0;
        t_BitmapInfo.biClrImportant = 0;

        infoHeader.bfType = 0x4d42;

        lzo_uint outlen = 0;
        lzo1x_decompress(pBuffer,width*height*3,bufferUncompression,&outlen, 0);


        infoHeader.bfSize = (DWORD)(outlen + sizeof(infoHeader)+sizeof(BITMAPINFOHEADER));
        infoHeader.bfReserved1 = 0;
        infoHeader.bfReserved2 = 0;
        infoHeader.bfOffBits = sizeof(infoHeader)+sizeof(BITMAPINFOHEADER);

        String filedst = FilePath::getFilenameWithoutExtension(filenameSrc);
        String filedstBmp = filedst + ".bmp";
        ITF::File*  filewrite = FILEMANAGER->openFile(FinalDestination+"\\HD\\"+filedstBmp ,ITF_FILE_ATTR_WRITE|ITF_FILE_ATTR_CREATE_NEW);

        if (filewrite)
        {
            filewrite->write(&infoHeader,sizeof(infoHeader));
            filewrite->write(&t_BitmapInfo,sizeof(t_BitmapInfo));

            filewrite->write(bufferUncompression, u32(outlen));
            FILEMANAGER->closeFile(filewrite);
        }
        else
        {

            SF_DEL_ARRAY(bufferUncompression);
            printf("failed to restore the file\n");
            return;
        }

        if (_quality != 0)
        {
            u8* srcBuffer = bufferUncompression;
            u8* dstBufferJpg = pBuffer; 

            JPEGDATA	st_JpegData;
            u8			*JpegData;

            Invert24YAndRB((u8 *) srcBuffer, (u8 *) srcBuffer, width, height);

            JpegData = dstBufferJpg;

            ITF_MemSet(&st_JpegData,0, sizeof(JPEGDATA));
            st_JpegData.width = width;
            st_JpegData.height = height;
            st_JpegData.ptr = (u8 *) srcBuffer;
            st_JpegData.output_file = dstBufferJpg;
            st_JpegData.quality = _quality;
            lJPEG_INPOUT_FILE_LENGHT = 0;
            JpegWrite(&st_JpegData);
            size_t v = lJPEG_INPOUT_FILE_LENGHT;
            String filedstJpg= filedst + ".jpg";
            ITF::File* writeFileJpg = FILEMANAGER->openFile(FinalDestination+"\\LD\\"+filedstJpg,ITF_FILE_ATTR_WRITE|ITF_FILE_ATTR_CREATE_NEW);

            if (writeFileJpg)
            {
                writeFileJpg->write((void*) JpegData, u32(v));
                FILEMANAGER->closeFile(writeFileJpg);
            }
        }

        SF_DEL_ARRAY(pBuffer);
        SF_DEL_ARRAY(bufferUncompression);
    }
}
