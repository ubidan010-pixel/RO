#include "precompiled_GFXAdapter_DX12.h"

#include "GFXAdapter_DX12.h"
#include "DX12/Helpers.h"
#include "VRAMAllocator/VRAMAllocator.h"

#include "Label_DX12.h"
#include "TimeStamp_DX12.h"

namespace ITF
{

    GFXAdapter_DX12::GFXAdapter_DX12() = default;

    GFXAdapter_DX12::~GFXAdapter_DX12() = default;

    bool GFXAdapter_DX12::createDXGIFactory()
    {
    #if defined(ITF_WIN64)
        UINT dxgiFactoryFlags = 0;
        #if defined(ITF_ENABLE_DX12_GRAPHICS_DEBUGGING) && ITF_ENABLE_DX12_GRAPHICS_DEBUGGING
            if (m_debugController != nullptr)
            {
                dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
            }
        #endif
        ITF_VERIFY_HR_CALL(CreateDXGIFactory2(dxgiFactoryFlags, DX12_IID_COMPTR_ARGS(m_dxgifactory)));
        return m_dxgifactory != nullptr;
    #else
        return true;
    #endif
        
    }

    bool GFXAdapter_DX12::enumerateOutputs()
    {
    #if defined(ITF_WIN64)
        UINT i = 0;
        ComPtr<IDXGIOutput> output{};
        for (UINT i = 0; m_dxgiAdapter->EnumOutputs(i, output.ReleaseAndGetAddressOf()) == DXGI_ERROR_NOT_FOUND; ++i)
        {
            m_dxgiOutputs.push_back(std::move(output));
            if (i == 0)
                break; // on XBox, we just want the first output
        }
    #else
        m_dxgiOutputs.resize(1);
        m_dxgiAdapter->EnumOutputs(0, m_dxgiOutputs.back().ReleaseAndGetAddressOf());
    #endif
        return !m_dxgiOutputs.empty();
    }

    bool GFXAdapter_DX12::getDXGIDeviceAndAdapterOfD3D12Device()
    {
        ITF_VERIFY_HR_CALL(m_device->QueryInterface(DX12_IID_COMPTR_ARGS(m_dxgiDevice)));

        if (m_dxgiDevice)
        {
            ITF_VERIFY_HR_CALL(m_dxgiDevice->GetAdapter(m_dxgiAdapter.ReleaseAndGetAddressOf()));
        }

        return m_dxgiDevice != nullptr && m_dxgiAdapter != nullptr;
    }

    bool GFXAdapter_DX12::createDebugControler()
    {
    #if defined(ITF_ENABLE_DX12_GRAPHICS_DEBUGGING) && ITF_ENABLE_DX12_GRAPHICS_DEBUGGING
        if (SUCCEEDED(D3D12GetDebugInterface(DX12_IID_COMPTR_ARGS(m_debugController))))
        {
            m_debugController->EnableDebugLayer();
        }
    #endif
        return true;
    }

