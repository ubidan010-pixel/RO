#include "precompiled_GFXAdapter_NVN.h"

#include "GFXAdapter_NVN.h"

#include "core/memory/MemMng/MemMngPage_Nintendo.h"
#include "Label_NVN.h"
#include "VRAMAllocator/VRAMAllocator.h"
#include "GFXAdapter_struct_NVN.h"
#include "ShaderProgram_NVN.h"

#include "Texture_NVN.h"
#include "RenderTarget_NVN.h"
#include "TexturePool_NVN.h"
#include "SamplerPool_NVN.h"

#include "NVN/Helpers.h"
#include <nvn/nvn_CppFuncPtrImpl.h> // include only once, put the global fct ptrs

#include <nv/nv_MemoryManagement.h>

#ifdef ITF_SUPPORT_NINTENDO_PROFILER
    #include <nn/profiler.h>
#endif

namespace ITF
{
    // Functions for allocations inside NVN

    static void* NvAllocateFunction(size_t size, size_t alignment, void* userPtr)
    {
        ITF_UNUSED(userPtr);
        return Memory::alignedMallocCategory(BitTweak::GetAboveAlign(size, alignment), alignment, MemoryId::mId_GfxAdapter);
    }

    static void NvFreeFunction(void* addr, void* userPtr)
    {
        ITF_UNUSED(userPtr);
        Memory::free(addr);
    }

    static void* NvReallocateFunction(void* addr, size_t newSize, void* userPtr)
    {
        ITF_UNUSED(userPtr);
        // reallocating "unaligned" as no alignment is provided in the arguments (API not consistent)
        // Set to 32 as a minimal alignment.
        size_t alignment = 32;
        // no alignedReallocCategory, so forget it.
        return Memory::alignedRealloc(addr, BitTweak::GetAboveAlign(newSize, alignment), alignment);
    }

#if ITF_ENABLE_NVN_GRAPHICS_DEBUGGING
    static void* NvAllocateFunctionDebug(size_t size, size_t alignment, void* userPtr)
    {
        ITF_UNUSED(userPtr);
        return Memory::alignedMallocCategory(BitTweak::GetAboveAlign(size, alignment), alignment, MemoryId::mId_Debug);
    }

    static void NvFreeFunctionDebug(void* addr, void* userPtr)
    {
        ITF_UNUSED(userPtr);
        Memory::free(addr);
    }

    static void* NvReallocateFunctionDebug(void* addr, size_t newSize, void* userPtr)
    {
        return NvReallocateFunction(addr, newSize, userPtr);
    }
#endif // ITF_ENABLE_NVN_GRAPHICS_DEBUGGING

    // Methods

    GFXAdapter_NVN::GFXAdapter_NVN()
    {
    }

    GFXAdapter_NVN::~GFXAdapter_NVN()
    {
    }


    void GFXAdapter_NVN::nvnDebugLayerCallback(nvn::DebugCallbackSource::Enum source, nvn::DebugCallbackType::Enum type,
        int id, nvn::DebugCallbackSeverity::Enum severity, const char* message)
    {
        LOG("NVN Debug Layer Callback:");
        LOG("  source:       0x%08x", source);
        LOG("  type:         0x%08x", type);
        LOG("  id:           0x%08x", id);
        LOG("  severity:     0x%08x", severity);
        LOG("  message:      %s", message);

        ITF_ASSERT_MSG(0, "NVN Debug layer callback hit");
    }

    extern "C" PFNNVNGENERICFUNCPTRPROC NVNAPIENTRY nvnBootstrapLoader(const char* name);

