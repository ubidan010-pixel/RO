#include "precompiled_GFXAdapter_GCM.h"

#include <cell/sysmodule.h>
#include <sysutil/sysutil_sysparam.h>
#include <cell/dbgfont.h>

#include "Tools_GCM.h"
#include "VRAMAllocator_GCM.h"
#include "ShaderProgram_GCM.h"
#include "GFXAdapter_GCM.h"
#include "VRAMAllocator/VRAMTools.h"

#ifndef _ITF_MODULEMANAGER_PS3_H_
#include "core/system/PS3/ModuleManager_ps3.h"
#endif //_ITF_MODULEMANAGER_PS3_H_

///////////////////////////////////////////////////////////////////////////////
// Init/destroy methods of GFXAdapter_GCM
///////////////////////////////////////////////////////////////////////////////

namespace ITF
{


    class MinimalDisplay : public VThread
    {
    public:
        MinimalDisplay(GFXAdapter_GCM * _gfxAdapter)
            : m_gfxAdapter(_gfxAdapter)
            , m_started(false)
            , m_quitRequested(false)

        {
            ITF_ASSERT_CRASH(_gfxAdapter, "Gfx Adapter NULL");
        }

        void start()
        {
            if (!m_started)
            {
                m_started = true;
                m_quitRequested = false;
                VThread::start("MinimalDisplay");
            }
        }

        void stop()
        {
            if (m_started)
            {
                {   
                    ScopeLockMutex scopeLock(m_mutex);
                    m_quitRequested = true;
                }
                waitForExit();
                m_started = false;                
            }
        }

        bool isStarted() const { return m_started; }

    private:
        virtual u32 run()
        {
            ScopeLockMutex scopeLock(m_mutex);
            while(!m_quitRequested)
            {
                ScopeUnLockMutex scopeUnLock(m_mutex);
                m_gfxAdapter->minimalDisplay();
            }

            m_gfxAdapter->waitForGPUIdle();

            return 0;
        }

        GFXAdapter_GCM * m_gfxAdapter;

        bool m_started;