    static void filterD3D12Warnings(ComPtr<ID3D12Device> _device)
    {
    #ifdef ITF_WIN64
        // Deactivate the break on a warning
        ComPtr<ID3D12InfoQueue> infoQueue;
        if (SUCCEEDED(device.As(&infoQueue)))
        {
            D3D12_MESSAGE_ID denyIds[] =
            {
                D3D12_MESSAGE_ID_CREATEGRAPHICSPIPELINESTATE_INPUTLAYOUT_SHADER_MISMATCH
            };

            D3D12_INFO_QUEUE_FILTER filter = {};
            filter.DenyList.NumIDs = _countof(denyIds);
            filter.DenyList.pIDList = denyIds;

            infoQueue->SetBreakOnID(D3D12_MESSAGE_ID_CREATEGRAPHICSPIPELINESTATE_INPUTLAYOUT_SHADER_MISMATCH, FALSE);
        }
    #else
        // Stop to break on:
        // D3D12 ERROR: [0x73EC9EAF] ID3D12Device::CreateGraphicsPipelineState:
        // The provided input signature expects to read an element with SemanticName/Index: 'BLENDINDICES'/0 and component(s) of the type 'int32'.
        // However, the matching entry in the Input Layout declaration, element[2], specifies mismatched format: 'R8G8B8A8_UINT'.
        // This is not an error, since behavior is well defined:
        //      The element format determines what data conversion algorithm gets applied before it shows up in a shader register.
        // Independently, the shader input signature defines how the shader will interpret the data that has been placed in its input registers, with no change in the bits stored.
        // It is valid for the application to reinterpret data as a different type once it is in the vertex shader, so this warning is issued just in case reinterpretation was not intended by the author. [ STATE_CREATION WARNING #245 ]
        //
        // It means we should modify int4 iIndex : S_BLENDINDICES in the shader to use uint4.
        //_device->SetDebugErrorFilterX(0x73EC9EAFu, D3D12XBOX_DEBUG_FILTER_FLAG_DISABLE_BREAKS);
    #endif
    }

    bool GFXAdapter_DX12::createD3D12Device()
    {
    #ifdef ITF_WIN64
        // We let the system choose the default adapter by passing null as first parameter.
        // This could be reconsidered later.
        // By using D3D_FEATURE_LEVEL_11_0, we ensure that we can run on older GPU/OS.
        ITF_VERIFY_HR_CALL(D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_11_0, DX12_IID_COMPTR_ARGS(m_device)));
    #else
        D3D12XBOX_PROCESS_DEBUG_FLAGS processDebugFlags = D3D12XBOX_PROCESS_DEBUG_FLAG_NONE;
        #if defined(ITF_ENABLE_DX12_GRAPHICS_DEBUGGING) && ITF_ENABLE_DX12_GRAPHICS_DEBUGGING
            processDebugFlags |= D3D12XBOX_PROCESS_DEBUG_FLAG_DEBUG;
            processDebugFlags |= D3D12XBOX_PROCESS_DEBUG_FLAG_INSTRUMENTED;
        #endif

        D3D12XBOX_CREATE_DEVICE_PARAMETERS createDeviceParams = {};
        createDeviceParams.Version = D3D12_SDK_VERSION;
        createDeviceParams.ProcessDebugFlags = processDebugFlags;
        createDeviceParams.DisableTessellationShaderAllocations = true;
        createDeviceParams.DisableGeometryShaderAllocations = true;
        createDeviceParams.GraphicsCommandQueueRingSizeBytes = D3D12XBOX_DEFAULT_SIZE_BYTES;
        createDeviceParams.GraphicsScratchMemorySizeBytes = D3D12XBOX_DEFAULT_SIZE_BYTES;
        createDeviceParams.DisableDXR = true; // we don't use raytracing

        createDeviceParams.CreateDeviceFlags = D3D12XBOX_CREATE_DEVICE_FLAG_NONE;
        // TODO: test D3D12XBOX_CREATE_DEVICE_FLAG_ENABLE_AUTO_HDR
        // Note that it would require to call XDisplayTryEnableHdrMode
        // createDeviceParams.CreateDeviceFlags = D3D12XBOX_CREATE_DEVICE_FLAG_ENABLE_AUTO_HDR;

        // Create the D3D12 device passing null adapter to use the default one.
        ITF_VERIFY_HR_CALL(D3D12XboxCreateDevice(nullptr, &createDeviceParams, DX12_IID_COMPTR_ARGS(m_device)));
    #endif

        if (m_device != nullptr)
            filterD3D12Warnings(m_device);

