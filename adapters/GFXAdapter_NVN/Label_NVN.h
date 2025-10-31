#pragma once

#include "core/utility.h"
#include "core/templateSingleton.h"
#include "core/container/extendedVector.h"
#include "NVN/Helpers.h"

#include "VRAMAllocator/VRAMRelocHdl.h"

// manage a label used to synch gpu and cpu
namespace ITF::NVN
{

    class LabelManager : public TemplateSingleton<LabelManager>
    {
    public:
        enum : ux
        {
            InvalidLabelIndex = ux(-1),
        };

        // sce::Agc::Label is a 256 bit structure, with only the first 64 bits written.
        using PlatformLabel = nvn::CounterData;

        LabelManager() = delete;
        explicit LabelManager(ux nbMaxLabel, nvn::Device& device);

        LabelManager(const LabelManager&) = delete;
        LabelManager & operator = (const LabelManager&) = delete;

        PlatformLabel* alloc();
        iPtr getByteOffset(PlatformLabel* label) const { return sizeof(PlatformLabel) * (label - m_labelArea); }
        nvn::BufferAddress getBufferAddress(PlatformLabel* label) const { return m_labelBuffer.GetAddress() + getByteOffset(label); }
        void free(PlatformLabel* _pLabel);

        nvn::Device& getDevice() const { return m_nvnDevice; }

    private:
        NVN::SimpleVRAMRelocHandle m_mem;
        PlatformLabel* m_labelArea = nullptr;
        ux m_labelCount = 0;
        Vector<ux> m_freeIndices;
        nvn::Buffer m_labelBuffer;
        nvn::Device& m_nvnDevice;
    };

    struct Label
    {
        Label() = default;
        Label(const Label&) = delete;
        explicit Label(u64 _initValue, const char * name = nullptr)
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

        nvn::CounterData* getNVNData() const { return m_labelAddr; }
        nvn::BufferAddress getNVNBufferAddress() const { return m_bufferAddress; }

        u64 getCounter() const { return m_labelAddr->counter; }
        u64 getTime() const { return LabelManager::get().getDevice().GetTimestampInNanoseconds(m_labelAddr); }

        // GPU commands
        void setWriteOnEOP(nvn::CommandBuffer* _gfxCtxt, u32 _value)
        {
            _gfxCtxt->Barrier(nvn::BarrierBits::ORDER_FRAGMENTS);
            _gfxCtxt->ReportValue(_value, getNVNBufferAddress());
        }

        void setTimeStamp(nvn::CommandBuffer* _gfxCtxt)
        {
            _gfxCtxt->Barrier(nvn::BarrierBits::ORDER_FRAGMENTS);
            _gfxCtxt->ReportCounter(nvn::CounterType::TIMESTAMP, getNVNBufferAddress());
        }

        // wait that a label got a specific value (CPU blocking)
        void cpuWait(u64 _value)
        {
            auto waitTime = nn::TimeSpan::FromMicroSeconds(10);
            while (getCounter() != _value)
            {
                nn::os::SleepThread(waitTime);
            }
        }

        template<typename F>
        void cpuWaitUntil(F _fct) // loop waiting on the condition
        {
            auto waitTime = nn::TimeSpan::FromMicroSeconds(10);
            while (!_fct(getCounter()))
            {
                nn::os::SleepThread(waitTime);
            }
        }

    private:
        void initInternal(u32 _initialValue);
        LabelManager::PlatformLabel* m_labelAddr = nullptr;
        nvn::BufferAddress m_bufferAddress = nvn::BufferAddress(0);
    };

    // This class is used to time GPU without CPU synchronization
    // The template parameter NBFrame as to be set to the max number of frame
    // the CPU can take in advance (0 for synch at each frame).
    // GetTime() return the GPU timing NBFrame+1 time stamp before the current cpu frame.
    // Note that one TimeStamp can be used only once per frame (unless sync is done).
    template <ux NBFrame>
    class TimeStamp
    {
    public:
        TimeStamp()
            : m_currentIndex(ux(-1))
            , m_oldestTime(0)
        {
        }

        ~TimeStamp() { release();}

        void init(const char * name = nullptr); // reserve reports
        void release();

        void start(nvn::CommandBuffer* _gfxCtxt); // start measurement
        void stop(nvn::CommandBuffer* _gfxCtxt); // stop it

        u64     getNbNanoSeconds() const;
        f32     getTime() const;

    private:
        Label           m_reportStart[NBFrame+1];
        Label           m_reportStop[NBFrame+1];
        ux              m_currentIndex;
        u64             m_oldestTime;
    };

    // implementation
    template <ux NBFrame>
    inline void TimeStamp<NBFrame>::init(const char* _name)
    {
        m_currentIndex	 = 0;
        for(auto & label : m_reportStart)
        {
            label.init(0);
        }
        for(auto & label : m_reportStop)
        {
            label.init(0);
        }
    }

    template <ux NBFrame>
    inline void TimeStamp<NBFrame>::release()
    {
        m_currentIndex = ux(-1);
        for(auto & label : m_reportStart)
        {
            label.release();
        }
        for(auto & label : m_reportStop)
        {
            label.release();
        }
    }


    template <ux NBFrame>
    inline void TimeStamp<NBFrame>::start(nvn::CommandBuffer * _gfxCtxt) // start measurement
    {
        // we are restarting the timing, so we are supposed to get a frame elapsed,
        // so the previous 'stop' has been validated 
        m_oldestTime = m_reportStop[m_currentIndex].getTime() - m_reportStart[m_currentIndex].getTime();
        m_reportStart[m_currentIndex].setTimeStamp(_gfxCtxt);
    }

    template <ux NBFrame>
    inline void TimeStamp<NBFrame>::stop(nvn::CommandBuffer * _gfxCtxt) // stop it
    {
        m_reportStop[m_currentIndex].setTimeStamp(_gfxCtxt);
        m_currentIndex = (m_currentIndex+1)%(NBFrame+1);
    }

    template <u32 NBFrame>
    inline u64 TimeStamp<NBFrame>::getNbNanoSeconds() const
    {
        return m_oldestTime;
    }

    template <u32 NBFrame>  
    inline f32 TimeStamp<NBFrame>::getTime() const  
    {  
        return static_cast<f32>(static_cast<f64>(getNbNanoSeconds()) / 1e9);
    }

} // ITF::NVN