    void GFXAdapter_NVN::preInit()
    {
        ITF_ASSERT(!m_isPreinit);
        if (m_isPreinit)
            return;

        nv::SetGraphicsAllocator(NvAllocateFunction, NvFreeFunction, NvReallocateFunction, /* pUserData */ nullptr);

    #if ITF_ENABLE_NVN_GRAPHICS_DEBUGGING
        nv::SetGraphicsDevtoolsAllocator(NvAllocateFunctionDebug, NvFreeFunctionDebug, NvReallocateFunctionDebug, /* pUserData */ nullptr);
    #endif

    #if defined(ITF_NX)
        static_assert(  MemoryPageAreaAllocator::MemoryPageSizeGranularity >= nv::GraphicsMemoryGranularity &&
                        MemoryPageAreaAllocator::MemoryPageSizeGranularity % nv::GraphicsMemoryGranularity == 0);

        m_initGraphicMemoryBlock = MemoryPageAreaAllocator::allocArea(InitGraphicMemorySize);
        nv::InitializeGraphics(m_initGraphicMemoryBlock, InitGraphicMemorySize);
    #else
        nv::InitializeGraphics(nullptr, 0);
    #endif

        setResolution(m_newScreenWidth, m_newScreenHeight);
        m_needRebuildRenderTargets = false;

        m_isPreinit = true;
    }

    extern "C"
    {
        nvn::GenericFuncPtrFunc NVNAPIENTRY nvnBootstrapLoader(const char* name);
    #if ITF_ENABLE_NVN_GRAPHICS_DEBUGGING
        nvn::GenericFuncPtrFunc NVNAPIENTRY nvnDeviceGetToolsProcAddress(const nvn::Device* device, const char* name);
    #endif
    }

    void GFXAdapter_NVN::preInitNVNDebugLayer()
    {
    #if ITF_ENABLE_NVN_GRAPHICS_DEBUGGING
        // Install debug callback to catch errors that occur during the initialization phase.
        auto installDebugCB = reinterpret_cast<PFNNVNINSTALLGLOBALDEBUGCALLBACKPROC>(nvnBootstrapLoader("nvnInstallGlobalDebugCallback"));
        if (installDebugCB != nullptr)
        {
            installDebugCB([](NVNdebugCallbackSource source, NVNdebugCallbackType type,
                int id, NVNdebugCallbackSeverity severity, const char* message, void* userParam)
                {
                    reinterpret_cast<GFXAdapter_NVN*>(userParam)->nvnDebugLayerCallback(
                        static_cast<nvn::DebugCallbackSource::Enum>(source), static_cast<nvn::DebugCallbackType::Enum>(type),
                        id, static_cast<nvn::DebugCallbackSeverity::Enum>(severity), message);
                },
                this);
        }
    #endif
    }

    void GFXAdapter_NVN::createNVNDevice(nvn::DeviceGetProcAddressFunc nvnGetProcFct)
    {
        // ENABLE_SEPARATE_SAMPLER_TEXTURE_SUPPORT is probably not needed and consuming a bit of memory/cpu.
        // To change that, we need to work in index in SamplerPool and TexturePool, set them in FxParameterDB instead of SeparateTexture/Sampler Handle,
        // generate the common handle and merge ShaderParameterMapping::applyTextureParams and ShaderParameterMapping::applySamplerParams.
        int deviceFlags = nvn::DeviceFlagBits::ENABLE_SEPARATE_SAMPLER_TEXTURE_SUPPORT;
        if (m_hasNVNDebugLayer)
        {
        #ifdef ITF_ENABLE_NVN_GRAPHICS_DEBUGGING
            #ifdef ITF_DEBUG
                deviceFlags |= nvn::DeviceFlagBits::DEBUG_ENABLE_LEVEL_4;
            #elif defined(ITF_RELEASE)
                deviceFlags |= nvn::DeviceFlagBits::DEBUG_ENABLE_LEVEL_3;
            #endif
        #endif
        }

        nvn::DeviceBuilder deviceBuilder{};
        deviceBuilder.SetDefaults();
        deviceBuilder.SetFlags(deviceFlags);

        m_nvnDevice = NVN::makeBuilderInitialized<nvn::Device>(deviceBuilder);
        nvnLoadCPPProcs(m_nvnDevice.get(), nvnGetProcFct);

        
        int majorVersion = NVN::getDeviceProperty(*m_nvnDevice, nvn::DeviceInfo::API_MAJOR_VERSION);
        ITF_UNUSED(majorVersion);
        int minorVersion = NVN::getDeviceProperty(*m_nvnDevice, nvn::DeviceInfo::API_MINOR_VERSION);
        ITF_UNUSED(minorVersion);
        ITF_ASSERT_MSG(majorVersion == NVN_API_MAJOR_VERSION && minorVersion >= NVN_API_MINOR_VERSION, "NVN SDK not supported by current driver.");

        if (m_hasNVNDebugLayer)
        {
            m_nvnDevice->InstallDebugCallback(methodToCallback<&GFXAdapter_NVN::nvnDebugLayerCallback>(), this, NVN_TRUE);
        }

    #if defined(ITF_SUPPORT_NINTENDO_PROFILER) && defined(ITF_OUNCE)
        ITF_VERIFY_NN_CALL(nn::profiler::InstallGraphicsHooks());
        // Never delete, definitely given to the profiler.
        u8* profilerBuffer = new (MemoryId::mId_System) u8[nn::profiler::MinimumBufferSize];
        ITF_VERIFY_NN_CALL(nn::profiler::Initialize(profilerBuffer, nn::profiler::MinimumBufferSize));
    #endif

        m_deviceInit = true;
    }

