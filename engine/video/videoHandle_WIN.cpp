#include "precompiled_engine.h"

#include "engine/video/videoHandle.h"

#ifdef ITF_SUPPORT_BINK

#ifndef _ITF_DIRECTX9_DRAWPRIM_H_
#include "adapters/GFXAdapter_Directx9/GFXAdapter_Directx9.h"
#endif //_ITF_DIRECTX9_DRAWPRIM_H_

#ifndef _ITF_SOUNDADAPTER_H_XAUDIO_
#include "adapters/SoundAdapter_XAudio/SoundAdapter_XAudio.h"
#endif //_ITF_SOUNDADAPTER_H_XAUDIO_

#ifndef _ITF_RESOURCEMANAGER_H_
#include "engine/resources/ResourceManager.h"
#endif //_ITF_RESOURCEMANAGER_H_

#ifndef BINKTEXTURESH
#include "engine/video/binktextures.h"
#endif //BINKTEXTURESH

namespace  ITF
{
    static void Unlock_Bink_textures( LPDIRECT3DDEVICE9 d3d_device, BINKTEXTURESET * set_textures, HBINK Bink, videoHandle* _pHandle );
    
    static BINKTEXTURESET texture_set = { 0 };

    static void Decompress_frame( HBINK bink, BINKTEXTURESET * set_textures, videoHandle* _pHandle );

