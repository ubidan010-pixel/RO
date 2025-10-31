#include "precompiled_core.h"

#include <sys/ppu_thread.h>
#include <sys/event.h>
#include <sys/spu_thread_group.h>

#include <cell/spurs/trace.h>
#include <cell/spurs/job_chain.h>
#include <cell/spurs/lv2_event_queue.h>
#include <cell/spurs/trace_types.h>
#include <cell/sysmodule.h>

#include "Purs_PS3.h"
#include "PursUtil_PS3.h"

#ifndef _ITF_MODULEMANAGER_PS3_H_
#include "core/system/PS3/ModuleManager_ps3.h"
#endif //_ITF_MODULEMANAGER_PS3_H_

namespace ITF
{

namespace Purs
{

    SpursInstance::SpursInstance(const char * instancePrefix)
    : m_nbSpuForSpurs(0),
      m_bTraceEnable(false),
      m_traceBuffer(0),
      m_spursPrefix(instancePrefix)
    {
    }
    
    void SpursInstance::SetupSpurs(bool hasContext, u32 nbSpuForSpurs, u32 nSpuThreadGroupPriority, bool bReleaseWhenIdle)
    {
        m_nbSpuForSpurs = nbSpuForSpurs;

        // Load module
        ITF::MODULE_MANAGER->loadModule( CELL_SYSMODULE_SPURS );

        // Setup SPURS attributes
        SYS_ERROR_CHECK(
            cellSpursAttributeInitialize(&m_attributeSpurs,
                                          m_nbSpuForSpurs,
                                          nSpuThreadGroupPriority,
                                          PursUtil::GetCurrentThreadPriority() - 2,
                                          bReleaseWhenIdle)
            );

	    SYS_ERROR_CHECK(
            cellSpursAttributeSetNamePrefix(&m_attributeSpurs,
                                            m_spursPrefix.cStr(),
                                            m_spursPrefix.getLen())
            );

        SYS_ERROR_CHECK(
                    cellSpursAttributeEnableSpuPrintfIfAvailable(&m_attributeSpurs)
            );

        if (!hasContext)
        {
            SYS_ERROR_CHECK( 
                cellSpursAttributeSetSpuThreadGroupType(&m_attributeSpurs, 
                                                        SYS_SPU_THREAD_GROUP_TYPE_EXCLUSIVE_NON_CONTEXT) 
                );
        }

        // Initialize SPURS with the attributes we've just setup
        SYS_ERROR_CHECK(
            cellSpursInitializeWithAttribute2(&m_cellSpurs,
                                             &m_attributeSpurs)
            );
    }

    void SpursInstance::CleanUpSpurs()
    {
        SYS_ERROR_CHECK(cellSpursFinalize(&m_cellSpurs));
        ITF::MODULE_MANAGER->unloadModule( CELL_SYSMODULE_SPURS );
    }

    static const u32 SPURS_TRACE_MAX_PACKET = 54 * 1024;

    void SpursInstance::SetupSpursTrace()
    {
        int bufferSize = m_nbSpuForSpurs * SPURS_TRACE_MAX_PACKET;

        m_traceBuffer  = (CellSpursTracePacket *) Memory::alignedMalloc(bufferSize, CELL_SPURS_TRACE_BUFFER_ALIGN);
        memset(m_traceBuffer, 0, bufferSize);

        SYS_ERROR_CHECK(cellSpursTraceInitialize(&m_cellSpurs, m_traceBuffer, bufferSize, 1));
        SYS_ERROR_CHECK(cellSpursTraceStart(&m_cellSpurs));
    }

    void SpursInstance::CleanUpSpursTrace()
    {
        SYS_ERROR_CHECK(cellSpursTraceStop(&m_cellSpurs));
        SYS_ERROR_CHECK(cellSpursTraceFinalize(&m_cellSpurs));

        Memory::free(m_traceBuffer);
        m_traceBuffer = 0;
    }

    void SpursInstance::GetTraceBuffer(u32 spuId, const CellSpursTracePacket * & packet, u32 & sizeOfPacket)
    {
        ITF_ASSERT(m_traceBuffer != 0); // trace not started

        if (m_traceBuffer == 0)
        {
            packet = 0;
            sizeOfPacket = 0;
            return;
        }

        sizeOfPacket = SPURS_TRACE_MAX_PACKET;
        packet = (CellSpursTracePacket *)(((char *)m_traceBuffer) + SPURS_TRACE_MAX_PACKET * spuId);
    }

    void SpursInstance::Init(bool hasContext, u32 nbSpuForSpurs, bool bTraceEnable, bool bReleaseWhenIdle, u32 nSpuThreadGroupPriority)
    {
        SetupSpurs(hasContext, nbSpuForSpurs, nSpuThreadGroupPriority, bReleaseWhenIdle);

        m_bTraceEnable = bTraceEnable;
        if (m_bTraceEnable)
        {
            SetupSpursTrace();
        }
    }

    void SpursInstance::Shutdown()
    {
        m_nbSpuForSpurs = 0;

        if (m_bTraceEnable)
        {
            CleanUpSpursTrace();
            m_bTraceEnable = false;
        }

        CleanUpSpurs();
    }

    CellSpurs * SpursInstance::GetSpurs()
    {
        return &m_cellSpurs;
    }

    CellSpurs2 * SpursInstance::GetSpurs2()
    {
        return &m_cellSpurs;
    }

    u32 SpursInstance::GetNbSpuAssigned()
    {
        return m_nbSpuForSpurs;
    }

} // Purs
} // ITF