        Mutex m_mutex;
        bool m_quitRequested;
    };


GFXAdapter_GCM::GFXAdapter_GCM()
:   m_minimalDisplay(NULL)
,   m_successfullInit(false)
,   m_cmdBufferPageAddress(0)
,   m_cmdBufferAddress(0)
,   m_cpuFrameCount(0)
,   m_curswap(0)
,   m_nbFrameBuffer(0)
,   m_flipMgr(NULL)
,   m_usingLibResc(false)
,   m_finishCount(0)
{
    new GCM::ShaderProgramRepository(); // create and register singleton
    m_currentVertexStream.rsxLocation = CELL_GCM_LOCATION_LOCAL;
    m_currentVertexStream.rsxOffset = u32(-1);
    m_currentVertexStream.stride = u32(-1);

    preInit();

    if (m_successfullInit)
    {
        m_minimalDisplay = new MinimalDisplay(this);
        m_minimalDisplay->start();
    }
}

GFXAdapter_GCM::~GFXAdapter_GCM()
{

}

void GFXAdapter_GCM::destroy()
{
    IMPLEMENT_THIS
    ITF_ASSERT(0);
    if (m_flipMgr)
        m_flipMgr->close();
    delete m_flipMgr;
    GFXAdapter::destroy();
}

void GFXAdapter_GCM::destroyResources()
{
    GFXAdapter::destroyResources();
    if (m_minimalDisplay && m_minimalDisplay->isStarted())
    {
        m_minimalDisplay->stop();
    }
    else
        waitForGPUIdle();
}

void GFXAdapter_GCM::waitForGPUIdle()
{
    if (m_flipMgr)
    {
        // in case wait flip is delayed, do it now
        if (m_nbFrameBuffer>2
            || m_usingLibResc)
        {
            m_flipMgr->setWaitFlip(m_rdrCtxt);
        }
        m_rdrCtxt.gcmCtxt->Finish(m_finishCount++);
    }
}

f32 GFXAdapter_GCM::ms_cpuWaitForCBAvailable = 0.f;

// This overload is when using SNC with compiler option -> -Xnotocrestore=2
i32 (*g_defaultCallback)(struct CellGcmContextData* con, u32 size) = NULL;
static i32 TimedGCMCallback(struct CellGcmContextData* _ctxt, u32 _size)
{
    _ctxt->callback = g_defaultCallback;
    GCM::TimerProfiler profiler;
    profiler.Start();
    i32 res = cellGcmCallbackForSnc(_ctxt, _size);
    GFXAdapter_GCM::ms_cpuWaitForCBAvailable += profiler.Stop();
    _ctxt->callback = &TimedGCMCallback;
    return res;
}

bool GFXAdapter_GCM::initCmdBuffer()
{
    static const u32 GcmInitAllocatedArea                       = 4*1024*1024;	// allocated page size for CB
    static const u32 GcmCommandBufferPrefixSize					= 4u*1024u;		// 4KB of prefix for command buffer is asked
    static const u32 GcmCommandBufferRSXPreFetchFence			= 4u*1024u;     // Don't use last 4KB to avoid accessing unmapped area by RSX 4KB prefetch
    static const u32 GcmAdditionalAllocatedArea					= 0*1024;		// free gpu adressable data
    static const u32 GcmCommandBufferAlignment					= 1024*1024;
    static const u32 GcmCommandBufferSize = GcmInitAllocatedArea - GcmAdditionalAllocatedArea - GcmCommandBufferPrefixSize - GcmCommandBufferRSXPreFetchFence; // let the 4KB (min needed) + a few bytes of free cmd buffer

    // try CELL_GCM_DEFAULT_FIFO_MODE_OPTIMIZE
    ITF_VERIFY(CELL_OK == cellGcmInitDefaultFifoMode(CELL_GCM_DEFAULT_FIFO_MODE_TRADITIONAL));

    m_cmdBufferPageAddress = GCM::VRAMAllocator::AllocRSXMapableMainMemory(GcmInitAllocatedArea);
    int res = cellGcmInit(GcmCommandBufferSize, GcmInitAllocatedArea, m_cmdBufferPageAddress);
    m_cmdBufferAddress = m_cmdBufferPageAddress + GcmCommandBufferPrefixSize;

    if (res!= CELL_OK)
    {
        LOG("cellGcmInit failed\n");
        ITF_ASSERT(0);
        return false;
    }

    g_defaultCallback = gCellGcmCurrentContext->callback;
    gCellGcmCurrentContext->callback = &TimedGCMCallback;

#if 0
    u32 bufferSize = cellGcmGetDefaultCommandWordSize();
    u32 segmentSize = cellGcmGetDefaultSegmentWordSize();

    printf("Default Command Size %d\n", bufferSize);
    printf("Default Segment Size %d\n", segmentSize);

    static f32 s_segmentSize = 512; // 2Ko (32Ko is the default)
    
    cellGcmSetDefaultFifoSize(bufferSize, s_segmentSize);
#endif

    m_rdrCtxt.assignGcmRenderContext(gCellGcmCurrentContext); // assign default CB

    cellGcmGetConfiguration(&m_gcmConfig);

    return true;
}

bool GFXAdapter_GCM::initTools()
{
    new GCM::LabelManager(); // register singleton

    // VRAM Allocator
    static const u32 MainMemoryInitialAllocatedArea = 5*1024*1024; // main memory reserved for rendering stuff (right now used for 720p video)
    static const u32 MainMemoryReportInitialAllocatedArea = 1*1024*1024; // main memory reserved for report stuff
    GCM::VRAMAllocator::Init();
    GCM::VRAMAllocator::CreatePool(GCM::VRAMAddress(m_gcmConfig.localAddress), m_gcmConfig.localSize, CELL_GCM_LOCATION_LOCAL);
    GCM::VRAMAllocator::AllocAndCreateStaticMainPool(MainMemoryInitialAllocatedArea); 
    GCM::VRAMAllocator::AllocAndCreateStaticMainPool(MainMemoryReportInitialAllocatedArea, true);

    new GCM::VRAMAllocator::TileAreaAllocator(); // register singleton
    new GCM::VRAMAllocator::ZCullMemoryManager(); // register singleton

    new GCM::ReportManager(256, REPORT_LOCATION); // register singleton

    return true;
}

bool GFXAdapter_GCM::initDebugTools()
{
#ifdef _USE_FONTDEBUG

    m_cpuTimeProfiler.Start(); // just start it now to avoid undefined value at first present

    int frag_size = CELL_DBGFONT_FRAGMENT_SIZE; // Fragment shader size
    int font_tex = CELL_DBGFONT_TEXTURE_SIZE; // Font texture size
    int vertex_size = 2048 * CELL_DBGFONT_VERTEX_SIZE; // Vertex buffer size
    int local_size = frag_size + vertex_size + font_tex;
    m_fontMemory.LocalAllocAligned(local_size, 128, GCM::VRAM_Unknown);
    // (2) Initialize the library
    CellDbgFontConfigGcm cfg;
    memset(&cfg, 0, sizeof(CellDbgFontConfigGcm));
    cfg.localBufAddr = (sys_addr_t)m_fontMemory.GetAddress(); // Address allocated on local memory
    cfg.localBufSize = local_size; // Size allocated on local memory
    cfg.mainBufAddr = NULL; // Address allocated on main memory
    cfg.mainBufSize = 0; // Size allocated on main memory
    cfg.screenWidth = m_screenWidth; // Screen width
    cfg.screenHeight = m_screenHeight; // Screen height
    cfg.option =    CELL_DBGFONT_VERTEX_LOCAL| // Place vertex on local memory
        CELL_DBGFONT_TEXTURE_LOCAL| // Place texture on local memory
        //                    CELL_DBGFONT_SYNC_ON| // Synchronize PPU with RSX™ when drawing
        CELL_DBGFONT_VIEWPORT_ON| // Set viewport when drawing
        CELL_DBGFONT_MINFILTER_LINEAR| // Filter for reduced draw
        CELL_DBGFONT_MAGFILTER_LINEAR; // Filter for enlarged draw
    return CELL_OK == cellDbgFontInitGcm(&cfg);
#else
    return true;
#endif // _USE_FONTDEBUG
}

void GFXAdapter_GCM::initDefaultRenderStates()
{
    m_rdrCtxt.paramDBDirty = false;

    // these 2 values should be tweaked wisely for quality versus performance trade
    m_minLinearFilteringMode = CELL_GCM_TEXTURE_LINEAR_LINEAR;
    m_linearFilteringOptimization = 0;
    m_texLodBiasFloat = 0.f;

    m_alphaTestState = bfalse;

    // fill the default render state
    SamplerState defaultTexSamplerState;
    defaultTexSamplerState.alphaKill = 0;
    defaultTexSamplerState.bias = 0;
    defaultTexSamplerState.activated = CELL_GCM_FALSE;
    defaultTexSamplerState.minFilter = m_minLinearFilteringMode;
    defaultTexSamplerState.magFilter = CELL_GCM_TEXTURE_LINEAR;
    defaultTexSamplerState.wrapU = CELL_GCM_TEXTURE_WRAP;
    defaultTexSamplerState.wrapV = CELL_GCM_TEXTURE_WRAP;

    std::fill(m_defaultTexSamplerState, m_defaultTexSamplerState+NB_MAX_TEX_SAMPLER, defaultTexSamplerState);

    m_rdrCtxt.gcmCtxt->SetReportLocation(REPORT_LOCATION);
    m_gpuRenderTimer.start(m_rdrCtxt);

    resetSceneResolvedFlag();
}

bool GFXAdapter_GCM::createRenderingSurfaces()
{
    ITF_ASSERT(m_nbFrameBuffer>0);
    ITF_ASSERT(m_flipMgr != NULL);
    const u8	FrontBufferOutputBPP = 4;
    GCM::CreateTiledSurfaceGroup(m_frameBuffers, m_nbFrameBuffer, getScreenWidth(), getScreenHeight(), FrontBufferOutputBPP, 0);
    // create texture object as the backbuffers
    for(u32 i = 0; i<m_nbFrameBuffer; ++i)
    {
        m_frameBuffersAsTex[i] = GCM::CreateTextureFromSurface(m_frameBuffers[i]);
    }
    m_backBufferIndex = 0;
    return m_flipMgr->setDisplayBuffers(m_frameBuffers, m_nbFrameBuffer);
}

// load a shader from it's name, assuming default directory
bool GFXAdapter_GCM::loadShader(const char * _shaderName)
{
    static const String shadersFolder("Shaders/Compiled/PS3/");
    u32 nbShader = mp_shaderManager.getNumberShader();
    mp_shaderManager.loadShaderGroup(shadersFolder+_shaderName); //default shader.
    return     nbShader+1 == mp_shaderManager.getNumberShader()
            && NULL != mp_shaderManager.getShaderByIndex(nbShader);
}

bool GFXAdapter_GCM::loadCoreShaders()
{
    const char * HardCodedShaders[] =
    {   // Keep the same order, call to mp_shaderManager.getShaderByIndex are giving the hard-coded order of the shader
        "renderPCT",
        "afterfx",
        "refract",
        "font",
        "renderOVERDRAW",
        "renderFog",
        "movie"
    };

    for(ux i = 0; i<sizeof(HardCodedShaders)/sizeof(HardCodedShaders[0]); ++i)
    {
        if (!loadShader(HardCodedShaders[i]))
        {
            LOG("Cannot find shader %s\n", HardCodedShaders[i]);
            return false;
        }
    }

    mp_defaultShader = mp_shaderManager.getShaderByIndex(0);

    return true;
}


void GFXAdapter_GCM::createCoreVertexDeclarations()
{
    // aliases
    struct VDeclType
    {
        enum
        {
            Float1,
            Float2,
            Float3,
            Float4,
            Color,
            UByte4,
            UByte4N,
            Short2,
            Short4,
            Short2N,
            Short4N,
            UShort2N,
            UShort4N,
            UInt1,

            Count
        };
    };

    // aliases to (size,type) (common types of D3D)
    static const GCM::VertexDecl::Element::Type TypeConverter[] = 
    {	
        {1, CELL_GCM_VERTEX_F },     // Float1,
        {2, CELL_GCM_VERTEX_F },     // Float2,         
        {3, CELL_GCM_VERTEX_F },     // Float3,         
        {4, CELL_GCM_VERTEX_F },     // Float4,         
        {4, CELL_GCM_VERTEX_UB},     // Color,         
        {4, CELL_GCM_VERTEX_UB256},  // UByte4,         
        {4, CELL_GCM_VERTEX_UB},     // UByte4N,         
        {2, CELL_GCM_VERTEX_S32K},   // Short2,         
        {4, CELL_GCM_VERTEX_S32K},   // Short4,         
        {2, CELL_GCM_VERTEX_S1},     // Short2N,         
        {4, CELL_GCM_VERTEX_S1},     // Short4N,         
        {2, CELL_GCM_VERTEX_S1},     // UShort2N,  // NOT AVAILABLE -> put in a normalized signed short
        {4, CELL_GCM_VERTEX_S1},     // UShort4N,  // NOT AVAILABLE -> put in a normalized signed short
        {1, CELL_GCM_VERTEX_F}      // UInt1,     // NOT AVAILABLE -> put in a float
    };

    // mp_VDcl_PCT
    mp_VDcl_PCT.addElement(0, offsetof(VertexPCT, m_pos),   TypeConverter[VDeclType::Float3], GCM::VertexDecl::Usage::Position);
    mp_VDcl_PCT.addElement(0, offsetof(VertexPCT, m_color), TypeConverter[VDeclType::Color],  GCM::VertexDecl::Usage::Color0);
    mp_VDcl_PCT.addElement(0, offsetof(VertexPCT, m_uv),    TypeConverter[VDeclType::Float2], GCM::VertexDecl::Usage::TexCoord0);

    // mp_VDcl_PC;
    mp_VDcl_PC.addElement(0, offsetof(VertexPC, m_pos),     TypeConverter[VDeclType::Float3], GCM::VertexDecl::Usage::Position);
    mp_VDcl_PC.addElement(0, offsetof(VertexPC, m_color),   TypeConverter[VDeclType::Color],  GCM::VertexDecl::Usage::Color0);

    // mp_VDcl_PT;
    mp_VDcl_PT.addElement(0, offsetof(VertexPT, m_pos), TypeConverter[VDeclType::Float3], GCM::VertexDecl::Usage::Position);
    mp_VDcl_PT.addElement(0, offsetof(VertexPT, m_uv),  TypeConverter[VDeclType::Float2], GCM::VertexDecl::Usage::TexCoord0);

    // mp_VDcl_PCBT;
    mp_VDcl_PCBT.addElement(0, offsetof(VertexPCBT, m_pos),          TypeConverter[VDeclType::Float3], GCM::VertexDecl::Usage::Position);
    mp_VDcl_PCBT.addElement(0, offsetof(VertexPCBT, m_color),        TypeConverter[VDeclType::Color],  GCM::VertexDecl::Usage::Color0);
    mp_VDcl_PCBT.addElement(0, offsetof(VertexPCBT, m_blendindices), TypeConverter[VDeclType::UByte4], GCM::VertexDecl::Usage::BlendIndices);
    mp_VDcl_PCBT.addElement(0, offsetof(VertexPCBT, m_uv),           TypeConverter[VDeclType::Float2], GCM::VertexDecl::Usage::TexCoord0);

    // mp_VDcl_PNCT;
    mp_VDcl_PNCT.addElement(0, offsetof(VertexPNCT, m_pos),     TypeConverter[VDeclType::Float3], GCM::VertexDecl::Usage::Position);
    mp_VDcl_PNCT.addElement(0, offsetof(VertexPNCT, m_normal),  TypeConverter[VDeclType::Float3], GCM::VertexDecl::Usage::Normal);
    mp_VDcl_PNCT.addElement(0, offsetof(VertexPNCT, m_color),   TypeConverter[VDeclType::Color],  GCM::VertexDecl::Usage::Color0);
    mp_VDcl_PNCT.addElement(0, offsetof(VertexPNCT, m_uv),      TypeConverter[VDeclType::Float2], GCM::VertexDecl::Usage::TexCoord0);

    // mp_VDcl_PNC3T;
    mp_VDcl_PNC3T.addElement(0, offsetof(VertexPNC3T, m_pos),    TypeConverter[VDeclType::Float3], GCM::VertexDecl::Usage::Position);
    mp_VDcl_PNC3T.addElement(0, offsetof(VertexPNC3T, m_color),  TypeConverter[VDeclType::Color],  GCM::VertexDecl::Usage::Color0);
    mp_VDcl_PNC3T.addElement(0, offsetof(VertexPNC3T, m_uv),     TypeConverter[VDeclType::Float2], GCM::VertexDecl::Usage::TexCoord0);
    mp_VDcl_PNC3T.addElement(0, offsetof(VertexPNC3T, m_uv2),    TypeConverter[VDeclType::Float4], GCM::VertexDecl::Usage::TexCoord1);
    mp_VDcl_PNC3T.addElement(0, offsetof(VertexPNC3T, m_uv3),    TypeConverter[VDeclType::Float4], GCM::VertexDecl::Usage::TexCoord2);
    mp_VDcl_PNC3T.addElement(0, offsetof(VertexPNC3T, m_uv4),    TypeConverter[VDeclType::Float2], GCM::VertexDecl::Usage::TexCoord3);

    // mp_VDcl_P;
    mp_VDcl_P.addElement(0,  offsetof(VertexP, m_pos), TypeConverter[VDeclType::Float3], GCM::VertexDecl::Usage::Position);
}

// right now, 1 fxParamDB for all the engine. A pure FX behavior mapping should get one per FX (ie. per ITF_shader class)
void GFXAdapter_GCM::createShaderParametersDB()
{
    m_rdrCtxt.paramDB.registerTexSampler("TextureSampler0");
    m_rdrCtxt.paramDB.registerTexSampler("TextureSampler1");
    m_rdrCtxt.paramDB.registerTexSampler("TextureSampler2");
    m_rdrCtxt.paramDB.registerTexSampler("TextureSampler3");

    m_rdrCtxt.paramDB.registerMatrix4x4("g_mWorldViewProjection");
    m_rdrCtxt.paramDB.registerMatrix4x4("g_mUVmat");
    m_rdrCtxt.paramDB.registerMatrix4x4("g_mView");
    m_rdrCtxt.paramDB.registerMatrix4x4("g_mPatchVx");
    m_rdrCtxt.paramDB.registerMatrix4x4("g_mPatchUv");

    m_rdrCtxt.paramDB.registerFloat("g_fconst0");
    m_rdrCtxt.paramDB.registerFloat("g_fconst1");
    m_rdrCtxt.paramDB.registerFloat("g_fconst2");
    m_rdrCtxt.paramDB.registerFloat("g_fconst3");
    m_rdrCtxt.paramDB.registerFloat("g_fconst4");
    m_rdrCtxt.paramDB.registerFloat("g_fconst5");
    m_rdrCtxt.paramDB.registerFloat("g_fconst6");
    m_rdrCtxt.paramDB.registerFloat("g_fconst7");

    m_rdrCtxt.paramDB.registerFloat4Array("g_va0", 8*16);
    m_rdrCtxt.paramDB.registerFloat4Array("g_va1", 4*16);

    m_rdrCtxt.paramDB.registerInt("g_iconst0");

    m_rdrCtxt.paramDB.registerFloat4("g_vconst0");
    m_rdrCtxt.paramDB.registerFloat4("g_vconst1");
    m_rdrCtxt.paramDB.registerFloat4("g_vconst2");
    m_rdrCtxt.paramDB.registerFloat4("g_vconst3");
    m_rdrCtxt.paramDB.registerFloat4("g_vconst4");
    m_rdrCtxt.paramDB.registerFloat4("g_vconst5");
    m_rdrCtxt.paramDB.registerFloat4("g_vconst6");
    m_rdrCtxt.paramDB.registerFloat4("g_vconst7");
}

//----------------------------------------------------------------------------//

void GFXAdapter_GCM::preInit()
{
    m_successfullInit = true;
    m_successfullInit = m_successfullInit && initCmdBuffer(); // init gcm and its default command buffer
    m_successfullInit = m_successfullInit && initDisplay(); // choose the resolution and init the flip hardware
    m_successfullInit = m_successfullInit && initTools(); // init tools like the VRAMAllocator

    if (!m_successfullInit) return;

    initFlipEvent(); // flip state management

    // Create back and front buffers, primary depth stencil, etc
    m_successfullInit = m_successfullInit && createRenderingSurfaces();
    if (!m_successfullInit) return;

    initAfterFx();

    m_successfullInit = m_successfullInit && initDebugTools();
}

//----------------------------------------------------------------------------//

void GFXAdapter_GCM::init()
{
    if (!m_successfullInit) return;

    if (m_minimalDisplay)
    {
        m_minimalDisplay->stop();
    }

    createCoreVertexDeclarations();
    createShaderParametersDB();
    m_successfullInit = m_successfullInit && loadCoreShaders();

    if (!m_successfullInit) return;
    initDefaultRenderStates();

    /// create internal GFX VB.
    createInternalBuffers();

#ifdef VBMANAGER_USE_DYNAMICRING_VB
    m_VertexBufferManager.createDynamicRingVB();
#endif

    resetRenderStateAfterFlip();
}

//----------------------------------------------------------------------------//

void GFXAdapter_GCM::reshape()
{
    NOT_IMPLEMENTED_ON_GCM
    if (getScreenWidth() == 0 || getScreenHeight() == 0)
        LOG("GFXAdapter init error : resolution = 0, call setResolution(width,height) before init");

    GFX_RECT Viewport;
    Viewport.left = 0;
    Viewport.top = 0;
    Viewport.right = getScreenWidth();
    Viewport.bottom = getScreenHeight();
    setupViewport(&Viewport);
}

bool GFXAdapter_GCM::initDisplay(void)
{
    // we just keep the resolution the os give us (as it's supposed to be set in the disc boot nfo)
    if (CELL_OK != cellVideoOutGetState(CELL_VIDEO_OUT_PRIMARY, 0, &m_videoOutState))
    {
        return false;
    }
    if (CELL_OK != cellVideoOutGetResolution( m_videoOutState.displayMode.resolutionId, &m_videoOutResolution))
    {
        return false;
    }

    ITF_ASSERT(m_flipMgr == NULL);

    m_usingLibResc = m_videoOutResolution.height < 720u; // SD ?
    m_usingLibResc |= (m_videoOutState.displayMode.refreshRates != CELL_VIDEO_OUT_REFRESH_RATE_60HZ // or unusual refresh rate ?
                       && m_videoOutState.displayMode.refreshRates != CELL_VIDEO_OUT_REFRESH_RATE_59_94HZ);

    if (GCM::FlipMgr::AutoAdaptFrequency)
    {
        setMonitorRefreshRate(60.f);
    }
    else
    {
        if (m_videoOutState.displayMode.refreshRates == CELL_VIDEO_OUT_REFRESH_RATE_50HZ)
            setMonitorRefreshRate(50.f);
        else if (m_videoOutState.displayMode.refreshRates == CELL_VIDEO_OUT_REFRESH_RATE_30HZ) // 3D only ?
            setMonitorRefreshRate(30.f);
        else
            setMonitorRefreshRate(60.f);
    }

    if (m_usingLibResc) 
    {
        m_flipMgr = GCM::FlipMgr::create(GCM::FlipMgr::FLIPMGR_USING_RESC);
        // When using libresc, resolution is set to 720p and it's libresc job to downscale
        // Also as libresc is internally double buffering, we switch to single buffering
        m_nbFrameBuffer = 1;
        setResolution(1280, 720);        
    }
    else
    {
        m_flipMgr = GCM::FlipMgr::create(GCM::FlipMgr::FLIPMGR_USING_GCM);
        // in regular 1080i/p and 720p, use triple buffering and no downscale
        m_nbFrameBuffer = 3; 
        setResolution(m_videoOutResolution.width, m_videoOutResolution.height);
    }

    return m_flipMgr->init(m_videoOutState);
}

void GFXAdapter_GCM::onFlip(uint32_t arg)
{
    static_cast<GFXAdapter_GCM *>(GFXAdapter_GCM::getptr())->onFlip();
}

void GFXAdapter_GCM::onFlip()
{
    ScopeLockMutex autoLock(m_flipCBMutex);
    for(u32 i = 0, n = m_flipCBs.size(); i<n; ++i)
    {
        (*m_flipCBs[i].cb)(m_flipCBs[i].usrData);
    }
}

void GFXAdapter_GCM::addFlipEventCB(void (*_cb)(u64), u64 _userData)
{
    FlipCBInfo cbInfo;
    cbInfo.cb = _cb;
    cbInfo.usrData = _userData;
    ScopeLockMutex autoLock(m_flipCBMutex);
    m_flipCBs.push_back(cbInfo);
}

void GFXAdapter_GCM::removeFlipEventCB(void (*_cb)(u64 usrData), u64 _userData)
{
    ScopeLockMutex autoLock(m_flipCBMutex);
    for(u32 i = 0, n = m_flipCBs.size(); i<n; ++i)
    {
        if (m_flipCBs[i].cb == _cb && m_flipCBs[i].usrData == _userData)
        {
            m_flipCBs.erase(m_flipCBs.begin() + i);
            return;
        }
    }
}

void GFXAdapter_GCM::initFlipEvent(void)
{
    m_gpuRenderFrameCountLabel.init(0);
    m_gpuFlipFrameCountLabel.init(0);
    m_gpuPresentTimer.init();
    m_gpuRenderTimer.init();

    ITF_ASSERT(m_flipMgr!=NULL);
    m_flipMgr->setFlipHandler(onFlip);
}


void GFXAdapter_GCM::initAfterFx()
{
    m_curswap = 0;

    u32 targetsizeX = getScreenWidth();
    u32 targetsizeY = getScreenHeight();
    const u8 BufferOutputBPP = 4;

    if (UseAfterFXFullRT)
    {
        GCM::CreateTiledSurfaceGroup(m_pFullRTsurf, AfterFXNbSwapSurface, targetsizeX, targetsizeY, BufferOutputBPP, 0);
        for (u32 i = 0; i < AfterFXNbSwapSurface; ++i)
        {
            m_pFullRTtex[i] = GCM::CreateTextureFromSurface(m_pFullRTsurf[i]);
        }
    }
    else
    {
        for (u32 i = 0; i < AfterFXNbSwapSurface; ++i)
        {
            m_pFullRTsurf[i] = NULL;
            m_pFullRTtex[i] = NULL;
        }
    }

    if (UseAfterFXDown2x2)
    {
        GCM::CreateTiledSurfaceGroup(m_pSwapTextureSurfDown2x2, AfterFXNbSwapSurface, targetsizeX>>1u, targetsizeY>>1u, BufferOutputBPP, 0);
        for (u32 i = 0; i < AfterFXNbSwapSurface; ++i)
        {
            m_pSwapTextureDown2x2[i] = GCM::CreateTextureFromSurface(m_pSwapTextureSurfDown2x2[i]);
        }
    }
    else
    {
        for (u32 i = 0; i < AfterFXNbSwapSurface; ++i)
        {
            m_pSwapTextureDown2x2[i] = NULL;
            m_pSwapTextureSurfDown2x2[i] = NULL;
        }
    }

    if (UseAfterFXDown4x4)
    {
        GCM::CreateTiledSurfaceGroup(m_pSwapTextureSurfDown4x4, AfterFXNbSwapSurface, targetsizeX>>2u, targetsizeY>>2u, BufferOutputBPP, 0);
        for (u32 i = 0; i < AfterFXNbSwapSurface; ++i)
        {
            m_pSwapTextureDown4x4[i] = GCM::CreateTextureFromSurface(m_pSwapTextureSurfDown4x4[i]);
        }
    }
    else
    {
        for (u32 i = 0; i < AfterFXNbSwapSurface; ++i)
        {
            m_pSwapTextureSurfDown4x4[i] = NULL;
            m_pSwapTextureDown4x4[i] = NULL;
        }
    }
}


} // namespace ITF
