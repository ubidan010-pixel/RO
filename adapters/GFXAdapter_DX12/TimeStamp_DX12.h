#pragma once

#include "core/utility.h"
#include "core/templateSingleton.h"
#include "core/container/extendedVector.h"
#include "DX12/Helpers.h"
#include <numeric>

namespace ITF::DX12
{
    template <ux NBFrame>
    class TimeStampManager : public TemplateSingleton<TimeStampManager<NBFrame>>
    {
    public:
        enum : ux
        {
            InvalidLabelIndex = ux(-1),
        };

        TimeStampManager() = delete;
        explicit TimeStampManager(ux _nbMaxTimeStamp, ComPtr<ID3D12Device> _device, ComPtr<ID3D12CommandQueue> _queue);

        TimeStampManager(const TimeStampManager&) = delete;
        TimeStampManager& operator = (const TimeStampManager&) = delete;

        ~TimeStampManager() = default;

        using TimeValue = u64;

        struct StartStopPair
        {
            TimeValue start;
            TimeValue stop;
        };

        enum : ux
        {
            NBStartStopPairPerTimeStamp = NBFrame + 1,
            NBU64PerTimeStamp = NBStartStopPairPerTimeStamp * 2, // start and stop per pair
        };

        struct TimeStampValues
        {
            StartStopPair timeStamps[NBStartStopPairPerTimeStamp];
        };

        TimeStampValues* alloc();
        void free(TimeStampValues* _timeStampValues);

        void queryStartTimeStamp(ID3D12GraphicsCommandList* _cmdList, TimeStampValues* _timeStamp, ux indexOfPair);
        void queryStopTimeStampAndResolve(ID3D12GraphicsCommandList* _cmdList, TimeStampValues* _timeStamp, ux indexOfPair);

        f64 getTime(TimeValue _timeStamp) const // return time in seconds
        {
            return f64(_timeStamp) / f64(m_gpuFreq);
        }

        //D3D12_GPU_VIRTUAL_ADDRESS gpuAddressOf(const PlatformLabel* p) const;

    private:
        u64 m_gpuFreq = 0; // ticks per second from the queue
        ComPtr<ID3D12QueryHeap> m_queryHeap{};
        ComPtr<ID3D12Resource> m_timeStampValuesResource{};
        TimeStampValues * m_timeStampValues = nullptr;
        ux m_nbMaxTimeStamp = 0;
        Vector<ux> m_freeIndices{};
    };

    template <ux NBFrame>
    TimeStampManager<NBFrame>::TimeStampManager(ux _nbMaxTimeStamp, ComPtr<ID3D12Device> _device, ComPtr<ID3D12CommandQueue> _queue)
        : m_nbMaxTimeStamp(_nbMaxTimeStamp)
    {
        ITF_VERIFY_HR_CALL(_queue->GetTimestampFrequency(&m_gpuFreq));

        ux totalQueries = NBU64PerTimeStamp * m_nbMaxTimeStamp;

        D3D12_QUERY_HEAP_DESC qh = {};
        qh.Count = totalQueries;
        qh.Type = D3D12_QUERY_HEAP_TYPE_TIMESTAMP;
        ITF_VERIFY_HR_CALL(_device->CreateQueryHeap(&qh, DX12_IID_COMPTR_ARGS(&m_queryHeap)));

        const u64 bytesSize = u64(m_nbMaxTimeStamp) * sizeof(TimeStampValues);
        D3D12_HEAP_PROPERTIES heapProps = {};
        heapProps.Type = D3D12_HEAP_TYPE_READBACK;

        D3D12_RESOURCE_DESC desc = {};
        desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
        desc.Width = bytesSize;
        desc.Height = 1;
        desc.DepthOrArraySize = 1;
        desc.MipLevels = 1;
        desc.Format = DXGI_FORMAT_UNKNOWN;
        desc.SampleDesc.Count = 1;
        desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

        ITF_VERIFY_HR_CALL(_device->CreateCommittedResource(
            &heapProps, D3D12_HEAP_FLAG_NONE, &desc,
            D3D12_RESOURCE_STATE_COPY_DEST,
            nullptr, DX12_IID_COMPTR_ARGS(m_timeStampValuesResource)));


        void* rawDataPtr = nullptr;
        ITF_VERIFY_HR_CALL(m_timeStampValuesResource->Map(0, nullptr, &rawDataPtr));
        m_timeStampValues = reinterpret_cast<TimeStampValues*>(rawDataPtr);

        m_freeIndices.resize(m_nbMaxTimeStamp);
        std::iota(std::make_reverse_iterator(m_freeIndices.end()),
            std::make_reverse_iterator(m_freeIndices.begin()),
            0);
    }


    template <ux NBFrame>
    typename TimeStampManager<NBFrame>::TimeStampValues*
    TimeStampManager<NBFrame>::TimeStampManager::alloc()
    {
        if (m_freeIndices.empty())
        {
            ITF_ASSERT(0);
            return nullptr;
        }
        ux newIdx = m_freeIndices.back();
        m_freeIndices.pop_back();
        return m_timeStampValues + newIdx;
    }

