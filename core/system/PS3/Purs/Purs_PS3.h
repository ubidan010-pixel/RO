#ifndef ITF_CORE_PURS_H_
#define ITF_CORE_PURS_H_

#include <cell/spurs/types.h>
#include <cell/spurs/trace_types.h>
#include <cell/spurs/job_descriptor.h>
#include <cell/spurs/job_commands.h>
#include <cell/spurs/control.h>

#include "core/String8.h"
#include "PursUtil_PS3.h"

// Purs is a C++ style layer above spurs, adding facilities and constraint checks

#ifndef ITF_PS3
#error This file is only intended to be used on PS3
#endif

namespace ITF
{

namespace Purs
{
    class SpursInstance
    {
    public:        
        SpursInstance(const char * instancePrefix);

        void Init( bool hasContext = true,
                   u32 nbSpuForSpurs = 6,
                   bool bTraceEnable = false,
                   bool bReleaseWhenIdle = false,
                   u32 nSpuThreadGroupPriority = 250 );
        void Shutdown();

        u32 GetNbSpuAssigned();
        void GetTraceBuffer(u32 spuId, const CellSpursTracePacket * & packet, u32 & sizeOfPacket);

        CellSpurs * GetSpurs();
        CellSpurs2 * GetSpurs2();

    protected:
        void SetupSpurs(bool hasContext, u32 nbSpuForSpurs, u32 nSpuThreadGroupPriority, bool bReleaseWhenIdle);
        void CleanUpSpurs();

        void SetupSpursTrace();
        void CleanUpSpursTrace();

        CellSpurs2                  m_cellSpurs __attribute__((__aligned__(128))); // spurs object  
        CellSpursAttribute          m_attributeSpurs;
        
        u32                         m_nbSpuForSpurs;
        bool                        m_bTraceEnable;
        CellSpursTracePacket *      m_traceBuffer;
        String8                     m_spursPrefix;
    };
} // Purs
} // ITF

#include "PursJob_PS3.h"
// #include "PursTask.h" // todo

#endif // ITF_CORE_PURS_H_
