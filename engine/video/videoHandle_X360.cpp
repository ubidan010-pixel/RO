#include "precompiled_engine.h"

#include "engine/video/videoHandle.h"

#ifndef _ITF_DIRECTX9_DRAWPRIM_H_
#include "adapters/GFXAdapter_Directx9/GFXAdapter_Directx9.h"
#endif //_ITF_DIRECTX9_DRAWPRIM_H_

#ifndef _ITF_SOUNDADAPTER_H_XAUDIO_
#include "adapters/SoundAdapter_XAudio/SoundAdapter_XAudio.h"
#endif //_ITF_SOUNDADAPTER_H_XAUDIO_

#ifndef _ITF_RESOURCEMANAGER_H_
#include "engine/resources/ResourceManager.h"
#endif //_ITF_RESOURCEMANAGER_H_


namespace  ITF
{

//------------------------------------------------------------------------------------------------------------------------------------------------------

void videoHandle::update()
{

}
//------------------------------------------------------------------------------------------------------------------------------------------------------

void videoHandle::close()
{
    if (m_pVertexBuffer)
        GFX_ADAPTER->removeVertexBufferDelay (m_pVertexBuffer,1);

    m_pVertexBuffer = NULL;

    if (m_xmvPlayer)
        m_xmvPlayer->Release();

    m_xmvPlayer = NULL;
    m_context.close();

    Resource::forceDeletion(textureIDY[0].getResource());
    Resource::forceDeletion(textureIDY[1].getResource());
    Resource::forceDeletion(textureIDU[0].getResource());
    Resource::forceDeletion(textureIDU[1].getResource());
    Resource::forceDeletion(textureIDV[0].getResource());
    Resource::forceDeletion(textureIDV[1].getResource());

    SF_DEL_ARRAY(m_pMemory);
}

//------------------------------------------------------------------------------------------------------------------------------------------------------

void videoHandle::stop()
{
    videoHandleBase::stop();
}

//------------------------------------------------------------------------------------------------------------------------------------------------------

void videoHandle::render()
{
    if (m_stopped)
        return;

    HRESULT hr;
    LOCKED_TEXTURE lockRectY;
    LOCKED_TEXTURE lockRectU;
    LOCKED_TEXTURE lockRectV;

   
    Texture* pTextureY = (Texture*) textureIDY[m_dwTextureID].getResource();
    GFX_ADAPTER->lockTexture(pTextureY,&lockRectY);


    Texture* pTextureU = (Texture*) textureIDU[m_dwTextureID].getResource();
    GFX_ADAPTER->lockTexture(pTextureU,&lockRectU);

    Texture* pTextureV = (Texture*) textureIDV[m_dwTextureID].getResource();
    GFX_ADAPTER->lockTexture(pTextureV,&lockRectV);

    DWORD dwLuminanceSize   = m_VideoDescriptor.dwHeight * lockRectY.m_pitch;
    DWORD dwChrominanceSize = m_VideoDescriptor.dwHeight / 2 * lockRectU.m_pitch;

    XMEDIA_VIDEO_FRAME videoFrame;
    ZeroMemory( &videoFrame, sizeof( XMEDIA_VIDEO_FRAME ) );
    videoFrame.videoFormat = XMEDIA_VIDEO_FORMAT_I420;
    videoFrame.dwFlags = 0;
    videoFrame.lTimeToPresent = NULL;
    videoFrame.i420.pvYBuffer = lockRectY.mp_Bits;
    videoFrame.i420.pvUBuffer = lockRectU.mp_Bits;
    videoFrame.i420.pvVBuffer = lockRectV.mp_Bits;
    videoFrame.i420.dwYBufferSize = dwLuminanceSize;
    videoFrame.i420.dwUBufferSize = dwChrominanceSize;
    videoFrame.i420.dwVBufferSize = dwChrominanceSize;
    videoFrame.i420.dwYPitch = lockRectY.m_pitch;
    videoFrame.i420.dwUPitch = lockRectU.m_pitch;
    videoFrame.i420.dwVPitch = lockRectV.m_pitch;

    if( FAILED( hr = m_xmvPlayer->GetNextFrame( &videoFrame ) ) )
    {
        GFX_ADAPTER->unlockTexture(pTextureY);
        GFX_ADAPTER->unlockTexture(pTextureU);
        GFX_ADAPTER->unlockTexture(pTextureV);
        return ;
    }

    if (hr == XMEDIA_W_EOF )
    {
        if (!isLoop())
            stop();
    }

    GFX_ADAPTER->unlockTexture(pTextureY);
    GFX_ADAPTER->unlockTexture(pTextureU);
    GFX_ADAPTER->unlockTexture(pTextureV);

    if (m_stopped)
        return;

    Texture* array[3];
    array[0] = pTextureY;
    array[1] = pTextureU;
    array[2] = pTextureV;

    GFX_ADAPTER->drawMovie(NULL,getAlpha(),m_pVertexBuffer,array,3);

    //swap the texture id
    m_dwTextureID =!m_dwTextureID;

/*
#ifndef ITF_FINAL
    f32 _timeCurrent;
    getCurrentTime(_timeCurrent);
    LOG(">>VideoHandle  current time:%0.02f seconds ,total time:%0.02f seconds",_timeCurrent,m_totalTimeInSeconds);
#endif //ITF_FINAL
*/
}
//------------------------------------------------------------------------------------------------------------------------------------------------------

bbool videoHandle::getCurrentTime(f32& _timeInSeconds)
{
    if (m_xmvPlayer)
    {
        XMEDIA_PLAYBACK_STATUS status;
        m_xmvPlayer->GetStatus(&status);
        f32 time = (f32)(status.lDuration) *0.001f;
        _timeInSeconds = time;
        return btrue;
    }

    return bfalse;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------

void videoHandle::videoContext::close()
{
    if (m_pFile)
    {
        FILEMANAGER->closeFile(m_pFile);
        Synchronize::destroyCriticalSection(&m_cs);
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------------------

void videoHandle::videoContext::set(File* _pFile) 
{
    if (_pFile)
    {
        m_pFile = _pFile;
        Synchronize::createCriticalSection(&m_cs);
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------------------

HRESULT videoaudioStreamCallback(PVOID pvContext,  ULONGLONG ullOffset,   PVOID pvBuffer,    DWORD dwBytesToRead,    DWORD *pdwBytesRead)
{
    videoHandle::videoContext* pVideoContext = (videoHandle::videoContext*)(pvContext);

    File  *pFile = pVideoContext->getFile();
    if (pFile)
    {
        csAutoLock cs( pVideoContext->m_cs);
        pFile->seek(ullOffset,0);
        pFile->read(pvBuffer,dwBytesToRead,(u32*) pdwBytesRead);
    }
      
   
    return S_OK;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------

HRESULT audioStreamCallback(PVOID pvContext,  ULONGLONG ullOffset,   PVOID pvBuffer,    DWORD dwBytesToRead,    DWORD *pdwBytesRead)
{
  return videoaudioStreamCallback(pvContext,ullOffset, pvBuffer,dwBytesToRead,pdwBytesRead);
}

//------------------------------------------------------------------------------------------------------------------------------------------------------

HRESULT videoStreamCallback(PVOID pvContext,  ULONGLONG ullOffset,   PVOID pvBuffer,    DWORD dwBytesToRead,    DWORD *pdwBytesRead)
{
    return videoaudioStreamCallback(pvContext,ullOffset, pvBuffer,dwBytesToRead,pdwBytesRead);
}

//------------------------------------------------------------------------------------------------------------------------------------------------------

videoHandle::~videoHandle()
{
}


bbool videoHandle::open(const String& _filename)
{
    XMEDIA_XMV_CREATE_PARAMETERS XmvParams;

    ZeroMemory( &XmvParams, sizeof( XmvParams ) );

    // Use the default audio and video streams.
    // If using a wmv file with multiple audio or video streams
    // (such as different audio streams for different languages)
    // the dwAudioStreamId & dwVideoStreamId parameters can be used 
    // to select which audio (or video) stream will be played back

    GFXAdapter_Directx9* pAdapterDirectX    = static_cast<GFXAdapter_Directx9*> (GFX_ADAPTER);
    SoundAdapter_XAudio* pAdapterXAudio     = static_cast<SoundAdapter_XAudio*> (SINGLETONS.getSoundAdapter()); 
    IDirect3DDevice9* pDevice = pAdapterDirectX->getRenderingDevice();


    XMEDIA_CREATE_TYPE  modeReadFile = FILEMANAGER->isRemote() ? XMEDIA_CREATE_FROM_USER_IO : XMEDIA_CREATE_FROM_FILE;

    XmvParams.createType = isPlayedFromMemory() ? XMEDIA_CREATE_FROM_MEMORY : modeReadFile;
    XmvParams.dwAudioStreamId = XMEDIA_STREAM_ID_USE_DEFAULT;
    XmvParams.dwVideoStreamId = XMEDIA_STREAM_ID_USE_DEFAULT;
    XmvParams.dwVideoDecoderCpu = 4;
    XmvParams.dwAudioDecoderCpu = 5;
    XmvParams.dwFlags  =  XMEDIA_CREATE_CPU_AFFINITY|XMEDIA_CREATE_BACKGROUND_MUSIC;    // Mute the sound of the video in a custom music is playing

    ITF_MemoryBarrier();

    HRESULT hr = 0;
    String fullnameFromFile;
    StringConverter name;
    if (XmvParams.createType == XMEDIA_CREATE_FROM_FILE)
    {
        fullnameFromFile.setTextFormat("d:\\%ls.wmv",_filename.cStr());
        name.setData(fullnameFromFile);
        XmvParams.createFromFile.szFileName = name.getChar();
    }
    else
    {    
        String fullname = _filename+".wmv";

        File * pFile = FILEMANAGER->openFile(fullname,ITF_FILE_ATTR_READ);
        if (pFile)
        {
            if (isPlayedFromMemory())
            {
                m_fileSize = (u32)pFile->getLength();
                m_pMemory = newAlloc(mId_Temporary,u8[m_fileSize]);
                if (m_pMemory == NULL)
                {
                    ITF_ASSERT_MSG(0,"no enough memory to play the movie");
                    return bfalse;
                }

                u32 sizeRead = 0;
                pFile->read(m_pMemory,m_fileSize,&sizeRead);
                FILEMANAGER->closeFile(pFile);

                XmvParams.createFromMemory.pvBuffer      = m_pMemory;
                XmvParams.createFromMemory.dwBufferSize  = m_fileSize;
            }
            else
            {

                XmvParams.createFromUserIo.pfnAudioStreamReadCallback = audioStreamCallback;
                XmvParams.createFromUserIo.pfnVideoStreamReadCallback = videoStreamCallback;
                m_context.set(pFile);
            }
        }
        else
            return bfalse;

        XmvParams.createFromUserIo.pvAudioStreamContext = &m_context;
        XmvParams.createFromUserIo.pvVideoStreamContext = &m_context;

        
    }

    hr = XMedia2CreateXmvPlayer( pDevice, pAdapterXAudio->getXAudio2(), &XmvParams, &m_xmvPlayer );

    if( SUCCEEDED( hr ) )
    {
        m_xmvPlayer->GetVideoDescriptor(&m_VideoDescriptor);

        const int width = m_VideoDescriptor.dwWidth;
        const int height = m_VideoDescriptor.dwHeight;
              
        //create texture..

        //The final rendered image is a blend of these components. The Y texture is at the full resolution of the rendered image
        textureIDY[0] = RESOURCE_MANAGER->newEmptyResource(Resource::ResourceType_Texture)->getID();
        GFX_ADAPTER->createTexture((Texture*)textureIDY[0].getResource(),width,height,1, Texture::PF_L8);

        textureIDY[1] = RESOURCE_MANAGER->newEmptyResource(Resource::ResourceType_Texture)->getID();
        GFX_ADAPTER->createTexture((Texture*)textureIDY[1].getResource(),width,height,1, Texture::PF_L8);

        // The U and V textures use subsamples and are at a quarter of the size of the rendered image—half width and half height
        textureIDU[0] = RESOURCE_MANAGER->newEmptyResource(Resource::ResourceType_Texture)->getID();
        GFX_ADAPTER->createTexture((Texture*)textureIDU[0].getResource(),width>>1,height>>1,1, Texture::PF_L8);

        textureIDU[1] = RESOURCE_MANAGER->newEmptyResource(Resource::ResourceType_Texture)->getID();
        GFX_ADAPTER->createTexture((Texture*)textureIDU[1].getResource(),width>>1,height>>1,1, Texture::PF_L8);

        textureIDV[0] = RESOURCE_MANAGER->newEmptyResource(Resource::ResourceType_Texture)->getID();
        GFX_ADAPTER->createTexture((Texture*)textureIDV[0].getResource(),width>>1,height>>1,1, Texture::PF_L8);

        textureIDV[1] = RESOURCE_MANAGER->newEmptyResource(Resource::ResourceType_Texture)->getID();
        GFX_ADAPTER->createTexture((Texture*)textureIDV[1].getResource(),width>>1,height>>1,1, Texture::PF_L8);

        //create vertex buffer

        m_pVertexBuffer = GFX_ADAPTER->createVertexBuffer(4,VertexFormat_PT, sizeof(VertexPT), bfalse,VB_T_INTERNAL);

        const int hWidth = GFX_ADAPTER->getScreenWidth() / 2;
        const int hHeight = GFX_ADAPTER->getScreenHeight()  / 2;

        // Scale the output width.
        float left      = -hWidth * 1.0f;
        float right     = hWidth * 1.0f;
        float top       = -hHeight * 1.0f;
        float bottom    = hHeight * 1.0f;

        VertexPT* pVertices;
        m_pVertexBuffer->Lock((void **)&pVertices);

        pVertices[ 0 ].m_pos.set( hWidth + ( left  ),hHeight + ( top   ),0.0f);
        pVertices[ 0 ].m_uv.set(.0f,.0f);

        pVertices[ 1 ].m_pos.set(hWidth + ( right   ), hHeight + ( top ),0.0f);
        pVertices[ 1 ].m_uv.set(1.0f,.0f);

        pVertices[ 2 ].m_pos.set(hWidth + ( right   ), hHeight + ( bottom ),0.0f);
        pVertices[ 2 ].m_uv.set(1.0f,1.0f);

        pVertices[ 3 ].m_pos.set(hWidth + ( left   ), hHeight + ( bottom ),0.0f);
        pVertices[ 3 ].m_uv.set(.0f,1.0f);

        m_pVertexBuffer->Unlock();

        m_totalTimeInSeconds =(f32) (m_VideoDescriptor.dwClipDuration) *0.001f;

        m_opened  = btrue;
        m_stopped = bfalse;
        m_isPaused = bfalse;
    }



    return btrue;
}

void videoHandle::pause()
{
    if(m_isPaused) return;

    if (m_xmvPlayer)
    {
        m_xmvPlayer->Pause();
        m_isPaused = btrue;
    }

}

//------------------------------------------------------------------------------------------------------------------------------------------------------
}

