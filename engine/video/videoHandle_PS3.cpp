#include "precompiled_engine.h"

#include "engine/video/videoHandle.h"
#include "HLSailVideoPlayer_PS3/libhlPlayer.h"
#include "adapters/GFXAdapter_GCM/GFXAdapter_GCM.h"
#include "adapters/GFXAdapter_GCM/VRAMRelocHdl.h"
#include "adapters/GFXAdapter_GCM/Texture_GCM.h"
#include "adapters/GFXAdapter_GCM/Label_GCM.h"

#ifndef _ITF_SOUNDADAPTER_H_PS3_
#include "adapters/SoundAdapter_PS3/SoundAdapter_PS3.h"
#endif //_ITF_SOUNDADAPTER_H_PS3_

namespace  ITF
{
//////////////////////////////////////////////////////////////////////////
// allocation callbacks
//////////////////////////////////////////////////////////////////////////

class SailToItfMemAllocator : public cell::Sail::memallocator
{
public:
    SailToItfMemAllocator();
    virtual ~SailToItfMemAllocator();
    GCM::Label & getSyncLabel();

protected:
    virtual void *  Allocate(uint32_t size, uint32_t alignment=0);
    virtual void    Deallocate(void* pMemory);
    virtual void *  AllocateTexture(uint32_t size, uint32_t alignment=0);
    virtual void    DeallocateTexture(void* pMemory);

    typedef Vector<GCM::SimpleVRAMRelocHandle> VecGCMMemoryBuffers;
    VecGCMMemoryBuffers m_frameBuffers;
    GCM::Label m_syncLabel;
    u32 m_mainMemCount;
    u32 m_texMemCount;
};

SailToItfMemAllocator::SailToItfMemAllocator()
: m_mainMemCount(0)
, m_texMemCount(0)
{
    m_syncLabel.init(0);
}

SailToItfMemAllocator::~SailToItfMemAllocator()
{
    m_syncLabel.release();
}

GCM::Label & SailToItfMemAllocator::getSyncLabel()
{
    return m_syncLabel;
}

void* SailToItfMemAllocator::Allocate(uint32_t size, uint32_t alignment)
{
    void * ptr = Memory::alignedMalloc(size, alignment);
    m_mainMemCount +=  Memory::alignedSize(ptr);
    return ptr;
}

void SailToItfMemAllocator::Deallocate(void* pMemory)
{
    m_mainMemCount -= Memory::alignedSize(pMemory);
    return Memory::alignedFree(pMemory);
}

void* SailToItfMemAllocator::AllocateTexture(uint32_t _size, uint32_t _alignment)
{
    m_frameBuffers.push_back(GCM::SimpleVRAMRelocHandle());
    GCM::SimpleVRAMRelocHandle & vramHdl = m_frameBuffers.back();
    vramHdl.MainAllocAligned(_size, _alignment);
    m_texMemCount += vramHdl.MemSize();
    return vramHdl.GetAddress();
}

void SailToItfMemAllocator::DeallocateTexture(void* pMemory)
{
    for(VecGCMMemoryBuffers::iterator it = m_frameBuffers.begin();
        it!=m_frameBuffers.end();
        ++it)
    {
        if (it->GetAddress() == pMemory)
        {
            m_texMemCount -= it->MemSize();
            m_frameBuffers.erase(it);
            break;
        }
    }
}

//////////////////////////////////////////////////////////////////////////
// File system overload to get full video read
//////////////////////////////////////////////////////////////////////////

class MemCachedCustomSource : public cell::Sail::CustomSource
{
private:
    u8 * m_fileCache;
    u64  m_fileSize;

