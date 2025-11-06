#pragma once

#include "core/utility.h"
#include "core/templateSingleton.h"
#include "core/container/extendedVector.h"
#include "DX12/Helpers.h"

// manage a label used to synch gpu and cpu
namespace ITF::DX12
{
    class LabelManager : public TemplateSingleton<LabelManager>
    {
    public:
        enum : ux
        {
            InvalidLabelIndex = ux(-1),
        };

        using PlatformLabel = u32;

        LabelManager() = delete;
        explicit LabelManager(ux _nbMaxLabel, ComPtr<ID3D12Device> _device);

        LabelManager(const LabelManager&) = delete;
        LabelManager & operator = (const LabelManager&) = delete;

        [[nodiscard]] PlatformLabel* alloc();
        void free(PlatformLabel* _pLabel);

        [[nodiscard]] D3D12_GPU_VIRTUAL_ADDRESS gpuAddressOf(const PlatformLabel* p) const;

    private:
        ComPtr<ID3D12Resource> m_labelBuffer{};
        PlatformLabel* m_labelArea = nullptr;
        ux m_nbMaxLabel = 0;
        Vector<ux> m_freeIndices;
    };

    struct Label
    {
        Label() = default;
        Label(const Label&) = delete;
        explicit Label(u32 _initValue, const char * name = nullptr)
        {
            init(_initValue, name);
        }

        Label& operator = (const Label&) = delete;

        ~Label() { release(); }

        void init(u32 _initialValue, const char* name = nullptr) // allocate a label
        {
            initInternal(_initialValue);
         }

        void release(); // release the label

        [[nodiscard]] u64 getValue() const { return *m_labelAddr; }

        void setWriteOnEOP(ID3D12GraphicsCommandList2* commandList, u32 value);

        // wait that a label got a specific value (CPU blocking)
        void cpuWait(u32 _value)
        {
            while (getValue() != _value)
            {
                Sleep(0);
            }
        }

        template<typename F>
        void cpuWaitUntil(F _fct) // loop waiting on the condition
        {
            while (!_fct(getValue()))
            {
                Sleep(0);
            }
        }

    private:
        void initInternal(u32 _initialValue);
        LabelManager::PlatformLabel * m_labelAddr = nullptr;
        D3D12_GPU_VIRTUAL_ADDRESS m_labelGpuAddr{};
    };

} // ITF::DX12
