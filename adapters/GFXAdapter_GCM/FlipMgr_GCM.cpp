#include "precompiled_GFXAdapter_GCM.h"
#include <cell/resc.h>
#include "core/system/PS3/ModuleManager_ps3.h"
#include "VRAMAllocator/VRAMTools.h"
#include "FlipMgr_GCM.h"

namespace ITF
{
namespace GCM
{
    // class handling what has to be done at flip,
    // including resolution, aspect ratio or PAL 50Hz conversion
    
    // The policy is the following :
    // 1080p/i or 720p -> not using libresc
    // All other resolutions -> using libresc to convert from 720p
    // All resolution with aspect ratio 4/3 -> using libresc to convert to letter box mode (black area up and down)
    // 576p/i (PAL) -> using libresc to convert 60Hz to 50Hz (depending on AutoAdaptFrequency)

    //////////////////////////////////////////////////////////////////////////
    // Flip using libresc
    //////////////////////////////////////////////////////////////////////////

    class FlipMgrResc : public FlipMgr
    {
    public:
        FlipMgrResc()
            : m_tileEntry(0)
            , m_destWidth(0)
            , m_destHeight(0)
            , m_interlaceMode(false)
        {
            ITF::MODULE_MANAGER->loadModule(CELL_SYSMODULE_RESC);
        }

        ~FlipMgrResc()
        {
            ITF::MODULE_MANAGER->unloadModule(CELL_SYSMODULE_RESC);
        }

        bool init(const CellVideoOutState & _videoOutState)
        {
            m_interlaceMode = _videoOutState.displayMode.scanMode==CELL_VIDEO_OUT_SCAN_MODE_INTERLACE;

            CellRescInitConfig rescConf;
            memset(&rescConf, 0, sizeof(rescConf));

            rescConf.size = sizeof(rescConf);
            rescConf.resourcePolicy = CELL_RESC_MINIMUM_VRAM | CELL_RESC_MINIMUM_GPU_LOAD;
            rescConf.supportModes   = CELL_RESC_720x480 | CELL_RESC_720x576;
            rescConf.ratioMode = (_videoOutState.displayMode.aspect == CELL_VIDEO_OUT_ASPECT_4_3) ? 
                                    CELL_RESC_LETTERBOX
                                  : CELL_RESC_FULLSCREEN;
            rescConf.palTemporalMode = AutoAdaptFrequency?CELL_RESC_PAL_60_INTERPOLATE:CELL_RESC_PAL_50;
            rescConf.interlaceMode = m_interlaceMode?CELL_RESC_INTERLACE_FILTER:CELL_RESC_NORMAL_BILINEAR;
            rescConf.flipMode = CELL_RESC_DISPLAY_VSYNC;

            if (CELL_OK != cellRescInit(&rescConf))
            {
                return false;
            }

            CellRescBufferMode bufferMode;
            if (CELL_OK != cellRescVideoOutResolutionId2RescBufferMode((CellVideoOutResolutionId)_videoOutState.displayMode.resolutionId, &bufferMode))
            {
                return false;
            }

            struct RescDim
            {
                CellRescBufferMode dstBufferMode;
                ux width;
                ux height;
            } rescDim[] =
            {
                { CELL_RESC_720x480, 720u, 480u },
                { CELL_RESC_720x576, 720u, 576u }, 
                { CELL_RESC_1280x720, 1280u, 720u }, 
                { CELL_RESC_1920x1080, 1920u, 1080u }, 
            };

            m_destWidth = 0;
            m_destHeight = 0;
            for(ux i = 0; i<sizeof(rescDim)/sizeof(rescDim[0]); i++)
            {
                if (rescDim[i].dstBufferMode == bufferMode)
                {
                    m_destWidth = rescDim[i].width;
                    m_destHeight = rescDim[i].height;
                }
            }
            ITF_ASSERT(m_destWidth!=0);
            if (m_destWidth == 0) // defaulting to 720p
            {
                m_destWidth = 1280u;
                m_destHeight = 720u;
            }

            CellRescDsts rescDst;
            rescDst.format = CELL_RESC_SURFACE_A8R8G8B8;
            rescDst.pitch = cellGcmGetTiledPitchSize(m_destWidth*4u);
            rescDst.heightAlign = bufferMode==CELL_RESC_1920x1080?32u:64u;
            if (CELL_OK != cellRescSetDsts(bufferMode, &rescDst))
            {
                return false;
            }
            
            if (CELL_OK != cellRescSetDisplayMode(bufferMode))
            {
                return false;
            }

            // wait for the first flip
            while (cellRescGetFlipStatus()!=0)
            {
                sys_timer_usleep(300);
            }

            return true;
        }