    int loadFileInMem(const char * pUri)
    {
        if (m_fileCache)
            return CELL_OK;

        ux protocolNameLength = strlen(getProtocolName());
        const char* pPath = pUri + protocolNameLength + 3;

        int ret = CELL_OK;
        CellFsStat stat;
        ret = cellFsStat(pPath, &stat);
        if (CELL_OK == ret)
        {
            int fd = 0;
            ret = cellFsOpen(pPath, CELL_FS_O_RDONLY, &fd, NULL, 0);
            if (0 <= ret)
            {
                ret = ENOMEM;
                u8 * fileCache = (u8 *)Memory::alignedMalloc(ux(stat.st_size), 128u);
                if (NULL != fileCache)
                {
                    ret = CELL_OK;
                    u64 size = stat.st_size;
                    u8  *p   = fileCache;
                    for (;;)
                    {
                        u64 read;
                        ret = cellFsRead(fd, p, size, &read);
                        if (0 > ret)
                        {
                            break;
                        }
                        if (read == size)
                        {
                            break;
                        }
                        size -= read;
                        p    += read;
                    }

                    if (ret>=0)
                    {
                        ITF_ASSERT(m_fileCache == NULL);
                        m_fileCache = fileCache;
                        m_fileSize = stat.st_size;
                    }
                    else
                    {
                        Memory::alignedFree(fileCache);
                        fileCache = NULL;
                        m_fileCache = 0;
                        m_fileSize = 0;
                    }
                }
                cellFsClose(fd);
            }
        }
        if (0 > ret)
        {
            LOG("Error loading video in memory : 0x%08X", ret);
        }
        return ret;
    }

    ux m_outIndex;
    u64 m_curOffset;
    i64 m_remainSize;
    i64 m_checkSize;
    u32 m_sessionId;
    i32 m_sampleSize;

    enum { NbBufferItem = 2 };
    enum { SampleBlockSize = 4*1024*1024 };

    CellSailSourceBufferItem m_bufItems[NbBufferItem];

public:
    MemCachedCustomSource()
        : m_fileCache(NULL)
        , m_fileSize(0)
        , m_outIndex(0)
        , m_curOffset(0)
        , m_remainSize(0)
        , m_checkSize(0)
        , m_sessionId(0)
        , m_sampleSize(0)
    {
    }

    ~MemCachedCustomSource()
    {
        ITF_ASSERT(m_fileCache == NULL);
    }

protected:

    virtual const char * getProtocolName() const
    {
        return "x-mem-cache";
    }

    virtual int makeup(const char * pProtocol)
    {
        ITF_ASSERT(strcmp(getProtocolName(), pProtocol)==0);
        return CELL_OK;
    }

    virtual int cleanup()
    {
        Memory::alignedFree(m_fileCache);
        m_fileCache = NULL;
        m_fileSize = 0;
        return CELL_OK;
    }

    virtual void open(int streamType, void * pMediaInfo, const char * pUri, const CellSailSourceStreamingProfile * pProfile)
    {
        int ret = loadFileInMem(pUri);
        if (0 <= ret)
        {
            m_sampleSize = SampleBlockSize;
        }
        cellSailSourceNotifyCallCompleted(&getSource(), ret);
    }

    virtual void close()
    {
        Memory::alignedFree(m_fileCache);
        m_fileCache = NULL;
        m_fileSize = 0;
        cellSailSourceNotifyCallCompleted(&getSource(), CELL_OK);

    }

    virtual void start(const CellSailSourceStartCommand * pCommand, uint32_t sessionId)
    {
        int ret;
        uint64_t startFlags = pCommand->startFlags;
        if (CELL_SAIL_SOURCE_SEEK_ABSOLUTE_BYTE_POSITION != startFlags)
        {
            ret = CELL_SAIL_ERROR_INVALID_ARG;
        }
        else
        {
            u64 startOffset = pCommand->startArg;
            u64 readSize    = pCommand->lengthArg;
            if (m_fileSize < startOffset)
            {
                ret = CELL_SAIL_ERROR_INVALID_ARG;
            }
            else
            {
                readSize = Min(m_fileSize - startOffset, readSize);
                ITF_ASSERT(0 <= (i64)readSize);
                m_curOffset  = startOffset;
                m_remainSize = readSize;
                m_checkSize  = readSize;
                m_sessionId  = sessionId;
                m_outIndex   = 0;
                ret = CELL_OK;
            }
        }
        cellSailSourceNotifyCallCompleted(&getSource(), ret);
        if (ret>=0)
        {
            if (0 == m_remainSize)
            {
                cellSailSourceNotifyInputEos(&getSource());
            }
            else
            {
                for (int i = 0; i < NbBufferItem; ++i)
                {
                    // notify that we are ready for checkout.
                    cellSailSourceNotifyStreamOut(&getSource(), 1);
                    m_checkSize -= m_sampleSize;
                    if (0 >= m_checkSize)
                    {
                        break;
                    }
                }
            }
        }
    }