    void Lock_Bink_textures( BINKTEXTURESET* set_textures, S32 lock_both_for_writing, videoHandle* _pHandle );

//------------------------------------------------------------------------------------------------------------------------------------------------------

void videoHandle::update()
{

}

//------------------------------------------------------------------------------------------------------------------------------------------------------

void videoHandle::stop()
{
    videoHandleBase::stop();
}

//------------------------------------------------------------------------------------------------------------------------------------------------------

void videoHandle::pause()
{
    m_isPaused = btrue;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------

void Draw_Bink_textures(BINKTEXTURESET* set_textures,
                        u32 width,
                        u32 height,
                        f32 x_offset,
                        f32 y_offset,
                        f32 x_scale,
                        f32 y_scale,
                        f32 alpha_level,
                        i32 is_premultiplied_alpha,
                        videoHandle* _pHandle
                        )
{
    GFX_ADAPTER->setCullMode(GFX_CULL_NONE);
    GFX_ADAPTER->depthMask(0);
    GFX_ADAPTER->depthTest(0);

    /// are we completely opaque or somewhat transparent?.
    if ( alpha_level >= 0.999f )
    {
        GFX_ADAPTER->setAlphaBlend(GFX_BLEND_COPY);
    }
    else
    {
        if (is_premultiplied_alpha)
            GFX_ADAPTER->setAlphaBlend(GFX_BLEND_ALPHAPREMULT);
        else
            GFX_ADAPTER->setAlphaBlend(GFX_BLEND_ALPHA);
    }

    Texture* parray[4];
    parray[0] = (Texture*) _pHandle->YtextureDraw.getResource();
    parray[1] = (Texture*) _pHandle->cRtextureDraw.getResource();
    parray[2] = (Texture*) _pHandle->cBtextureDraw.getResource();
    parray[3] = (Texture*) _pHandle->AtextureDraw.getResource();

    u32 nbTex = 3;
    if (parray[3]->m_adapterimplementationData)
        nbTex++;

    GFX_ADAPTER->drawMovie(NULL, alpha_level, _pHandle->m_pVertexBuffer, parray, nbTex);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------

void videoHandle::render()
{
    if (m_stopped)
        return;

    BINKSUMMARY sum;
    BinkGetSummary(m_Bink,&sum);

    if ( !BinkWait( m_Bink ) )
    {
        Decompress_frame( m_Bink, &texture_set, this );
    }

    Draw_Bink_textures( &texture_set,
        GFX_ADAPTER->getScreenWidth(), GFX_ADAPTER->getScreenHeight(),
        0, 0,
        1.0f, 1.0f,
        m_alpha, 0, this );

/*
#ifndef ITF_FINAL
    f32 _timeCurrent;
    getCurrentTime(_timeCurrent);
    LOG(">>VideoHandle  current time:%0.02f seconds ,total time:%0.02f seconds",_timeCurrent,m_totalTimeInSeconds);
#endif //ITF_FINAL
*/

    if (sum.TotalPlayedFrames >= sum.TotalFrames && m_loop == bfalse)
    {
        m_stopped = true;
    }

}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------

S32 create_Bink_VideoMemTextures(BINKTEXTURESET * set_textures, videoHandle* _hand)
{
    /// Create our output draw texture (this should be in video card memory).
    BINKFRAMEBUFFERS* bb;

    bb = &set_textures->bink_buffers;

    /// Create Y plane.
    if ( bb->Frames[ 0 ].YPlane.Allocate )
    {
        GFX_ADAPTER->createTexture((Texture*)_hand->YtextureDraw.getResource(), bb->YABufferWidth, bb->YABufferHeight, 1, Texture::PF_A8, 0, btrue);
        //goto fail;
    }

    /// Create cR plane.
    if ( bb->Frames[ 0 ].cRPlane.Allocate )
    {
        GFX_ADAPTER->createTexture((Texture*)_hand->cRtextureDraw.getResource(), bb->cRcBBufferWidth, bb->cRcBBufferHeight, 1, Texture::PF_A8, 0, btrue);
        //goto fail;
    }

    /// Create cB plane.
    if ( bb->Frames[ 0 ].cBPlane.Allocate )
    {
        GFX_ADAPTER->createTexture((Texture*)_hand->cBtextureDraw.getResource(), bb->cRcBBufferWidth, bb->cRcBBufferHeight, 1, Texture::PF_A8, 0, btrue);
        //goto fail;
    }

    /// Create alpha plane.
    if ( bb->Frames[ 0 ].APlane.Allocate )
    {
        GFX_ADAPTER->createTexture((Texture*)_hand->AtextureDraw.getResource(), bb->YABufferWidth, bb->YABufferHeight, 1, Texture::PF_A8, 0, btrue);
             //goto fail;
    }

    return( 1 );
/*fail:

    return( 0 );*/
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------

void destroy_Bink_VideoMemTextures(BINKTEXTURESET* set_textures, videoHandle* _hand)
{
    Texture* pYtexture = (Texture*) _hand->YtextureDraw.getResource();
    if (pYtexture)
        GFX_ADAPTER->cleanupTexture(pYtexture);
 
    Texture* pcRtexture = (Texture*) _hand->cRtextureDraw.getResource();
    if (pcRtexture)
        GFX_ADAPTER->cleanupTexture(pcRtexture);

    Texture* pcBtexture = (Texture*) _hand->cBtextureDraw.getResource();
    if (pcBtexture)
        GFX_ADAPTER->cleanupTexture(pcBtexture);

    Texture* pAtexture = (Texture*) _hand->AtextureDraw.getResource();
    if (pAtexture)
        GFX_ADAPTER->cleanupTexture(pAtexture);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------

void Free_Bink_textures(BINKTEXTURESET *set_textures, videoHandle* _hand)
{
    for ( i32 i = 0; i < 2; ++i )
    {
        Texture* pYtexture = (Texture*) _hand->Ytexture[i].getResource();
        if (pYtexture)
            GFX_ADAPTER->cleanupTexture(pYtexture);
        Texture* pcRtexture = (Texture*) _hand->cRtexture[i].getResource();
        if (pcRtexture)
            GFX_ADAPTER->cleanupTexture(pcRtexture);

        Texture* pcBtexture = (Texture*) _hand->cBtexture[i].getResource();
        if (pcBtexture)
            GFX_ADAPTER->cleanupTexture(pcBtexture);

        Texture* pAtexture = (Texture*) _hand->Atexture[i].getResource();
        if (pAtexture)
            GFX_ADAPTER->cleanupTexture(pAtexture);
    }

    destroy_Bink_VideoMemTextures(set_textures, _hand);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------

i32 Create_Bink_textures( BINKTEXTURESET* set_textures, videoHandle* _hand)
{
    BINKFRAMEBUFFERS* bb;
    /// Create our system decompress textures (in system memory).

    bb = &set_textures->bink_buffers;

    for ( i32 i = 0; i < 2; ++i )
    {
        // Create Y plane.
        if ( bb->Frames[ i ].YPlane.Allocate )
        {
            GFX_ADAPTER->createTexture((Texture*)_hand->Ytexture[i].getResource(), bb->YABufferWidth, bb->YABufferHeight, 1, Texture::PF_A8, 2, bfalse);
            //goto fail;
        }

        // Create cR plane.
        if ( bb->Frames[ i ].cRPlane.Allocate )
        {
            GFX_ADAPTER->createTexture((Texture*)_hand->cRtexture[i].getResource(), bb->cRcBBufferWidth, bb->cRcBBufferHeight, 1, Texture::PF_A8, 2, bfalse);
            //goto fail;
        }

        // Create cB plane
        if ( bb->Frames[ i ].cBPlane.Allocate )
        {
            GFX_ADAPTER->createTexture((Texture*)_hand->cBtexture[i].getResource(), bb->cRcBBufferWidth, bb->cRcBBufferHeight, 1, Texture::PF_A8, 2, bfalse);
            //goto fail;
        }

        // Create alpha plane.
        if ( bb->Frames[ i ].APlane.Allocate )
        {
            GFX_ADAPTER->createTexture((Texture*)_hand->Atexture[i].getResource(), bb->YABufferWidth, bb->YABufferHeight, 1, Texture::PF_A8, 2, bfalse);
            //goto fail;
        }

#undef POOL_TYPE
    }

    if ( !create_Bink_VideoMemTextures(set_textures, _hand))
        goto fail;

    return( 1 );

fail:

    Free_Bink_textures( set_textures, _hand );
    return( 0 );
}

//------------------------------------------------------------------------------------------------------------------------------------------------------

static void Decompress_frame( HBINK bink, BINKTEXTURESET * set_textures, videoHandle* _pHandle )
{
    /// Lock the textures.
    Lock_Bink_textures(set_textures, 0, _pHandle );

    /// Register our locked texture pointers with Bink.
    BinkRegisterFrameBuffers( bink, &set_textures->bink_buffers );

    /// Decompress a frame.
    BinkDoFrame( bink );

    /// if we are falling behind, decompress an extra frame to catch up.
    while ( BinkShouldSkip( bink ) )
    {
        BinkNextFrame( bink );
        BinkDoFrame( bink );
    }

    /// Unlock the textures.
    GFXAdapter_Directx9* pAdapterDirectX    = static_cast<GFXAdapter_Directx9*> (GFX_ADAPTER);
    IDirect3DDevice9* pDevice = pAdapterDirectX->getRenderingDevice();

    Unlock_Bink_textures( pDevice, set_textures, bink, _pHandle );

    /// Keep playing the movie.
    BinkNextFrame( bink );
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------

void Lock_Bink_textures( BINKTEXTURESET * set_textures, S32 lock_both_for_writing, videoHandle* _pHandle )
{
    BINKFRAMEPLANESET* bp;
    LOCKED_TEXTURE lockRect;

    bp = set_textures->bink_buffers.Frames;

    i32 frame_cur = set_textures->bink_buffers.FrameNum;

    /// Lock the frame textures.
    for ( i32 i = 0; i < 2; ++i, ++bp )
    {
        u32 lock_flags = ( ( lock_both_for_writing == 0 ) & ( i == frame_cur ) ) ? D3DLOCK_NO_DIRTY_UPDATE : D3DLOCK_READONLY;

        Texture* pTextureY = (Texture*) _pHandle->Ytexture[i].getResource();
        GFX_ADAPTER->lockTexture(pTextureY, &lockRect, lock_flags);
        bp->YPlane.Buffer = lockRect.mp_Bits;
        bp->YPlane.BufferPitch = lockRect.m_pitch;

        Texture* pRtexture = (Texture*) _pHandle->cRtexture[i].getResource();
        GFX_ADAPTER->lockTexture(pRtexture, &lockRect, lock_flags);
        bp->cRPlane.Buffer = lockRect.mp_Bits;
        bp->cRPlane.BufferPitch = lockRect.m_pitch;

        Texture* pBtexture = (Texture*) _pHandle->cBtexture[i].getResource();
        GFX_ADAPTER->lockTexture(pBtexture, &lockRect, lock_flags);
        bp->cBPlane.Buffer = lockRect.mp_Bits;
        bp->cBPlane.BufferPitch = lockRect.m_pitch;

        /// Lock the alpha texture.
        Texture* pAtexture = (Texture*) _pHandle->Atexture[i].getResource();
        if ( pAtexture->m_adapterimplementationData )
        {
            GFX_ADAPTER->lockTexture(pAtexture, &lockRect, lock_flags);
            bp->APlane.Buffer = lockRect.mp_Bits;
            bp->APlane.BufferPitch = lockRect.m_pitch;
        }
    }
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------

void pause();

void Unlock_Bink_textures( LPDIRECT3DDEVICE9 d3d_device, BINKTEXTURESET * set_textures, HBINK Bink, videoHandle* _pHandle )
{
    BINKFRAMEPLANESET* bp;

    /// Unlock the frame textures.
    bp = set_textures->bink_buffers.Frames;

    for ( i32 i = 0; i < set_textures->bink_buffers.TotalFrames; ++i, /*++bt,*/ ++bp )
    {
        Texture* pTextureY = (Texture*) _pHandle->Ytexture[i].getResource();
        GFX_ADAPTER->unlockTexture(pTextureY);
        bp->YPlane.Buffer = NULL;

        Texture* pTextureR = (Texture*) _pHandle->cRtexture[i].getResource();
        GFX_ADAPTER->unlockTexture(pTextureR);
        bp->cRPlane.Buffer = NULL;

        Texture* pTextureB = (Texture*) _pHandle->cBtexture[i].getResource();
        GFX_ADAPTER->unlockTexture(pTextureB);
        bp->cBPlane.Buffer = NULL;

        /// Unlock the alpha texture.
        Texture* pTextureA = (Texture*) _pHandle->Atexture[i].getResource();
        if ( pTextureA->m_adapterimplementationData )
        {
            GFX_ADAPTER->unlockTexture(pTextureA);
            bp->APlane.Buffer = NULL;
        }
    }

    /// Now we have to update the pixels on the video card texture.
    i32 num_rects;
    BINKFRAMETEXTURES* bt_src;

    bt_src = &set_textures->textures[ set_textures->bink_buffers.FrameNum ];
    i32 frame = set_textures->bink_buffers.FrameNum;

    num_rects = BinkGetRects( Bink, BINKSURFACEFAST );
    if ( num_rects > 0 )
    {
        BINKRECT* brc;
        RECT rc;

        for ( i32 i = 0; i < num_rects; ++i )
        {
            brc = &Bink->FrameRects[ i ];

            rc.left = brc->Left;
            rc.top = brc->Top;
            rc.right = rc.left + brc->Width;
            rc.bottom = rc.top + brc->Height;
            //bt_src->Ytexture->AddDirtyRect( &rc );
            //Texture* pTextureY = (Texture*) _pHandle->Ytexture[i].getResource();

            /*if ( bt_src->Atexture )
            {
                bt_src->Atexture->AddDirtyRect( &rc );
            }*/

            rc.left >>= 1;
            rc.top >>= 1;
            rc.right >>= 1;
            rc.bottom >>= 1;
            //bt_src->cRtexture->AddDirtyRect( &rc );
            //bt_src->cBtexture->AddDirtyRect( &rc );
        }

        Texture* pYtextureScr = (Texture*) _pHandle->Ytexture[frame].getResource();
        Texture* pcRtextureScr = (Texture*) _pHandle->cRtexture[frame].getResource();
        Texture* pcBtextureScr = (Texture*) _pHandle->cBtexture[frame].getResource();

        Texture* pYtextureDst = (Texture*) _pHandle->YtextureDraw.getResource();
        Texture* pcRtextureDst = (Texture*) _pHandle->cRtextureDraw.getResource();
        Texture* pcBtextureDst = (Texture*) _pHandle->cBtextureDraw.getResource();
       
        d3d_device->UpdateTexture( (LPDIRECT3DTEXTURE9)pYtextureScr->m_adapterimplementationData, (LPDIRECT3DTEXTURE9)pYtextureDst->m_adapterimplementationData);
        d3d_device->UpdateTexture( (LPDIRECT3DTEXTURE9)pcRtextureScr->m_adapterimplementationData, (LPDIRECT3DTEXTURE9)pcRtextureDst->m_adapterimplementationData);
        d3d_device->UpdateTexture( (LPDIRECT3DTEXTURE9)pcBtextureScr->m_adapterimplementationData, (LPDIRECT3DTEXTURE9)pcBtextureDst->m_adapterimplementationData);

        Texture* pAtextureScr = (Texture*) _pHandle->Atexture[frame].getResource();
        if ( pAtextureScr->m_adapterimplementationData )
        {
            Texture* pAtextureDst = (Texture*) _pHandle->AtextureDraw.getResource();
            d3d_device->UpdateTexture( (LPDIRECT3DTEXTURE9)pAtextureScr->m_adapterimplementationData, (LPDIRECT3DTEXTURE9)pAtextureDst->m_adapterimplementationData);
        }
    }
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------

void fillVertexBuffer(ITF_VertexBuffer* _vb)
{
    f32 hWidth = (f32)GFX_ADAPTER->getScreenWidth();
    f32 hHeight = (f32)GFX_ADAPTER->getScreenHeight();

    VertexPT* pVertices;
    _vb->Lock((void **)&pVertices);
    
    pVertices[ 0 ].m_pos.set( 0,0,0.0f);
    pVertices[ 0 ].m_uv.set(0.f,0.f);

    pVertices[ 1 ].m_pos.set(hWidth, 0,0.0f);
    pVertices[ 1 ].m_uv.set(1.f,0.f);

    pVertices[ 2 ].m_pos.set(0.f, hHeight,0.0f);
    pVertices[ 2 ].m_uv.set(0.f,1.f);

    pVertices[ 3 ].m_pos.set(hWidth, hHeight,0.0f);
    pVertices[ 3 ].m_uv.set(1.f,1.f);
    
    _vb->Unlock();
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------

void videoHandle::OnResetDevice()
{
    create_Bink_VideoMemTextures( &texture_set , this);

    fillVertexBuffer(m_pVertexBuffer);
}

void videoHandle::OnLostDevice()
{
    destroy_Bink_VideoMemTextures(&texture_set, this);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------

bbool videoHandle::getCurrentTime(f32& _timeInSeconds)
{
    if (m_Bink)
    {
        BINKREALTIME realtime;
        BinkGetRealtime(m_Bink,&realtime,0);
        f32 time = (f32)realtime.FrameNum/m_fps; //switch to seconds
        _timeInSeconds = time;
        return btrue;
    }

    return bfalse;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------

void videoHandle::close()
{
    BinkClose(m_Bink);
    Free_Bink_textures(&texture_set, this);

    if (m_pVertexBuffer)
        GFX_ADAPTER->removeVertexBuffer(m_pVertexBuffer);

    m_pVertexBuffer = NULL;

    for (u32 i=0; i<2; i++)
    {
        Resource::forceDeletion(Ytexture[i].getResource());
        Resource::forceDeletion(cRtexture[i].getResource());
        Resource::forceDeletion(cBtexture[i].getResource());
        Resource::forceDeletion(Atexture[i].getResource());    
    }

    Resource::forceDeletion(YtextureDraw.getResource());
    Resource::forceDeletion(cRtextureDraw.getResource());
    Resource::forceDeletion(cBtextureDraw.getResource());
    Resource::forceDeletion(AtextureDraw.getResource());
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------

bbool videoHandle::open(const String& _filename)
{
    BinkSoundUseDirectSound( 0 );

    String fullName;
    FILEMANAGER->TranslatePath(fullName, _filename);
     
    StringConverter charName(fullName+".bik");

    m_Bink = BinkOpen(charName.getChar(), BINKSNDTRACK | BINKNOFRAMEBUFFERS | BINKALPHA );

    if (!m_Bink)
        return bfalse;

    BinkSetWillLoop(m_Bink,0);

    BinkGetFrameBuffersInfo( m_Bink, &texture_set.bink_buffers );

    YtextureDraw = RESOURCE_MANAGER->newEmptyResource(Resource::ResourceType_Texture)->getID();
    cRtextureDraw = RESOURCE_MANAGER->newEmptyResource(Resource::ResourceType_Texture)->getID();
    cBtextureDraw = RESOURCE_MANAGER->newEmptyResource(Resource::ResourceType_Texture)->getID();
    AtextureDraw = RESOURCE_MANAGER->newEmptyResource(Resource::ResourceType_Texture)->getID();

    for (u32 i=0; i<2; i++)
    {
        Ytexture[i] = RESOURCE_MANAGER->newEmptyResource(Resource::ResourceType_Texture)->getID();
        cRtexture[i] = RESOURCE_MANAGER->newEmptyResource(Resource::ResourceType_Texture)->getID();
        cBtexture[i] = RESOURCE_MANAGER->newEmptyResource(Resource::ResourceType_Texture)->getID();
        Atexture[i] = RESOURCE_MANAGER->newEmptyResource(Resource::ResourceType_Texture)->getID();
    }

    if ( Create_Bink_textures( &texture_set, this ) )
    {
        if ( !BinkWait( m_Bink ) )
        {
            Decompress_frame( m_Bink, &texture_set, this );
        }
    }

    m_pVertexBuffer = GFX_ADAPTER->createVertexBuffer(4, VertexFormat_PT, sizeof(VertexPT), bfalse, VB_T_INTERNAL);
    fillVertexBuffer(m_pVertexBuffer);

    m_stopped = bfalse;
    m_opened  = btrue;

    /// compute total time and fps.
    BINKSUMMARY summary;
    BinkGetSummary(m_Bink,&summary);

    m_fps = summary.FrameRate/(f32) summary.FrameRateDiv; 
    m_totalTimeInSeconds =  (f32)(summary.TotalFrames/m_fps);

#ifndef ITF_FINAL
    LOG(">>VideoHandle :%s  total time:%0.02f seconds",charName.getChar(),m_totalTimeInSeconds);
#endif //ITF_FINALs

    return  1;
  
}

//------------------------------------------------------------------------------------------------------------------------------------------------------
}

#endif
