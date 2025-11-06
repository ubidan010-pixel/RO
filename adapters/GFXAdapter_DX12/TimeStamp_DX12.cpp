#include "precompiled_GFXAdapter_DX12.h"

#include "TimeStamp_DX12.h"

#include <numeric>

namespace ITF::DX12
{
#ifdef ITF_DEBUG
    // Instanciation test

    using TimeStampManager2 = TimeStampManager<2>;

    void createTimeStampStartStopManager(ux _nbMaxTimeStamp, ComPtr<ID3D12Device> _device, ComPtr<ID3D12CommandQueue> _queue)
    {
        static TimeStampManager2 mgr(_nbMaxTimeStamp, _device, _queue);
    }

    using TimeStamp2 = TimeStamp<2>;

    f64 addTimeStampStartStop(ID3D12GraphicsCommandList* _cmdList)
    {
        static TimeStamp2 stamp{TimeStamp2::Init};

        stamp.start(_cmdList);
        stamp.stop(_cmdList);
        return stamp.getTime();
    }

    void create()
    {
        TimeStamp2 stamp{ TimeStamp2::Init };
    }

#endif

} // ITF::DX12