    virtual void stop()
    {
        cellSailSourceNotifyCallCompleted(&getSource(), CELL_OK);
    }

    virtual void cancel()
    {
        // nothing to do
    }

    virtual int checkout(const CellSailSourceBufferItem ** ppItem)
    {
        CellSailSourceBufferItem *pItem = &m_bufItems[m_outIndex];
        m_outIndex = (m_outIndex + 1u) % NbBufferItem;

        i64 size = (m_remainSize < m_sampleSize) ?
                    m_remainSize :
                    m_sampleSize;
        ITF_ASSERT(size > 0);

        pItem->pBuf      = m_fileCache + m_curOffset;
        pItem->size      = size;
        pItem->sessionId = m_sessionId;

        m_curOffset  += size;
        m_remainSize -= size;
        ITF_ASSERT(m_curOffset  <= m_fileSize);
        ITF_ASSERT(m_remainSize >= 0);

        *ppItem = pItem;

        if (0 == m_remainSize)
        {
            cellSailSourceNotifyInputEos(&getSource());
        }
        return CELL_OK;
    }

    virtual int checkin(const CellSailSourceBufferItem * pItem)
    {
        if (0 < m_checkSize)
        {
            cellSailSourceNotifyStreamOut(&getSource(), 1);
            m_checkSize -= m_sampleSize;
        }
        return CELL_OK;
    }

    virtual int clear()
    {
        m_curOffset  = 0;
        m_checkSize  = 0;
        m_remainSize = 0;
        return CELL_OK;
    }

    virtual void read(int streamType, void * pMediaInfo, const char * pUri, uint64_t offset, uint8_t * pBuf, size_t size, uint64_t * pTotalSize)
    {
        int ret = readSync(streamType,
                           pMediaInfo,
                           pUri,
                           offset,
                           pBuf,
                           size,
                           pTotalSize);
        cellSailSourceNotifyCallCompleted(&getSource(), ret);
    }

    virtual int readSync(int streamType, void * pMediaInfo, const char * pUri, uint64_t offset, uint8_t * pBuf, size_t size, uint64_t * pTotalSize)
    {
        ITF_ASSERT(pUri);
        ITF_ASSERT(pBuf);

        int ret = loadFileInMem(pUri);
        if (0 <= ret)
        {
            *pTotalSize = m_fileSize;
            u64 realReadSize = Min(u64(size), m_fileSize - offset);
            memcpy(pBuf, m_fileCache+offset, ux(realReadSize));
            if (realReadSize == size)
            {
                ret = 0;
            }
            else
            {
                ret = (int)realReadSize;
            }
        }
        return ret;
    }

    virtual int getCapabilities(int streamType, void * pMediaInfo, const char * pUri, uint64_t * pCapability)
    {
        *pCapability = (CELL_SAIL_SOURCE_CAPABILITY_SEEK_ABSOLUTE_BYTE_POSITION |
                        CELL_SAIL_SOURCE_CAPABILITY_PAUSE                       |
                        CELL_SAIL_SOURCE_CAPABILITY_EOS);
        return CELL_OK;
    }

