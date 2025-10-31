#include "precompiled_engine.h"

#ifdef ITF_SUPPORT_VIDEOCAPTURE

#include <algorithm>

#ifndef _ITF_VIDEOCAPTURE_H_
#include "engine/display/videoCapture.h"
#endif //_ITF_VIDEOCAPTURE_H_

#include "../extern/CaptureJpeg/Source/INTERFCE.h"

#ifndef __MINILZO_H
#include "engine/lzo/minilzo.h"
#endif //__MINILZO_H

#ifndef _ITF_DIRECTORY_H_
#include "core/file/Directory.h"
#endif //_ITF_DIRECTORY_H_

#ifndef _ITF_PUGIXMLWRAP_H_
#include "core/XML/PugiXMLWrap.h"
#endif //_ITF_PUGIXMLWRAP_H_

#ifndef _ITF_FILESERVER_H_
#include "core/file/FileServer.h"
#endif //_ITF_FILESERVER_H_

#ifndef _ITF_AUDIOCAPTURE_H_
#include "engine/sound/AudioCapture.h"
#endif //_ITF_AUDIOCAPTURE_H_

#ifndef _ITF_DEBUGDRAW_H_
#include "engine/debug/DebugDraw.h"
#endif //_ITF_DEBUGDRAW_H_

#ifndef _ITF_FILEPATH_H_
#include "core/file/filepath.h"
#endif //_ITF_FILEPATH_H_

