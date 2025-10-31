#ifndef ITF_LABEL_PS5_H_
#define ITF_LABEL_PS5_H_

#pragma once

#include <agc/commandbuffer.h>
#include <agc/core/timestamps.h>
#include "core/utility.h"
#include "core/templateSingleton.h"
#include "core/container/extendedVector.h"
#include "Gnmp_Helpers.h"

// manage a label used to synch gpu and cpu
namespace ITF::Gnmp
{
    class LabelManager : public TemplateSingleton<LabelManager>
    {
    public:
        enum : ux
        {
            InvalidLabelIndex = ux(-1),
        };

        // sce::Agc::Label is a 256 bit structure, with only the first 64 bits written.
        using PlatformLabel = sce::Agc::Label;

        LabelManager() = delete;
        LabelManager(u8* _labelArea, ux _labelMemorySize);

        LabelManager(const LabelManager&) = delete;
        LabelManager & operator = (const LabelManager&) = delete;

        PlatformLabel* alloc();
        void free(PlatformLabel* _pLabel);

    private:
        PlatformLabel* m_labelArea = nullptr;
        ux m_labelCount = 0;
        Vector<ux> m_freeIndices;
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

        void init(u64 _initialValue, const char* name = nullptr) // allocate a label
        {
            initInternal(_initialValue);
            if (name != nullptr)
            {
                m_resHandler.set(ITF_AGC_REGISTER_RESOURCE(m_labelAddr, "%s", name));
            }
        }

        void release(); // release the label

        u64 getValue() const { return m_labelAddr->m_value; }
        volatile u64 * getAddress() const { return &m_labelAddr->m_value; }
        sce::Agc::Label* getAgcLabel() const { return m_labelAddr; }

        // GPU commands
        void setWriteOnEOP(Gnmx::GfxContext& _gfxCtxt, u64 _value)
        {
            _gfxCtxt.writeImmediateAtEndOfPipe(Gnmp::kEopFlushCbDbCaches, m_labelAddr, _value, Gnmp::kCacheActionNone);
        }

        void setTimeStamp(Gnmx::GfxContext & _gfxCtxt)
        {
            sce::Agc::Core::writeTimestamp(&_gfxCtxt.m_agcCtx.m_dcb, m_labelAddr, sce::Agc::Core::TimestampType::kBottomOfPipe);
        }

        static constexpr f64 timeStampFrequency() { return 800000000.; } // GPU clock is 800Mhz

        // wait that a label got a specific value (CPU blocking)
        void cpuWait(u64 _value)
        {
            while (getValue() != _value)
            {
                sceKernelUsleep(100);
            }
        }

        template<typename F>
        void cpuWaitUntil(F _fct) // loop waiting on the condition
        {
            while (!_fct(getValue()))
            {
                sceKernelUsleep(100);
            }
        }

        ResourceHandler m_resHandler;

    private:
        void initInternal(u64 _initialValue);
        LabelManager::PlatformLabel* m_labelAddr = nullptr;
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

        void start(Gnmx::GfxContext & _gfxCtxt); // start measurement
        void stop(Gnmx::GfxContext & _gfxCtxt); // stop it

        u64     getNbTick() const;
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
            if (_name!=nullptr)
                label.m_resHandler.set(ITF_AGC_REGISTER_RESOURCE(label.getAgcLabel(), "%s_start_%u", _name, distInRange(m_reportStart, label)));
        }
        for(auto & label : m_reportStop)
        {
            label.init(0);
            if (_name != nullptr)
                label.m_resHandler.set(ITF_AGC_REGISTER_RESOURCE(label.getAgcLabel(), "%s_stop_%u", _name, distInRange(m_reportStop, label)));
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
    inline void TimeStamp<NBFrame>::start(Gnmx::GfxContext & _gfxCtxt) // start measurement
    {
        // we are restarting the timing, so we are supposed to get a frame elapsed,
        // so the previous 'stop' has been validated 
        m_oldestTime = m_reportStop[m_currentIndex].getValue() - m_reportStart[m_currentIndex].getValue();
        m_reportStart[m_currentIndex].setTimeStamp(_gfxCtxt);
    }

    template <ux NBFrame>
    inline void TimeStamp<NBFrame>::stop(Gnmx::GfxContext & _gfxCtxt) // stop it
    {
        m_reportStop[m_currentIndex].setTimeStamp(_gfxCtxt);
        m_currentIndex = (m_currentIndex+1)%(NBFrame+1);
    }

    template <u32 NBFrame>
    inline u64 TimeStamp<NBFrame>::getNbTick() const
    {
        return m_oldestTime;
    }

    template <u32 NBFrame>
    inline f32 TimeStamp<NBFrame>::getTime() const
    {
        return f32(f64(getNbTick())*(1./Label::timeStampFrequency())); // keep the operation in f64 as we could loose some precision
    }
} // ITF::Gnmp

#endif // ITF_LABEL_PS5_H_
