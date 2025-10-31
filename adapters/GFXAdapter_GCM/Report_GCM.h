#ifndef ITF_REPORTGCM_H_
#define ITF_REPORTGCM_H_

#include "RenderContext_GCM.h"
#include "VRAMRelocHdl.h"

// Manage a reports used to get rendering stats and info
// Can also be used for synchronization by setting the 'zero' field to non zero
namespace ITF
{
namespace GCM
{
    class ReportManager : public TemplateSingleton<ReportManager>
    {
    public:
        enum 
        {
            InvalidReportIndex = u32(-1)
        };

        ReportManager(u32 _nbMaxReport, u32 _rsxLocation);

        u32 allocIndex();
        void freeIndex(u32 _index);
        CellGcmReportData * getAddressFromIndex(u32 _index) const;

    private:
        Vector<u32> m_freeIndices;
        SimpleVRAMRelocHandle m_vramMainReportArea;
        const u32 m_nbIndices;
        const u32 m_rsxLocation;
    };

    struct Report
    {
        Report()
            : reportIdx(ReportManager::InvalidReportIndex)
            , reportAddr(NULL)
        {}

        ~Report() { release(); }

        void init(); // allocate the report index
        void release(); // release the report index
        bool isValid() const { return reportIdx != ReportManager::InvalidReportIndex; }

        u64 getTimeStamp() const { ITF_ASSERT(isValid()); return reportAddr->timer; }
        u32 getStat() const { ITF_ASSERT(isValid()); return reportAddr->value; }
        u32 getZero() const { ITF_ASSERT(isValid()); return reportAddr->zero; }

        // GPU commands
        // Ask the GPU to report a rendering stat
        void setStat(RenderContext & _rdrCtxt, u32 _type)
        {
            _rdrCtxt.gcmCtxt->SetReport(_type, reportIdx);
        }

        void setTimeStamp(RenderContext & _rdrCtxt)
        {
            _rdrCtxt.gcmCtxt->SetTimeStamp(reportIdx);
        }

        u32 reportIdx;
        CellGcmReportData * volatile reportAddr;        
    };

    // This class is used to time GPU without CPU synchronization
    // The template parameter NBFrame as to be set to the max number of frame
    // the CPU can take in advance (0 for synch at each frame).
    // GetTime() return the GPU timing NBFrame+1 time stamp before the current cpu frame.
    // Note that one TimeStamp can be used only once per frame (unless sync is done).
    template <u32 NBFrame>
    class TimeStamp
    {
    public:
        TimeStamp()
            : m_currentIndex(u32(-1))
            , m_oldestTime(0.)
        {}
        ~TimeStamp() { release();}
        
        void init(); // reserve reports
        void release();
        
        void start(RenderContext & _rdrCtxt); // start measurement
        void stop(RenderContext & _rdrCtxt); // stop it
        
        u64     getTime();
        f32     getTimeMilliSec();

    private:
        Report		    m_reportStart[NBFrame+1];
        Report		    m_reportStop[NBFrame+1];
        u32             m_currentIndex;
        u64             m_oldestTime;
    };

    // implementation
    template <u32 NBFrame>
    inline void TimeStamp<NBFrame>::init()
    {
        m_currentIndex	 = 0;
        for(u32 i =0; i < NBFrame+1; ++i)
        {
            m_reportStart[i].init();
            m_reportStop[i].init();
            ITF_ASSERT(m_reportStop[i].isValid());
            ITF_ASSERT(m_reportStart[i].isValid());
        }
    }

    template <u32 NBFrame>
    inline void TimeStamp<NBFrame>::release()
    {
        m_currentIndex = u32(-1);

        for(u32 i =0; i < NBFrame+1; ++i)
        {
            m_reportStart[i].release();
            m_reportStop[i].release();
        }
    }

    template <u32 NBFrame>
    inline void TimeStamp<NBFrame>::start(RenderContext & _rdrCtxt) // start measurement
    {
        // we are restarting the timing, so we are supposed to get a frame elapsed,
        // so the previous 'stop' has been validated 
        m_oldestTime = m_reportStop[m_currentIndex].getTimeStamp() - m_reportStart[m_currentIndex].getTimeStamp();
        m_reportStart[m_currentIndex].setTimeStamp(_rdrCtxt);
    }
    
    template <u32 NBFrame>
    inline void TimeStamp<NBFrame>::stop(RenderContext & _rdrCtxt) // stop it
    {
        m_reportStop[m_currentIndex].setTimeStamp(_rdrCtxt);
        m_currentIndex = (m_currentIndex+1)%(NBFrame+1);
    }

    template <u32 NBFrame>
    inline u64 TimeStamp<NBFrame>::getTime()
    {
        return m_oldestTime;
    }

    template <u32 NBFrame>
    inline f32 TimeStamp<NBFrame>::getTimeMilliSec()
    {
        return f32(f64(getTime())*(1./1000000.)); // keep the divide in f64 as we could loose some precision
    }


} // GCM
} // ITF

#endif // ITF_REPORTGCM_H_