namespace ITF
{

VideoCapture*  VideoCapture::m_Instance = NULL;

void    VideoCaptureBuffer::setOffscreenBuffer(u32 _width,u32 _height,void* _offscreenBuffer,void* _stretchBuffer,void* _renderTarget)
{
    m_height = _height;
    m_width = _width;
    m_captureOffscreenSurface = _offscreenBuffer;
    m_captureStretchSurface   = _stretchBuffer;
    m_captureRenderTarget     = _renderTarget;
}

void VideoCapture::recordInMemory(u8* pBuffer,u32 size,u32 frameCount)
{
    //keep the files in memory;   
    csAutoLock cs(m_csScreenCaptureLowRes);
    ImageMemory& imageMemory = m_LowResCapture.m_vImageMemory[m_LowResCapture.m_currentFrameArray];


    if (imageMemory.pBuffer)
    {
        if (imageMemory.size != size) //need a new allocation
        {
            SF_DEL_ARRAY(imageMemory.pBuffer);
            imageMemory.pBuffer = new u8[size];
        }
    }
    else
        imageMemory.pBuffer = new u8[size];

    ITF_Memcpy(imageMemory.pBuffer,pBuffer,size);
    imageMemory.size    = size;
    imageMemory.frame   = frameCount;

    m_LowResCapture.m_incFrameMemory++;
    m_LowResCapture.m_currentFrameArray++;
    m_LowResCapture.m_currentFrameArray=  m_LowResCapture.m_currentFrameArray%m_LowResCapture.m_uMaxFrameToCapture;

}

void    VideoCapture::jpeg_save(const char *_pz_fileName, images_Desc *_pt_Desc,u8* pMemory,u32 quality,u32 frameCount)
{
    JPEGDATA	st_JpegData;
    u8			*JpegData;

    if(_pt_Desc->u8_BPP != 24) return;

    Invert24YAndRB((u8 *) _pt_Desc->p_Bitmap, (u8 *) _pt_Desc->p_Bitmap, _pt_Desc->u16_Width, _pt_Desc->u16_Height);

    JpegData = pMemory;

    ITF_MemSet(&st_JpegData,0, sizeof(JPEGDATA));
    st_JpegData.width = _pt_Desc->u16_Width;
    st_JpegData.height = _pt_Desc->u16_Height;
    st_JpegData.ptr = (u8 *) _pt_Desc->p_Bitmap;
    st_JpegData.output_file = JpegData;
    st_JpegData.quality = quality;
    lJPEG_INPOUT_FILE_LENGHT = 0;
    JpegWrite(&st_JpegData);
    int fileLenght = int(lJPEG_INPOUT_FILE_LENGHT);

    delete[] ((u8*)_pt_Desc->p_Bitmap);

    if (m_Instance->m_bSaveInMemory)
    {
        m_Instance->recordInMemory(JpegData,fileLenght,frameCount);
    }
    else
    {
        File* writeFile = FILEMANAGER->openFile(_pz_fileName,ITF_FILE_ATTR_WRITE|ITF_FILE_ATTR_CREATE_NEW);

        if (writeFile)
        {
            writeFile->write((void*) JpegData,fileLenght);
            FILEMANAGER->closeFile(writeFile);
        }
    }
}


//----------------------------------------------------------------------------------------------------------------------------------------------------------

VideoCapture::VideoCapture(u32 _threadUsage,bbool _bStepByStep)
{
    m_Instance = this;


    if (_bStepByStep)  //don t use any thread on stepbystep mode
    {
        m_uMaxThreadUsed = 0;
        m_pendingFrameInfo = NULL;
        m_csScreenCapture = NULL;
    }
    else
    {
        m_uMaxThreadUsed = _threadUsage;

        m_pendingFrameInfo = new  ITF_VECTOR<FrameCaptureInfo>[m_uMaxThreadUsed];
        m_csScreenCapture = new ITF_THREAD_CRITICAL_SECTION[m_uMaxThreadUsed];

        Synchronize::createCriticalSection(&m_csScreenCaptureLowRes);

        for (u32 index = 0;index<m_uMaxThreadUsed;++index)
            Synchronize::createCriticalSection(&m_csScreenCapture[index]);
    }
    
    m_bStepByStep = _bStepByStep;

    m_StoreFrameBuffer  = NULL;
    m_captureBuffer     = NULL;
    m_bEnableCapture    = bfalse;
    m_bLowResolution    = bfalse;
    m_bSaveInMemory     = m_bLowResolution;

    m_LowResCapture.m_uMaxFrameToCapture = 30*360;//30fps in 180 seconds
    m_LowResCapture.m_currentFrameArray  = 0;

    cleanITFScreenDirectories();

    m_AudioCapture = NULL;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------


struct directoryCompareDate
{

    directoryCompareDate():m_filename(NULL),m_date(0) {}
    directoryCompareDate(String* _Filename,u64 _date):m_filename(_Filename),m_date(_date)
    {

    };
    String* m_filename;
    u64     m_date;
};

static  bool        compareDateDirectories(directoryCompareDate& _p1, directoryCompareDate& _p2) { return _p1.m_date > _p2.m_date; }

void VideoCapture::cleanITFScreenDirectories()
{
    //clean debug VIDEO
    ITF_VECTOR<String> vPath;
    String rootScreen = FILESERVER->getWorkingDepot()+"ITF_SCREEN/";

    Directory::getDirectories(rootScreen,vPath);
    String directory;

    const int maxDirectoriesKeep = 10;
    if (vPath.size()>maxDirectoriesKeep)
    {
        ITF_VECTOR<directoryCompareDate> vDirectoryCompare;
        vDirectoryCompare.reserve(vPath.size());

        for (ITF_VECTOR<String>::iterator iter = vPath.begin();iter!=vPath.end();iter++)
        {
            FilePath::normPath(*iter);
            u64 dateFile = Directory::getLastTimeWriteAccess(*iter);
            vDirectoryCompare.push_back(directoryCompareDate(&(*iter),dateFile));
        }

        std::sort(vDirectoryCompare.begin(), vDirectoryCompare.end(), compareDateDirectories);

        //keep only the first maxDirectoriesKeep
        for (u32 index=maxDirectoriesKeep;index<vDirectoryCompare.size();++index)
        {
            Directory::deleteDirectories(*vDirectoryCompare[index].m_filename);
        }
    }

}

//----------------------------------------------------------------------------------------------------------------------------------------------------------

VideoCapture::~VideoCapture()
{
    m_Instance = NULL;

    stop();

    if (!m_bStepByStep)
    {
        SF_DEL_ARRAY(m_pendingFrameInfo);

        for (u32 index = 0;index<m_uMaxThreadUsed;++index)
            Synchronize::destroyCriticalSection(&m_csScreenCapture[index]);

        Synchronize::destroyCriticalSection(&m_csScreenCaptureLowRes);

        SF_DEL_ARRAY(m_csScreenCapture);
    }
}
//----------------------------------------------------------------------------------------------------------------------------------------------------------

void VideoCapture::FrameCaptureInfo::compress(const String8& _szPath,u8* compressTargetBuffer)
{
    String8 name;
    String8 Extension;

    if (m_bBMPOutput)
        Extension = "bmp";
    else
        Extension = m_bLosslessCompression ? "kao" : "jpg";

    name.setTextFormat("%s\\screenITF%08d.%s",_szPath.cStr(),m_uFrameCount, Extension.cStr());

    if (m_bBMPOutput)
    {
        BITMAPFILEHEADER fileHeader;


        //Extra byte lines:
        u32 extraLine = m_width-(m_width/4)*4;

        fileHeader.bfType = 0x4d42;//BM
        fileHeader.bfSize = (m_width+extraLine)*m_height*3 +sizeof(fileHeader)+sizeof(BITMAPINFOHEADER) ;
        fileHeader.bfReserved1 = 0;
        fileHeader.bfReserved2 = 0;
        fileHeader.bfOffBits = sizeof(fileHeader)+sizeof(BITMAPINFOHEADER);

        // Bitmap info header 
        BITMAPINFOHEADER	infoHeader;
        ITF_MemSet(&infoHeader, 0, sizeof(BITMAPINFOHEADER));
        infoHeader.biSize = sizeof(BITMAPINFOHEADER);
        infoHeader.biWidth = m_width;
        infoHeader.biHeight = m_height;
        infoHeader.biPlanes = 1;
        infoHeader.biBitCount = (WORD) 24;
        infoHeader.biCompression = 0;
        infoHeader.biSizeImage = 0;
        infoHeader.biXPelsPerMeter = 0;
        infoHeader.biYPelsPerMeter = 0;
        infoHeader.biClrUsed = 0;
        infoHeader.biClrImportant = 0;

        File* writeFile = FILEMANAGER->openFile(name,ITF_FILE_ATTR_WRITE|ITF_FILE_ATTR_CREATE_NEW);

        if (writeFile)
        {
            writeFile->write(&fileHeader,sizeof(fileHeader));
            writeFile->write(&infoHeader,sizeof(infoHeader));
            if (extraLine)
            {
                char c = 0;
                for (u32 index =0;index<m_height;index++)
                {
                    writeFile->write(m_pBuffer+(index*m_width*3),m_width*3);
                    for (u32 i = 0;i<extraLine;i++)
                        writeFile->write(&c,sizeof(c));
                }
            }
            else
            {
                writeFile->write(m_pBuffer,m_bufferSize);
            }


            FILEMANAGER->closeFile(writeFile);
        }

        delete[] m_pBuffer;

    }
    else
        if (m_bLosslessCompression)
        {
            lzo_uint outlen = 0;
            static LZO_HEAP_ALLOC(wrkmem, LZO1X_1_MEM_COMPRESS);
            lzo1x_1_compress(m_pBuffer,m_width*m_height*3,compressTargetBuffer,&outlen,wrkmem);

            delete[] m_pBuffer;

            File* writeFile = FILEMANAGER->openFile(name,ITF_FILE_ATTR_WRITE|ITF_FILE_ATTR_CREATE_NEW);

            if (writeFile)
            {
                writeFile->write(&m_width,sizeof(m_width));
                writeFile->write(&m_height,sizeof(m_height));
                writeFile->write(compressTargetBuffer, u32(outlen));
                FILEMANAGER->closeFile(writeFile);
            }

        }
        else
        {
            images_Desc t_Image;

            t_Image.u16_Width = (u16) m_width;
            t_Image.u16_Height = (u16) m_height;
            t_Image.p_Bitmap = m_pBuffer;
            t_Image.p_Palette = NULL;
            t_Image.u8_PaletteBPC = 0;
            t_Image.u16_PaletteLength = 0;
            t_Image.u8_BPP = 24;


            VideoCapture::jpeg_save(name.cStr(), &t_Image,compressTargetBuffer,m_quality,m_uFrameCount);
        }
}
//----------------------------------------------------------------------------------------------------------------------------------------------------------


void   VideoCapture::convertToJPG(u8* compressTargetBuffer,u32 dirIdx)
{
    u32 myListId = m_idCurrentArray++;

    ITF_VECTOR<FrameCaptureInfo> workingFile;
    while (!m_bExitCapture)
    {
        ThreadSettings::threadStartFrame();

        Sleep(2);
        {
            csAutoLock cs(m_csScreenCapture[myListId]);
            workingFile = m_pendingFrameInfo[myListId];
            m_pendingFrameInfo[myListId].clear();
        }

        for (ITF_VECTOR<FrameCaptureInfo> ::iterator iter = workingFile.begin();iter!=workingFile.end();++iter)
        {
            FrameCaptureInfo& frameinfo = (*iter);
            frameinfo.compress(m_ScreenCaptureDirectory,compressTargetBuffer);
        }
    }

    delete[] compressTargetBuffer;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------

u32 WINAPI ConvertToJPG(void* user)
{   
    VideoCapture::ThreadCaptureContainer* pThreadContainer = (VideoCapture::ThreadCaptureContainer*) user;
    pThreadContainer->m_pVideoCapture->convertToJPG(pThreadContainer->memBufferTarget,pThreadContainer->dirIdx);
    return 0;
}
//----------------------------------------------------------------------------------------------------------------------------------------------------------

void    VideoCapture::assignCaptureOffscreenBuffer(VideoCaptureBuffer* _pBufferOffscreenBuffer)
{
    if (m_captureBuffer)
        SF_DEL(m_captureBuffer);

    m_captureBuffer = _pBufferOffscreenBuffer;
}
//----------------------------------------------------------------------------------------------------------------------------------------------------------

void    VideoCapture::resolveBufferScreen()
{
   if (m_bEnableCapture)
   {

       if (useLowResolution())
       {
             GFX_ADAPTER->draw2dBox(Vec2d(100.0f,10.0f),28.0f,20.0f,COLOR_RED,COLOR_RED,COLOR_RED,COLOR_RED);
             GFX_ADAPTER->drawDBGText("REC", Color::white(), 102.0f,12.0f);
       }
   
       ITF_ASSERT(m_captureBuffer);

       m_CaptureFrameCount++;
       if (m_bModeOddFrame && m_CaptureFrameCount&1) //capture only odd pair
           return;
       
       MEMORYSTATUSEX statex;

       statex.dwLength = sizeof (statex);

       //TODO MANAGE A MEMORY THINGIE
       GlobalMemoryStatusEx (&statex);

       if (statex.ullAvailVirtual <= m_FlushThresholdBytes)
       {
          while (statex.ullAvailVirtual<m_MemoryThreshold)
          {
              GlobalMemoryStatusEx (&statex);
              sleep(50);
          }
       }
       u32 width    = 0;
       u32 height   = 0;

       u32 source_width    = m_captureBuffer->getWidth();
       u32 source_height   = m_captureBuffer->getHeight();

       width    = source_width;
       height   = source_height;

       u32 pitch    = m_captureBuffer->getPitch();
       unsigned long* pBits = static_cast<unsigned long*>(m_captureBuffer->getDataPtr());

       u32 end = (source_height-1)*(pitch/sizeof(unsigned long));
       unsigned long* src = pBits+end;//start from the end

       u32 globalSize = height*width*3;           

       u32 u = 0;

       bbool sendToCompression = btrue;
       u8*  destinationBuffer = 0;

       if (m_bModeEntrelace)
       {
           if ((m_uFrameCountEntrelace&1) == 0)//pair just store;
           {
               ITF_Memcpy(m_StoreFrameBuffer,(void*)pBits,width*height*4);
               sendToCompression = bfalse;//skip one frame

           }
           else
           {
               destinationBuffer = new u8[globalSize];
               u32 endImage0 = (source_height-1)*(pitch/sizeof(unsigned long));
               unsigned long* src0 = (unsigned long*)(m_StoreFrameBuffer)+endImage0;


               for (u32 y = 0; y < source_height; y++)
               {                       
                   if ((y&1) == m_StartEntrelaceLine)
                   {
                       for (u32 x = 0; x<source_width; x++)
                       {
                           destinationBuffer[u] = static_cast<u8>(src[x] & 0xFF);
                           destinationBuffer[u+1] = static_cast<u8>((src[x] >> 8) & 0xFF);
                           destinationBuffer[u+2] =  static_cast<u8>((src[x] >> 16)  & 0xFF);

                           u+=3;
                       }
                   }
                   else
                   {

                       for (u32 x = 0; x<source_width; x++)
                       {
                           destinationBuffer[u] = static_cast<u8>(src0[x] & 0xFF);
                           destinationBuffer[u+1] = static_cast<u8>((src0[x] >> 8) & 0xFF);
                           destinationBuffer[u+2] =  static_cast<u8>((src0[x] >> 16)  & 0xFF);

                           u+=3;
                       }
                   }

                   src0 -= pitch / sizeof(unsigned long);
                   src  -= pitch / sizeof(unsigned long);
               }

           }

       }
       else
       {
           destinationBuffer = new u8[globalSize];

           {
               for (u32 y = 0; y < source_height; y++)
               {
                   for (u32 x = 0; x<source_width; x++)
                   {
                       destinationBuffer[u] = static_cast<u8>(src[x] & 0xFF);
                       destinationBuffer[u+1] = static_cast<u8>((src[x] >> 8) & 0xFF);
                       destinationBuffer[u+2] =  static_cast<u8>((src[x] >> 16)  & 0xFF);

                       u+=3;
                   }


                   src -= pitch / sizeof(unsigned long);
               }


           }
       }

       m_uFrameCountEntrelace++;

       FrameCaptureInfo info;
       info.m_quality                   = m_quality;
       info.m_uFrameCount               = m_uScreenNum;
       info.m_bufferSize                = globalSize;
       info.m_pBuffer                   = destinationBuffer;
       info.m_width                     = width;
       info.m_height                    = height;
       info.m_bLosslessCompression      = m_bLossless;
       info.m_bBMPOutput                = m_bBitmapOutput;
       m_uScreenNum++;

       if (m_bStepByStep)
       {
           info.compress(m_ScreenCaptureDirectory,NULL);
       }
       else
       {
           if (sendToCompression)
           {
               {
                   csAutoLock cs(&m_csScreenCapture[m_CurrentCoreUsage]);
                   m_pendingFrameInfo[m_CurrentCoreUsage].push_back(info);
               }

               m_CurrentCoreUsage++;
               if(m_CurrentCoreUsage >= m_uMaxThreadUsed)
                   m_CurrentCoreUsage = 0;
           }
       }
    

        
    }
}
//----------------------------------------------------------------------------------------------------------------------------------------------------------

void    VideoCapture::start(u32 _width,u32 _height)
{
    if (m_bEnableCapture)
        return;

    m_CaptureFrameCount     = 1;//because the resolve pre increment the frame count

    m_uScreenNum            = 0;
    m_CurrentCoreUsage      = 0;
    m_bEnableCapture        = btrue;
    m_idCurrentArray        = 0;
    m_captureBuffer         = NULL;

    m_StartEntrelaceLine     = 0;
    m_uFrameCountEntrelace   = 0;
    m_StoreFrameBuffer       = 0;
    m_bExitCapture           = bfalse;
    m_bLossless              = bfalse;
    m_bModeEntrelace         = bfalse;
    m_bBitmapOutput          = m_bStepByStep ? btrue : bfalse;
    m_CompressLibEnable      = bfalse;
    m_bModeOddFrame          = bfalse;

    m_LowResCapture.m_currentFrameArray      = 0;
    m_LowResCapture.m_incFrameMemory         = 0;

    if (m_bLowResolution)
        m_LowResCapture.m_vImageMemory.resize(m_LowResCapture.m_uMaxFrameToCapture);

    u32 percentCapture = 90;

    PugiXMLWrap XML;
    u32 FlushThresholdMB = 0;
    if (XML.openFile("screencapture.xml", bfalse, PugiXMLWrap::LogWarning))
    {
        m_quality = XML.readIntAttribute("Info/Quality", 100, PugiXMLWrap::LogWarning);
        m_ScreenCaptureDirectory  = XML.readAttribute("Info/Directory", "", PugiXMLWrap::LogWarning);
        percentCapture = XML.readIntAttribute("Info/PercentCapture", 90, PugiXMLWrap::LogWarning);
        FlushThresholdMB = XML.readIntAttribute("Info/FlushThresholdMB", 300, PugiXMLWrap::LogWarning);
        m_bModeEntrelace = XML.readIntAttribute("Info/Entrelace", 0, PugiXMLWrap::LogWarning);
        m_StartEntrelaceLine = XML.readIntAttribute("Info/StartEntrelaceLine", 0, PugiXMLWrap::LogWarning);
        m_bLossless = XML.readIntAttribute("Info/Lossless", 0, PugiXMLWrap::LogWarning);
        m_bBitmapOutput = m_bBitmapOutput | XML.readIntAttribute("Info/BitmapOutput", 0, PugiXMLWrap::LogWarning);
        m_bModeOddFrame = XML.readIntAttribute("Info/modeOddFrame", 0, PugiXMLWrap::LogWarning);//only odd frame are captured
    }
    else
    {
        FlushThresholdMB = 300;
        m_quality = 100;
        m_ScreenCaptureDirectory = "d:\\ITF_SCREEN\\session";
    }

    if (m_bLowResolution)
    {    
        m_quality = 75;
        if (m_bSaveInMemory)
        {
            m_ScreenCaptureDirectory.setTextFormat("%lsITF_SCREEN/session",FILESERVER->getWorkingDepot().cStr());        
        }
    }
   
    

    if (m_bLossless && m_bBitmapOutput!=0 && !m_CompressLibEnable)
    {
        lzo_init();
        m_CompressLibEnable = btrue;
    }

    if (m_bModeOddFrame)
    {
        m_bModeEntrelace  = bfalse;
    }

    m_FlushThresholdBytes = FlushThresholdMB * 1024*1024L;
    MEMORYSTATUSEX statex;
    statex.dwLength = sizeof (statex);
    GlobalMemoryStatusEx (&statex);
    m_MemoryAvailableAtStartCapture = statex.ullAvailVirtual ;
    m_MemoryThreshold = (m_MemoryAvailableAtStartCapture*percentCapture)/100; 

    String8 directory;

    u32 count = 0;
    bbool bContinue = btrue;
    while (bContinue)
    {
        directory.setTextFormat("%s%03d",m_ScreenCaptureDirectory.cStr(),count);

        if (FILEMANAGER->fileExists(directory))
        {
            ++count;
        }
        else
        {
            Directory::create(directory);
            Directory::create(directory+"\\sound");
            bContinue = bfalse;
        }
    }

    m_ScreenCaptureDirectory = directory;

    m_backbufferWidth  = _width;
    m_backbufferHeight = _height;

    if (m_bModeEntrelace)
    {
        m_StoreFrameBuffer = new u32[_height*_width];
    }

    u32 dirIdx = 0;
    u32 bufferForCompressionSize = _height*_width*3;//RGB

    ITF_ASSERT(m_uMaxThreadUsed <= eThreadId_VideoCapture4 - eThreadId_VideoCapture1);

    for (u32 i= 0;i<m_uMaxThreadUsed;++i)
    {
        ThreadCaptureContainer* threadContainer = new ThreadCaptureContainer();
        threadContainer->m_pVideoCapture = this;
        threadContainer->dirIdx = dirIdx;
        threadContainer->memBufferTarget = new u8[bufferForCompressionSize];
        m_ThreadContainers.push_back(threadContainer);

        dirIdx = (++dirIdx)%2;
        THREADMANAGER->createThread(ConvertToJPG, threadContainer, ThreadSettings::m_settings[eThreadId_VideoCapture1 + i]);
    }
    
    //AUDIO
    if (m_AudioCapture)
        m_AudioCapture->start(directory+"\\sound\\stream.wav");


}
//----------------------------------------------------------------------------------------------------------------------------------------------------------

static  bool   compareImageMemory(VideoCapture::ImageMemory& _p1, VideoCapture::ImageMemory& _p2) { return _p1.frame < _p2.frame; }

void    VideoCapture::stop()
{
    if (!m_bEnableCapture)
        return;


    //AUDIO STOP
    if (m_AudioCapture)
        m_AudioCapture->stop();


    SF_DEL_ARRAY(m_StoreFrameBuffer);

    m_bEnableCapture = bfalse;
    m_bExitCapture = btrue;
    
    //Make everything thread exited
    Sleep(2000);

    u8* pBufferCompression = new u8[m_backbufferWidth*m_backbufferHeight*sizeof(DWORD)];

    //Make sure all pending are finish
    for (u32 indexThread=0;indexThread<m_uMaxThreadUsed;++indexThread)
    {
        ITF_VECTOR<FrameCaptureInfo>& arrayFrameCapture = m_pendingFrameInfo[indexThread];
        const u32 sizeArray = arrayFrameCapture.size();
        for (u32 Frameid = 0;Frameid<sizeArray;++Frameid )
        {
            FrameCaptureInfo& captureInfo = arrayFrameCapture[Frameid];
            captureInfo.compress(m_ScreenCaptureDirectory,pBufferCompression);
        }

        arrayFrameCapture.clear();
    }

    delete[] pBufferCompression;

    //we captured something in memory
    if (m_LowResCapture.m_incFrameMemory!=0)
    {
        u32 maxFrameCaptured = std::min(m_LowResCapture.m_uMaxFrameToCapture,m_LowResCapture.m_incFrameMemory);

        //copy it
        ITF_VECTOR<ImageMemory>  sortedvImageMemory;
        sortedvImageMemory.resize(maxFrameCaptured);
        for (u32 index = 0;index<maxFrameCaptured;index++)
        {
            sortedvImageMemory[index] = m_LowResCapture.m_vImageMemory[index];
        }


        std::sort(sortedvImageMemory.begin(), sortedvImageMemory.end(), compareImageMemory);

        //now write this sequence
        
        String filename;
        for (u32 index = 0;index<maxFrameCaptured;index++)
        {
            ImageMemory& imgMem = sortedvImageMemory[index];
            filename.setTextFormat("%s\\screenITF%08d.%jpg",m_ScreenCaptureDirectory.cStr(),index);
            File* writeFile = FILEMANAGER->openFile(filename,ITF_FILE_ATTR_WRITE|ITF_FILE_ATTR_CREATE_NEW);

            if (writeFile)
            {
                writeFile->write((void*) imgMem.pBuffer,imgMem.size);
                FILEMANAGER->closeFile(writeFile);
            }
        }
    }



    for (ITF_VECTOR<ImageMemory>::iterator iter = m_LowResCapture.m_vImageMemory.begin();iter!= m_LowResCapture.m_vImageMemory.end();++iter)
    {
        ImageMemory& imageMemory = (*iter);
        SF_DEL_ARRAY(imageMemory.pBuffer);
        imageMemory.clear();
    }

    for (u32 index = 0;index<m_ThreadContainers.size();index++)
    {
       SF_DEL(m_ThreadContainers[index]);
    }

    m_ThreadContainers.clear();

    if (m_captureBuffer)
        m_captureBuffer->release();

    SF_DEL(m_captureBuffer);
}
//----------------------------------------------------------------------------------------------------------------------------------------------------------

void VideoCapture::getFileNameCapture(String& _filename)
{
   _filename.setTextFormat("%s\\screenITF%08d.jpg",m_ScreenCaptureDirectory.cStr(),0);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------

void VideoCapture::onSwitchToWorld(World* _pWorld, bbool _bSwitchDone)
{
    if (!m_bEnableCapture && !_bSwitchDone)
        return;

    start( GFX_ADAPTER->getScreenWidth(),GFX_ADAPTER->getScreenHeight());
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------

void    VideoCapture::cancel()
{
    if (!m_bEnableCapture)
        return;
    
    stop();

    ITF_VECTOR<String> vFiles;

    String dir = m_ScreenCaptureDirectory+"\\";
    Directory::searchFiles(vFiles,dir,"*.*",btrue);
    
    for (ITF_VECTOR<ITF::String>::const_iterator iterFiles = vFiles.begin();iterFiles!=vFiles.end();++iterFiles)
    {
        FILEMANAGER->deleteFile(*iterFiles);
    }

    RemoveDirectoryA(m_ScreenCaptureDirectory.cStr());
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------

}

#endif  //ITF_SUPPORT_VIDEOCAPTURE