        bool setDisplayBuffers(GCM::Surface * _displayBuffers[], ux _nbDisplayBufffer)
        {
            if (m_interlaceMode)
            {
                if (!createInterlaceTable(_displayBuffers[0]->height))
                    return false;
            }

            i32 colorBufferSize;
            i32 vertexArraySize;
            i32 fragmentShaderSize;
            if (CELL_OK != cellRescGetBufferSize(&colorBufferSize, &vertexArraySize, &fragmentShaderSize))
            {
                return false;
            }
            m_verticesBuffer.AllocAligned(vertexArraySize, 128);
            m_shaderBuffer.AllocAligned(fragmentShaderSize, 128);
            allocTiledColorBuffer(m_colorBuffer, m_tileEntry, colorBufferSize, m_destWidth, m_destHeight);
            ITF_ASSERT(m_colorBuffer.MemSize() == colorBufferSize);

            if (CELL_OK != cellRescSetBufferAddress(m_colorBuffer.GetAddress(),
                                                    m_verticesBuffer.GetAddress(),
                                                    m_shaderBuffer.GetAddress()))
            {
                return false;
            }

            for(ux i = 0; i<_nbDisplayBufffer; ++i)
            {
                CellRescSrc src;
                if (CELL_OK != cellRescGcmSurface2RescSrc(_displayBuffers[i], &src))
                {
                    return false;
                }
                if (CELL_OK != cellRescSetSrc(i, &src))
                {
                    return false;
                }
            }
            return true;
        }

        void close()
        {
            cellRescExit();
            if (0 == VRAMAllocator::TileAreaAllocator::get().releaseIndex(m_tileEntry))
            {
                cellGcmUnbindTile(m_tileEntry);
            }
        }

        void setFlipHandler(void (*onFlipFct)(uint32_t arg))
        {
            cellRescSetFlipHandler(onFlipFct);
        }

        void setFlip(RenderContext & _rdrCtxt, ux _idx)
        {
            cellRescSetConvertAndFlip(_rdrCtxt.gcmCtxtNoInline, _idx);
        }

        void setWaitFlip(RenderContext & _rdrCtxt)
        {
            cellRescSetWaitFlip(_rdrCtxt.gcmCtxtNoInline);
        }

        u64 getLastFlipTime() const
        {
            return cellRescGetLastFlipTime();
        }

    private:
        static void allocTiledColorBuffer(GCM::SimpleVRAMRelocHandle & _colorBuffer, ux & _tileEntry, ux _colorBufferSize, ux _width, ux _height)
        {
            _tileEntry = VRAMAllocator::TileAreaAllocator::get().allocUnusedIndex();
            ITF_ASSERT(_tileEntry!=VRAMAllocator::TileAreaAllocator::InvalidTileIndex);
            
            u32 bpp = 4;
            u32 pitch = cellGcmGetTiledPitchSize(_width*bpp);

            // memory block size -> align on 64K
            _colorBuffer.LocalAllocAligned(_colorBufferSize, 64u*1024u, VRAM_RenderTarget);

            // finally bind tile area
            ITF_VERIFY(CELL_OK == cellGcmSetTileInfo(   _tileEntry,
                                                        _colorBuffer.GetLocation(),
                                                        _colorBuffer.GetOffset(),
                                                        _colorBufferSize,
                                                        pitch,
                                                        CELL_GCM_COMPMODE_DISABLED,
                                                        0, 0));
            ITF_VERIFY(CELL_OK == cellGcmBindTile(_tileEntry));
        }