        return m_device != nullptr;
    }

    bool GFXAdapter_DX12::initMemoryManagement()
    {
        DX12::VRAMAllocator::init(m_device);
        return DX12::VRAMAllocator::getDevice() != nullptr;
    }

    bool GFXAdapter_DX12::createSwapChain()
    {
    #if defined(ITF_WIN64)
        DXGI_SWAP_CHAIN_DESC1 desc = {};
        desc.BufferCount = FRAME_BUFFER_COUNT;
        desc.Width = getScreenWidth();
        desc.Height = getScreenHeight();
        desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
        desc.SampleDesc.Count = 1;

        ComPtr<IDXGISwapChain1> tempSwapChain;
        HRESULT hrCreateSwapChain = m_dxgifactory->CreateSwapChainForHwnd(m_mainContext->getQueue(), m_hwnd, &desc, nullptr, nullptr, &tempSwapChain);
        ITF_ASSERT_HR_RESULT(hrCreateSwapChain);
        m_frameBufferIndex = 0;
        if (SUCCEEDED(hrCreateSwapChain))
        {
            tempSwapChain.As(&m_swapChain);
            m_frameBufferIndex = m_swapChain->GetCurrentBackBufferIndex();
        }
        return SUCCEEDED(hrCreateSwapChain) && m_swapChain != nullptr;
    #else
        ComPtr<IDXGIOutput> dxgiOutput = m_dxgiOutputs.empty() ? nullptr : m_dxgiOutputs[0];

        // Set frame interval and register for frame events
        // TODO => try D3D12XBOX_FRAME_INTERVAL_120_HZ (or D3D12XBOX_FRAME_INTERVAL_FLAG_60FPS_PLUS)
        HRESULT hrSetFrameIntervalX = m_device->SetFrameIntervalX(dxgiOutput.Get(), D3D12XBOX_FRAME_INTERVAL_60_HZ, FRAME_BUFFER_COUNT, D3D12XBOX_FRAME_INTERVAL_FLAG_NONE);
        ITF_ASSERT_HR_RESULT(hrSetFrameIntervalX);

        // Start scheduling frame events
        HRESULT hrScheduleFrameEventX = m_device->ScheduleFrameEventX(D3D12XBOX_FRAME_EVENT_ORIGIN, 0U, nullptr, D3D12XBOX_SCHEDULE_FRAME_EVENT_FLAG_NONE);
        ITF_ASSERT_HR_RESULT(hrScheduleFrameEventX);

        m_frameBufferIndex = 0;

        return SUCCEEDED(hrSetFrameIntervalX) && SUCCEEDED(hrScheduleFrameEventX);
    #endif
    }

    bool GFXAdapter_DX12::createDeviceContext()
    {
        m_mainContext = makeUnique<MemoryId::mId_GfxAdapter, DX12::GfxDeviceContext>(m_device);
        return m_mainContext && m_mainContext->getQueue() && m_mainContext->getContext();
    }

    bool GFXAdapter_DX12::createRTVDescriptorHeap()
    {
        m_rtvDescriptorPool = makeUnique< MemoryId::mId_GfxAdapter, DX12::RenderTargetViewDescriptorPool>(m_device);
        return m_rtvDescriptorPool->getNbMaxDescriptor() != 0 && m_rtvDescriptorPool->getNbFreeDescriptor() != 0;
    }

    bool GFXAdapter_DX12::createFrontAndBackBuffers()
    {
        String8 debugName;
        for (u32 i = 0; i < ITF_ARRAY_SIZE(m_swapChainBuffers); ++i)
        {
            debugName.setTextFormat("MainRT_%d", i);
        #if defined(ITF_WIN64)
            ComPtr<ID3D12Resource> resource{};
            ITF_VERIFY_HR_CALL(m_swapChain->GetBuffer(i, DX12_IID_COMPTR_ARGS(resource)));
            m_swapChainBuffers[i] = DX12RenderTarget::createFromResource(std::move(resource), *m_rtvDescriptorPool.get(), debugName.cStr());
        #else
            m_swapChainBuffers[i] = DX12::RenderTarget::createDisplayable(getScreenWidth(), getScreenHeight(), Color::zero(), *m_rtvDescriptorPool, debugName.cStr());
        #endif
            m_swapChainBuffers[i].getOrCreateAsTexture(*m_textureDescriptorPool);
        }

        return std::end(m_swapChainBuffers) == std::find_if(std::begin(m_swapChainBuffers), std::end(m_swapChainBuffers), [](auto& p) { return p.getResource() == nullptr; });
    }

    bool GFXAdapter_DX12::createSamplerPool()
    {
        m_samplerPool = makeUnique<MemoryId::mId_GfxAdapter, DX12::SamplerDescriptorPool>(m_device);
        return m_samplerPool->getHeap() != nullptr;
    }

    bool GFXAdapter_DX12::createTextureDescriptorPool()
    {
        m_textureDescriptorPool = makeUnique<MemoryId::mId_GfxAdapter, DX12::TextureDescriptorPool>(m_device);
        return m_textureDescriptorPool->getHeap() != nullptr;
    }

    bool GFXAdapter_DX12::createUniformBufferPool()
    {
        m_uniformBufferPool = makeUnique<MemoryId::mId_GfxAdapter, DX12::UniformBufferPool>(m_device);
        return m_uniformBufferPool != nullptr;
    }

    bool GFXAdapter_DX12::createUploadBufferRequestManager()
    {
        m_uploadBufferRequestManager = makeUnique<MemoryId::mId_GfxAdapter, DX12::UploadBufferRequestManager>(m_device);
        return m_uploadBufferRequestManager != nullptr
            && m_uploadBufferRequestManager->getCopyQueue() != nullptr
            && m_uploadBufferRequestManager->getCopyCommandList() != nullptr;
    }

    bool GFXAdapter_DX12::createRenderingPipeline(int _alphaBits, int _depthBits, int _stencilBits, bbool _fullscreen, void* _hwnd)
    {
        m_hwnd = static_cast<HWND>(_hwnd);

        bool success = true;
        auto initStep = [&](auto f)
            {
                if constexpr (std::is_member_function_pointer_v<decltype(f)>)
                    success = success && (this->*f)();
                else
                    success = success && f();
            };

        initStep(&GFXAdapter_DX12::createDXGIFactory);
        initStep(&GFXAdapter_DX12::createDebugControler);
        initStep(&GFXAdapter_DX12::createD3D12Device);
        initStep(&GFXAdapter_DX12::initMemoryManagement);
        initStep(&GFXAdapter_DX12::getDXGIDeviceAndAdapterOfD3D12Device);
        initStep(&GFXAdapter_DX12::enumerateOutputs);
        initStep(&GFXAdapter_DX12::createDeviceContext);
        initStep(&GFXAdapter_DX12::createTextureDescriptorPool);
        initStep(&GFXAdapter_DX12::createSwapChain);
        initStep(&GFXAdapter_DX12::createRTVDescriptorHeap);
        initStep(&GFXAdapter_DX12::createFrontAndBackBuffers);
        initStep(&GFXAdapter_DX12::createSamplerPool);
        initStep(&GFXAdapter_DX12::createUniformBufferPool);
        initStep(&GFXAdapter_DX12::createUploadBufferRequestManager);

        return success;
    }

    void GFXAdapter_DX12::init()
    {
        ITF_ASSERT(m_device!=nullptr);

        createShaderParametersDB();

        loadCoreShaders();

        m_VertexBufferManager.createDynamicRingVB();

        // Create IB and VB for dynamic usage
        createInternalBuffers();

        initAfterFx();
    }

    void GFXAdapter_DX12::destroy()
    {
        GFXAdapter::destroy();
        if (DX12::ShaderProgramRepository::exists())
            DX12::ShaderProgramRepository::instance()->clear();
    }

    void GFXAdapter_DX12::destroyResources()
    {
        GFXAdapter::destroyResources();
    }

} // namespace ITF