    void GFXAdapter_NVN::initMemoryManagement()
    {
        NVN::VRAMAllocator::init(*m_nvnDevice);
        new NVN::LabelManager(2048, *m_nvnDevice);
    }

    bbool GFXAdapter_NVN::createDevice(void* _hwnd)
    {
        ITF_ASSERT(m_isPreinit);

        ITF_ASSERT(m_newScreenWidth == getScreenWidth());
        ITF_ASSERT(m_newScreenHeight == getScreenHeight());

        m_nativeWindow = _hwnd;

        preInitNVNDebugLayer();

        nvn::DeviceGetProcAddressFunc nvnGetProcFct = reinterpret_cast<nvn::DeviceGetProcAddressFunc>(nvnBootstrapLoader("nvnDeviceGetProcAddress"));
        nvnLoadCPPProcs(nullptr, nvnGetProcFct); // fill globals fct ptr needed to create the device

        int hasNVNDebugLayer = 0;
        m_nvnDevice->GetInteger(nvn::DeviceInfo::SUPPORTS_DEBUG_LAYER, &hasNVNDebugLayer); // m_device can be null here (specific to this fct)
        m_hasNVNDebugLayer = hasNVNDebugLayer != 0;

        createNVNDevice(nvnGetProcFct);
        initMemoryManagement();

        m_nvnDevice->SetWindowOriginMode(nvn::WindowOriginMode::UPPER_LEFT); // Follow Direct3D conventions.

        m_textureDescriptorPool = NVN::makeUnique<NVNTexturePool>(*m_nvnDevice);
        m_samplerDescriptorPool = NVN::makeUnique<NVNSamplerPool>(*m_nvnDevice);
        m_uniformBufferPool = NVN::makeUnique<NVNUniformBufferPool>(*m_nvnDevice);

        m_shaderProgramCache = NVN::makeUnique<NVN::ShaderProgramCache>();
        m_polygonStateCache = NVN::makeUnique<NVN::PolygonStateCache>();
        m_blendStateCache = NVN::makeUnique<NVN::BlendStateCache>();

        createRenderTargets();
        m_mainContext = new (MemoryId::mId_GfxAdapter) NVNGfxDeviceContext(*m_nvnDevice);

        return btrue;
    }


    void GFXAdapter_NVN::init()
    {
        ITF_ASSERT(m_isPreinit);

        createShaderParametersDB();

        loadCoreShaders();

        initDefaultRenderStates();

        m_VertexBufferManager.createDynamicRingVB();

        // Create IB and VB for dynamic usage
        createInternalBuffers();

        initAfterFx();
    }

    void GFXAdapter_NVN::destroy()
    {
        GFXAdapter::destroy();
        // destroy of the NVN GFX adapter is never supposed to be called (the process is killed by the OS)
        NOT_IMPLEMENTED_ON_NX
    }

    void GFXAdapter_NVN::destroyResources()
    {
        GFXAdapter::destroyResources();
        // As destroy, should never be called
        NOT_IMPLEMENTED_ON_NX
    }

    void GFXAdapter_NVN::initDefaultRenderStates()
    {
        // ?
    }

} // namespace ITF