        bool createInterlaceTable(ux _height)
        {
            enum { INTERLACE_TABLE_LENGTH = 128};
            enum { INTERLACE_TABLE_SIZE = sizeof(float)*4*INTERLACE_TABLE_LENGTH};
            m_interlaceTable.Alloc(INTERLACE_TABLE_SIZE);
            return CELL_OK == cellRescCreateInterlaceTable(m_interlaceTable.GetAddress(), float(_height), CELL_RESC_ELEMENT_FLOAT, INTERLACE_TABLE_LENGTH);
        }

        GCM::SimpleVRAMRelocHandle m_shaderBuffer;
        GCM::SimpleVRAMRelocHandle m_verticesBuffer;
        GCM::SimpleVRAMRelocHandle m_colorBuffer;
        GCM::SimpleVRAMRelocHandle m_interlaceTable;

        ux m_tileEntry;
        ux m_destWidth;
        ux m_destHeight;
        bool m_interlaceMode;
    };

    //////////////////////////////////////////////////////////////////////////
    // Flip using regular gcm
    /////////////////////////////////////////////////////////////////////////

    class FlipMgrGcm : public FlipMgr
    {
    public:
        FlipMgrGcm()
        {
        }

        ~FlipMgrGcm()
        {
        }

        bool init(const CellVideoOutState & _videoOutState)
        {
            const u8	FrontBufferOutputFormat     = CELL_VIDEO_OUT_BUFFER_COLOR_FORMAT_X8R8G8B8;
            const u8	FrontBufferOutputBPP        = 4;
            CellVideoOutResolution videoOutRes;
            if (CELL_OK != cellVideoOutGetResolution(_videoOutState.displayMode.resolutionId, &videoOutRes))
            {
                return false;
            }

            CellVideoOutConfiguration config;
            memset(&config, 0, sizeof(config));
            config.resolutionId = _videoOutState.displayMode.resolutionId;
            config.format = FrontBufferOutputFormat;
            config.aspect = _videoOutState.displayMode.aspect; // or maybe CELL_VIDEO_OUT_ASPECT_AUTO ?
            config.pitch = cellGcmGetTiledPitchSize(videoOutRes.width * FrontBufferOutputBPP); // get the pitch advice from gcm

            // definitely set the output
            if (CELL_OK != cellVideoOutConfigure(CELL_VIDEO_OUT_PRIMARY, &config, NULL, 0))
            {
                return false;
            }

            cellGcmSetFlipMode(CELL_GCM_DISPLAY_VSYNC);
            // wait 1 flip
            while (cellGcmGetFlipStatus()!=0)
            {
                sys_timer_usleep(300);
            }

            return true;
        }

        bool setDisplayBuffers(GCM::Surface * _displayBuffers[], ux _nbDisplayBufffer)
        {
            for(u32 i = 0; i<_nbDisplayBufffer; ++i)
            {
                GCM::Surface * & frameBuffer = _displayBuffers[i];
                if (CELL_OK != cellGcmSetDisplayBuffer(i,  frameBuffer->colorOffset[0], frameBuffer->colorPitch[0], frameBuffer->width,  frameBuffer->height))
                {
                    return false;
                }
            }
            return true;
        }

        void close()
        {
        }

        void setFlipHandler(void (*onFlipFct)(uint32_t arg))
        {
            cellGcmSetFlipHandler(onFlipFct);
        }

        void setFlip(RenderContext & _rdrCtxt, ux _idx)
        {
            ITF_VERIFY(CELL_OK == _rdrCtxt.gcmCtxtNoInline->SetFlip(_idx));
        }

        void setWaitFlip(RenderContext & _rdrCtxt)
        {
            _rdrCtxt.gcmCtxt->SetWaitFlip();
        }

        u64 getLastFlipTime() const
        {
            return cellGcmGetLastFlipTime();
        }
    };


    FlipMgr * FlipMgr::create(Type _type)
    {
        if (_type == FLIPMGR_USING_RESC)
        {
            return new FlipMgrResc();
        }
        else
        {
            return new FlipMgrGcm();
        }
    }

}

}
