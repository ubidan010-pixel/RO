#include "precompiled_ImGuiAdapter_xboxseries.h"

#ifdef ITF_SUPPORT_IMGUI
#include "adapters/ImGuiAdapter_xboxseries/ImGuiAdapter_xboxseries.h"
#include "adapters/InputAdapter_xboxseries/InputAdapter_xboxseries.h"

#include "imgui.h"
#include "backends/scarlett/imgui_impl_scarlett.h"
#include "backends/imgui_impl_dx12.h"

#include <adapters/GFXAdapter_DX12/GFXAdapter_DX12.h>

namespace ITF {

    struct ImGuiDescriptorHeapAllocator
    {
        ID3D12DescriptorHeap* Heap = nullptr;
        D3D12_CPU_DESCRIPTOR_HANDLE HeapStartCpu{};
        D3D12_GPU_DESCRIPTOR_HANDLE HeapStartGpu{};
        UINT                        HeapHandleIncrement = 0;
        ImVector<int>               FreeIndices;

        void Create(ID3D12Device* device, ID3D12DescriptorHeap* heap)
        {
            IM_ASSERT(Heap == nullptr && FreeIndices.empty());
            Heap = heap;

            D3D12_DESCRIPTOR_HEAP_DESC desc = heap->GetDesc();
            HeapStartCpu = heap->GetCPUDescriptorHandleForHeapStart();
            HeapStartGpu = heap->GetGPUDescriptorHandleForHeapStart();
            HeapHandleIncrement = device->GetDescriptorHandleIncrementSize(desc.Type);

            FreeIndices.reserve((int)desc.NumDescriptors);
            for (int n = (int)desc.NumDescriptors; n > 0; n--)
                FreeIndices.push_back(n - 1);
        }

        void Destroy()
        {
            Heap = nullptr;
            FreeIndices.clear();
        }

        void Alloc(D3D12_CPU_DESCRIPTOR_HANDLE* out_cpu_desc_handle, D3D12_GPU_DESCRIPTOR_HANDLE* out_gpu_desc_handle)
        {
            IM_ASSERT(FreeIndices.Size > 0 && Heap != nullptr);
            int idx = FreeIndices.back();
            FreeIndices.pop_back();
            out_cpu_desc_handle->ptr = HeapStartCpu.ptr + (SIZE_T(idx) * HeapHandleIncrement);
            out_gpu_desc_handle->ptr = HeapStartGpu.ptr + (SIZE_T(idx) * HeapHandleIncrement);
        }

        void Free(D3D12_CPU_DESCRIPTOR_HANDLE cpu_desc_handle, D3D12_GPU_DESCRIPTOR_HANDLE gpu_desc_handle)
        {
            if (!Heap)
                return;

            int cpu_idx = int((cpu_desc_handle.ptr - HeapStartCpu.ptr) / HeapHandleIncrement);
            int gpu_idx = int((gpu_desc_handle.ptr - HeapStartGpu.ptr) / HeapHandleIncrement);
            IM_ASSERT(cpu_idx == gpu_idx);
            FreeIndices.push_back(cpu_idx);
        }
    };

    static ImGuiDescriptorHeapAllocator s_ImGuiSrvAlloc;

    static void ImGuiDX12_SrvAlloc(ImGui_ImplDX12_InitInfo*, D3D12_CPU_DESCRIPTOR_HANDLE* out_cpu, D3D12_GPU_DESCRIPTOR_HANDLE* out_gpu)
    {
        s_ImGuiSrvAlloc.Alloc(out_cpu, out_gpu);
    }

    static void ImGuiDX12_SrvFree(ImGui_ImplDX12_InitInfo*, D3D12_CPU_DESCRIPTOR_HANDLE cpu, D3D12_GPU_DESCRIPTOR_HANDLE gpu)
    {
        s_ImGuiSrvAlloc.Free(cpu, gpu);
    }

    bbool ImGuiAdapter_xboxseries::initialize_internal() {
        auto* pDX = static_cast<GFXAdapter_DX12*>(GFX_ADAPTER);
        if (!pDX)
            return bfalse;

        ID3D12Device* device = pDX->getDevice();
        ID3D12CommandQueue* commandQueue = pDX->getCommandQueue();
        ID3D12DescriptorHeap* srvHeap = pDX->getImGuiSrvHeap();

        if (!device || !commandQueue || !srvHeap)
            return bfalse;

        s_ImGuiSrvAlloc.Create(device, srvHeap);

        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

        if (!ImGui_ImplScarlett_Init(INPUT_ADAPTER_XBOXSERIES->getGameInput(), GFX_ADAPTER->getScreenWidth(), GFX_ADAPTER->getScreenHeight()))
            return bfalse;

        ImGui_ImplDX12_InitInfo init_info = {};
        init_info.Device = device;
        init_info.CommandQueue = commandQueue;
        init_info.NumFramesInFlight = (int)GFXAdapter_DX12::FRAME_BUFFER_COUNT;
        init_info.RTVFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
        init_info.DSVFormat = DXGI_FORMAT_UNKNOWN;
        init_info.SrvDescriptorHeap = srvHeap;
        init_info.SrvDescriptorAllocFn = &ImGuiDX12_SrvAlloc;
        init_info.SrvDescriptorFreeFn = &ImGuiDX12_SrvFree;

        if (!ImGui_ImplDX12_Init(&init_info))
            return bfalse;

        return btrue;
    }

    void ImGuiAdapter_xboxseries::shutdown_internal()
    {
        ImGui_ImplDX12_Shutdown();
        ImGui_ImplScarlett_Shutdown();
        s_ImGuiSrvAlloc.Destroy();
    }

    void ImGuiAdapter_xboxseries::beginFrame_internal()
    {
        ImGui_ImplDX12_NewFrame();
        ImGui_ImplScarlett_NewFrame();
    }

    void ImGuiAdapter_xboxseries::render_internal()
    {
        auto* pDX = static_cast<GFXAdapter_DX12*>(GFX_ADAPTER);
        if (!pDX || !pDX->isFrameStarted())
            return;

        ID3D12GraphicsCommandList* cmdList = pDX->getRenderingContext();
        ID3D12DescriptorHeap* heap = pDX->getImGuiSrvHeap();

        if (!cmdList || !heap)
            return;

        ID3D12DescriptorHeap* heaps[] = { heap };
        cmdList->SetDescriptorHeaps(1, heaps);

        ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), cmdList);
    }

    bbool ImGuiAdapter_xboxseries::isToggleVisiblePressed()
    {
        InputAdapter::PressStatus buttons[JOY_MAX_BUT];
        INPUT_ADAPTER->getGamePadButtons(InputAdapter::EnvironmentEngine, 0, buttons, JOY_MAX_BUT);

        if (buttons[m_joyButton_TriggerLeft] == InputAdapter::Pressed && buttons[m_joyButton_TriggerRight] == InputAdapter::JustPressed)
        {
            return btrue;
        }

        return bfalse;
    }
} // namespace ITF

#endif