    virtual int inquireCapability(int streamType, void * pMediaInfo, const char * pUri, const CellSailSourceStartCommand * pCommand)
    {
        if (CELL_SAIL_SOURCE_SEEK_ABSOLUTE_BYTE_POSITION != pCommand->startFlags)
        {
            return CELL_SAIL_ERROR_INVALID_ARG;
        }
        return CELL_OK;
    }
};


//////////////////////////////////////////////////////////////////////////
// Video high level management
//////////////////////////////////////////////////////////////////////////

videoHandle::videoHandle()
: m_playerInit(NULL)
, m_player(NULL)
, m_memAllocator(NULL)
, m_customSrc(NULL)
, m_isStopping(true)
, m_firstFrameDisplayed(false)
, m_pVertexBuffer(NULL)
, m_clearScreen(false)
{

}

videoHandle::~videoHandle()
{
    close();
}

void videoHandle::destroy()
{
    close();
}

void videoHandle::update()
{
    if (m_player && !m_player->isPlaying())
    {
        m_stopped = true;
        if (m_isStopping)
            close();
    }
    
}

void videoHandle::play()
{
    if (m_player->isPlaying())
    {
        m_player->Resume();
    }
}

void videoHandle::pause()
{
    
    if (m_player)
    {
        m_player->Pause();
        m_isPaused = btrue;
    }
}

void videoHandle::stop()
{
    videoHandleBase::stop();
    if (m_player)
    {
        m_isStopping = true;
        m_player->Stop();
    }
}

void videoHandle::render()
{
    if (!m_player || !m_player->isPlaying())
        return;

    VideoFrameInfo frameInfo;
    bool frameAvailable = m_player->vsyncGetFrame(&frameInfo);
    if (frameAvailable)
    {
        m_firstFrameDisplayed = true;
        u32 ioOffset = 0; 
        cellGcmAddressToOffset(frameInfo.buffer, &ioOffset);

        GCM::Texture texArray[3];
        GCM::Texture * arrayOfTexPtr[3];
        u32 nbTex =3;
        if (frameInfo.RGBAFrameHint)
        {
            texArray[0].format = GCM::TextureFormat::ARGB32Linear;
            texArray[0].mipmap = 1;
            texArray[0].dimension = 2;
            texArray[0].cubemap	= 0;
            texArray[0].remap = GCM_TEX_REMAP_ARGB;
            texArray[0].width = frameInfo.width;
            texArray[0].height = frameInfo.height;
            texArray[0].depth = 1;
            texArray[0].pitch = GCM::GetPitch(texArray[0].format, frameInfo.width, false);
            texArray[0].offset = ioOffset;
            texArray[0].location = CELL_GCM_LOCATION_MAIN;
            arrayOfTexPtr[0] = &texArray[0];
            nbTex = 1;
        }
        else
        {
            CellGcmTexture tex;
            tex.format = GCM::TextureFormat::L8Linear;
            tex.mipmap = 1;
            tex.dimension = 2;
            tex.cubemap	= 0;
            tex.remap = GCM_TEX_REMAP_1B11; // remap B to RED
            tex.width = frameInfo.width;
            tex.height = frameInfo.height;
            tex.depth = 1;
            tex.pitch = GCM::GetPitch(tex.format, frameInfo.width, false);
            tex.offset = ioOffset;
            tex.location = CELL_GCM_LOCATION_MAIN;

            CellGcmTexture * tex0 = &texArray[0];
            CellGcmTexture * tex1 = &texArray[1];
            CellGcmTexture * tex2 = &texArray[2];
            (*tex0) = tex;
            (*tex1) = tex;
            tex1->offset = tex0->offset + tex0->pitch * tex0->height;
            tex1->width /= 2;
            tex1->height /= 2;
            tex1->pitch /= 2;

            (*tex2) = (*tex1);
            tex2->offset = tex1->offset + tex1->pitch * tex1->height;

            arrayOfTexPtr[0] = &texArray[0];
            arrayOfTexPtr[1] = &texArray[1];
            arrayOfTexPtr[2] = &texArray[2];

            nbTex = 3;
        }
        
        GFXAdapter::getptr()->drawMovie(NULL, getAlpha(), m_pVertexBuffer, reinterpret_cast<Texture **>(&arrayOfTexPtr), nbTex);
    }
    else if (!m_firstFrameDisplayed) // start with black screen
    {
        if (m_clearScreen)
            GFXAdapter::getptr()->clear(GFX_CLEAR_COLOR, 0.f, 0.f, 0.f, getAlpha());
    }
    // sync 
    GCM::Label & syncLabel = static_cast<SailToItfMemAllocator *>(m_memAllocator)->getSyncLabel();
    GCM::RenderContext & rdrCtxt = static_cast<GFXAdapter_GCM *>(GFXAdapter::getptr())->getGCMRenderCtxt();
//    syncLabel.setGpuWait(rdrCtxt, 0);
    syncLabel.setWriteOnCommand(rdrCtxt, frameInfo.syncTag);
    rdrCtxt.gcmCtxt->Flush();
}

void videoHandle::onFlipStatic(u64 _tHis)
{
    reinterpret_cast<videoHandle *>(_tHis)->onFlip();
}

void videoHandle::onFlip()
{
    // be sure to unregister onFlip before deletion of m_memAllocator
    GCM::Label & syncLabel = static_cast<SailToItfMemAllocator *>(m_memAllocator)->getSyncLabel();
    u32 tag = *syncLabel.labelAddr;
//    (*syncLabel.labelAddr) = 0;
    uint64_t timeBase = cellGcmGetLastFlipTime();
    m_player->updateVideoSync(tag, timeBase);
}

bbool videoHandle::open(const String& _fileName)
{
    if (_fileName.getLen()==0)
        return bfalse;

    if (m_player!=NULL) // already opened !
    {
        return bfalse;
    }

    m_clearScreen = (_fileName == "cine_fin"); // HACK

    String convertedFileName;
    String fullname = _fileName+".pam";

    FILEMANAGER->TranslatePath(convertedFileName, fullname);
    StringConverter strFileName(convertedFileName);
    
    //###E3HACK -> we should avoid 
    if (!hwFile::exists(convertedFileName))
    {
        return bfalse;
    }
    
    m_isStopping = false;

    ITF::SoundAdapter_PS3 * soundAdapterPS3 = static_cast<ITF::SoundAdapter_PS3*>( ITF::SINGLETONS.getSoundAdapter() ); 

    m_playerInit = new VideoPlayerInit;
    memset(m_playerInit, 0, sizeof(*m_playerInit));
    m_playerInit->pSharedSpurs = soundAdapterPS3->getSpurs();
	m_playerInit->width = 0;
	m_playerInit->height = 0;
	m_playerInit->RGBAOutput = false;

    m_memAllocator = new SailToItfMemAllocator;
    if (m_playFromMemory)
        m_customSrc = new MemCachedCustomSource;
    else
        m_customSrc = NULL;

    m_player = new cell::Sail::hlPlayer(*m_playerInit, m_memAllocator, m_customSrc);

    // full screen VB
    m_pVertexBuffer = GFXAdapter::getptr()->createVertexBuffer(4,VertexFormat_PT, sizeof(VertexPT), bfalse,VB_T_INTERNAL);
    u32 screenW = GFXAdapter::getptr()->getScreenWidth();
    u32 screenH = GFXAdapter::getptr()->getScreenHeight();
    VertexPT* pVertices;
    m_pVertexBuffer->Lock((void **)&pVertices);
    pVertices[ 0 ].m_pos.set( 0.f, 0.f, 0.f);
    pVertices[ 0 ].m_uv.set(0.f,.0f);

    pVertices[ 1 ].m_pos.set(screenW, 0.f, 0.f);
    pVertices[ 1 ].m_uv.set(1.f, 0.f);

    pVertices[ 2 ].m_pos.set(screenW, screenH, 0.0f);
    pVertices[ 2 ].m_uv.set(1.f, 1.f);

    pVertices[ 3 ].m_pos.set(0.f, screenH, 0.0f);
    pVertices[ 3 ].m_uv.set(0.f, 1.f);
    m_pVertexBuffer->Unlock();

    static_cast<GFXAdapter_GCM *>(GFXAdapter::getptr())->addFlipEventCB(&onFlipStatic, u64(this));

    if (!m_player->Play(strFileName.getChar(), false))
    {
        LOG("File not found or invalid, please check that the path and filename are correct.\n");
        close();
        return bfalse;
    }

    m_totalTimeInSeconds = f32(m_player->EndTime() - m_player->StartTime())/1000.f;

    m_stopped = bfalse;
    m_opened = true;
    m_firstFrameDisplayed = false;

    return btrue;
}

bbool videoHandle::getCurrentTime(f32& _timeInSeconds)
{
    if (m_player==NULL)
    {
        _timeInSeconds = 0.f;
        return bfalse;
    }
    _timeInSeconds = f32(m_player->CurrentTime())/1000.f;
    return btrue;
}

void videoHandle::close()
{
    // call it before deletion of m_player or m_memAllocator
    static_cast<GFXAdapter_GCM *>(GFXAdapter::getptr())->removeFlipEventCB(&onFlipStatic, u64(this));

    SF_DEL(m_player);
    SF_DEL(m_playerInit);
    SF_DEL(m_memAllocator);
    SF_DEL(m_customSrc);

    if (m_pVertexBuffer)
    {
        GFXAdapter::getptr()->releaseVertexBuffer(m_pVertexBuffer);
        m_pVertexBuffer = NULL;
    }
    m_stopped = btrue;
    m_opened = false;
    m_clearScreen = false;
}
}