    template <ux NBFrame>
    void TimeStampManager<NBFrame>::free(typename TimeStampManager<NBFrame>::TimeStampValues* _timeStamp)
    {
        u32 index = u32(_timeStamp - m_timeStampValues);
        ITF_ASSERT(index < uPtr(m_nbMaxTimeStamp));
        // check it's not yet released
        ITF_ASSERT(m_freeIndices.end() == std::find(m_freeIndices.begin(), m_freeIndices.end(), index));
        m_freeIndices.push_back(index);
    }

    template <ux NBFrame>
    void TimeStampManager<NBFrame>::queryStartTimeStamp(ID3D12GraphicsCommandList* _cmdList, TimeStampManager<NBFrame>::TimeStampValues* _timeStamp, ux indexOfPair)
    {
        ITF_ASSERT(indexOfPair < NBStartStopPairPerTimeStamp);
        u32 indexOfTimeStamp = u32(_timeStamp - m_timeStampValues);
        u32 indexOfStartTimeStampU64Value = indexOfTimeStamp * NBU64PerTimeStamp + indexOfPair * 2; // *2 as we got pair of start/stop
        ITF_ASSERT(indexOfStartTimeStampU64Value + 1 < NBU64PerTimeStamp * m_nbMaxTimeStamp);
        _cmdList->EndQuery(m_queryHeap.Get(), D3D12_QUERY_TYPE_TIMESTAMP, indexOfStartTimeStampU64Value);
    }

    template <ux NBFrame>
    void TimeStampManager<NBFrame>::queryStopTimeStampAndResolve(ID3D12GraphicsCommandList* _cmdList, TimeStampValues* _timeStamp, ux indexOfPair)
    {
        ITF_ASSERT(indexOfPair < NBStartStopPairPerTimeStamp);
        u32 indexOfTimeStamp = u32(_timeStamp - m_timeStampValues);
        u32 indexOfStartTimeStampU64Value = indexOfTimeStamp * NBU64PerTimeStamp + indexOfPair * 2; // *2 as we got pair of start/stop
        u32 indexOfStopTimeStampU64Value = indexOfStartTimeStampU64Value + 1;

        ITF_ASSERT(indexOfStopTimeStampU64Value < NBU64PerTimeStamp * m_nbMaxTimeStamp);
        _cmdList->EndQuery(m_queryHeap.Get(), D3D12_QUERY_TYPE_TIMESTAMP, indexOfStopTimeStampU64Value);
        _cmdList->ResolveQueryData(m_queryHeap.Get(), D3D12_QUERY_TYPE_TIMESTAMP,
            indexOfStartTimeStampU64Value, 2, // 2 as we want to resolve start and stop
            m_timeStampValuesResource.Get(),
            indexOfStartTimeStampU64Value * sizeof(u64));
    }

    // This class is used to time GPU without CPU synchronization
    // The template parameter NBFrame as to be set to the max number of frame
    // the CPU can take in advance (0 for synch at each frame).
    // GetTime() return the GPU timing NBFrame+1 time stamp before the current cpu frame.
    // Note that one TimeStamp can be used only once per frame (unless sync is done).
    template <ux NBFrame>
    class TimeStamp
    {
    public:
        TimeStamp() = default;
        enum Flag { Init };
        TimeStamp(Flag initAtConstruction)
        {
            init();
        }

        TimeStamp(const TimeStamp&) = delete;
        ~TimeStamp() { release(); }

        TimeStamp & operator=(const TimeStamp&) = delete;

        void init()
        {
            release();
            m_report = getTimeStampManager().alloc();
            *m_report = {};

            m_currentIndex = 0;
            m_oldestTime = 0.0;
        }

        void release()
        {
            if (m_report != nullptr)
                getTimeStampManager().free(m_report);
            m_currentIndex = 0;
            m_oldestTime = 0.0;
        }

        void start(ID3D12GraphicsCommandList* _cmdList)
        {
            m_oldestTime = getTimeStampManager().getTime(getCurrentFrameDelta());
            getTimeStampManager().queryStartTimeStamp(_cmdList, m_report, m_currentIndex);
        }

        void stop(ID3D12GraphicsCommandList* _cmdList)
        {
            getTimeStampManager().queryStopTimeStampAndResolve(_cmdList, m_report, m_currentIndex);
            advanceFrame();
        }

        f64 getTime() const { return m_oldestTime; } // return time in seconds between start and stop of the NBFrame+1 previous measurement

    private:
        using TimeStampManager = TimeStampManager<NBFrame>;
        static TimeStampManager& getTimeStampManager() { return TimeStampManager::get(); }
        using TimeStampValues = typename TimeStampManager::TimeStampValues;
        using StartStopPair = typename TimeStampManager::StartStopPair;
        auto getCurrentFrameDelta() { return m_report->timeStamps[m_currentIndex].stop - m_report->timeStamps[m_currentIndex].start; }
        void advanceFrame() { m_currentIndex = (m_currentIndex + 1) % (NBFrame + 1); }
        TimeStampValues * m_report = nullptr;
        ux m_currentIndex = 0;
        f64 m_oldestTime = 0.0;
    };
} // ITF::DX12
